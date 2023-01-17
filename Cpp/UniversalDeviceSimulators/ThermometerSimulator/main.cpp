#include <iostream>
#include <thread>
#include <chrono>

#include "Constants.h"
#include "MessageCreator.h"

using namespace std::literals;

int main()
{
    auto id = Uuid();
    auto temperature = 20.0f;
    auto delta = 0.1;
    while (true)
    {
        //send current temperature to server
        auto message = MessageCreator::Create(Constants::device_type_thermometer, id, Constants::subject_thermometer_current_value, {
            { "value", temperature }
        });

        std::this_thread::sleep_for(std::chrono::duration(1s));
    }
    return 0;
}
