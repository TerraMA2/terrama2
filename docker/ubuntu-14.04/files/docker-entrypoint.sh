#!/bin/bash

# Start PostgreSQL
service postgresql start

# Start GeoServer
nohup $GEOSERVER_HOME/bin/startup.sh &

ls -lah $TERRAMA2_INSTALL_PATH

if [ ! -f "$TERRAMA2_INSTALL_PATH/terrama2.lock" ]; then
  cd $TERRAMA2_INSTALL_PATH/webapp
  npm start npm --name webapp -- start

  cd $TERRAMA2_INSTALL_PATH/webmonitor
  npm start npm --name webmonitor -- start
else
  # Start TerraMA² Services
  pm2 start webapp
  pm2 start webmonitor
fi

trap "service postgresql stop; pm2 stop webapp; pm2 stop webmonitor" EXIT HUP INT QUIT TERM

# Lock terminal. TODO: Export PM2 Log
# pm2 log webapp
bash