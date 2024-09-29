FROM debian:stable-slim

EXPOSE 7315
EXPOSE 7316

# update
RUN apt-get update && apt-get upgrade -y

# install all needed for build
RUN apt-get install -y wget lsb-release wget software-properties-common gnupg make cmake libasio-dev libboost-all-dev libcurl4-openssl-dev uuid-dev libsqlite3-dev git jq

# get clang 17
RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x ./llvm.sh
RUN ./llvm.sh 17

# set working dir
WORKDIR /src

# copy source
COPY ./src ./src
COPY ./vendor ./vendor
COPY ./scripts ./scripts
COPY CMakeLists.txt .
COPY Version.hpp.in .

# build
RUN ./scripts/build.sh

# create configuration
RUN echo '{ "port": 7315, "dbPath": "/opt/UniversalDevice/UniversalDeviceService.db", "certificatePath": "/opt/UniversalDevice/ssl/backend.crt", "keyPath": "/opt/UniversalDevice/ssl/backend.key", "authPath": "/opt/UniversalDevice/authentication.json"}' > ./bin/UniversalDeviceService.json
RUN echo '{ "servicePort": 7315, "frontendPort": 7316, "certificatePath": "/opt/UniversalDevice/ssl/frontend.crt", "keyPath": "/opt/UniversalDevice/ssl/frontend.key", "dhPath": "/opt/UniversalDevice/ssl/dhparam.pem", "authPath": "/opt/UniversalDevice/authentication.json" }' > ./bin/UniversalDeviceFrontendService.json

ENTRYPOINT [ "./scripts/docker_entry_point.sh" ]
