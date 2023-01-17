#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "Constants.h"
#include "MessageCreator.h"
#include "PostRequestHelper.h"

using namespace std::literals;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "port is missing..." << std::endl;
        return -1;
    }
    int port = atoi(argv[1]);

    std::random_device random_device;
    std::mt19937 random_generator(random_device());
    std::uniform_int_distribution<> distribution(1,10);

    auto id = Uuid();
    auto temperature = 20.0f;
    auto delta = 0.1;

    while (true)
    {
        //prepare message with current temperature
        auto message = MessageCreator::Create(Constants::DeviceTypeThermometer, id, Constants::SubjectThermometerCurrentValue, {
            { "value", temperature }
        });
        //send current temperature to server
        PostRequestHelper::DoInformRequest({"127.0.0.1", port, "/api/inform"}, message);
        //generate new value
        auto random_value = distribution(random_generator);
        switch (random_value)
        {
        case 1:
            temperature += delta;
            break;
        case 2:
            temperature -= delta;
            break;
        }
        //sleep
        std::this_thread::sleep_for(std::chrono::duration(1s));
    }
    return 0;
}
