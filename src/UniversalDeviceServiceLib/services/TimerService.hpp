#pragma once

#include "BaseService.hpp"

class TimerService final : public BaseService {
public:
    TimerService();

    virtual ~TimerService() = default;

private:
    void TimerFunction();

private:
    friend class ServiceExtension;
};
