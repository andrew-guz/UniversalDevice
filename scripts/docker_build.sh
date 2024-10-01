#!/bin/bash

VERSION=$(./scripts/docker_get_version.sh)

docker build . --tag=universaldevice:$VERSION
