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

# checkout sources
RUN git clone --recurse-submodules --depth 1 https://github.com/andrew-guz/UniversalDevice.git .

# build
RUN ./scripts/build.sh

# modify configuration
RUN jq --arg dbPath "/opt/UniversalDevice/UniversalDeviceService.db" '.dbPath=$dbPath' ./bin/UniversalDeviceService.json > ./bin/UniversalDeviceService.json.tmp && mv ./bin/UniversalDeviceService.json.tmp ./bin/UniversalDeviceService.json
RUN jq --arg certificatePath "/opt/UniversalDevice/ssl/backend.crt" '.certificatePath=$certificatePath' ./bin/UniversalDeviceService.json > ./bin/UniversalDeviceService.json.tmp && mv ./bin/UniversalDeviceService.json.tmp ./bin/UniversalDeviceService.json
RUN jq --arg keyPath "/opt/UniversalDevice/ssl/backend.key" '.keyPath=$keyPath' ./bin/UniversalDeviceService.json > ./bin/UniversalDeviceService.json.tmp && mv ./bin/UniversalDeviceService.json.tmp ./bin/UniversalDeviceService.json
RUN jq --arg authPath "/opt/UniversalDevice/authentication.json" '.authPath=$authPath' ./bin/UniversalDeviceService.json > ./bin/UniversalDeviceService.json.tmp && mv ./bin/UniversalDeviceService.json.tmp ./bin/UniversalDeviceService.json
RUN jq --arg certificatePath "/opt/UniversalDevice/ssl/frontend.crt" '.certificatePath=$certificatePath' ./bin/UniversalDeviceFrontendService.json > ./bin/UniversalDeviceFrontendService.json.tmp && mv ./bin/UniversalDeviceFrontendService.json.tmp ./bin/UniversalDeviceFrontendService.json
RUN jq --arg keyPath "/opt/UniversalDevice/ssl/frontend.key" '.keyPath=$keyPath' ./bin/UniversalDeviceFrontendService.json > ./bin/UniversalDeviceFrontendService.json.tmp && mv ./bin/UniversalDeviceFrontendService.json.tmp ./bin/UniversalDeviceFrontendService.json
RUN jq --arg dhPath "/opt/UniversalDevice/ssl/dhparam.pem" '.dhPath=$dhPath' ./bin/UniversalDeviceFrontendService.json > ./bin/UniversalDeviceFrontendService.json.tmp && mv ./bin/UniversalDeviceFrontendService.json.tmp ./bin/UniversalDeviceFrontendService.json
RUN jq --arg authPath "/opt/UniversalDevice/authentication.json" '.authPath=$authPath' ./bin/UniversalDeviceFrontendService.json > ./bin/UniversalDeviceFrontendService.json.tmp && mv ./bin/UniversalDeviceFrontendService.json.tmp ./bin/UniversalDeviceFrontendService.json

ENTRYPOINT [ "./scripts/docker_entry_point.sh" ]
