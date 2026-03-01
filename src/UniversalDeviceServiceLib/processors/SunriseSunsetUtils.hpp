#pragma once

#include "Position.hpp"
struct SunriseSunsetTime {
    int day = -1;
    int month = -1;
    int sunriseHour = -1;
    int sunriseMinute = -1;
    int sunsetHour = -1;
    int sunsetMinute = -1;
};

#define LATITUDE_SPB  59.945673
#define LONGITUDE_SPB 30.342361

#define TIMEZONE_SPB  3

SunriseSunsetTime getSunriseSunsetTime(int day,
                                       int month,
                                       int year,
                                       Position position =
                                           Position{
                                               ._latitude = LATITUDE_SPB,
                                               ._longitude = LONGITUDE_SPB,
                                           },
                                       int timezone = TIMEZONE_SPB);
