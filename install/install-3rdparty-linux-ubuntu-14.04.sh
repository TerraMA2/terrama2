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
    exit
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
# GMock
# Site: https://github.com/google/googletest
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libgmock.a" ]; then
  echo "installing GMock..."
  sleep 1s

  unzip -o googletest-master.zip
  valid $? "Error: could not uncompress googletestmaster.zip!"

  cd googletest-master/googletest
  valid $? "Error: could not enter googletestmaster!"

  cmake .
  valid $? "Error: could not configure googletest!"

  make
  valid $? "Error: could not make googletest!"

  cp libgtest.a $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libgtest.a!"

  cp libgtest_main.a $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libgtest_main.a!"

  cp -r include/gtest/ $TERRAMA2_DEPENDENCIES_DIR/include/
  valid $? "Error: could not copy include dir!"

  cd ../googlemock

  cmake .
  valid $? "Error: could not configure googlemock!"

  make
  valid $? "Error: could not make googlemock!"

  cp libgmock.a $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libgmock.a!"

  cp libgmock_main.a $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libgmock_main.a!"

  cp -r include/gmock $TERRAMA2_DEPENDENCIES_DIR/include/
  valid $? "Error: could not copy include dir!"

  cd ../..
fi


#
#
# TerraLib
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/terralib5/lib/libterralib_mod_common.so" ]; then
  echo "installing TerraLib..."
  echo ""
  sleep 1s

  tar jxvf terralib5-2015-09-28.bz2
  valid $? "Error: could not uncompress terralib5-2015-09-28.bz2!"

  mkdir terralib5_build_make
  #valid $? "Error: could not create terralib5_build_make!"

  cd terralib5_build_make
  valid $? "Error: could not enter terralib5_build_make!"

  cmake ../terralib5/build/cmake -DCMAKE_BUILD_TYPE:STRING='Release' -DCMAKE_INSTALL_PREFIX:PATH="$TERRAMA2_DEPENDENCIES_DIR/terralib5" -DCMAKE_PREFIX_PATH:PATH="$TERRAMA2_DEPENDENCIES_DIR;$TERRAMA2_DEPENDENCIES_DIR/lib;$TERRAMA2_DEPENDENCIES_DIR/gdal2;$TERRAMA2_DEPENDENCIES_DIR/pgsql" -DTERRALIB_BUILD_EXAMPLES_ENABLED:BOOL=OFF -DTERRALIB_BUILD_UNITTEST_ENABLED:BOOL=OFF -DTERRALIB_DOXYGEN_ENABLED:BOOL=OFF -DTERRALIB_QHELP_ENABLED:BOOL=OFF -DTERRALIB_QTRANSLATION_ENABLED:BOOL=OFF -DTERRALIB_MOD_BINDING_JAVA_ENABLED:BOOL=OFF -DTERRALIB_MOD_BINDING_PYTHON_ENABLED:BOOL=OFF -DTERRALIB_MOD_BINDING_LUA_ENABLED:BOOL=OFF -DGEOS_INCLUDE_DIR:PATH="$TERRAMA2_DEPENDENCIES_DIR/include/geos" -DGNUGETTEXT_INCLUDE_DIR:PATH="/usr/include" -DGNUGETTEXT_LIBRARY:FILEPATH='/usr/lib/x86_64-linux-gnu/libgettextpo.so' -DGNUICONV_LIBRARY:FILEPATH='/usr/lib/x86_64-linux-gnu/libc.so'
  valid $? "Error: could not configure terralib5!"

  make -j 4
  valid $? "Error: could not make terralib5"

  make install
  valid $? "Error: Could not install terralib5"

  cd ..
fi

#
# Quazip
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libquazip.so" ]; then
  echo "installing Quazip..."
  echo ""
  sleep 1s

  unzip -o quazip-0.7.zip &> /dev/null
  valid $? "Error: could not uncompress quazip-0.7.zip!"

  cd quazip-0.7
  valid $? "Error: could not enter quazip-0.7!"

  qmake PREFIX=$TERRAMA2_DEPENDENCIES_DIR
 
  valid $? "Error: could not make Quazip!"

  make
  valid $? "Error: could not make Quazip!"

  make install
  valid $? "Error: Could not install Quazip!"

  cp quazip/libquazip.so $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libquazip.so!"

  cp -r $TERRAMA2_DEPENDENCIES_DIR/quazip/include/quazip/ $TERRAMA2_DEPENDENCIES_DIR/include/
  valid $? "Error: could not copy include dir!"
  
  cd ..
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
# TerraMA2 Web Dependencies
#

if [ -f "$2/webapp/app.js" ]; then
  echo "Installing TerraMA2 web dependencies..."
  echo ""

  unzip -o terrama2-web-dependencies.zip  &> /dev/null
  valid $? "Error: Could not install web dependencies!"
  mv externals "$2/webapp/public" &> /dev/null
  valid $? "Error: Verify if the externals directory already exists."

  echo "Finished TerraMA2 Web Dependencies"
  echo ""

  sleep 1s

fi

#
# Finished!
#
clear
echo "*****************************************************************"
echo "* TerraMA2 3rd-party Libraries Installer for Linux Ubuntu 14.04 *"
echo "*****************************************************************"
echo ""
echo "finished successfully!"
