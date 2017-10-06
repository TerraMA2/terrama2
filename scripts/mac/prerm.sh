#!/bin/bash

function valid()
{
  if [ $1 -ne 0 ]; then
    echo $2
    echo ""
    exit 1
  fi
}

if [ -z "$TERRAMA2_APP_DIR" ]; then
  export TERRAMA2_APP_DIR=..
fi


echo "Pre-removal script..."

#
# Stop running services
#
(
  cd ${TERRAMA2_APP_DIR}/webapp/bin
  ./webapp-stop.js
  valid $? "Unable to stop NodeJs webapp."
)

#
# Stop webapp and webmonitor
#
(
  /usr/bin/osascript -e 'do shell script "pm2 stop webapp webmonitor | pm2 delete webapp webmonitor | pm2 save" with administrator privileges'
  valid $? "Unable to finish pm2 webapp and webmonitor."
)

#
# Drop TerraMA2 postgresql database
#
# (
#   psql -p5432 postgres -c "drop database terrama2"
#   valid $? "Unable to remove terrama2 database."
# )

echo "End pre-removal script..."

exit 0
