#ifndef _BASE_EVENT_EDITOR_H_
#define _BASE_EVENT_EDITOR_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>

#include <nlohmann/json.hpp>

#include "IEventEditorWidget.h"
#include "Event.h"
#include "DeviceComboBox.h"

class BaseEventEditor : Wt::WContainerWidget, public IEventEditorWidget
{
public:
    BaseEventEditor(const std::vector<ComponentDescription>& devices);

    virtual ~BaseEventEditor() = default;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    template <typename T>
    T GetEvent() const { return T(); }

    virtual void FillFromUi(Event& event) const override;

protected:
    std::vector<ComponentDescription>   _devices;
    Wt::WGridLayout*                    _mainLayout;
    Wt::WLineEdit*                      _name;
    Wt::WCheckBox*                      _active;
};

#endif //_BASE_EVENT_EDITOR_H_
