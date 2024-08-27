#include "EventsTableModel.hpp"

#include <string>

#include <fmt/format.h>

#include "Enums.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermostatEvent.hpp"
#include "TimerEvent.hpp"

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

int EventsTableModel::columnCount(const Wt::WModelIndex& parent) const {
    if (!parent.isValid())
        return Columns::Count;
    return 0;
}

int EventsTableModel::rowCount(const Wt::WModelIndex& parent) const {
    if (!parent.isValid())
        return _data.size();
    return 0;
}

Wt::WModelIndex EventsTableModel::index(int row, int column, const Wt::WModelIndex& parent) const {
    if (row >= 0 && (size_t)row < _data.size() && column >= 0 && column < Columns::Count && !parent.isValid())
        return createIndex(row, column, nullptr);
    return Wt::WModelIndex();
}

Wt::WModelIndex EventsTableModel::parent(const Wt::WModelIndex& index) const { return Wt::WModelIndex(); }

Wt::cpp17::any EventsTableModel::data(const Wt::WModelIndex& index, Wt::ItemDataRole role) const {
    if (index.isValid() && index.row() >= 0 && (size_t)index.row() < _data.size() && index.column() >= 0 && index.column() < Columns::Count) {
        auto event = _data[index.row()].get<Event>();
        if (role == Wt::ItemDataRole::Display) {
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
        if (role == Wt::ItemDataRole::User)
            return _data[index.row()];
    }
    return {};
}

Wt::cpp17::any EventsTableModel::headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const {
    if (section >= 0 && section < Columns::Count && orientation == Wt::Orientation::Horizontal && role == Wt::ItemDataRole::Display) {
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

std::string EventsTableModel::EventTypeDisplayName(const EventType eventType) {
    switch (eventType) {
        case EventType::Undefined:
            LOG_ERROR_MSG("Invalid event type");
            break;
        case EventType::Timer:
            return "Таймер";
        case EventType::Thermometer:
            return "Термометр";
        case EventType::Relay:
            return "Рэле";
        case EventType::Thermostat:
            return "Термостат";
            break;
    }
    return {};
}

std::string EventsTableModel::EventAdditionalInfo(const EventType eventType, const nlohmann::json& eventJson) {
    switch (eventType) {
        case EventType::Undefined:
            LOG_ERROR_MSG("Invalid event type");
            return {};
        case EventType::Timer: {
            const auto timerEvent = eventJson.get<TimerEvent>();
            return fmt::format("Сработает в {:02}:{:02}", timerEvent._hour, timerEvent._minute);
        };
        case EventType::Thermometer: {
            const auto thermometerEvent = eventJson.get<ThermometerEvent>();
            return fmt::format("Сработает {} {:.1f} °C", (thermometerEvent._lower ? "ниже " : "выше "), thermometerEvent._temperature);
        };
        case EventType::Relay: {
            const auto relayEvent = eventJson.get<RelayEvent>();
            return fmt::format("Срабатывает при {} реле", (relayEvent._state ? "включенном" : "выключенном"));
        };
        case EventType::Thermostat: {
            const auto thermostatEvent = eventJson.get<ThermostatEvent>();
            return fmt::format("Поддерживает температуру {:.1f}±{:.1f} °C", thermostatEvent._temperature, thermostatEvent._delta);
        };
    }
    return {};
}
