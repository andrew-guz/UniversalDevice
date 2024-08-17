#include "WidgetHelper.hpp"

#include <Wt/WLineEdit.h>
#include <Wt/WRegExpValidator.h>
#include <Wt/WSpinBox.h>
#include <Wt/WText.h>

using namespace Wt;

std::string WidgetHelper::TextWithFontSize(const std::string& txt, int size) {
    std::stringstream ss;
    ss << "<div style='font-size:" << size << "px'>" << txt << "</div>";
    ss.flush();
    return ss.str();
}

void WidgetHelper::SetUsualButtonSize(WPushButton* button) {
    button->setMinimumSize(100, 50);
    button->setMaximumSize(100, 50);
}

std::tuple<WDialog*, WGridLayout*, WLineEdit*, Wt::WLineEdit*, WSpinBox*, Wt::WPushButton*>
WidgetHelper::CreateBaseSettingsDialog(WContainerWidget* parent, int height, const WString& name, const Wt::WString& group, float period,
                                       bool useDefaultValidation) {
    auto dialog = parent->addChild(std::make_unique<WDialog>("Настройки"));
    auto layout = dialog->contents()->setLayout(std::make_unique<WGridLayout>());
    dialog->setMinimumSize(400, height);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->rejectWhenEscapePressed(true);
    // name
    layout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    auto nameEdit = layout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    auto nameValidator = std::make_shared<WRegExpValidator>("[\\w\\s\u0401\u0451\u0400-\u04ff]{0,50}");
    nameValidator->setMandatory(true);
    nameEdit->setValidator(nameValidator);
    nameEdit->setText(name);
    nameEdit->setFocus();
    // group
    layout->addWidget(std::make_unique<WText>("Группа (разбивается по '/'):"), 1, 0);
    auto groupEdit = layout->addWidget(std::make_unique<WLineEdit>(), 1, 1);
    auto groupValidator = std::make_shared<WRegExpValidator>("[\\w\\s\u0401\u0451\u0400-\u04ff]{0,50}");
    groupEdit->setValidator(groupValidator);
    groupEdit->setText(group);
    // period
    layout->addWidget(std::make_unique<WText>("Период обновления (сек):"), 2, 0);
    auto periodEdit = layout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    periodEdit->setMinimum(1);
    periodEdit->setMaximum(600);
    periodEdit->setValue(period / 1000);
    // ok button
    auto ok = dialog->footer()->addWidget(std::make_unique<WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(dialog, &WDialog::accept);
    if (useDefaultValidation) {
        // validation
        auto okValidation = [&ok, &nameEdit, &periodEdit]() {
            ok->setDisabled(nameEdit->validate() != Wt::ValidationState::Valid || periodEdit->validate() != Wt::ValidationState::Valid);
        };
        nameEdit->keyWentUp().connect(okValidation);
        groupEdit->keyWentUp().connect(okValidation);
        periodEdit->valueChanged().connect(okValidation);
        periodEdit->keyWentUp().connect(okValidation);
        okValidation();
    }
    return std::make_tuple(dialog, layout, nameEdit, groupEdit, periodEdit, ok);
}

void WidgetHelper::ShowSimpleErrorMessage(Wt::WWidget* parent, const std::string& header, const std::string& message) {
    auto dialog = parent->addChild(std::make_unique<WDialog>(header));
    auto layout = dialog->contents()->setLayout(std::make_unique<WGridLayout>());
    dialog->setMinimumSize(200, 100);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->rejectWhenEscapePressed(true);
    dialog->enterPressed().connect([&dialog]() { dialog->accept(); });
    layout->addWidget(std::make_unique<WText>(message), 0, 0, AlignmentFlag::Center);
    dialog->exec();
}
