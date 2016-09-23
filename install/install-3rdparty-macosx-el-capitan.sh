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
#  Description: Install all required software for TerraMA2 on MAC OS X El Capitan.
#
#  Author: Gilberto Ribeiro de Queiroz
#
#
#  Example:
#  $ ./install-3rdparty-macosx-el-capitan.sh /home/gribeiro/MyLibs /home/gribeiro/MyDevel/terrama2/codebase
#

echo "**********************************************"
echo "* TerraMA2 Installer for Mac OS X El Capitan *"
echo "**********************************************"
echo ""
sleep 1s


#
# If first argument is false it aborts the script and
# report the message passed as second argument.
#
function valid()
{
  if [ $1 -ne 0 ]; then
    echo $2
    echo ""
    exit
  fi
}


#
# Check for terrama2-3rdparty-macosx-el-capitan.tar.gz
#
if [ ! -f ./terrama2-3rdparty-macosx-el-capitan.tar.gz ]; then
  echo "Please, make sure to have terrama2-3rdparty-macosx-el-capitan.tar.gz in the current directory!"
  exit
fi


#
# Extract packages
#
echo "extracting packages..."
sleep 1s

tar xzvf terrama2-3rdparty-macosx-el-capitan.tar.gz
valid $? "Error: could not extract 3rd party libraries (terrama2-3rdparty-macosx-el-capitan.tar.gz)"

echo "packages extracted!"
sleep 1s


#
# Go to 3rd party libraries dir
#
cd terrama2-3rdparty-macosx-el-capitan
valid $? "Error: could not enter 3rd-party libraries dir (terrama2-3rdparty-macosx-el-capitan)"


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
sleep 1s

#
# Quazip
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libquazip5.dylib" ]; then
  echo "installing Quazip..."
  echo ""
  sleep 1s

  unzip -o quazip-0.7.2.zip &> /dev/null
  valid $? "Error: could not uncompress quazip-0.7.2.zip!"

  cd quazip-0.7.2
  valid $? "Error: could not enter quazip-0.7.2"

  cmake . -DCMAKE_INSTALL_PREFIX=$TERRAMA2_DEPENDENCIES_DIR

  valid $? "Error: could not configure Quazip!"

  make
  valid $? "Error: could not make Quazip!"

  make install
  valid $? "Error: Could not install Quazip!"

  cd ..
fi

#
# NodeJS 4.5.0
#
nodejs --version
if [ $? -eq 0 ]; then
  echo "Installing NodeJS..."
  echo ""
  sleep 1s

  curl -O https://nodejs.org/dist/v4.5.0/node-v4.5.0.pkg &> /dev/null
  valid $? "Error: could not download nodejs 4.5.0!"

  sudo installer -pkg node-v4.5.0.pkg -target /
  valid $? "Error: could not install node-v4.5.0.pkg"
fi

#
# Finished!
#
clear
echo "********************************************"
echo "* TerraMA2 Installer for Mac OS X Yosemite *"
echo "********************************************"
echo ""
echo "finished successfully!"
