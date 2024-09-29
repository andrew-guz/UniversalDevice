#!/bin/bash

./scripts/docker_stop.sh

git fetch --all

git pull

git submodule update --init

./scripts/docker_start.sh
