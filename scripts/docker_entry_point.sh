#!/bin/bash

cd ./bin

./UniversalDeviceService &

./UniversalDeviceFrontendService &

# wihtout this will finish container
/bin/bash
