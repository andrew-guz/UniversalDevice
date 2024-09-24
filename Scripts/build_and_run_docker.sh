#!/bin/bash

docker-compose build

docker-compose run -d --service-ports universaldevice
