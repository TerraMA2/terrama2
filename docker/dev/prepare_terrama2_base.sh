#!/bin/bash

#
# Valid parameter val or abort script
#
function valid()
{
  if [[ $1 -ne 0 ]]; then
    echo $2
    echo ""
    exit 1
  fi
}

sudo apt-get update
(
  cd ${DEPENDENCIES_DIR}
  sudo apt install doxygen graphviz gnutls-bin gsasl libghc-gsasl-dev libgnutls-dev libssl-dev debhelper devscripts
  wget https://github.com/kisli/vmime/archive/v0.9.2.tar.gz
  cmake -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE:STRING="Release" \
        -DVMIME_HAVE_MESSAGING_PROTO_SENDMAIL:BOOL=false \
        -DVMIME_BUILD_SAMPLES:BOOL=false \
        -DCMAKE_INSTALL_PREFIX:PATH=${DEPENDENCIES_DIR}
  make -j4
  make install

  rm -rf v0.9.2*
  rm -rf vmime*
)

(
  sudo apt-get install libcurl3-dev libpython2.7-dev libquazip-dev libxerces-c-dev libgeos++-dev libproj-dev
  mkdir -p ${TERRAMA2_DIR}/codebase
  cd ${TERRAMA2_DIR}/codebase
  # check if terrama2 code is available
  git status
  if [[ $1 -ne 0 ]]; then
    git clone https://github.com/TerraMA2/terrama2.git .
  fi
)

(
  mkdir -p ${TERRAMA2_DIR}/build-debug
  cd ${TERRAMA2_DIR}/build-debug
  # check terrama2 build
  cmake .
  if [[ $1 -ne 0 ]]; then
    cmake -G "CodeBlocks - Unix Makefiles" \
            -DCMAKE_BUILD_TYPE:STRING="Debug" \
            -DCMAKE_INSTALL_PREFIX:PATH=${DEPENDENCIES_DIR} \
            -DCMAKE_PREFIX_PATH:PATH=${DEPENDENCIES_DIR};${TERRALIB_DIR}/build-debug \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            ../codebase/build/cmake
    make -j4
  fi
)

(
  cd ${TERRAMA2_DIR}/codebase
  ln -s ${TERRAMA2_DIR}/build-debug/compile_commands.json .
)
