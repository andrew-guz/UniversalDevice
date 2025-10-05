#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"
#include "DevicesController.hpp"

class ThermometerProcessor final : public BaseProcessorWithQueryExecutor {
public:
    ThermometerProcessor(IQueryExecutor* queryExecutor, DevicesController& devicesController);

    virtual ~ThermometerProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessThermometerCurrentValueMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

private:
    DevicesController& _devicesController;
};
