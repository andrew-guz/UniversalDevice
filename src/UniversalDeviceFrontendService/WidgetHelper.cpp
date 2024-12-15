#include "WidgetHelper.hpp"

#include <filesystem>

#include <Wt/WApplication.h>
#include <Wt/WFileUpload.h>
#include <Wt/WGlobal.h>
#include <Wt/WLineEdit.h>
#include <Wt/WRegExpValidator.h>
#include <Wt/WSpinBox.h>
#include <Wt/WString.h>
#include <Wt/WText.h>
#include <Wt/WTimer.h>

using namespace Wt;

std::string WidgetHelper::TextWithFontSize(const std::string& txt, int size) {
    return fmt::format("<div style='font-size:{}px'>{}</div>", size, txt);
}

void WidgetHelper::SetUsualButtonSize(WPushButton* button) {
    button->setMinimumSize(100, 50);
    button->setMaximumSize(100, 50);
}

std::tuple<WDialog*, WGridLayout*, WLineEdit*, Wt::WLineEdit*, WSpinBox*, Wt::WPushButton*>
WidgetHelper::CreateBaseSettingsDialog(WContainerWidget* parent,
                                       int height,
                                       const WString& name,
                                       const Wt::WString& group,
                                       float period,
                                       bool useDefaultValidation,
                                       std::function<void(std::filesystem::path)> uploadFirmwareFunction,
                                       std::function<void(void)> restartFunction) {
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
    auto groupValidator = std::make_shared<WRegExpValidator>("[\\w\\s\u0401\u0451\u0400-\u04ff/]{0,50}");
    groupEdit->setValidator(groupValidator);
    groupEdit->setText(group);
    // period
    layout->addWidget(std::make_unique<WText>("Период обновления (сек):"), 2, 0);
    auto periodEdit = layout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    periodEdit->setMinimum(1);
    periodEdit->setMaximum(600);
    periodEdit->setValue(period / 1000);
    // upload firmware button
    dialog->footer()->addWidget(std::make_unique<WText>("Загрузка прошивки:"));
    auto firmwarePath = dialog->footer()->addWidget(std::make_unique<WFileUpload>());
    auto firmwareButton = dialog->footer()->addWidget(std::make_unique<WPushButton>("Прошить"));
    firmwareButton->disable();
    firmwareButton->setDefault(false);
    firmwarePath->changed().connect(firmwarePath, &WFileUpload::upload);
    firmwarePath->uploaded().connect([firmwareButton]() { firmwareButton->enable(); });
    firmwareButton->clicked().connect([firmwarePath, uploadFirmwareFunction, firmwareButton]() {
        const std::filesystem::path filePath = firmwarePath->spoolFileName();
        uploadFirmwareFunction(filePath);
        firmwareButton->disable();
    });
    // separator
    dialog->footer()->addWidget(std::make_unique<WText>("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"));
    // restart button
    auto restart = dialog->footer()->addWidget(std::make_unique<WPushButton>("Перезагрузить"));
    restart->setDefault(false);
    restart->clicked().connect(restartFunction);
    // separator
    dialog->footer()->addWidget(std::make_unique<WText>("|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|"));
    // ok button
    auto ok = dialog->footer()->addWidget(std::make_unique<WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(dialog, &WDialog::accept);
    if (useDefaultValidation) {
        // validation
        auto okValidation = [ok, nameEdit, periodEdit]() {
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

void WidgetHelper::ShowSimpleMessage(Wt::WWidget* parent, const std::string& header, const std::string& message, int timeout) {
    auto dialog = parent->addChild(std::make_unique<WDialog>(header));
    auto layout = dialog->contents()->setLayout(std::make_unique<WGridLayout>());
    dialog->setMinimumSize(200, 100);
    dialog->setClosable(timeout == 0);
    dialog->setResizable(false);
    if (timeout == 0)
        dialog->rejectWhenEscapePressed(true);
    dialog->enterPressed().connect([&dialog]() { dialog->accept(); });
    layout->addWidget(std::make_unique<WText>(message), 0, 0, AlignmentFlag::Center);
    if (timeout != 0) {
        auto timer = parent->addChild(std::make_unique<Wt::WTimer>());
        timer->setInterval(std::chrono::seconds(2));
        timer->timeout().connect([&]() {
            dialog->accept();
            parent->removeChild(timer);
        });
        timer->start();
    }
    dialog->exec();
}
