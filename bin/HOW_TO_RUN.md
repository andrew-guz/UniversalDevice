## Inside bin folder:

./UniversalDeviceService

./UniversalDeviceFrontendService

./ThermometerSimulator

./RealySimulator

./Scripts/start.sh

## To generate certificates:

openssl genrsa -out NAME.key 2048

openssl req -key NAME.key -new -x509 -days 365 -out NAME.crt

openssl dhparam -dsaparam -out dhparam.pem 4096

## To run Docker

docker run -d --rm -it -p 7315:7315/tcp -p 7316:7316/tcp --restart unless-stopped universaldevice:{$VERSION}

docker-compose run -d --service-ports universaldevice
