#include <chrono>
#include <ctime>
#include <filesystem>
#include <optional>
#include <tuple>
#include <variant>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Command.hpp"
#include "CommandsController.hpp"
#include "Constants.hpp"
#include "Device.hpp"
#include "DeviceDescription.hpp"
#include "DevicesController.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventsController.hpp"
#include "EventsProcessor.hpp"
#include "MessageHelper.hpp"
#include "Provider.hpp"
#include "Receiver.hpp"
#include "RelayEvent.hpp"
#include "RelayState.hpp"
#include "RelayValue.hpp"
#include "ScenariosController.hpp"
#include "SettingsController.hpp"
#include "Storage.hpp"
#include "SunriseEvent.hpp"
#include "SunriseSunsetUtils.hpp"
#include "SunsetEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerLedBrightness.hpp"
#include "ThermometerValue.hpp"
#include "ThermostatEvent.hpp"
#include "TimeHelper.hpp"
#include "TimerEvent.hpp"
#include "Uuid.hpp"
#include <bits/chrono.h>

const Uuid thermometerDeviceReceiverId;
const Uuid relayDeviceReceiverId;
const Uuid thermometerDeviceSenderId;
const Uuid relayDeviceSenderId;

void createDevices(DevicesController& deviceController) {
    REQUIRE(deviceController.Add(Device{
                ._id = thermometerDeviceReceiverId,
                ._type = DeviceType::Thermometer,
                ._timestamp = std::chrono::system_clock::now(),
            }) == true);
    REQUIRE(deviceController.Add(Device{
                ._id = relayDeviceReceiverId,
                ._type = DeviceType::Relay,
                ._timestamp = std::chrono::system_clock::now(),
            }) == true);
}

