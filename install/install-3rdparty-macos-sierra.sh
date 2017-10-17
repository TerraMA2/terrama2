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
#  Description: Install all required software for TerraMA2 on MAC OS Sierra.
#
#  Author: Gilberto Ribeiro de Queiroz
#
#  Example:
#  $ ./install-3rdparty-macos-sierra.sh /home/gribeiro/MyLibs /home/gribeiro/MyDevel/terrama2/codebase
#

echo "****************************************"
echo "* TerraMA2 Installer for Mac OS Sierra *"
echo "****************************************"
echo ""
sleep 1s

#
# Used to update the package location at header of generated third parties.
# Usage: fixRPath <arg1>, where arg1 is a list of files (full path) to be fixed.
# Note: used in build proccess of Boost and QWT packages.
#
function fixRPath()
{
  _FILES=$1 
  
  for _FILE in $_FILES;
  do
    install_name_tool -id $_FILE $_FILE
    
    for _FILE2 in $_FILES;
    do
      install_name_tool -change @rpath/`basename "$_FILE"` $_FILE $_FILE2
    done
  done
}

#
# Used to append informations on build log file.
# Usage: log_lib <arg1>, where arg1 the name of package being logged.
#
function log_lib()
{
  echo "=========================================================================" >> build.log
  echo "Warnings for $1">> build.log
}

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
# Check installation dir
#

if [ -z "$TERRAMA2_DEPENDENCIES_DIR" ]; then
  echo "Please, select the installation folder of the third-party libraries through the variable TERRAMA2_DEPENDENCIES_DIR."
  exit
fi

export PATH=$PATH:$HOME/Qt5.4.1/5.4/clang_64/bin:/Applications/CMake.app/Contents/bin

export LD_LIBRARY_PATH="$PATH:$TERRAMA2_DEPENDENCIES_DIR/lib"

echo "installing 3rd-party libraries to '$TERRAMA2_DEPENDENCIES_DIR' ..."
sleep 1s

#
# Brew
#
which -s brew > /dev/null
if [[ $? != 0 ]] ; then
  echo "Installing Brew..."
  sleep 1s
  
  /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" 
fi

#
# GnuTLS
#
if [ ! -d "/usr/local/Cellar/gnutls" ]; then
  echo "Installing GnuTLS..."
  sleep 1s

  brew install gnutls > /dev/null 2>> ../build.log
  valid $? "Error: could not install GnuTLS!"
fi

#
# GnuSASL
#
gsasl --version > /dev/null
if [[ $? != 0 ]] ; then
  echo "Installing GnuSASL..."
  sleep 1s

  brew install gsasl > /dev/null 2>> ../build.log
  valid $? "Error: could not install GnuSASL!"
fi

#
# Quazip
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libquazip5.dylib" ]; then
  echo "Installing Quazip..."
  sleep 1s
  log_lib "QUAZIP" 

  unzip -o quazip-0.7.3.zip &> /dev/null
  valid $? "Error: could not uncompress quazip-0.7.3.zip!"

  cd quazip-0.7.3 > /dev/null
  valid $? "Error: could not enter quazip-0.7.3"

  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_PREFIX_PATH:PATH=${TERRAMA2_DEPENDENCIES_DIR} -DCMAKE_INSTALL_PREFIX=$TERRAMA2_DEPENDENCIES_DIR > /dev/null 2>> ../build.log
  valid $? "Error: could not configure Quazip!"

  make -j 4 > /dev/null 2>> ../build.log
  valid $? "Error: could not make Quazip!"

  make install > /dev/null 2>> ../build.log
  valid $? "Error: Could not install Quazip!"

  cd .. > /dev/null

  fixRPath "$TERRAMA2_DEPENDENCIES_DIR/lib/libquazip5.dylib"  > /dev/null
fi

#
# VMime
#
if [ ! -f "$TERRAMA2_DEPENDENCIES_DIR/lib/libvmime.dylib" ]; then
  echo "Installing VMime..."
  sleep 1s
  log_lib "VMIME" 

  unzip -o vmime-master.zip &> /dev/null
  valid $? "Error: could not uncompress vmime-master.zip!"

  cd vmime-master > /dev/null
  valid $? "Error: could not enter vmime-master"

  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_MACOSX_RPATH:BOOL=YES -DCMAKE_PREFIX_PATH:PATH=/usr -DCMAKE_INSTALL_PREFIX=$TERRAMA2_DEPENDENCIES_DIR > /dev/null 2>> ../build.log
  valid $? "Error: could not configure VMime!"

  make -j 4 > /dev/null 2>> ../build.log
  valid $? "Error: could not make VMime!"

  make install > /dev/null 2>> ../build.log
  valid $? "Error: Could not install VMime!"

  cd .. > /dev/null

  fixRPath "$TERRAMA2_DEPENDENCIES_DIR/lib/libvmime.1.dylib"  > /dev/null
fi

#
# NodeJS
#
node --version > /dev/null
if [[ $? != 0 ]] ; then
  echo "Installing NodeJS..."
  sleep 1s

  curl -O https://nodejs.org/dist/v6.11.3/node-v6.11.3.pkg &> /dev/null
  valid $? "Error: could not download nodejs!"

  sudo installer -pkg node-v6.11.3.pkg -target /
  valid $? "Error: could not install nodejs!"
fi

#
# Finished!
#
clear
echo ""
echo "****************************************"
echo "* TerraMA2 Installer for Mac OS Sierra *"
echo "****************************************"
echo ""
echo "finished successfully!"
