#include "TemperatureChartModel.hpp"

#include <Wt/WDate.h>
#include <Wt/WDateTime.h>
#include <Wt/WLocalDateTime.h>
#include <Wt/WTime.h>

using namespace Wt;

TemperatureChartModel::TemperatureChartModel(const std::vector<ExtendedThermometerCurrentValue>& data) : _data(data) {}

void TemperatureChartModel::UpdateData(const std::vector<ExtendedThermometerCurrentValue>& data) {
    _data = data;
    dataChanged().emit(index(0, 0), index(_data.size() - 1, 1));
}

int TemperatureChartModel::columnCount(const WModelIndex& parent) const {
    if (!parent.isValid())
        return 2;
    return 0;
}

int TemperatureChartModel::rowCount(const WModelIndex& parent) const {
    if (!parent.isValid())
        return _data.size();
    return 0;
}

WModelIndex TemperatureChartModel::index(int row, int column, const WModelIndex& parent) const {
    if (row >= 0 && (size_t)row < _data.size() && column >= 0 && column < 2 && !parent.isValid())
        return createIndex(row, column, nullptr);
    return WModelIndex();
}

WModelIndex TemperatureChartModel::parent(const WModelIndex& index) const { return WModelIndex(); }

cpp17::any TemperatureChartModel::data(const WModelIndex& index, ItemDataRole role) const {
    if (index.isValid() && index.row() >= 0 && (size_t)index.row() < _data.size() && index.column() >= 0 && index.column() < 2 &&
        role == ItemDataRole::Display) {
        if (index.column() == 0) {
            auto timestamp = _data[index.row()]._timestamp;
            auto time_t = std::chrono::system_clock::to_time_t(timestamp);
            auto tm = std::localtime(&time_t);
            auto updatedTime_t = timegm(tm);
            return WDateTime::fromTime_t(updatedTime_t);
        }
        if (index.column() == 1)
            return _data[index.row()]._value;
    }
    return {};
}
