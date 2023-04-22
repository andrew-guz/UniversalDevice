#include "ProcessorsFactory.h"

#include "Logger.h"
#include "Constants.h"
#include "DeviceRegistrationProcessor.h"
#include "EventsProcessor.h"
#include "ThermometerProcessor.h"
#include "RelayProcessor.h"
#include "MotionRelayProcessor.h"
#include "WebsocketProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor)
{
    auto& messageHeader = message._header;

    Processors processors;

    if (messageHeader._subject == Constants::SubjectTimerEvent) //timer event - check events
    {
        //process events due to timer
        processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));
    }
    else if (messageHeader._subject == Constants::SubjectGetDeviceInformation)
    {
        //we need information about device - here we should call all device processors maybe some one will return data
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));
        processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor)));
        processors.push_back(std::shared_ptr<IProcessor>(new MotionRelayProcessor(queryExecutor)));
    }
    else if (messageHeader._subject == Constants::SubjectThermometerCurrentValue) //concrete message to register new data from thermometer
    {
        //register thermometer if needed
        processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
        //since this is thermometer - add ThermometerProcessor
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));            
        //process events due to thermometer state
        processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));            
    }
    else if (messageHeader._subject == Constants::SubjectRelayCurrentState) //concrete message to register new data from relay
    {
        //register relay if needed
        processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
        //since this is relay - add RelayProcessor
        processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor)));
        //process events due to relay state
        processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));            
    }
    else if (messageHeader._subject == Constants::SubjectMotionRelayCurrentState) //concrete message to register new data from motion relay
    {
        //register motion relay if needed
        processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
        //since this is motion relay - add MotionRelayProcessor
        processors.push_back(std::shared_ptr<IProcessor>(new MotionRelayProcessor(queryExecutor)));
        //process events due to motion relay state
        processors.push_back(std::shared_ptr<IProcessor>(new EventsProcessor(queryExecutor)));            
    }
    else if (messageHeader._subject == Constants::SubjectWebSocketGetSettings ||
        messageHeader._subject == Constants::SubjectWebSocketGetCommands)
    {
        //answer on get settings or command from websocket request
        processors.push_back(std::shared_ptr<IProcessor>(new WebSocketProcessor(queryExecutor)));
    }

    return processors;
}
