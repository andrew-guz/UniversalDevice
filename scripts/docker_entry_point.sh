#!/bin/bash

cd ./bin

./UniversalDeviceService &

./UniversalDeviceFrontendService &

if [ "$1" = "test" ]; then
    ./ThermometerSimulator &
    ./RelaySimulator &
    ./UniversalDeviceSimulator &
fi

# without this will finish container
/bin/bash
