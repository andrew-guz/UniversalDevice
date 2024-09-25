#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "Message.hpp"

class IProcessor {
public:
    IProcessor() = default;

    virtual ~IProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) = 0;
};

typedef std::shared_ptr<IProcessor> Processor;
typedef std::vector<Processor> Processors;
