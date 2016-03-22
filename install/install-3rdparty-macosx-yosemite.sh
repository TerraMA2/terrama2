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
#  Description: Install all required software for TerraMA2 on MAC OS X Yosemite.
#
#  Author: Gilberto Ribeiro de Queiroz
#
#
#  Example:
#  $ TERRAMA2_DEPENDENCIES_DIR="/Users/gribeiro/MyLibs" ./install-3rdparty-macosx-yosemite.sh
#

echo "********************************************"
echo "* TerraMA2 Installer for Mac OS X Yosemite *"
echo "********************************************"
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
# Check for terrama2-3rdparty-macosx-yosemite.tar.gz
#
if [ ! -f ./terrama2-3rdparty-macosx-yosemite.tar.gz ]; then
  echo "Please, make sure to have terrama2-3rdparty-macosx-yosemite.tar.gz in the current directory!"
  exit
fi


#
# Extract packages
#
echo "extracting packages..."
sleep 1s

tar xzvf terrama2-3rdparty-macosx-yosemite.tar.gz
valid $? "Error: could not extract 3rd party libraries (terrama2-3rdparty-macosx-yosemite.tar.gz)"

echo "packages extracted!"
sleep 1s


#
# Go to 3rd party libraries dir
#
cd terrama2-3rdparty-macosx-yosemite
valid $? "Error: could not enter 3rd-party libraries dir (terrama2-3rdparty-macosx-yosemite)"


#
# Check installation dir
#
if [ "$TERRAMA2_DEPENDENCIES_DIR" == "" ]; then
  TERRAMA2_DEPENDENCIES_DIR = "/opt/terrama2"
fi

export PATH="$PATH:$TERRAMA2_DEPENDENCIES_DIR/bin"
export LD_LIBRARY_PATH="$PATH:$TERRAMA2_DEPENDENCIES_DIR/lib"

echo "installing 3rd-party libraries to '$TERRAMA2_DEPENDENCIES_DIR' ..."
sleep 1s

#
# GMock
# Site: https://github.com/google/googletest
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libgmock.a" ]; then
  echo "installing GMock..."
  sleep 1s

  unzip googletest-master.zip
  valid $? "Error: could not uncompress googletest-master.zip!"

  cd googletest-master/googletest
  valid $? "Error: could not enter googletest-master!"

  cmake .
  valid $? "Error: could not configure googletest!"

  make
  valid $? "Error: could not make googletest!"

  cp libgtest.a $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libgtest.a!"

  cp libgtest_main.a $TERRAMA2_DEPENDENCIES_DIR/lib/
  valid $? "Error: could not copy libgtest_main.a!"

  cp -r include/gtest $TERRAMA2_DEPENDENCIES_DIR/include/
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
# TerraLib
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/terralib5/lib/libterralib_mod_common.dylib" ]; then
  echo "installing TerraLib..."
  echo ""
  sleep 1s

  tar jxvf terralib5-2015-09-28.bz2
  valid $? "Error: could not uncompress terralib5-2015-09-28.bz2!"

  mkdir terralib5_build_make
  #valid $? "Error: could not create terralib5_build_make!"

  cd terralib5_build_make
  valid $? "Error: could not enter terralib5_build_make!"

  cmake ../terralib5/build/cmake -DCMAKE_BUILD_TYPE:STRING='Release' -DCMAKE_INSTALL_PREFIX:PATH="$TERRAMA2_DEPENDENCIES_DIR/terralib5" -DCMAKE_PREFIX_PATH:PATH="$TERRAMA2_DEPENDENCIES_DIR;$TERRAMA2_DEPENDENCIES_DIR/lib;$TERRAMA2_DEPENDENCIES_DIR/gdal2;$TERRAMA2_DEPENDENCIES_DIR/pgsql" -DTERRALIB_BUILD_EXAMPLES_ENABLED:BOOL=OFF -DTERRALIB_BUILD_UNITTEST_ENABLED:BOOL=OFF -DTERRALIB_DOXYGEN_ENABLED:BOOL=OFF -DTERRALIB_QHELP_ENABLED:BOOL=OFF -DTERRALIB_QTRANSLATION_ENABLED:BOOL=OFF -DTERRALIB_QTRANSLATION_ENABLED:BOOL=OFF -DTERRALIB_MOD_BINDING_JAVA_ENABLED:BOOL=OFF -DTERRALIB_MOD_BINDING_PYTHON_ENABLED:BOOL=OFF -DTERRALIB_MOD_BINDING_LUA_ENABLED:BOOL=OFF -DGEOS_INCLUDE_DIR:PATH="$TERRAMA2_DEPENDENCIES_DIR/include/geos" -DGEOS_LIBRARY:FILEPATH="$TERRAMA2_DEPENDENCIES_DIR/lib/libgeos.dylib" -DGEOSC_INCLUDE_DIR:PATH="$TERRAMA2_DEPENDENCIES_DIR/include" -DGEOSC_LIBRARY:FILEPATH="$TERRAMA2_DEPENDENCIES_DIR/lib/libgeos_c.dylib" -DGDAL_INCLUDE_DIR:PATH="$TERRAMA2_DEPENDENCIES_DIR/gdal2/include" -DGDAL_LIBRARY:FILEPATH="$TERRAMA2_DEPENDENCIES_DIR/gdal2/lib/libgdal.dylib" -DGDAL_DATA_DIR:PATH="$TERRAMA2_DEPENDENCIES_DIR/gdal2/share/gdal" -DGDAL_CONFIG:FILEPATH="$TERRAMA2_DEPENDENCIES_DIR/gdal2/bin/gdal-config" -DPROJ4_INCLUDE_DIR:PATH="$TERRAMA2_DEPENDENCIES_DIR/include" -DPROJ4_LIBRARY:FILEPATH="$TERRAMA2_DEPENDENCIES_DIR/lib/libproj.dylib"
  valid $? "Error: could not configure terralib5!"

  make -j 4
  valid $? "Error: could not make terralib5"

  make install
  valid $? "Error: Could not install terralib5"

  cd ..
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
