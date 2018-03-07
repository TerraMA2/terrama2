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
#  Description: Uninstall TerraLib and some required software on Linux Ubuntu 14.04.
#
#  Author: Carolina Galvão dos Santos
#
#
#  Example:
#  $ ./uninstall.sh
#

echo "***********************************************"
echo "* TerraLib Uninstaller for Linux Ubuntu 14.04 *"
echo "***********************************************"
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
# Uninstall dpkg debian packages
#

#
# TerraMa2 version 4.0.0
#
terrama2_test=`dpkg -s terrama2-4.0.0-alpha6 | grep Status`

if [ "$terrama2_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terrama2-4.0.0-alpha6
  valid $? "Error: could not uninstall terrama2!"
  echo "terrama2 uninstalled!"
else
  echo "terrama2 already uninstalled!"
fi

#
# Node.js
#
nodejs_test=`dpkg -s nodejs | grep Status`

if [ "$nodejs_test" == "Status: install ok installed" ]; then
  sudo dpkg -r nodejs
  valid $? "Error: could not uninstall nodejs!"
  echo "nodejs uninstalled!"
else
  echo "nodejs already uninstalled!"
fi

#
# TerraLib
#
terralib_test=`dpkg -s terralib-5.3.1 | grep Status`

if [ "$terralib_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terralib-5.3.1
  valid $? "Error: could not uninstall terralib!"
  echo "terralib uninstalled!"
else
  echo "terralib already uninstalled!"
fi

#
# Boost version 1.60
#
boost_test=`dpkg -s terralib-boost | grep Status`

if [ "$boost_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terralib-boost
  valid $? "Error: could not uninstall boost!"
  echo "boost uninstalled!"
else
  echo "boost already uninstalled!"
fi

#
# GDAL/OGR 2.0.1
#
gdal2_test=`dpkg -s terralib-gdal2 | grep Status`

if [ "$gdal2_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terralib-gdal2
  valid $? "Error: could not uninstall gdal2!"
  echo "gdal2 uninstalled!"
else
  echo "gdal2 already uninstalled!"
fi

#
# Qt Property Browser version 2.6.1
#
qtpb_test=`dpkg -s terralib-qtpb | grep Status`

if [ "$qtpb_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terralib-qtpb
  valid $? "Error: could not uninstall qtpropertybrowser!"
  echo "qtpropertybrowser uninstalled!"
else
  echo "qtpropertybrowser already uninstalled!"
fi

#
# Qwt version 6.1.2
#
qwt_test=`dpkg -s terralib-qwt| grep Status`

if [ "$qwt_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terralib-qwt
  valid $? "Error: could not uninstall qwt!"
  echo "qwt uninstalled!"
else
  echo "qwt already uninstalled!"
fi

#
# Wtss Cxx version 0.3.0
#
wtss_test=`dpkg -s terralib-wtss-cxx| grep Status`

if [ "$wtss_test" == "Status: install ok installed" ]; then
  sudo dpkg -r terralib-wtss-cxx
  valid $? "Error: could not uninstall wtss-cxx!"
  echo "wtss-cxx uninstalled!"
else
  echo "wtss-cxx already uninstalled!"
fi

#
# Automatically removes unused libraries
#
sudo apt-get autoremove -y

#
# Finished!
#
echo "***********************************************"
echo "* TerraLib Uninstaller for Linux Ubuntu 14.04 *"
echo "***********************************************"
echo ""
echo "finished successfully!"
