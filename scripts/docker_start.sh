#!/bin/bash

VERSION=$(./scripts/docker_get_version.sh)

docker build . --tag=universaldevice:$VERSION

docker run --name universaldevice -d -it --restart always -p 0.0.0.0:7315:7315 -p 0.0.0.0:7316:7316 -v ./opt:/opt/UniversalDevice universaldevice:$VERSION
