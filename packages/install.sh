#!/bin/bash
#
#  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of the TerraLib - a Framework for building GIS enabled applications.
#
#  TerraLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraLib. See COPYING. If not, write to
#  TerraLib Team at <terralib-team@terralib.org>.
#
#
#  Description: Install TerraLib and some required software on Linux Ubuntu 14.04.
#
#  Author: Carolina Galvão dos Santos
#
#
#  Example:
#  $ ./install.sh
#

echo "*********************************************"
echo "* TerraMa2 Installer for Linux Ubuntu 14.04 *"
echo "*********************************************"
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
# Update Ubuntu install list
#
sudo apt-get update
valid $? "Error: could not update apt-get database list"

#
# Install dpkg debian packages
#

#
# Boost version 1.60
#
sudo dpkg -i terralib-boost_1.60.0_amd64.deb
boost_test=`dpkg -s terralib-boost | grep Status`

if [ "$boost_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terralib-boost_1.60.0_amd64.deb
  valid $? "Error: could not install boost! Please, install readline: sudo dpkg -i terralib-boost_1.60.0_amd64.deb"
  echo "boost installed!"
else
  echo "boost already installed!"
fi

#
# GDAL/OGR 2.0.1
#
sudo dpkg -i terralib-gdal_2.0.1_amd64.deb
gdal2_test=`dpkg -s terralib-gdal | grep Status`

if [ "$gdal2_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terralib-gdal_2.0.1_amd64.deb
  valid $? "Error: could not install gdal2! Please, install readline: sudo dpkg -i terralib-gdal_2.0.1_amd64.deb"
  echo "gdal2 installed!"
else
  echo "gdal2 already installed!"
fi

#
# Qt Property Browser version 2.6.1
#
sudo dpkg -i terralib-qtpb_2.6.1_amd64.deb
qtpb_test=`dpkg -s terralib-qtpb | grep Status`

if [ "$qtpb_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terralib-qtpb_2.6.1_amd64.deb
  valid $? "Error: could not install qtpropertybrowser! Please, install readline: sudo dpkg -i terralib-qtpb_2.6.1_amd64.deb"
  echo "qtpropertybrowser installed!"
else
  echo "qtpropertybrowser already installed!"
fi

#
# Qwt version 6.1.2
#
sudo dpkg -i terralib-qwt_6.1.2_amd64.deb
qwt_test=`dpkg -s terralib-qwt| grep Status`

if [ "$qwt_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terralib-qwt_6.1.2_amd64.deb
  valid $? "Error: could not install qwt! Please, install readline: sudo dpkg -i terralib-qwt_6.1.2_amd64.deb"
  echo "qwt installed!"
else
  echo "qwt already installed!"
fi

#
# Wtss Cxx version 0.3.0
#
sudo dpkg -i terralib-wtss-cxx_0.3.0_amd64.deb
wtss_test=`dpkg -s terralib-wtss-cxx| grep Status`

if [ "$wtss_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terralib-wtss-cxx_0.3.0_amd64.deb
  valid $? "Error: could not install wtss-cxx! Please, install readline: sudo dpkg -i terralib-wtss-cxx_0.3.0_amd64.deb"
  echo "wtss-cxx installed!"
else
  echo "wtss-cxx already installed!"
fi

#
# TerraLib
#
sudo dpkg -i terralib-5.3.1.deb
terralib_test=`dpkg -s terralib-5.3.1 | grep Status`

if [ "$terralib_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terralib-5.3.1.deb
  valid $? "Error: could not install terralib! Please, install readline: sudo dpkg -i terralib-5.3.1.deb"
  echo "terralib installed!"
else
  echo "terralib already installed!"
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
# VMime
#
sudo dpkg -i terrama2-vmime_1.0.0_amd64.deb
terrama2_test=`dpkg -s terrama2-vmime_1.0.0_amd64 | grep Status`

if [ "$terrama2_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terrama2-vmime_1.0.0_amd64.deb
  valid $? "Error: could not install VMime! Please, install readline: sudo dpkg -i terrama2-vmime_1.0.0_amd64.deb"
  echo "VMime installed!"
else
  echo "VMime already installed!"
fi


#
# TerraMa2 version 4.0.0
#
sudo dpkg -i terrama2-4.0.0-beta2-x64.deb
terrama2_test=`dpkg -s terrama2-4.0.0-beta2 | grep Status`

if [ "$terrama2_test" != "Status: install ok installed" ]; then
  sudo apt-get install -y -f
  sudo dpkg -i terrama2-4.0.0-beta2-x64.deb
  valid $? "Error: could not install terrama2! Please, install readline: sudo dpkg -i terrama2-4.0.0-beta2-x64.deb"
  echo "terrama2 installed!"
else
  echo "terrama2 already installed!"
fi

#
# Finished!
#
echo "*********************************************"
echo "* TerraMa2 Installer for Linux Ubuntu 14.04 *"
echo "*********************************************"
echo ""
echo "finished successfully!"
