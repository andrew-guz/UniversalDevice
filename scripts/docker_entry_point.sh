#!/bin/bash

cd ./bin

./UniversalDeviceService &

./UniversalDeviceFrontendService &

# without this will finish container
/bin/bash
