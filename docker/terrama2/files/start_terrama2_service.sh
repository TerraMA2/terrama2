#!/bin/bash

# Arguments
TERRAMA2_SERVICE_TYPE=$1
TERRAMA2_SERVICE_VERSION=$2

# Start TerraMA2 Service Daemon
nohup terrama2_service $TERRAMA2_SERVICE_TYPE $TERRAMA2_SERVICE_VERSION &
TERRAMA2_SERVICE_PID=$(echo $!)

# Lock Session
bash