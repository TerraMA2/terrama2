#!/bin/bash

function valid()
{
  if [ $1 -ne 0 ]; then
    echo "$2"
    exit $1
  else
    echo "done"
  fi
}

if [ "$1" == "compile" ]; then
  cd ${TERRAMA2_CODEBASE_PATH}/packages/debian-package

  export BUILD_DIR=`pwd`/../../../build-package
  ./deb-terrama2.sh

  mv $BUILD_DIR/*.deb .
  rm -rf $BUILD_DIR
else
  echo -ne "Downloading TerraMA² ... "
  curl -O http://www.dpi.inpe.br/jenkins-data/terrama2/installers/linux/TerraMA2-4.0.0-rc3-linux-x64-Ubuntu-14.04.deb --silent
  valid $? "Error: Could not fetch TerraMA²"
fi

echo "Installing TerraMA² ... "
dpkg -i TerraMA2-4.0.0-rc3-linux-x64-Ubuntu-14.04.deb 2> /dev/null

apt-get install -f -y
valid $? "Error: Could not install TerraMA²"

echo -ne "Configuring PostgreSQL ... "
sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD 'postgres'"
valid $? "Error: Could not configure postgresql. Check if it running."

# No error
exit 0