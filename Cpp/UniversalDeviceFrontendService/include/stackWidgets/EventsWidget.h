#ifndef _EVENTS_WIDGET_H_
#define _EVENTS_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <nlohmann/json.hpp>

#include "BaseStackWidget.h"
#include "Settings.h"

class EventsWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    EventsWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~EventsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    std::vector<nlohmann::json> GetEvents();

private:
    Wt::WGridLayout*            _mainLayout;
    std::vector<nlohmann::json> _eventJsons;
};

#endif //_EVENTS_WIDGET_H_
