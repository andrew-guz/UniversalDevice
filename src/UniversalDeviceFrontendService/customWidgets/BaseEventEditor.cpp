#include "BaseEventEditor.hpp"

#include <algorithm>
#include <memory>
#include <set>
#include <variant>

#include <Wt/WContainerWidget.h>
#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>

#include "Device.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventUtils.hpp"
#include "IEventEditorWidget.hpp"

using namespace Wt;

BaseEventEditor::BaseEventEditor() :
    WContainerWidget(),
    IEventEditorWidget() {
    _mainLayout = setLayout(std::make_unique<WVBoxLayout>());
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    auto commonCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto commonLayout = commonCanvas->setLayout(std::make_unique<WHBoxLayout>());
    commonLayout->setContentsMargins(0, 0, 0, 0);

    commonLayout->addWidget(std::make_unique<WText>("Имя:"), 0, AlignmentFlag::Left | AlignmentFlag::Top);
    _name = commonLayout->addWidget(std::make_unique<WLineEdit>(), 1, AlignmentFlag::Top);
    _active = commonLayout->addWidget(std::make_unique<WCheckBox>("Активно"), 0, AlignmentFlag::Right | AlignmentFlag::Top);
}

void BaseEventEditor::SetDevices(const Devices& devices) { _devices = devices; }

void BaseEventEditor::Cleanup() {
    _name->setText({});
    _active->setChecked(false);
}

void BaseEventEditor::FillUi(const Event& event) {
    _name->setText(GetEventName(event));
    _active->setChecked(GetEventActivity(event));
}

bool BaseEventEditor::IsValid() const { return _name->text().toUTF8().size(); }

void BaseEventEditor::FillFromUi(Event& event) const {
    std::visit([&](auto& e) { e._name = _name->text().toUTF8(); }, event);
    std::visit([&](auto& e) { e._active = _active->isChecked(); }, event);
}

Devices BaseEventEditor::FilteredDevices(const DeviceType type) { return FilteredDevices(std::set<DeviceType>{ type }); }

Devices BaseEventEditor::FilteredDevices(const std::set<DeviceType>& types) {
    auto devices = _devices;
    auto newEnd = std::remove_if(devices.begin(), devices.end(), [&](const auto& d) { return types.count(d._type) == 0; });
    devices.erase(newEnd, devices.end());
    return devices;
}
