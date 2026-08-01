#!/bin/bash

cd ./bin

./UniversalDeviceService &

./UniversalDeviceFrontendService &

if [ "$1" = "test" ]; then
    ./ThermometerSimulator &
    ./RelaySimulator &
    ./UniversalDeviceSimulator &
fi

# wait for any process to be finished
wait -n

# return fail code
exit $?
