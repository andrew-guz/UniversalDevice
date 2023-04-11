#include "LogsWidget.h"

#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>

#include "JsonExtension.h"
#include "WidgetHelper.h"
#include "RequestHelper.h"

using namespace Wt;

LogsWidget::LogsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    auto mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Devices, {});
    });

    auto refreshButton = mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([&](){
        Refresh();
    });

    _logFiles = mainLayout->addWidget(std::make_unique<WComboBox>(), 1, 0, 1, 2);
    _logFiles->setMinimumSize(_logFiles->minimumWidth(), 50);
    _logFiles->setMaximumSize(_logFiles->maximumWidth(), 50);
    _logFiles->changed().connect([&](){
        OnLogSelected();
    });

    _logContent = mainLayout->addWidget(std::make_unique<WTextArea>(), 2, 0, 1, 2);
    _logContent->setReadOnly(true);    

    Refresh();
}

void LogsWidget::Initialize(const std::string& data)
{
    Refresh();
}

void LogsWidget::Clear()
{
    _logFiles->clear();
    _logContent->setText({});
    _logs.clear();
}

void LogsWidget::Refresh()
{
    Clear();
    _logs = GetLogs();
    if (_logs.size())
    {
        for(auto& log : _logs)
            _logFiles->addItem(log._fileName);
        _logFiles->setCurrentIndex(0);
        OnLogSelected();
    }
}

std::vector<LogInformation> LogsWidget::GetLogs()
{
    auto replyJson =  RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_LOGS }, Constants::LoginService);
    return JsonExtension::CreateVectorFromJson<LogInformation>(replyJson);
}

void LogsWidget::OnLogSelected()
{
    size_t index = _logFiles->currentIndex();
    if (index < 0 &&
        index >= _logs.size())
        _logContent->setText({});
    else
        _logContent->setText(_logs[index]._fileContent);
}
