# base image
FROM debian:stable-slim AS base-image

# for apt
ENV DEBIAN_FRONTEND=noninteractive

# update
RUN apt-get update && apt-get upgrade -y

# set timezone
RUN ln -sf /usr/share/zoneinfo/Europe/Moscow /etc/localtime

# install all needed for build
RUN apt-get install -y wget lsb-release wget gnupg make cmake libssl-dev libasio-dev libboost-all-dev uuid-dev libsqlite3-dev git jq

# get clang 17
RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x ./llvm.sh
RUN ./llvm.sh 19

# add links
RUN ln -s /usr/bin/clang++-19 /usr/bin/clang++
RUN ln -s /usr/bin/clang-format-19 /usr/bin/clang-format

# main image
FROM base-image AS universal-device-image

# open ports
EXPOSE 7315
EXPOSE 7316

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

# copy WT resources
COPY ./bin/resources ./bin/resources

# create configuration
RUN echo '{ "port": 7315, "dbPath": "/opt/UniversalDevice/UniversalDeviceService.db", "certificatePath": "/opt/UniversalDevice/ssl/backend.crt", "keyPath": "/opt/UniversalDevice/ssl/backend.key", "authPath": "/opt/UniversalDevice/authentication.json", "logPath": "/opt/UniversalDevice/UniversalDeviceService.log" }' > ./bin/UniversalDeviceService.json
RUN echo '{ "servicePort": 7315, "frontendPort": 7316, "certificatePath": "/opt/UniversalDevice/ssl/frontend.crt", "keyPath": "/opt/UniversalDevice/ssl/frontend.key", "dhPath": "/opt/UniversalDevice/ssl/dhparam.pem", "authPath": "/opt/UniversalDevice/authentication.json", "logPath": "/opt/UniversalDevice/UniversalDeviceFrontendService.log" }' > ./bin/UniversalDeviceFrontendService.json

ENTRYPOINT [ "./scripts/docker_entry_point.sh" ]
