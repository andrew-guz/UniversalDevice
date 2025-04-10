#include "LogsWidget.hpp"

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>

#include "Constants.hpp"
#include "FileUtils.hpp"
#include "LogInformation.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

LogsWidget::LogsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    auto mainLayout = setLayout(std::make_unique<WVBoxLayout>());

    auto buttonsCanvas = mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WHBoxLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto backButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, AlignmentFlag::Left | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto cleanupButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Очистить логи"), 0, AlignmentFlag::Center | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(cleanupButton);
    cleanupButton->clicked().connect([this]() { CleanupLogs(); });

    auto refreshButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, AlignmentFlag::Right | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    _logFiles = mainLayout->addWidget(std::make_unique<WComboBox>(), 0, AlignmentFlag::Top);
    _logFiles->setMinimumSize(_logFiles->minimumWidth(), 50);
    _logFiles->setMaximumSize(_logFiles->maximumWidth(), 50);
    _logFiles->changed().connect([this]() { OnLogSelected(); });

    _logContent = mainLayout->addWidget(std::make_unique<WTextArea>(), 1);
    _logContent->setReadOnly(true);

    Refresh();
}

void LogsWidget::Initialize(const std::string& data) { Refresh(); }

void LogsWidget::Clear() {
    _logFiles->clear();
    _logContent->setText({});
    _logs.clear();
}

void LogsWidget::Refresh() {
    Clear();
    _logs = GetLogs();
    if (_logs.size()) {
        for (auto& log : _logs)
            _logFiles->addItem(log._fileName);
        _logFiles->setCurrentIndex(0);
        OnLogSelected();
    }
}

void LogsWidget::CleanupLogs() {
    const int response = RequestHelper::DoDeleteRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_LOGS }, Constants::LoginService);
    if (response != 200)
        LOG_ERROR_MSG("Failed to cleanup backend logs");
    else
        Logger::Cleanup();
    Refresh();
}

std::vector<LogInformation> LogsWidget::GetLogs() {
    std::vector<LogInformation> result;
    auto replyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_LOGS }, Constants::LoginService);
    if (!replyJson.is_null())
        result.push_back(replyJson.get<LogInformation>());
    result.push_back(ReadApplicationLogFile());
    return result;
}

void LogsWidget::OnLogSelected() {
    size_t index = _logFiles->currentIndex();
    if (index < 0 && index >= _logs.size())
        _logContent->setText({});
    else
        _logContent->setText(_logs[index]._fileContent);
}
