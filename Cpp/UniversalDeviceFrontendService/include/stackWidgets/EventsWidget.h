#ifndef _EVENTS_WIDGET_H_
#define _EVENTS_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WTableView.h>
#include <nlohmann/json.hpp>

#include "BaseStackWidget.h"
#include "Settings.h"
#include "EventsTableModel.h"

class EventsWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    EventsWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~EventsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    std::vector<nlohmann::json> GetEvents();

private:
    Wt::WGridLayout*                    _mainLayout;
    std::vector<nlohmann::json>         _eventJsons;
    std::shared_ptr<EventsTableModel>   _eventsTableModel;
    Wt::WTableView*                     _eventsTable;
};

#endif //_EVENTS_WIDGET_H_
