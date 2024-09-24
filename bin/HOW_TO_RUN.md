## Inside bin folder:

./UniversalDeviceService

./UniversalDeviceFrontendService --docroot . --https-listen 0.0.0.0:7316 --ssl-certificate ./ssl/frontend.crt --ssl-private-key ./ssl/frontend.key --ssl-tmp-dh ./ssl/dhparam.pem

./ThermometerSimulator

./RealySimulator

## To generate certificates:

openssl genrsa -out NAME.key 2048
openssl req -key NAME.key -new -x509 -days 365 -out NAME.crt
openssl dhparam -dsaparam -out dhparam.pem 4096

## To run Docker

docker run -d --rm -it -p 7315:7315/tcp -p 7316:7316/tcp universaldevice:1.12.2
docker-compose run -d --service-ports universaldevice
