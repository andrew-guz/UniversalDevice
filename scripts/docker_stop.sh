#!/bin/bash

docker stop $(docker ps -a -q -f name=universaldevice)

docker rm $(docker ps -a -q -f name=universaldevice)
