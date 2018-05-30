#!/bin/bash

# Change this variables according to your environment
export CODEBASE=`pwd`/../..
export TL_THIRD_DIR=$HOME/mylibs
export TM_THIRD_DIR=${TL_THIRD_DIR}
export TL_PATH=${TL_THIRD_DIR}/terralib-install/lib/cmake/terralib-5.3.1/
export PATH=$PATH:$HOME/Qt5.4.1/5.4/clang_64/bin:/Applications/CMake.app/Contents/bin

echo "* ------------------------ *"
echo "* TerraMa2 Release Package *"
echo "* ------------------------ *"
echo ""

export TM_INSTALL=${TE_THIRD_DIR}/terrama2-package
export TM_OUT_DIR=${CODEBASE}/../build-package

if [ -d "${TL_PATH}" ]; then

  mkdir -p ${TM_OUT_DIR}
  cp terrama2.conf.cmake ${TM_OUT_DIR}

  cd ${TM_OUT_DIR}

  cmake -G "Unix Makefiles" -C ./terrama2.conf.cmake -DCMAKE_BUILD_TYPE="Release" ${CODEBASE}/build/cmake
  make package -j 8

fi

echo "* ---------- *"
echo "* Finished ! *"
echo "* ---------- *"
echo ""
