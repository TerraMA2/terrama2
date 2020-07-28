#!/bin/bash

CURRENT_DIR=$(pwd)

cd /home/$USER

echo "************************"
echo "* Configuring TerraMA² *"
echo "************************"
echo ""

TERRAMA_ROOT_DIR="/home/$USER/mydevel/terrama2"
TERRAMA_BUILD_PATH="/home/$USER/mydevel/terrama2/build"
TERRAMA_3RD_PARTY_PATH="/home/$USER/mydevel/terrama2/3rdparty"
TERRAMA_MYLIBS_PATH="/home/$USER/mydevel/terrama2/mylibs"
TERRAMA_CODEBASE_PATH="/home/$USER/mydevel/terrama2/codebase"

mkdir -p $TERRAMA_ROOT_DIR
mkdir -p $TERRAMA_BUILD_PATH
mkdir -p $TERRAMA_3RD_PARTY_PATH
mkdir -p $TERRAMA_MYLIBS_PATH
mkdir -p $TERRAMA_CODEBASE_PATH

TERRALIB_ROOT_DIR="/home/$USER/mydevel/terrama2/terralib/"
TERRALIB_BUILD_PATH="/home/$USER/mydevel/terrama2/terralib/build"
TERRALIB_3RD_PARTY_PATH="/home/$USER/mydevel/terrama2/terralib/3rdparty"
TERRALIB_CODEBASE_PATH="/home/$USER/mydevel/terrama2/terralib/codebase"

mkdir -p $TERRALIB_ROOT_DIR
mkdir -p $TERRALIB_BUILD_PATH
mkdir -p $TERRALIB_3RD_PARTY_PATH
mkdir -p $TERRALIB_CODEBASE_PATH

echo "********************"
echo "* Installing CMake *"
echo "********************"
echo ""

if type -P cmake >/dev/null; then
    echo "Cmake already installed"
else
    wget -c https://github.com/Kitware/CMake/releases/download/v3.11.4/cmake-3.11.4-Linux-x86_64.sh
	sudo chmod +x cmake-3.11.4-Linux-x86_64.sh
	sudo ./cmake-3.11.4-Linux-x86_64.sh --skip-license --exclude-subdir --prefix=/usr/local
    rm -f cmake-3.11.4-Linux-x86_64.sh
fi

echo "***********************"
echo "* Installing packages *"
echo "***********************"
echo ""

sudo apt-get install -y apt-transport-https software-properties-common ca-certificates gnupg-agent

if type -P psql >/dev/null; then
    echo "Postgresql already installed"
else
    sudo sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt/ xenial-pgdg main" > /etc/apt/sources.list.d/pgdg.list'
    wget -qO- https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
    sudo apt-get update
    sudo apt-get install -y postgresql-server-dev-11 postgresql-11-postgis-2.5 pgadmin4
fi

if type -P code >/dev/null; then
    echo "VS Code already installed"
else
    wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg
    sudo install -o root -g root -m 644 microsoft.gpg /etc/apt/trusted.gpg.d/
    sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main" > /etc/apt/sources.list.d/vscode.list'
    rm -f microsoft.gpg
    sudo apt-get update
    sudo apt-get install -y code
fi

if type -P node >/dev/null; then
    echo "Nodejs already installed"
else
    wget -qO- https://deb.nodesource.com/gpgkey/nodesource.gpg.key | sudo apt-key add -
    sudo sh -c "echo 'deb https://deb.nodesource.com/node_8.x xenial main' > /etc/apt/sources.list.d/nodesource.list"
    sudo sh -c "echo 'deb-src https://deb.nodesource.com/node_8.x xenial main' >> /etc/apt/sources.list.d/nodesource.list"
    sudo apt-get update
    sudo apt-get install -y nodejs
fi

sudo apt-get update

sudo apt-get install -y qtcreator curl unzip locales supervisor libcurl3-dev libpython2.7-dev libproj-dev libgeos++-dev \
libssl-dev libxerces-c-dev screen doxygen graphviz gnutls-bin gsasl libgsasl7 libghc-gsasl-dev libgnutls-dev zlib1g-dev \
debhelper devscripts git ssh openssh-server libpq-dev openjdk-8-jdk build-essential python-psycopg2

export PATH=$PATH:/usr/lib/node_modules/npm/bin

sudo npm install -g grunt-cli

sudo chmod 755 -R /home/$USER/.npm

sudo chown $USER:$USER -R /home/$USER/.npm

export PATH=$PATH:/usr/lib/node_modules/grunt-cli/bin

sudo service postgresql start

sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD 'postgres'"

echo "********************"
echo "* Cloning projects *"
echo "********************"
echo ""

git clone -b b4.1.0 -o upstream https://github.com/TerraMA2/terrama2.git $TERRAMA_CODEBASE_PATH
GIT_SSL_NO_VERIFY=false git clone -o upstream -b 5.4.5 https://gitlab.dpi.inpe.br/terralib/terralib.git $TERRALIB_CODEBASE_PATH

echo "************"
echo "* TerraLib *"
echo "************"
echo ""

cd $TERRALIB_3RD_PARTY_PATH

