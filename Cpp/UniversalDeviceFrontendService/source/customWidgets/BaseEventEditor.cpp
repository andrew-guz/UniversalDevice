#include "BaseEventEditor.h"

#include "Defines.h"
#include "Event.h"
#include "RequestHelper.h"

using namespace Wt;

BaseEventEditor::BaseEventEditor(int servicePort) :
    WContainerWidget(),
    IEventEditorWidget(),
    _servicePort(servicePort)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    _mainLayout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    _name = _mainLayout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    _active = _mainLayout->addWidget(std::make_unique<WCheckBox>("Активно"), 1, 0, 1, 2);
}

void BaseEventEditor::Cleanup()
{
    _name->setText({});
    _active->setChecked(false);
}

void BaseEventEditor::FillUi(const Event& event)
{
    _name->setText(event._name);
    _active->setChecked(event._active);
}

bool BaseEventEditor::IsValid() const
{
    return _name->textSize() > 0;
}

void BaseEventEditor::FillFromUi(Event& event) const
{
    event._name = _name->text().toUTF8();
    event._active = _active->isChecked();
}

std::vector<ComponentDescription> BaseEventEditor::GetDevices() const
{
    auto resultJson = RequestHelper::DoGetRequest({ "127.0.0.1", _servicePort, API_CLIENT_DEVICES }, Constants::LoginService);
    return JsonExtension::CreateVectorFromJson<ComponentDescription>(resultJson);
}
