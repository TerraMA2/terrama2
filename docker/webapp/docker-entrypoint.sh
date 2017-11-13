#!/bin/bash

cd $TERRAMA2_INSTALL_PATH/webapp
pm2 start npm --name webapp -- start

trap "pm2 stop webapp" EXIT HUP INT QUIT TERM

pm2 log webapp
