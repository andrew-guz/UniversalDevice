#include "EventsTableModel.hpp"

#include "Constants.hpp"
#include "Event.hpp"
#include "IJson.hpp"
#include "JsonExtension.hpp"
#include "RelayEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermostatEvent.hpp"
#include "TimerEvent.hpp"

#include <iomanip>
#include <string>

using namespace Wt;

namespace {
    enum Columns {
        Name,
        Type,
        Activity,
        Additional,
        Count,
    };
}

EventsTableModel::EventsTableModel(const std::vector<nlohmann::json>& data) : _data(data) {}

void EventsTableModel::UpdateData(const std::vector<nlohmann::json>& data) {
    _data = data;
    dataChanged().emit(index(0, Columns::Name), index(_data.size() - 1, Columns::Additional));
}

int EventsTableModel::columnCount(const WModelIndex& parent) const {
    if (!parent.isValid())
        return Columns::Count;
    return 0;
}

int EventsTableModel::rowCount(const WModelIndex& parent) const {
    if (!parent.isValid())
        return _data.size();
    return 0;
}

WModelIndex EventsTableModel::index(int row, int column, const WModelIndex& parent) const {
    if (row >= 0 && (size_t)row < _data.size() && column >= 0 && column < Columns::Count && !parent.isValid())
        return createIndex(row, column, nullptr);
    return WModelIndex();
}

WModelIndex EventsTableModel::parent(const WModelIndex& index) const { return WModelIndex(); }

cpp17::any EventsTableModel::data(const WModelIndex& index, ItemDataRole role) const {
    if (index.isValid() && index.row() >= 0 && (size_t)index.row() < _data.size() && index.column() >= 0 && index.column() < Columns::Count) {
        auto event = JsonExtension::CreateFromJson<Event>(_data[index.row()]);
        if (role == ItemDataRole::Display) {
            switch (index.column()) {
                case Columns::Name:
                    return event._name;
                    break;
                case Columns::Type:
                    return EventTypeDisplayName(event._type);
                    break;
                case Columns::Activity:
                    return event._active ? "Активно" : "Неактивно";
                    break;
                case Columns::Additional:
                    return EventAdditionalInfo(event._type, _data[index.row()]);
                    break;
            }
        }
        if (role == ItemDataRole::User)
            return _data[index.row()];
    }
    return {};
}

cpp17::any EventsTableModel::headerData(int section, Orientation orientation, ItemDataRole role) const {
    if (section >= 0 && section < Columns::Count && orientation == Orientation::Horizontal && role == ItemDataRole::Display) {
        switch (section) {
            case Columns::Name:
                return "Имя";
                break;
            case Columns::Type:
                return "Тип";
                break;
            case Columns::Activity:
                return "Активно";
                break;
            case Columns::Additional:
                return "Доп. информация";
                break;
        }
    }
    return {};
}

std::string EventsTableModel::EventTypeDisplayName(const std::string_view eventType) {
    if (eventType == Constants::EventTypeTimer)
        return "Таймер";
    else if (eventType == Constants::EventTypeThermometer)
        return "Термометр";
    else if (eventType == Constants::EventTypeRelay)
        return "Рэле";
    else if (eventType == Constants::EventTypeThermostat)
        return "Термостат";
    return {};
}

std::string EventsTableModel::EventAdditionalInfo(const std::string_view eventType, const nlohmann::json& eventJson) {
    std::ostringstream sstream;
    if (eventType == Constants::EventTypeTimer) {
        const auto timerEvent = JsonExtension::CreateFromJson<TimerEvent>(eventJson);
        sstream << "Сработает в " << std::setw(2) << std::setfill('0') << timerEvent._hour << ":" << std::setw(2) << std::setfill('0')
                << timerEvent._minute;
        return sstream.str();
    } else if (eventType == Constants::EventTypeThermometer) {
        const auto thermometerEvent = JsonExtension::CreateFromJson<ThermometerEvent>(eventJson);
        sstream << "Сработает " << (thermometerEvent._lower ? std::string{"ниже "} : std::string{"выше "}) << std::fixed << std::setprecision(1)
                << thermometerEvent._temperature << "°C";
    } else if (eventType == Constants::EventTypeRelay) {
        const auto relayEvent = JsonExtension::CreateFromJson<RelayEvent>(eventJson);
        sstream << "Срабатывает при " << (relayEvent._state ? std::string{"включенном"} : std::string{"выключенном"}) << " реле";
    } else if (eventType == Constants::EventTypeThermostat) {
        const auto thermostatEvent = JsonExtension::CreateFromJson<ThermostatEvent>(eventJson);
        sstream << "Поддерживает температуру " << thermostatEvent._temperature << "°C";
    }
    return sstream.str();
}
