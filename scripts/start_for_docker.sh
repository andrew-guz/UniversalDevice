#!/bin/bash

cd ./bin

./UniversalDeviceService &

./UniversalDeviceFrontendService --docroot . --https-listen 0.0.0.0:7316 --ssl-certificate ./ssl/frontend.crt --ssl-private-key ./ssl/frontend.key --ssl-tmp-dh ./ssl/dhparam.pem &

# wihtout this will finish container
/bin/bash
