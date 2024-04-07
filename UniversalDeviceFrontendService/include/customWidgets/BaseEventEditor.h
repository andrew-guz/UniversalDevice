#ifndef _BASE_EVENT_EDITOR_H_
#define _BASE_EVENT_EDITOR_H_

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>

#include <nlohmann/json.hpp>

#include "DeviceComboBox.h"
#include "Event.h"
#include "IEventEditorWidget.h"

class BaseEventEditor : public Wt::WContainerWidget, public IEventEditorWidget {
public:
    BaseEventEditor();

    virtual ~BaseEventEditor() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    template<typename T>
    T* GetEvent() const {
        return new T();
    }

    virtual void FillFromUi(Event& event) const override;

protected:
    std::vector<ExtendedComponentDescription> FilteredDevices(std::string_view type);

    std::vector<ExtendedComponentDescription> FilteredDevices(const std::set<std::string_view>& types);

protected:
    std::vector<ExtendedComponentDescription> _devices;
    Wt::WGridLayout* _mainLayout;
    Wt::WLineEdit* _name;
    Wt::WCheckBox* _active;
};

#endif //_BASE_EVENT_EDITOR_H_
