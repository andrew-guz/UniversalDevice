#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "Parameters.h"
#include "Constants.h"
#include "Defines.h"
#include "ThermometerCurrentValue.h"
#include "MessageHelper.h"
#include "RequestHelper.h"

using namespace std::literals;

int main(int argc, char* argv[])
{
    auto parameters = Parameters::ReadFromFile("ThermometerSimulator.json");

    std::random_device random_device;
    std::mt19937 random_generator(random_device());
    std::uniform_int_distribution<> distribution(1,10);

    auto temperature = parameters._startTemperature;

    while (true)
    {
        //prepare message with current temperature
        ThermometerCurrentValue currentValue;
        currentValue._value = temperature;
        auto message = MessageHelper::Create(Constants::DeviceTypeThermometer, parameters._id, Constants::DeviceServiceType, Uuid::Empty(), Constants::SubjectThermometerCurrentValue, currentValue.ToJson());
        //send current temperature to server
        RequestHelper::DoPostRequestWithNoAnswer({"127.0.0.1", parameters._port, API_DEVICE_INFORM}, message.ToJson());
        //generate new value
        auto random_value = distribution(random_generator);
        switch (random_value)
        {
        case 1:
            temperature += parameters._delta;
            break;
        case 2:
            temperature -= parameters._delta;
            break;
        }
        //sleep
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::seconds(parameters._sleepTimeout)));
    }
    return 0;
}