TEST_CASE("TimerEvent") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        EventsProcessor eventsProcessor{ eventsController, commandsController };

        createDevices(devicesController);

        REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == false);
        REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == false);

        int expectedBrightness = 3;
        int expectedState = 1;
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&tt);

        TimerEvent thermometerTimerEvent{
            ._hour = tm.tm_hour,
            ._minute = tm.tm_min,
        };
        thermometerTimerEvent._provider = EventProvider{
            ._id = Constants::PredefinedIdTimer,
        };
        thermometerTimerEvent._receiver = Receiver{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceReceiverId,
        };
        thermometerTimerEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        TimerEvent relayTimerEvent{
            ._hour = tm.tm_hour,
            ._minute = tm.tm_min,
        };
        relayTimerEvent._provider = EventProvider{
            ._id = Constants::PredefinedIdTimer,
        };
        relayTimerEvent._receiver = Receiver{
            ._type = DeviceType::Relay,
            ._id = relayDeviceReceiverId,
        };
        relayTimerEvent._command = RelayState{
            ._state = expectedState,
        };

        REQUIRE(eventsController.Add(thermometerTimerEvent) == true);
        REQUIRE(eventsController.Add(relayTimerEvent) == true);

        eventsProcessor.ProcessMessage(
            now, MessageHelper::CreateEventMessage(Constants::PredefinedIdTimer, Subject::SunriseEvent, nlohmann::json::object_t{}));

        auto checkCommands = [&]() {
            REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == true);
            REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == true);

            const std::optional<Command> thermometerCommand = commandsController.Get(thermometerDeviceReceiverId);
            REQUIRE(std::holds_alternative<ThermometerLedBrightness>(thermometerCommand.value()) == true);
            REQUIRE(std::get<ThermometerLedBrightness>(thermometerCommand.value())._brightness == expectedBrightness);

            const std::optional<Command> relayCommand = commandsController.Get(relayDeviceReceiverId);
            REQUIRE(std::holds_alternative<RelayState>(relayCommand.value()) == true);
            REQUIRE(std::get<RelayState>(relayCommand.value())._state == expectedState);
        };

        checkCommands();

        expectedBrightness = 5;
        thermometerTimerEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };
        expectedState = 0;
        relayTimerEvent._command = RelayState{
            ._state = expectedState,
        };

        Event e1 = thermometerTimerEvent;
        REQUIRE(eventsController.Update(e1).has_value() == true);
        Event e2 = relayTimerEvent;
        REQUIRE(eventsController.Update(e2).has_value() == true);

        eventsProcessor.ProcessMessage(
            now, MessageHelper::CreateEventMessage(Constants::PredefinedIdTimer, Subject::SunriseEvent, nlohmann::json::object_t{}));

        checkCommands();
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("ThermometerEvent") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        EventsProcessor eventsProcessor{ eventsController, commandsController };

        createDevices(devicesController);

        REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == false);
        REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == false);

        int expectedBrightness = 3;
        int expectedState = 1;
        const float temperature = 10.0f;
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        ThermometerEvent thermometerThermometerEvent{
            ._temperature = temperature,
            ._lower = true,
        };
        thermometerThermometerEvent._provider = DeviceDescription{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceSenderId,
        };
        thermometerThermometerEvent._receiver = Receiver{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceReceiverId,
        };
        thermometerThermometerEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        ThermometerEvent relayThermometerEvent{
            ._temperature = temperature,
            ._lower = true,
        };
        relayThermometerEvent._provider = DeviceDescription{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceSenderId,
        };
        relayThermometerEvent._receiver = Receiver{
            ._type = DeviceType::Relay,
            ._id = relayDeviceReceiverId,
        };
        relayThermometerEvent._command = RelayState{
            ._state = expectedState,
        };

        REQUIRE(eventsController.Add(thermometerThermometerEvent) == true);
        REQUIRE(eventsController.Add(relayThermometerEvent) == true);

        eventsProcessor.ProcessMessage(now,
                                       MessageHelper::CreateDeviceMessage(DeviceType::Thermometer,
                                                                          thermometerDeviceSenderId,
                                                                          Subject::ThermometerCurrentValue,
                                                                          ThermometerValue{
                                                                              ._value = temperature - 1.0f,
                                                                              ._timestamp = now,
                                                                          }));

        auto checkCommands = [&]() {
            REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == true);
            REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == true);

            const std::optional<Command> thermometerCommand = commandsController.Get(thermometerDeviceReceiverId);
            REQUIRE(std::holds_alternative<ThermometerLedBrightness>(thermometerCommand.value()) == true);
            REQUIRE(std::get<ThermometerLedBrightness>(thermometerCommand.value())._brightness == expectedBrightness);

            const std::optional<Command> relayCommand = commandsController.Get(relayDeviceReceiverId);
            REQUIRE(std::holds_alternative<RelayState>(relayCommand.value()) == true);
            REQUIRE(std::get<RelayState>(relayCommand.value())._state == expectedState);
        };

        checkCommands();

        expectedBrightness = 5;
        thermometerThermometerEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };
        expectedState = 0;
        relayThermometerEvent._command = RelayState{
            ._state = expectedState,
        };

        Event e1 = thermometerThermometerEvent;
        REQUIRE(eventsController.Update(e1).has_value() == true);
        Event e2 = relayThermometerEvent;
        REQUIRE(eventsController.Update(e2).has_value() == true);

        eventsProcessor.ProcessMessage(now,
                                       MessageHelper::CreateDeviceMessage(DeviceType::Thermometer,
                                                                          thermometerDeviceSenderId,
                                                                          Subject::ThermometerCurrentValue,
                                                                          ThermometerValue{
                                                                              ._value = temperature - 1.0f,
                                                                              ._timestamp = now,
                                                                          }));

        checkCommands();
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("RelayEvent") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        EventsProcessor eventsProcessor{ eventsController, commandsController };

        createDevices(devicesController);

        REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == false);
        REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == false);

        int expectedBrightness = 3;
        int expectedState = 1;
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        RelayEvent thermometerRelayEvent{
            ._state = 1,
        };
        thermometerRelayEvent._provider = DeviceDescription{
            ._type = DeviceType::Relay,
            ._id = relayDeviceSenderId,
        };
        thermometerRelayEvent._receiver = Receiver{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceReceiverId,
        };
        thermometerRelayEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        RelayEvent relayRelayEvent{
            ._state = 1,
        };
        relayRelayEvent._provider = DeviceDescription{
            ._type = DeviceType::Relay,
            ._id = relayDeviceSenderId,
        };
        relayRelayEvent._receiver = Receiver{
            ._type = DeviceType::Relay,
            ._id = relayDeviceReceiverId,
        };
        relayRelayEvent._command = RelayState{
            ._state = expectedState,
        };

        REQUIRE(eventsController.Add(thermometerRelayEvent) == true);
        REQUIRE(eventsController.Add(relayRelayEvent) == true);

        eventsProcessor.ProcessMessage(now,
                                       MessageHelper::CreateDeviceMessage(DeviceType::Relay,
                                                                          relayDeviceSenderId,
                                                                          Subject::RelayCurrentState,
                                                                          RelayValue{
                                                                              ._state = 1,
                                                                              ._timestamp = now,
                                                                          }));

        auto checkCommands = [&]() {
            REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == true);
            REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == true);

            const std::optional<Command> thermometerCommand = commandsController.Get(thermometerDeviceReceiverId);
            REQUIRE(std::holds_alternative<ThermometerLedBrightness>(thermometerCommand.value()) == true);
            REQUIRE(std::get<ThermometerLedBrightness>(thermometerCommand.value())._brightness == expectedBrightness);

            const std::optional<Command> relayCommand = commandsController.Get(relayDeviceReceiverId);
            REQUIRE(std::holds_alternative<RelayState>(relayCommand.value()) == true);
            REQUIRE(std::get<RelayState>(relayCommand.value())._state == expectedState);
        };

        checkCommands();

        expectedBrightness = 5;
        thermometerRelayEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };
        expectedState = 0;
        relayRelayEvent._command = RelayState{
            ._state = expectedState,
        };

        Event e1 = thermometerRelayEvent;
        REQUIRE(eventsController.Update(e1).has_value() == true);
        Event e2 = relayRelayEvent;
        REQUIRE(eventsController.Update(e2).has_value() == true);

        eventsProcessor.ProcessMessage(now,
                                       MessageHelper::CreateDeviceMessage(DeviceType::Relay,
                                                                          relayDeviceSenderId,
                                                                          Subject::RelayCurrentState,
                                                                          RelayValue{
                                                                              ._state = 1,
                                                                              ._timestamp = now,
                                                                          }));

        checkCommands();
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("ThermostatEvent") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        EventsProcessor eventsProcessor{ eventsController, commandsController };

        createDevices(devicesController);

        REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == false);
        REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == false);

        int expectedBrightness = 3;
        int expectedState = 1;
        const float temperature = 10.0f;
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        // This type of event works only for relays

        ThermostatEvent relayThermostatEvent{
            ._temperature = temperature,
        };
        relayThermostatEvent._provider = DeviceDescription{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceSenderId,
        };
        relayThermostatEvent._receiver = Receiver{
            ._type = DeviceType::Relay,
            ._id = relayDeviceReceiverId,
        };

        REQUIRE(eventsController.Add(relayThermostatEvent) == true);

        eventsProcessor.ProcessMessage(now,
                                       MessageHelper::CreateDeviceMessage(DeviceType::Thermometer,
                                                                          thermometerDeviceSenderId,
                                                                          Subject::ThermometerCurrentValue,
                                                                          ThermometerValue{
                                                                              ._value = temperature - 1.0f,
                                                                              ._timestamp = now,
                                                                          }));

        auto checkCommands = [&]() {
            REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == true);

            const std::optional<Command> relayCommand = commandsController.Get(relayDeviceReceiverId);
            REQUIRE(std::holds_alternative<RelayState>(relayCommand.value()) == true);
            REQUIRE(std::get<RelayState>(relayCommand.value())._state == expectedState);
        };

        checkCommands();

        expectedState = 0;

        Event event = relayThermostatEvent;
        REQUIRE(eventsController.Update(event).has_value() == true);

        eventsProcessor.ProcessMessage(now,
                                       MessageHelper::CreateDeviceMessage(DeviceType::Thermometer,
                                                                          thermometerDeviceSenderId,
                                                                          Subject::ThermometerCurrentValue,
                                                                          ThermometerValue{
                                                                              ._value = temperature + 1.0f,
                                                                              ._timestamp = now,
                                                                          }));

        checkCommands();
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("SunriseEvent") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        EventsProcessor eventsProcessor{ eventsController, commandsController };

        createDevices(devicesController);

        REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == false);
        REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == false);

        int expectedBrightness = 3;
        int expectedState = 1;

        SunriseEvent thermometerSunriseEvent{};
        thermometerSunriseEvent._provider = EventProvider{
            ._id = Constants::PredefinedIdSunrise,
        };
        thermometerSunriseEvent._receiver = Receiver{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceReceiverId,
        };
        thermometerSunriseEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        SunriseEvent relaySunriseEvent{};
        relaySunriseEvent._provider = EventProvider{
            ._id = Constants::PredefinedIdSunrise,
        };
        relaySunriseEvent._receiver = Receiver{
            ._type = DeviceType::Relay,
            ._id = relayDeviceReceiverId,
        };
        relaySunriseEvent._command = RelayState{
            ._state = expectedState,
        };

        REQUIRE(eventsController.Add(thermometerSunriseEvent) == true);
        REQUIRE(eventsController.Add(relaySunriseEvent) == true);

        const auto processEvent = [&]() {
            const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            int day;
            int month;
            int year;
            std::tie(day, month, year) = TimeHelper::GetDayMonthYear(now);
            SunriseSunsetTime sunriseTime = getSunriseSunsetTime(day, month, year);
            std::time_t tt = std::chrono::system_clock::to_time_t(now);
            std::tm tm = *std::localtime(&tt);
            tm.tm_hour = sunriseTime.sunriseHour;
            tm.tm_min = sunriseTime.sunriseMinute;
            tm.tm_sec = 0;
            const std::chrono::system_clock::time_point sunriseTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
            eventsProcessor.ProcessMessage(
                sunriseTimePoint,
                MessageHelper::CreateEventMessage(Constants::PredefinedIdSunrise, Subject::SunriseEvent, nlohmann::json::object_t{}));
        };

        processEvent();

        auto checkCommands = [&]() {
            REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == true);
            REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == true);

            const std::optional<Command> thermometerCommand = commandsController.Get(thermometerDeviceReceiverId);
            REQUIRE(std::holds_alternative<ThermometerLedBrightness>(thermometerCommand.value()) == true);
            REQUIRE(std::get<ThermometerLedBrightness>(thermometerCommand.value())._brightness == expectedBrightness);

            const std::optional<Command> relayCommand = commandsController.Get(relayDeviceReceiverId);
            REQUIRE(std::holds_alternative<RelayState>(relayCommand.value()) == true);
            REQUIRE(std::get<RelayState>(relayCommand.value())._state == expectedState);
        };

        checkCommands();

        expectedBrightness = 5;
        thermometerSunriseEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        expectedState = 0;
        relaySunriseEvent._command = RelayState{
            ._state = expectedState,
        };

        Event e1 = thermometerSunriseEvent;
        REQUIRE(eventsController.Update(e1).has_value() == true);
        Event e2 = relaySunriseEvent;
        REQUIRE(eventsController.Update(e2).has_value() == true);

        processEvent();

        checkCommands();
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("SunsetEvent") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        EventsProcessor eventsProcessor{ eventsController, commandsController };

        createDevices(devicesController);

        REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == false);
        REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == false);

        int expectedBrightness = 3;
        int expectedState = 1;

        SunsetEvent thermometerSunsetEvent{};
        thermometerSunsetEvent._provider = EventProvider{
            ._id = Constants::PredefinedIdSunset,
        };
        thermometerSunsetEvent._receiver = Receiver{
            ._type = DeviceType::Thermometer,
            ._id = thermometerDeviceReceiverId,
        };
        thermometerSunsetEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        SunsetEvent relaySetEvent{};
        relaySetEvent._provider = EventProvider{
            ._id = Constants::PredefinedIdSunset,
        };
        relaySetEvent._receiver = Receiver{
            ._type = DeviceType::Relay,
            ._id = relayDeviceReceiverId,
        };
        relaySetEvent._command = RelayState{
            ._state = expectedState,
        };

        REQUIRE(eventsController.Add(thermometerSunsetEvent) == true);
        REQUIRE(eventsController.Add(relaySetEvent) == true);

        const auto processEvent = [&]() {
            const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            int day;
            int month;
            int year;
            std::tie(day, month, year) = TimeHelper::GetDayMonthYear(now);
            SunriseSunsetTime sunsetTime = getSunriseSunsetTime(day, month, year);
            std::time_t tt = std::chrono::system_clock::to_time_t(now);
            std::tm tm = *std::localtime(&tt);
            tm.tm_hour = sunsetTime.sunsetHour;
            tm.tm_min = sunsetTime.sunsetMinute;
            tm.tm_sec = 0;
            const std::chrono::system_clock::time_point sunsetTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
            eventsProcessor.ProcessMessage(
                sunsetTimePoint, MessageHelper::CreateEventMessage(Constants::PredefinedIdSunset, Subject::SunsetEvent, nlohmann::json::object_t{}));
        };

        processEvent();

        auto checkCommands = [&]() {
            REQUIRE(commandsController.Get(thermometerDeviceReceiverId).has_value() == true);
            REQUIRE(commandsController.Get(relayDeviceReceiverId).has_value() == true);

            const std::optional<Command> thermometerCommand = commandsController.Get(thermometerDeviceReceiverId);
            REQUIRE(std::holds_alternative<ThermometerLedBrightness>(thermometerCommand.value()) == true);
            REQUIRE(std::get<ThermometerLedBrightness>(thermometerCommand.value())._brightness == expectedBrightness);

            const std::optional<Command> relayCommand = commandsController.Get(relayDeviceReceiverId);
            REQUIRE(std::holds_alternative<RelayState>(relayCommand.value()) == true);
            REQUIRE(std::get<RelayState>(relayCommand.value())._state == expectedState);
        };

        checkCommands();

        expectedBrightness = 5;
        thermometerSunsetEvent._command = ThermometerLedBrightness{
            ._brightness = expectedBrightness,
        };

        expectedState = 0;
        relaySetEvent._command = RelayState{
            ._state = expectedState,
        };

        Event e1 = thermometerSunsetEvent;
        REQUIRE(eventsController.Update(e1).has_value() == true);
        Event e2 = relaySetEvent;
        REQUIRE(eventsController.Update(e2).has_value() == true);

        processEvent();

        checkCommands();
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
