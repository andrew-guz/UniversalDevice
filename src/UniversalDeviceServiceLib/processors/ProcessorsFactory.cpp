#include "ProcessorsFactory.hpp"

#include "Constants.hpp"
#include "DeviceRegistrationProcessor.hpp"
#include "EventsProcessor.hpp"
#include "Logger.hpp"
#include "MotionRelayProcessor.hpp"
#include "RelayProcessor.hpp"
#include "ThermometerProcessor.hpp"
#include "WebsocketProcessor.hpp"

Processors ProcessorsFactory::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor) {
    Processors processors;

    switch (message._header._subject) {
        case Subject::Undefined:
            break;
        case Subject::TimerEvent:
            // process events due to timer
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));
            break;
        case Subject::GetDeviceInformation:
            // we need information about device - here we should call all device processors maybe some one will return data
            processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));
            processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor)));
            processors.push_back(std::shared_ptr<IProcessor>(new MotionRelayProcessor(queryExecutor)));
            break;
        case Subject::ThermometerCurrentValue:
            // register thermometer if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
            // since this is thermometer - add ThermometerProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));
            // process events due to thermometer state
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));
            break;
        case Subject::RelayCurrentState:
            // register relay if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
            // since this is relay - add RelayProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor)));
            // process events due to relay state
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));
            break;
        case Subject::MotionRelayCurrentState:
            // register motion relay if needed
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
            // since this is motion relay - add MotionRelayProcessor
            processors.push_back(std::shared_ptr<IProcessor>(new MotionRelayProcessor(queryExecutor)));
            // process events due to motion relay state
            processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));
            break;
        case Subject::WebSocketAuthorization:
            break;
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
            // answer on get settings or command from websocket request
            processors.push_back(std::shared_ptr<IProcessor>(new WebSocketProcessor(queryExecutor)));
            break;
    }

    return processors;
}
