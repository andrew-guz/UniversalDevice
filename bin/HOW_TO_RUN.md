## Inside bin folder:

./UniversalDeviceService

./UniversalDeviceFrontendService --docroot . --https-listen 0.0.0.0:7316 --ssl-certificate ./ssl/frontend.crt --ssl-private-key ./ssl/frontend.key --ssl-tmp-dh ./ssl/dhparam.pem

./ThermometerSimulator

./RealySimulator

## To generate certificates:

openssl genrsa -out NAME.key 2048
openssl req -key NAME.key -new -x509 -days 365 -out NAME.crt
openssl dhparam -dsaparam -out dhparam.pem 4096
