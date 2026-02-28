#pragma once

#include <string>
#include <utility>
#include <vector>

#include <Wt/cpp17/any.hpp>

#include "BaseChartModel.hpp"
#include "Enums.hpp"
#include "UniversalData.hpp"
#include "UniversalValue.hpp"

class UniversalDeviceChartModel : public BaseChartModel<UniversalValue> {
public:
    UniversalDeviceChartModel(const std::vector<UniversalValue>& data, std::string parameterName) :
        BaseChartModel<UniversalValue>(data),
        _parameterName(std::move(parameterName)) {}

    virtual ~UniversalDeviceChartModel() = default;

    void SetData(const std::vector<UniversalValue>& data) { UpdateData(data); }

protected:
    virtual const Wt::cpp17::any GetValue(const int index) const override {
        const UniversalData& value = this->_data[index]._values.at(_parameterName);
        switch (value.GetType()) {
            case UniversalDataType::Bool:
                return value.Get<bool>();
            case UniversalDataType::Int:
                return value.Get<int>();
            case UniversalDataType::Double:
                return value.Get<double>();
            case UniversalDataType::String:
                return value.Get<std::string>();
        }

        return {};
    }

protected:
    std::string _parameterName;
};
