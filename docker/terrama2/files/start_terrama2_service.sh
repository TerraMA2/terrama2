#!/bin/bash

set -x

# Start Services
service ssh start

# Arguments
TERRAMA2_SERVICE_TYPE=$1
TERRAMA2_SERVICE_VERSION=$2

echo "TerraMA² Service Type : $TERRAMA2_SERVICE_TYPE"
echo "TerraMA² Service Version : $TERRAMA2_SERVICE_VERSION"

cd $TERRAMA2_INSTALL_PATH

ls -lah .

# Start TerraMA2 Service Daemon
echo "Starting $TERRAMA2_INSTALL_PATH/bin/terrama2_service ... "
nohup $TERRAMA2_INSTALL_PATH/bin/terrama2_service $TERRAMA2_SERVICE_TYPE $TERRAMA2_SERVICE_VERSION -platform offscreen &

# Lock Session
bash