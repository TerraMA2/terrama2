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
# The parameter -platform offscreen is required to run in daemon mode without GUI display.
# This patch does not fix when starting a TerraMA² Service through WebApp. Although an error occurs, the TerraMA² runs
# in daemon mode.
nohup $TERRAMA2_INSTALL_PATH/bin/terrama2_service $TERRAMA2_SERVICE_TYPE $TERRAMA2_SERVICE_VERSION -platform offscreen &

# Lock Session
bash