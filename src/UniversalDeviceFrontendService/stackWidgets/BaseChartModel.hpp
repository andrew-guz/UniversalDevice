#pragma once

#include <chrono>
#include <vector>

#include <Wt/WAbstractItemModel.h>
#include <Wt/WDateTime.h>
#include <Wt/WModelIndex.h>
#include <Wt/cpp17/any.hpp>

template<typename T>
class BaseChartModel : public Wt::WAbstractItemModel {
public:
    BaseChartModel(const std::vector<T>& data = {}) :
        _data(data) {}

    void UpdateData(const std::vector<T>& data) {
        _data = data;
        dataChanged().emit(index(0, 0), index(_data.size() - 1, 1));
    }

    virtual int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override {
        if (!parent.isValid())
            return 2;
        return 0;
    }

    virtual int rowCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override {
        if (!parent.isValid())
            return _data.size();
        return 0;
    }

    virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex& parent = Wt::WModelIndex()) const override {
        if (row >= 0 && (size_t)row < _data.size() && column >= 0 && column < 2 && !parent.isValid())
            return createIndex(row, column, nullptr);
        return Wt::WModelIndex();
    }

    virtual Wt::WModelIndex parent(const Wt::WModelIndex& index) const override { return Wt::WModelIndex(); }

    virtual Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override {
        if (index.isValid() && index.row() >= 0 && (size_t)index.row() < _data.size() && index.column() >= 0 && index.column() < 2 &&
            role == Wt::ItemDataRole::Display) {
            if (index.column() == 0) {
                auto timestamp = _data[index.row()]._timestamp;
                auto time_t = std::chrono::system_clock::to_time_t(timestamp);
                auto tm = std::localtime(&time_t);
                auto updatedTime_t = timegm(tm);
                return Wt::WDateTime::fromTime_t(updatedTime_t);
            }
            if (index.column() == 1)
                return _data[index.row()]._value;
        }
        return {};
    }

private:
    std::vector<T> _data;
};
