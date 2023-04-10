#ifndef _BASE_DEVICE_WIDGET_H_
#define _BASE_DEVICE_WIDGET_H_

#include <mutex>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WText.h>
#include <Wt/WTimer.h>

#include "Uuid.h"
#include "JsonExtension.h"
#include "DeviceInformationDescription.h"
#include "BaseStackWidget.h"
#include "RequestHelper.h"
#include "UrlHelper.h"
#include "MessageHelper.h"

class BaseDeviceWidget : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    BaseDeviceWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~BaseDeviceWidget() = default;

    virtual void Initialize(const std::string& data) override final;

protected:
    virtual void OnBack() {}

    virtual void Initialize() = 0;

    enum ClearType
    {
        Name = 0x1,
        Data = 0x2,
        All = Name | Data
    };

    void Clear(ClearType type);

    virtual void ClearData() = 0;
    
    template<typename TSettings>
    TSettings GetSettings()
    {
        auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, Constants::LoginService);
        return JsonExtension::CreateFromJson<TSettings>(replyJson);
    }

    //return 1 last point
    template<typename TValues>
    std::vector<TValues> GetValues(const std::string& type)
    {
        return GetValues<TValues>(type, (uint64_t)0);
    }


    //return data from last N seconds or at least one last known point
    template<typename TValues>
    std::vector<TValues> GetValues(const std::string& type, uint64_t seconds)
    {
        DeviceInformationDescription messageData;
        messageData._type = type;
        messageData._id = _deviceId;
        messageData._seconds = seconds;
        auto postMessage = MessageHelper::Create({}, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
        auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE_GET_INFO }, Constants::LoginService, postMessage.ToJson());
        return JsonExtension::CreateVectorFromJson<TValues>(replyJson);
    }

    void UpdateName();

    virtual void OnSettingsButton() = 0;

protected:
    std::mutex          _requestMutex;
    Wt::WGridLayout*    _mainLayout;
    Wt::WText*          _nameText;
    Wt::WText*          _timeText;
    Uuid                _deviceId;
    std::string         _deviceName;
    Wt::WTimer*         _refreshTimer;
};

#endif //_BASE_DEVICE_WIDGET_H_
