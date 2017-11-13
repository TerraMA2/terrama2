#!/bin/bash

cd $TERRAMA2_INSTALL_PATH/webmonitor

pm2 start npm --name webmonitor -- start

trap "pm2 stop webmonitor" EXIT HUP INT QUIT TERM

pm2 log webmonitor