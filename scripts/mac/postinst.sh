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

# Check manually installed thirdparties
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

# Install web dependencies
(
  echo "Installing Webapp dependencies..."
  cd ${TERRAMA2_APP_DIR}/webapp

  npm install bower &>/dev/null
  valid $? "Unable to install Bower."
  npm install grunt &>/dev/null
  valid $? "Unable to install Grunt."
  
  npm install >/dev/null
  valid $? "Unable to install NodeJs dependencies."
  npm run bower -- install --allow-root >/dev/null
  valid $? "Unable to install Bower dependencies."
  npm run grunt >/dev/null
  valid $? "Error executing grunt."

  chmod +x bin/webapp-stop.js
)

(
  echo "Installing Webcomponents dependencies..."
  cd ${TERRAMA2_APP_DIR}/webcomponents

  npm install grunt &>/dev/null
  valid $? "Unable to install Grunt."

  npm install >/dev/null
  valid $? "Unable to install NodeJs dependencies."
  npm run grunt >/dev/null
  valid $? "Error executing grunt."
)

(
  echo "Installing Webmonitor dependencies..."
  cd ${TERRAMA2_APP_DIR}/webmonitor

  npm install bower &>/dev/null
  valid $? "Unable to install Bower."
  npm install grunt &>/dev/null
  valid $? "Unable to install Grunt."

  npm install >/dev/null
  valid $? "Unable to install NodeJs dependencies."
  npm run bower -- install --allow-root >/dev/null
  valid $? "Unable to install Bower dependencies."
  npm run grunt >/dev/null
  valid $? "Error executing grunt."
)

# Create default config files
(
  cd ${TERRAMA2_APP_DIR}/webapp/config
  if ! [ -d "instances" ]; then
    cp -r sample_instances instances
  fi
)
(
  cd ${TERRAMA2_APP_DIR}/webmonitor/config
  if ! [ -d "instances" ]; then
    cp -r sample_instances instances
  fi
)

# If not root, end script
if [ "$EUID" -ne 0 ]; then
  echo "End post-installation script..."
  exit 0
fi

# Install and configure pm2
(
  # Check if pm2 is already installed
  npm list pm2 -g >/dev/null
  if [ $? != 0 ]; then
    echo "Configuring pm2..."

    # Install pm2
    npm install pm2 -g >/dev/null
    valid $? "Error installing pm2."
    pm2 install pm2-logrotate >/dev/null
    valid $? "Error installing pm2-logrotate."

    # Allows the service to start with the system
    /usr/bin/osascript -e 'do shell script "env PATH=$PATH:/usr/local/bin pm2 startup" with administrator privileges'
    valid $? "Error adding pm2 to startup."
  fi
)

# Create a terrama2 user
# and default data folder
(
  adduser --system --no-create-home terrama2
  if [ $? -eq 0 ]; then
    # new user created
    # setting new password
    echo terrama2:terrama4 | chpasswd
    sudo usermod -a -G sudo terrama2
  fi

  mkdir -p /var/lib/terrama2/data
  chown terrama2 /var/lib/terrama2 -R
)

# Add webapp to pm2 startup
(
  echo "Adding Webapp in pm2 startup..."
  cd ${TERRAMA2_APP_DIR}/webapp

  pm2 start npm --name webapp -- start >/dev/null
  valid $? "Error executing webapp."
  pm2 save  >/dev/null
  valid $? "Error saving pm2 state."
)

# Add webmonitor to pm2 startup
(
  echo "Adding Webcomponents in pm2 startup..."
  cd ${TERRAMA2_APP_DIR}/webmonitor

  pm2 start npm --name webmonitor -- start >/dev/null
  valid $? "Error executing webmonitor."
  pm2 save  >/dev/null
  valid $? "Error saving pm2 state."
)

echo "End post-installation script..."
exit 0