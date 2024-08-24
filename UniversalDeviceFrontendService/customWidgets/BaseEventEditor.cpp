#include "BaseEventEditor.hpp"

#include "Event.hpp"

using namespace Wt;

BaseEventEditor::BaseEventEditor() : WContainerWidget(), IEventEditorWidget() {
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    _mainLayout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    _name = _mainLayout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    _active = _mainLayout->addWidget(std::make_unique<WCheckBox>("Активно"), 1, 0, 1, 2);
}

void BaseEventEditor::SetDevices(const std::vector<ExtendedComponentDescription>& devices) { _devices = devices; }

void BaseEventEditor::Cleanup() {
    _name->setText({});
    _active->setChecked(false);
}

void BaseEventEditor::FillUi(const Event& event) {
    _name->setText(event._name);
    _active->setChecked(event._active);
}

bool BaseEventEditor::IsValid() const { return _name->text().toUTF8().size(); }

void BaseEventEditor::FillFromUi(Event& event) const {
    event._name = _name->text().toUTF8();
    event._active = _active->isChecked();
}

std::vector<ExtendedComponentDescription> BaseEventEditor::FilteredDevices(const DeviceType type) {
    return FilteredDevices(std::set<DeviceType>{ type });
}

std::vector<ExtendedComponentDescription> BaseEventEditor::FilteredDevices(const std::set<DeviceType>& types) {
    auto devices = _devices;
    auto newEnd =
        std::remove_if(devices.begin(), devices.end(), [&](const auto& d) { return !d.isDeviceType() || types.count(d.getDeviceType()) == 0; });
    devices.erase(newEnd, devices.end());
    return devices;
}
