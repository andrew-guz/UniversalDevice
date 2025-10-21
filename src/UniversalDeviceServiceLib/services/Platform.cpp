#include "Platform.hpp"

#include <memory>
#include <stdexcept>

#include <crow.h>

#include "DeviceRegistrationProcessor.hpp"
#include "Enums.hpp"
#include "EventsProcessor.hpp"
#include "IQueryExecutor.hpp"
#include "Middleware.hpp"
#include "MotionRelayProcessor.hpp"
#include "RelayProcessor.hpp"
#include "ThermometerProcessor.hpp"
#include "TimeProcessor.hpp"
#include "UniversalDeviceProcessor.hpp"
#include "WebsocketProcessor.hpp"

std::shared_ptr<Platform> Platform::s_instance = nullptr;
std::once_flag Platform::s_instanceFlag;

Platform::Platform(CrowApp& app, IQueryExecutor* queryExecutor) :
    _settingsController(queryExecutor),
    _commandsController(queryExecutor),
    _devicesController(queryExecutor, _settingsController, _commandsController),
    _thermometerValuesController(queryExecutor, _devicesController),
    _relayValuesController(queryExecutor),
    _settingsService(app, _settingsController),
    _commandsService(app, _commandsController),
    _deviceService(queryExecutor, _devicesController) //
{
    _deviceService.Initialize(app);
}

void Platform::Init(CrowApp& app, IQueryExecutor* queryExecutor) {
    std::call_once(
        Platform::s_instanceFlag,
        [](CrowApp& app, IQueryExecutor* queryExecutor) -> void {
            if (Platform::s_instance != nullptr)
                throw std::runtime_error("Platform is already initialized");
            Platform::s_instance = std::shared_ptr<Platform>(new Platform{ app, queryExecutor });
        },
        app,
        queryExecutor);
}

Processors Platform::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor) {
    Processors processors;

    switch (message._header._subject) {
        case Subject::Undefined:
            break;
        case Subject::TimerEvent:
            // process events due to timer
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor, s_instance->_commandsController)));
            // send time to devices
            processors.push_back(std::shared_ptr<IProcessor>(new TimeProcessor(queryExecutor)));
            break;
        case Subject::GetDeviceInformation:
            // we need information about device - here we should call all device processors maybe some one will return data
            processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor, s_instance->_thermometerValuesController)));
            processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor, s_instance->_relayValuesController)));
            processors.push_back(std::shared_ptr<IProcessor>(new MotionRelayProcessor(queryExecutor)));
            processors.push_back(std::shared_ptr<IProcessor>(new UniversalDeviceProcessor(queryExecutor)));
            break;
        case Subject::ThermometerCurrentValue:
            // register thermometer if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor, s_instance->_devicesController)));
            // since this is thermometer - add ThermometerProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor, s_instance->_thermometerValuesController)));
            // process events due to thermometer state
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor, s_instance->_commandsController)));
            break;
        case Subject::RelayCurrentState:
            // register relay if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor, s_instance->_devicesController)));
            // since this is relay - add RelayProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor, s_instance->_relayValuesController)));
            // process events due to relay state
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor, s_instance->_commandsController)));
            break;
        case Subject::MotionRelayCurrentState:
            // register motion relay if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor, s_instance->_devicesController)));
            // since this is motion relay - add MotionRelayProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new MotionRelayProcessor(queryExecutor)));
            // process events due to motion relay state
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor, s_instance->_commandsController)));
            break;
        case Subject::WebSocketAuthorization:
            break;
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
            // answer on get settings or command from websocket request
            processors.push_back(
                std::shared_ptr<IProcessor>(new WebSocketProcessor(queryExecutor, s_instance->_settingsController, s_instance->_commandsController)));
            break;
        case Subject::UniversalDeviceCurrentState:
            // register universal device if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor, s_instance->_devicesController)));
            // since this is universal device - add UniversalDeviceProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new UniversalDeviceProcessor(queryExecutor)));
            break;
        case Subject::SunriseEvent:
            // process events due to timer
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor, s_instance->_commandsController)));
            break;
        case Subject::SunsetEvent:
            // process events due to timer
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor, s_instance->_commandsController)));
            break;
    }

    return processors;
}
