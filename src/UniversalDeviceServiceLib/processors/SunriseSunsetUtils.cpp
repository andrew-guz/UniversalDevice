#include "SunriseSunsetUtils.hpp"

#include <cmath>
#include <sunset.h>

#include "Position.hpp"

SunriseSunsetTime getSunriseSunsetTime(const int day, const int month, const int year, const Position position, const int timezone) {
    SunSet sunset;
    sunset.setPosition(position._latitude, position._longitude, timezone);
    sunset.setCurrentDate(1900 + year, month + 1, day);
    const double sunriseMinutes = sunset.calcSunrise();
    const double sunsetMinutes = sunset.calcSunset();

    // FIXME: chrono floor
    SunriseSunsetTime result = {
        .day = day,
        .month = month,
        .sunriseHour = static_cast<int>(std::floor(sunriseMinutes / 60.0)),
        .sunriseMinute = static_cast<int>(std::round(sunriseMinutes - result.sunriseHour * 60.0)),
        .sunsetHour = static_cast<int>(std::floor(sunsetMinutes / 60.0)),
        .sunsetMinute = static_cast<int>(std::round(sunsetMinutes - result.sunsetHour * 60.0)),
    };

    return result;
}
