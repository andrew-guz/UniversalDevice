FROM debian:stable-slim

WORKDIR /src

COPY . .

RUN apt update && apt upgrade -y

