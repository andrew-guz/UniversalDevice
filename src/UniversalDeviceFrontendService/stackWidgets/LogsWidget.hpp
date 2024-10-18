#pragma once

#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTextArea.h>

#include "BaseStackWidget.hpp"
#include "LogInformation.hpp"
#include "Settings.hpp"

class LogsWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    LogsWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~LogsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    void CleanupLogs();

    std::vector<LogInformation> GetLogs();

    void OnLogSelected();

private:
    Wt::WComboBox* _logFiles;
    Wt::WTextArea* _logContent;
    std::vector<LogInformation> _logs;
};
