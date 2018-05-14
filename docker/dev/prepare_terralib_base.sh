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
  wget http://www.dpi.inpe.br/terralib5-devel/3rdparty/src/5.3/terralib-3rdparty-linux-ubuntu-16.04.tar.gz
  sudo apt-get install -y dpkg apt-utils
  TERRALIB_DEPENDENCIES_DIR=${DEPENDENCIES_DIR} ./install-3rdparty-linux-ubuntu-16.04.sh
  rm -rf terralib-3rdparty-linux-ubuntu-16.04*
)

(
  mkdir -p ${TERRALIB_DIR}/codebase
  cd ${TERRALIB_DIR}/codebase
  # check if terralib code is available
  git status
  if [[ $1 -ne 0 ]]; then
    GIT_SSL_NO_VERIFY=true git clone -b release-5.3 https://gitlab.dpi.inpe.br/terralib/terralib.git .
  fi
)

(
  mkdir -p ${TERRALIB_DIR}/build-debug
  cd ${TERRALIB_DIR}/build-debug
  # check terralib build
  cmake .
  if [[ $1 -ne 0 ]]; then
    cmake -G "CodeBlocks - Unix Makefiles" \
            -DCMAKE_BUILD_TYPE:STRING="Debug" \
            -DCMAKE_INSTALL_PREFIX:PATH=${DEPENDENCIES_DIR} \
            -DCMAKE_PREFIX_PATH:PATH=${DEPENDENCIES_DIR} \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            ../codebase/build/cmake
    make -j4
  fi
)

(
  cd ${TERRALIB_DIR}/codebase
  ln -s ${TERRALIB_DIR}/build-debug/compile_commands.json .
)
