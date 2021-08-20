# TerraMA² build instructions on Ubuntu 16.04

Use the following script to build it automatically: [terrama2-ubuntu-16-04-config.sh](install/terrama2-ubuntu-16-04-config.sh).

## Dependencies

First of all make sure that your machine has the following installed requirements, before proceeding:

```bash
sudo apt-get install -y curl libcurl3-dev doxygen git build-essential unzip locales supervisor libpython2.7-dev libproj-dev libgeos++-dev libssl-dev libxerces-c-dev screen graphviz gnutls-bin gsasl libgsasl7 libghc-gsasl-dev libgnutls-dev zlib1g-dev debhelper devscripts ssh openssh-server libpq-dev openjdk-8-jdk python-psycopg2
```

### Installing CMake

```bash
wget -c https://github.com/Kitware/CMake/releases/download/v3.11.4/cmake-3.11.4-Linux-x86_64.sh
sudo chmod +x cmake-3.11.4-Linux-x86_64.sh
sudo ./cmake-3.11.4-Linux-x86_64.sh --skip-license --exclude-subdir --prefix=/usr/local
rm -f cmake-3.11.4-Linux-x86_64.sh
```

### Installing PostgreSQL

```bash
sudo touch /etc/apt/sources.list.d/pgdg.list
sudo sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt/ xenial-pgdg main" > /etc/apt/sources.list.d/pgdg.list'
curl https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo apt-get update
sudo apt-get install -y postgresql-11-postgis-2.5 postgis postgresql-server-dev-11 pgadmin4
```

### Installing Nodejs

```bash
curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -
sudo apt-get -y install nodejs
```

### Installing QtCreator

```bash
sudo apt-get -y install qtcreator
```

### Installing VSCode

```bash
curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg
sudo install -o root -g root -m 644 microsoft.gpg /etc/apt/trusted.gpg.d/
sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main" > /etc/apt/sources.list.d/vscode.list'
rm -f microsoft.gpg
sudo apt-get install code
```

## Creating TerraMA² directories

```bash
mkdir -p /home/$USER/mydevel/terrama2
mkdir -p /home/$USER/mydevel/terrama2/build
mkdir -p /home/$USER/mydevel/terrama2/3rdparty
mkdir -p /home/$USER/mydevel/terrama2/mylibs
mkdir -p /home/$USER/mydevel/terrama2/codebase
```

## Building Terralib

