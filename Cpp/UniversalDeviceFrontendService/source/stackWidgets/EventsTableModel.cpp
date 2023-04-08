#include "EventsTableModel.h"

#include "IJson.h"
#include "JsonExtension.h"
#include "Event.h"

using namespace Wt;

EventsTableModel::EventsTableModel(const std::vector<nlohmann::json>& data) :
    _data(data)
{
}

void EventsTableModel::UpdateData(const std::vector<nlohmann::json>& data)
{
    _data = data;
    dataChanged().emit(index(0, 0), index(_data.size() - 1, 1));
}

int EventsTableModel::columnCount(const WModelIndex& parent) const
{
    if (!parent.isValid())
        return 2;
    return 0;
}

int EventsTableModel::rowCount(const WModelIndex& parent) const
{
    if (!parent.isValid())
        return _data.size();
    return 0;
}

WModelIndex EventsTableModel::index(int row, int column, const WModelIndex& parent) const
{
    if (row >= 0 &&
        (size_t)row < _data.size() &&
        column >= 0 &&
        column < 2 &&
        !parent.isValid())
        return createIndex(row, column, nullptr);
    return WModelIndex();
}

WModelIndex EventsTableModel::parent(const WModelIndex& index) const
{
    return WModelIndex();
}

cpp17::any EventsTableModel::data(const WModelIndex& index, ItemDataRole role) const
{
    if (index.isValid() &&
        index.row()>= 0 &&
        (size_t)index.row() < _data.size() &&
        index.column() >= 0 &&
        index.column() < 2)
    {
        auto event = JsonExtension::CreateFromJson<Event>(_data[index.row()]);
        if (role == ItemDataRole::Display)
        {            
            if (index.column() == 0)
                return event._name;
            if (index.column() == 1)
                return event._active ? "Активно" : "Неактивно";
        }
        if (role == ItemDataRole::User)
            return _data[index.row()];
    }
    return {};
}

cpp17::any EventsTableModel::headerData(int section, Orientation orientation, ItemDataRole role) const
{
    if (section >= 0 &&
        section < 2 &&
        orientation == Orientation::Horizontal &&
        role == ItemDataRole::Display)
    {
        if (section == 0)
            return "Имя";
        if (section == 1)
            return "Активно";
    }
    return {};
}
