#include <chrono>

#include <catch2/catch_all.hpp>
#include <nlohmann/json_fwd.hpp>

#include "ExtendedThermometerCurrentValue.hpp"
#include "Marshaling.hpp"
#include "MotionRelaySettings.hpp"
#include "TimerEvent.hpp"

template<typename T>
void test(T& object) {
    const nlohmann::json json = object;
    T object2 = json.get<T>();
    const nlohmann::json json2 = object2;
    REQUIRE(json == json2);
}

TEST_CASE("DerivedObjectsMarshaling") {
    TimerEvent timerEvent;
    timerEvent._name = "test";
    timerEvent._hour = 12;
    timerEvent._minute = 59;

    test(timerEvent);

    MotionRelaySettings motionRelaySettings;
    motionRelaySettings._period = 123;
    motionRelaySettings._activityTime = 5;

    test(motionRelaySettings);

    ExtendedThermometerCurrentValue extThermValue;
    extThermValue._timestamp = std::chrono::system_clock::now();
    test(extThermValue);
}