You must build and install Terralib package required by TerraMA². Follow the next steps to do so. For more information access this link: [Build and install TerraLib Package](http://www.dpi.inpe.br/terralib5/wiki/doku.php?id=wiki:documentation:devguide#downloading_the_source_code_and_building_instructions).

### Creating directories

Create Terralib directories inside terrama2 folder.

```bash
mkdir -p /home/$USER/mydevel/terrama2/terralib
mkdir -p /home/$USER/mydevel/terrama2/terralib/build
mkdir -p /home/$USER/mydevel/terrama2/terralib/3rdparty
mkdir -p /home/$USER/mydevel/terrama2/terralib/mylibs
mkdir -p /home/$USER/mydevel/terrama2/terralib/codebase
```

### Cloning Terralib codebase

Clone the source code inside codebase folder.

```bash
cd /home/$USER/mydevel/terrama2/terralib/codebase
GIT_SSL_NO_VERIFY=false git clone -o upstream -b 5.4.5 https://gitlab.dpi.inpe.br/terralib/terralib.git .
```

### Download and compile Terralib dependencies

```bash
cd /home/$USER/mydevel/terrama2/terralib/3rdparty
wget -c http://www.dpi.inpe.br/terralib5-devel/3rdparty/src/terralib-3rdparty-linux-ubuntu-16.04.tar.gz TERRALIB_DEPENDENCIES_DIR="/home/$USER/mydevel/terrama2/mylibs" /home/$USER/mydevel/terrama2/terralib/codebase/install/install-3rdparty-linux-ubuntu-16.04.sh
```

### Compiling Terralib

```bash
cd /home/$USER/mydevel/terrama2/terralib/build
cmake -G "CodeBlocks - Unix Makefiles" \
	-DCMAKE_PREFIX_PATH:PATH="/home/$USER/mydevel/terrama2/mylibs" \
	-DTERRALIB_BUILD_AS_DEV:BOOL="ON" \
	-DTERRALIB_BUILD_EXAMPLES_ENABLED:BOOL="OFF" \
	-DTERRALIB_BUILD_UNITTEST_ENABLED:BOOL="OFF" /home/$USER/mydevel/terrama2/terralib/build/cmake

make -j $(($(nproc)/2))
```

## Building TerraMA²

### Cloning TerraMA² codebase

```bash
git clone -b b4.1.2 -o upstream https://github.com/TerraMA2/terrama2.git /home/$USER/mydevel/terrama2/codebase
```

### TerraMA² dependencies

```bash
wget -c http://www.dpi.inpe.br/jenkins-data/terradocs/terrama2-3rdparty.zip
unzip terrama2-3rdparty.zip
```

### Compiling TerraMA²

```bash
cd /home/$USER/mydevel/terrama2/build

cmake -G "CodeBlocks - Unix Makefiles" \
	-DCMAKE_PREFIX_PATH:PATH="/home/$USER/mydevel/terrama2/mylibs" \
	-DCMAKE_BUILD_TYPE:STRING="Debug" \
	-DCMAKE_SKIP_BUILD_RPATH:BOOL="OFF" \
	-DCMAKE_BUILD_WITH_INSTALL_RPATH:BOOL="OFF" \
	-DCMAKE_INSTALL_RPATH_USE_LINK_PATH:BOOL="ON" \
	-DCMAKE_PREFIX_PATH:PATH="/home/$USER/mydevel/terrama2/mylibs" \
	-Dterralib_DIR:PATH="/home/$USER/mydevel/terrama2/terralib/build" \
	-DBoost_INCLUDE_DIR="/home/$USER/mydevel/terrama2/mylibs/include" \
	-DQUAZIP_INCLUDE_DIR="/home/$USER/mydevel/terrama2/3rdparty/quazip-install/include/quazip" \
	-DQUAZIP_LIBRARIES="/home/$USER/mydevel/terrama2/3rdparty/quazip-install/lib/libquazip.so" \
	-DQUAZIP_LIBRARY_DIR="/home/$USER/mydevel/terrama2/3rdparty/quazip-install/lib" \
	-DQUAZIP_ZLIB_INCLUDE_DIR="/home/$USER/mydevel/terrama2/3rdparty/quazip-install/include" \
	-DVMIME_INCLUDE_DIR="/home/$USER/mydevel/terrama2/3rdparty/vmime-install/include" \
	-DVMIME_LIBRARY="/home/$USER/mydevel/terrama2/3rdparty/vmime-install/lib/libvmime.so" \
	-DVMIME_LIBRARY_DIR="/home/$USER/mydevel/terrama2/3rdparty/vmime-install/lib" /home/$USER/mydevel/terrama2/codebase/build/cmake

make -j $(($(nproc)/2))
```

## Webapp

### Running npm

```bash
cd /home/$USER/mydevel/terrama2/codebase/webapp/
npm install
```

### Running grunt

```bash
cd /home/$USER/mydevel/terrama2/codebase/webapp/
grunt
```

### Copying configuration files

```bash
cd /home/$USER/mydevel/terrama2/codebase/webapp/config
cp db.json.example db.json
cp settings.json.example settings.json
```

## Configuring Webcomponents

### Running npm

```bash
cd /home/$USER/mydevel/terrama2/codebase/webcomponents/
npm install
```

### Running grunt

```bash
cd /home/$USER/mydevel/terrama2/codebase/webcomponents/
grunt
```

## Configuring Webmonitor

### Running npm

```bash
cd /home/$USER/mydevel/terrama2/codebase/webmonitor/
npm install
```

### Running grunt

```bash
cd /home/$USER/mydevel/terrama2/codebase/webmonitor/
grunt
```

### Copying configuration files

```bash
cd /home/$USER/mydevel/terrama2/codebase/webmonitor/config
cp -r sample_instances instances/
```

## Installing Geoserver

```bash
cd /home/${USER}
wget -c https://ufpr.dl.sourceforge.net/project/geoserver/GeoServer/2.12.5/geoserver-2.12.5-bin.zip
unzip geoserver-2.12.5-bin.zip
rm -f geoserver-2.12.5-bin.zip
```
