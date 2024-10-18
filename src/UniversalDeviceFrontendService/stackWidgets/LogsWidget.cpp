#include "LogsWidget.hpp"

#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>

#include "FileUtils.hpp"
#include "LogInformation.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

LogsWidget::LogsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    auto mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto cleanupButton = mainLayout->addWidget(std::make_unique<WPushButton>("Очистить логи"), 0, 1, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(cleanupButton);
    cleanupButton->clicked().connect([this]() { CleanupLogs(); });

    auto refreshButton = mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 2, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    _logFiles = mainLayout->addWidget(std::make_unique<WComboBox>(), 1, 0, 1, 3);
    _logFiles->setMinimumSize(_logFiles->minimumWidth(), 50);
    _logFiles->setMaximumSize(_logFiles->maximumWidth(), 50);
    _logFiles->changed().connect([this]() { OnLogSelected(); });

    _logContent = mainLayout->addWidget(std::make_unique<WTextArea>(), 2, 0, 1, 3);
    _logContent->setReadOnly(true);

    mainLayout->setRowStretch(0, 0);
    mainLayout->setRowStretch(1, 0);
    mainLayout->setRowStretch(2, 1);

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
