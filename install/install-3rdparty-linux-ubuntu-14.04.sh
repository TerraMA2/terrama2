#!/bin/bash
#
#  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of TerraMA2 - a free and open source computational
#  platform for analysis, monitoring, and alert of geo-environmental extremes.
#
#  TerraMA2 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraMA2 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraMA2. See LICENSE. If not, write to
#  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
#
#
#  Description: Install all required software for TerraMA2 on Linux Ubuntu 14.04.
#
#  Author: Gilberto Ribeiro de Queiroz
#          Paulo R. M. Oliveira
#
#
#  Example:
#  $ ./install-3rdparty-linux-ubuntu-14.04.sh /home/gribeiro/MyLibs /home/gribeiro/MyDevel/terrama2/codebase
#

echo "*****************************************************************"
echo "* TerraMA2 3rd-party Libraries Installer for Linux Ubuntu 14.04 *"
echo "*****************************************************************"
echo ""
sleep 1s

#
# Valid parameter val or abort script
#
function valid()
{
  if [ $1 -ne 0 ]; then
    echo $2
    echo ""
    exit 1
  fi
}


#
# Check for terrama2-3rdparty-linux-ubuntu-14.04.tar.gz
#
if [ ! -f ./terrama2-3rdparty-linux-ubuntu-14.04.tar.gz ]; then
  echo "Please, make sure to have terrama2-3rdparty-linux-ubuntu-14.04.tar.gz in the current directory!"
  echo ""
  exit
fi


#
# Extract packages
#
echo "extracting packages..."
echo ""
sleep 1s

tar xzvf terrama2-3rdparty-linux-ubuntu-14.04.tar.gz
valid $? "Error: could not extract 3rd party libraries (terrama2-3rdparty-linux-ubuntu-14.04.tar.gz)"

echo "packages extracted!"
echo ""
sleep 1s


#
# Go to 3rd party libraries dir
#
cd terrama2-3rdparty-linux-ubuntu-14.04
valid $? "Error: could not enter 3rd-party libraries dir (terrama2-3rdparty-linux-ubuntu-14.04)"


#
# Check installation dir
#
if [ "$1" == "" ]; then
  TERRAMA2_DEPENDENCIES_DIR="/opt/terrama2"
else
  TERRAMA2_DEPENDENCIES_DIR="$1"
fi

export PATH="$PATH:$TERRAMA2_DEPENDENCIES_DIR/bin"
export LD_LIBRARY_PATH="$PATH:$TERRAMA2_DEPENDENCIES_DIR/lib"

echo "installing 3rd-party libraries to '$TERRAMA2_DEPENDENCIES_DIR' ..."
echo ""
sleep 1s

mkdir -p "$TERRAMA2_DEPENDENCIES_DIR/lib/"
mkdir -p "$TERRAMA2_DEPENDENCIES_DIR/bin/"

#
# Quazip
#

if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libquazip5.so" ]; then
 echo "installing Quazip..."
 echo ""
 sleep 1s

 unzip -o quazip-0.7.2.zip &> /dev/null
 valid $? "Error: could not uncompress quazip-0.7.2.zip!"

 (
   cd quazip-0.7.2
   valid $? "Error: could not enter quazip-0.7.2"

   cmake . -DCMAKE_INSTALL_PREFIX:PATH="$TERRAMA2_DEPENDENCIES_DIR" -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL="ON"

   valid $? "Error: could not configure Quazip!"

   make
   valid $? "Error: could not make Quazip!"

   make install
   valid $? "Error: Could not install Quazip!"
 )
fi


#
# Screen
#
screen_test=`dpkg -s screen | grep Status`

if [ "$screen_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y screen
  valid $? "Error: could not install Screen! Please, install screen: sudo apt-get -y install screen"

  echo "Screen installed!"
else
  echo "Screen already installed!"
fi


#
# Node.js
#
nodejs_test=`dpkg -s nodejs | grep Status`

if [ "$nodejs_test" != "Status: install ok installed" ]; then
  curl -sL https://deb.nodesource.com/setup_4.x | sudo -E bash -
  valid $? "Error: could not setup Node.js installation!"

  sudo apt-get install -y nodejs
  valid $? "Error: could not install Node.js! Please, install Node.js: sudo apt-get -y install nodejs"

  echo "Node.js installed!"
else
  echo "Node.js already installed!"
fi


#
# Npm PM2 Globally
#
sudo npm install -g pm2
valid $? "Error: could not install pm2"


#
# Python
#
python_test=`dpkg -s python2.7-dev | grep Status`

if [ "$python_test" != "Status: install ok installed" ]; then
  sudo apt-get install python2.7-dev
  valid $? "Error: could not install python2.7-dev! Please, install python2.7-dev: sudo apt-get install python2.7-dev"

  echo "python2.7-dev installed!"
else
  echo "python2.7-dev already installed!"
fi

#
# Pip
#
pip_test=`dpkg -s python-pip | grep Status`

if [ "$pip_test" != "Status: install ok installed" ]; then
  sudo apt-get -y install python-pip
  valid $? "Error: could not install python-pip! Please, install python-pip: sudo apt-get -y install python-pip"

  echo "python-pip installed!"
else
  echo "python-pip already installed!"
fi

#
# Pylint
#
pylint_test=`pip show pylint --disable-pip-version-check`

if [ "$pylint_test" != "" ]; then
  echo "pylint already installed!"
else
  sudo pip install pylint
  valid $? "Error: could not install pylint! Please, install pylint: sudo pip install pylint"

  echo "pylint installed!"
fi

#
# Finished!
#
echo "*****************************************************************"
echo "* TerraMA2 3rd-party Libraries Installer for Linux Ubuntu 14.04 *"
echo "*****************************************************************"
echo ""
echo "finished successfully!"
