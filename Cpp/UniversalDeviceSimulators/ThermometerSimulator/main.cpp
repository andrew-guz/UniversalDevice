#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "Parameters.h"
#include "Constants.h"
#include "MessageCreator.h"
#include "PostRequestHelper.h"

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
        auto message = MessageCreator::Create(Constants::DeviceTypeThermometer, parameters._id, Constants::SubjectThermometerCurrentValue, {
            { "value", temperature }
        });
        //send current temperature to server
        PostRequestHelper::DoInformRequest({"127.0.0.1", parameters._port, "/api/inform"}, message);
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
