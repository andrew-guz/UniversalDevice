#include <iostream>
#include <thread>
#include <chrono>

#include "Constants.h"
#include "Uuid.h"
#include "Message.h"

using namespace std::literals;

int main()
{
    auto id = Uuid();
    auto temperature = 20.0f;
    auto delta = 0.1;
    while (true)
    {
        //send current temperature to server
        Message message;
        message._header._from_type = Constants::device_type_thermometer;
        message._header._from_id = id;
        message._header._subject = Constants::subject_thermometer_current_value;
        message._data = {
            { "value", temperature }
        };

        std::this_thread::sleep_for(std::chrono::duration(1s));
    }
    return 0;
}
