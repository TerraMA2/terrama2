#!/bin/sh
export PATH=$PATH:/usr/local/bin

if [[ $TERRAMA2_STATUS == 'start' ]]; then
    pm2 start webapp webmonitor
    pm2 save
elif [[ $TERRAMA2_STATUS == 'stop' ]]; then
    pm2 stop webapp webmonitor
    pm2 save
else
    pm2 describe webapp &>/dev/null
fi

if [[ $? -eq 0 ]]; then
    exit 0
else
    exit 1
fi