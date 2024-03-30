#ifndef _LOGS_WIDGET_H_
#define _LOGS_WIDGET_H_

#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTextArea.h>

#include "BaseStackWidget.h"
#include "LogInformation.h"
#include "Settings.h"

class LogsWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    LogsWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~LogsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    std::vector<LogInformation> GetLogs();

    void OnLogSelected();

private:
    Wt::WComboBox* _logFiles;
    Wt::WTextArea* _logContent;
    std::vector<LogInformation> _logs;
};

#endif //_LOGS_WIDGET_H_
