FROM debian:stable-slim

EXPOSE 7315
EXPOSE 7316

# update
RUN apt update && apt upgrade -y

# install all needed for buildapt install iproute2 -y
RUN apt install -y wget lsb-release wget software-properties-common gnupg make cmake libasio-dev libboost-all-dev libcurl4-openssl-dev uuid-dev libsqlite3-dev

# get clang 17
RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x ./llvm.sh
RUN ./llvm.sh 17

# set working dir
WORKDIR /src

# copy source code
COPY . .

# build
RUN ./Scripts/build.sh

ENTRYPOINT [ "./Scripts/start_for_docker.sh" ]