if test -f "terralib-3rdparty-linux-ubuntu-16.04.tar.gz"; then
    echo "3RD Party already installed"
else 
	wget -c http://www.dpi.inpe.br/terralib5-devel/3rdparty/src/terralib-3rdparty-linux-ubuntu-16.04.tar.gz
	TERRALIB_DEPENDENCIES_DIR="$TERRAMA_MYLIBS_PATH" $TERRALIB_CODEBASE_PATH/install/install-3rdparty-linux-ubuntu-16.04.sh
fi

echo "*************"
echo "* Compiling *"
echo "*************"
echo ""

cd $TERRALIB_BUILD_PATH

cmake -G "CodeBlocks - Unix Makefiles" \
	-DCMAKE_PREFIX_PATH:PATH="$TERRAMA_MYLIBS_PATH" \
    -DTERRALIB_BUILD_AS_DEV:BOOL="ON" \
    -DTERRALIB_BUILD_EXAMPLES_ENABLED:BOOL="OFF" \
    -DTERRALIB_BUILD_UNITTEST_ENABLED:BOOL="OFF" $TERRALIB_CODEBASE_PATH/build/cmake

make -j $(($(nproc)/2))

echo "************"
echo "* TerraMA² *"
echo "************"
echo ""

cd $TERRAMA_3RD_PARTY_PATH

if test -f "terrama2-3rdparty.zip"; then
    echo "3RD Party already installed"
else 
	wget -c http://www.dpi.inpe.br/jenkins-data/terradocs/terrama2-3rdparty.zip
	unzip terrama2-3rdparty.zip
fi

echo "*************"
echo "* Compiling *"
echo "*************"
echo ""

cd $TERRAMA_BUILD_PATH

cmake -G "CodeBlocks - Unix Makefiles" \
	-DCMAKE_PREFIX_PATH:PATH="$TERRAMA_MYLIBS_PATH" \
	-DCMAKE_BUILD_TYPE:STRING="Debug" \
	-DCMAKE_SKIP_BUILD_RPATH:BOOL="OFF" \
	-DCMAKE_BUILD_WITH_INSTALL_RPATH:BOOL="OFF" \
	-DCMAKE_INSTALL_RPATH_USE_LINK_PATH:BOOL="ON" \
	-DCMAKE_PREFIX_PATH:PATH="$TERRAMA_MYLIBS_PATH" \
	-Dterralib_DIR:PATH="$TERRALIB_BUILD_PATH" \
	-DBoost_INCLUDE_DIR="$TERRAMA_MYLIBS_PATH/include" \
	-DQUAZIP_INCLUDE_DIR="$TERRAMA_3RD_PARTY_PATH/quazip-install/include/quazip" \
	-DQUAZIP_LIBRARIES="$TERRAMA_3RD_PARTY_PATH/quazip-install/lib/libquazip.so" \
	-DQUAZIP_LIBRARY_DIR="$TERRAMA_3RD_PARTY_PATH/quazip-install/lib" \
	-DQUAZIP_ZLIB_INCLUDE_DIR="$TERRAMA_3RD_PARTY_PATH/quazip-install/include" \
	-DVMIME_INCLUDE_DIR="$TERRAMA_3RD_PARTY_PATH/vmime-install/include" \
	-DVMIME_LIBRARY="$TERRAMA_3RD_PARTY_PATH/vmime-install/lib/libvmime.so" \
	-DVMIME_LIBRARY_DIR="$TERRAMA_3RD_PARTY_PATH/vmime-install/lib" $TERRAMA_CODEBASE_PATH/build/cmake

make -j $(($(nproc)/2))

echo "### Running npm install... ###"

echo "### Webapp... ###"

cd $TERRAMA_CODEBASE_PATH/webapp/
npm install
grunt
cd config
cp -a db.json.example db.json
cp -a settings.json.example settings.json

echo "### Webcomponents... ###"

cd $TERRAMA_CODEBASE_PATH/webcomponents/
npm install
grunt

echo "### Webmonitor... ###"

cd $TERRAMA_CODEBASE_PATH/webmonitor/
npm install
grunt
cd config/
cp -a sample_instances instances/

echo "### Installing Geoserver... ###"

cd /home/${USER}

if test -d "geoserver-2.12.5"; then
    echo "Geoserver already installed"
else 
    wget -O geoserver-2.12.5-bin.zip -L https://ufpr.dl.sourceforge.net/project/geoserver/GeoServer/2.12.5/geoserver-2.12.5-bin.zip
    unzip geoserver-2.12.5-bin.zip
    rm -f geoserver-2.12.5-bin.zip
fi

echo -e "
alias gs-start=\"cd ~/geoserver-2.12.5/bin;./startup.sh\"
alias adm-start=\"cd ~/mydevel/terrama2/codebase/webapp;npm start\"
alias monitor-start=\"cd ~/mydevel/terrama2/codebase/webmonitor;npm start\"" >> ~/.bashrc

source ~/.bashrc

cp -a $CURRENT_DIR/.vscode/ $TERRAMA_CODEBASE_PATH

sudo apt-get autoremove

sudo chown -R $USER:$USER ~/.config

sudo chown -R $USER:$USER ~/.npm
