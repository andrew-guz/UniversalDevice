#!/bin/bash

VERSION=$(./scripts/docker_get_version.sh)

docker build --no-cache --tag=universaldevice:$VERSION .
