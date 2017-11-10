#!/bin/bash

function valid() 
{
  if [ $1 -ne 0 ]; then
    echo $2
    exit $1
  else
    echo "done."
  fi
}

function finish()
{
  echo -ne "Stopping PostgreSQL ... "
  service postgresql stop > /tmp/terrama2_pg.log
  valid $? "Could not stop PostgreSQL $(cat /tmp/terrama2_pg.log)"

  echo -ne "Stopping WebApp ... "
  pm2 stop webapp > /tmp/web_app.log
  valid $? "Could not stop TerraMA² WebApp - $(cat /tmp/web_app.log)"

  echo -ne "Stopping Web Monitor ... "
  pm2 stop webmonitor > /tmp/web_monitor.log
  valid $? "Could not stop TerraMA² WebMonitor - $(cat /tmp/web_monitor.log)"

  echo -ne "Stopping GeoServer ... "
  kill -2 $GEOSERVER_PID_N
  valid $? "Could not stop GeoServer"
}

# Start PostgreSQL
service postgresql start

# Start GeoServer
nohup $GEOSERVER_HOME/bin/startup.sh > /tmp/tmp_geoserver_f.log 2>&1 &
GEOSERVER_PID_N=$(echo $!)

if [ ! -f "$TERRAMA2_INSTALL_PATH/terrama2.lock" ]; then
  cd $TERRAMA2_INSTALL_PATH/webapp
  pm2 start npm --name webapp -- start

  cd $TERRAMA2_INSTALL_PATH/webmonitor
  pm2 start npm --name webmonitor -- start
else
  # Start TerraMA² Services
  pm2 start webapp
  pm2 start webmonitor
fi

trap finish EXIT HUP INT QUIT TERM

# Lock terminal. TODO: Export PM2 Log
pm2 log
