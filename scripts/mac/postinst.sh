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


echo "Post-installation script..."

#
# Install Optional Thirdparties
#
(
  echo "Checking for Postgresql version..."
  which psql &>/dev/null
  if [[ $? != 0 ]] ; then
      echo "Cannot find Postgresql. Please install a compatible version and add the correct path in settings."
  fi

  echo "Checking for Node version..."
  node --version &>/dev/null
  if [[ $? != 0 ]] ; then
      echo "Cannot find NodeJs. Please install a compatible version and add the correct path in settings."
  fi
)


#
# Install web dependencies
#
(
  echo "Installing Bower and Grunt..."
  npm install bower -g &>/dev/null
  npm install grunt -g &>/dev/null
  valid $? "Error installing web dependencies."
)

(
  echo "Installing Webapp dependencies..."
  cd ${TERRAMA2_APP_DIR}/webapp
  npm install 
  valid $? "Unable to install NodeJs dependencies."
  bower install --allow-root >/dev/null
  valid $? "Unable to install Bower dependencies."
  grunt >/dev/null
  valid $? "Error executing grunt."

  chmod +x bin/webapp-stop.js
)

(
  echo "Installing Webcomponents dependencies..."
  cd ${TERRAMA2_APP_DIR}/webcomponents
  npm install >/dev/null
  valid $? "Unable to install NodeJs dependencies."
  grunt >/dev/null
  valid $? "Error executing grunt."
)

(
  echo "Installing Webmonitor dependencies..."
  cd ${TERRAMA2_APP_DIR}/webmonitor
  npm install >/dev/null
  valid $? "Unable to install NodeJs dependencies."
  bower install --allow-root >/dev/null
  valid $? "Unable to install Bower dependencies."
)

#
# Install and configure pm2
#
(
  # Check if pm2 is already installed
  npm list pm2 -g >/dev/null
  if [ $? != 0 ]; then
    echo "Configuring pm2..."
    # Install pm2
    npm install pm2 -g >/dev/null
    pm2 install pm2-logrotate >/dev/null
    valid $? "Error installing pm2."
    # Allows the service to start with the system
    /usr/bin/osascript -e 'do shell script "env PATH=$PATH:/usr/local/bin pm2 startup" with administrator privileges'
    valid $? "Error adding pm2 to startup."
  fi
)

(
  # Add webapp to pm2 startup
  echo "Adding Webapp in pm2 startup..."
  cd ${TERRAMA2_APP_DIR}/webapp
  pm2 start npm --name webapp -- start >/dev/null
  valid $? "Error executing webapp."
  pm2 save  >/dev/null
  valid $? "Error saving pm2 state."
)

(
  # Add webmonitor to pm2 startup
  echo "Adding Webcomponents in pm2 startup..."
  cd ${TERRAMA2_APP_DIR}/webmonitor
  pm2 start npm --name webmonitor -- start >/dev/null
  valid $? "Error executing webmonitor."
  pm2 save  >/dev/null
  valid $? "Error saving pm2 state."
)

echo "End post-installation script..."

exit 0