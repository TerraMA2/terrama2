#!/bin/bash

echo "* ------------------------ *"
echo "* TerraMa2 Release Package *"
echo "* ------------------------ *"
echo ""

#
# Set global session variables:
#

export TM_INSTALL=/opt/terrama2/master
export TL_PATH=/opt/terralib/5.4.5
export TM_OUT_DIR=`pwd`/../../../build-package
export CODEBASE=`pwd`/../..

# Where to find Vmime and Quazip libraries
if [ -z "$DEPENDENCIES" ]; then
  export DEPENDENCIES=`pwd`/../../../dependencies
fi

echo ""
echo "Check TerraLib Installation:"
echo ""

terralib_test=`dpkg -s terralib-5.4.5 | grep Status`

if [ "$terralib_test" != "Status: install ok installed" ]; then
  echo "TerraLib is not installed!"
  exit 1
else
  echo "TerraLib already installed!"
fi

echo ""
echo "Creating Folders..."
echo ""

mkdir -p $TM_OUT_DIR
cp terrama2.conf.cmake $TM_OUT_DIR
cd $TM_OUT_DIR

echo ""
echo "Configuring CMake..."
echo ""

cmake -G "Unix Makefiles" -C ./terrama2.conf.cmake -DCMAKE_BUILD_TYPE="Release" ${CODEBASE}/build/cmake

echo ""
echo "Making Package..."
echo ""

LD_LIBRARY_PATH=`pwd`/bin make package -j 4

echo "* ---------- *"
echo "* Finished ! *"
echo "* ---------- *"
echo ""
