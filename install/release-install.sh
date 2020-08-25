#!/bin/bash

wget -qO- https://deb.nodesource.com/gpgkey/nodesource.gpg.key | sudo apt-key add -
sudo sh -c "echo 'deb https://deb.nodesource.com/node_8.x xenial main' > /etc/apt/sources.list.d/nodesource.list"
sudo sh -c "echo 'deb-src https://deb.nodesource.com/node_8.x xenial main' >> /etc/apt/sources.list.d/nodesource.list"

sudo touch /etc/apt/sources.list.d/pgdg.list
sudo sh -c "echo 'deb http://apt.postgresql.org/pub/repos/apt/ xenial-pgdg main' > /etc/apt/sources.list.d/pgdg.list"
wget -qO- https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -

sudo apt-get update
sudo apt-get install -y cmake libquazip-dev zip unzip screen openssh-server doxygen supervisor locales libgsasl7 postgresql-11-postgis-2.5 postgresql-server-dev-11 nodejs python-psycopg2 gdal-bin openjdk-8-jdk

mkdir $HOME/.ssh
ssh-keygen -t rsa -b 4096 -C "terrama2-team@dpi.inpe.br" -N "" -f $HOME/.ssh/id_rsa <<< y

mkdir terralib-installer
cd terralib-installer
if [ ! -f "terralib-5.4.5-ubuntu-16.04.tar.gz" ]
then
    wget -q http://www.dpi.inpe.br/jenkins-data/terrama2/3rdparty/terralib-5.4.5-ubuntu-16.04.tar.gz
fi

tar xf terralib-5.4.5-ubuntu-16.04.tar.gz
./install.sh

cd ..

mkdir terrama2-deb

cd terrama2-deb
if [ ! -f "TerraMA2-master-linux-x64-Ubuntu-16.04.deb" ]
then
    wget -q http://www.dpi.inpe.br/jenkins-data/terrama2/installers/linux/final/TerraMA2-master-linux-x64-Ubuntu-16.04.deb
fi

sudo dpkg -i TerraMA2-master-linux-x64-Ubuntu-16.04.deb

sudo apt install -y -f

if [ ! -f "terrama2-doc-master.deb" ]
then
    wget -q http://www.dpi.inpe.br/jenkins-data/terrama2/installers/linux/final/terrama2-doc-master.deb
fi

sudo dpkg -i terrama2-doc-master.deb
