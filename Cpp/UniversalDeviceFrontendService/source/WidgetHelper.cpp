#include "WidgetHelper.h"

#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSpinBox.h>
#include <Wt/WRegExpValidator.h>

using namespace Wt;

void WidgetHelper::SetUsualButtonSize(WPushButton* button)
{
    button->setMinimumSize(100, 50);
    button->setMaximumSize(100, 50);
}

std::tuple<WDialog*, WGridLayout*, WLineEdit*, WSpinBox*>WidgetHelper::CreateNamePeriodSettingsDialog(WContainerWidget* parent, const WString& name, float period)
{
    auto dialog = parent->addChild(std::make_unique<WDialog>("Настройки"));
    auto layout = dialog->contents()->setLayout(std::make_unique<WGridLayout>());
    dialog->setMinimumSize(400, 150);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->rejectWhenEscapePressed(true);
    //name
    layout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    auto validator = std::make_shared<WRegExpValidator>("[\\w\\s\u0401\u0451\u0400-\u04ff]{0,50}");
    validator->setMandatory(true);
    auto nameEdit = layout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    nameEdit->setValidator(validator);
    nameEdit->setText(name);
    nameEdit->setFocus();
    //period
    layout->addWidget(std::make_unique<WText>("Период обновления (сек):"), 1, 0);
    auto periodEdit = layout->addWidget(std::make_unique<WSpinBox>(), 1, 1);
    periodEdit->setMinimum(1);
    periodEdit->setMaximum(600);
    periodEdit->setValue(period / 1000);
    //ok button
    auto ok = dialog->footer()->addWidget(std::make_unique<WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(dialog, &WDialog::accept); 
    auto okValidation = [=]()
    {
        ok->setDisabled(nameEdit->validate() != Wt::ValidationState::Valid ||
                        periodEdit->validate() != Wt::ValidationState::Valid);
    };
    nameEdit->keyWentUp().connect(okValidation);
    periodEdit->valueChanged().connect(okValidation);
    periodEdit->keyWentUp().connect(okValidation);
    okValidation();
    return std::make_tuple(dialog, layout, nameEdit, periodEdit);
}
