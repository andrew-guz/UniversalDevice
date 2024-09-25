#pragma once

#include <vector>

#include <Wt/WAbstractItemModel.h>
#include <Wt/WModelIndex.h>
#include <Wt/cpp17/any.hpp>

#include "ExtendedThermometerCurrentValue.hpp"

class TemperatureChartModel : public Wt::WAbstractItemModel {
public:
    TemperatureChartModel(const std::vector<ExtendedThermometerCurrentValue>& data = {});

    void UpdateData(const std::vector<ExtendedThermometerCurrentValue>& data);

    virtual int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual int rowCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual Wt::WModelIndex parent(const Wt::WModelIndex& index) const override;

    virtual Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override;

private:
    std::vector<ExtendedThermometerCurrentValue> _data;
};
