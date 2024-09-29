#!/bin/bash

./scripts/docker_start.sh

git fetch --all

git pull

git submodule update --init

./scripts/docker_start.sh
