# TerraMA² - Build and Generate Package in Linux

## Dependencies that require manual installation on Linux

First of all make sure that your machine has the following installed requirements, before proceeding:
* Git (call in shell command line):
```
$ sudo apt-get install git
```
* Build-Essentials (call in shell command line):
```
$ sudo apt-get install build-essential
```
* [CMake (preferably version 3.0.2)](https://cmake.org/download/) or call in shell command line:
```
$ sudo apt-get install cmake-gui
```

## Build and Install TerraLib

You should build and install TerraLib package required by TerraMA², according to the link below:
 - [Build and install TerraLib Package](http://www.dpi.inpe.br/terralib5/wiki/doku.php?id=wiki:documentation:devguide#downloading_the_source_code_and_building_instructions).

## Cloning TerraMA² Repository

1.1. Open the shell command line.

1.2. Make a new folder to host TerraMA² source code:
```
$ mkdir -p /home/USER/mydevel/terrama2/codebase
```

1.3. Change the current directory to that new folder:
```
$ cd /home/USER/mydevel/terrama2/codebase
```

1.4. Make a local copy of TerraMA² repository:
```
$ git clone https://github.com/terrama2/terrama2.git .
```

1.5. Then change to the branch you want to build, if necessary:
```
$ git checkout -b master origin/master
```

We will assume that the codebase (all the source tree) is located at: `/home/USER/mydevel/terrama2/codebase`

## Building dependencies on Linux

To compile TerraMA² from a fresh git clone you'll need:

- libcurl3-dev
- libpython2.7-dev
- libquazip0-dev
- libproj-dev
- libgeos++-dev
- libxerces-c-dev
- screen
- postgresql-9.3-postgis-2.1

All can be installed with the command:

Ubuntu 14.04
```
$ sudo apt-get install curl libcurl3-dev libpython2.7-dev libquazip0-dev libproj-dev libgeos++-dev libxerces-c-dev screen postgresql-9.3-postgis-2.1
```

Ubuntu 16.04
```
$ sudo apt-get install curl libcurl3-dev libpython2.7-dev libquazip-dev libproj-dev libgeos++-dev libxerces-c-dev screen postgresql-9.5-postgis-2.2
````

We also need the VMime library, as the ubuntu version is outdated we should use the [VMime](https://github.com/kisli/vmime/archive/v0.9.2.tar.gz) source version.

Or generate the VMime debian package (especially if you want to generate the TerraMA² package) as shown below:

### VMime

1.1. Install the vmime and debian package dependencies with the command:
```
$ sudo apt install doxygen graphviz gnutls-bin gsasl libghc-gsasl-dev libgnutls-dev libssl-dev debhelper devscripts cmake
```

1.2. Download the VMime source used by the development team, from the link below:
  - [vv0.9.2.tar.gz](https://github.com/kisli/vmime/archive/v0.9.2.tar.gz).

1.3. Open the shell command line and change the current directory to that folder:
```
$ cd /home/USER/mydevel/terrama2/codebase/packages/deb-package
```

1.4. Copy the downloaded source to current directory.

1.5. In the shell command line, call the script *deb-vmime.sh* that will generate the debian package for VMime:
```
$ ./deb-vmime.sh
```

1.6. When the script finishes, just run the following command to install VMime package:
```
$ sudo dpkg -i terrama2-vmime_0.9.2-ubuntu16.04_amd64.deb
```

1.7. Then check and install any missing library, finishing the dependencies installation with the command:
```
$ sudo apt install -f
```

### NodeJs

The NodeJs can be installed with the commands:
```
$ curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
$ sudo apt-get install -y nodejs
```

## Build Instructions

After choosing the right branch or tag to work on, make sure you have all the third-party library dependencies needed before trying to build TerraMA².

The `build/cmake` folder contains a CMake project for building TerraMA².

You should use at least CMake version 2.8.12. Older versions than this may not work properly.

### Development Environment

1.1. Open a Command Prompt (Shell).

1.2. Create a folder out of the TerraMA² source tree to generate the build system, for example:
```
$ cd /home/USER/mydevel/terrama2
$ mkdir build-release
$ cd build-release
```
**Note:** for the sake of simplicity create this directory in the same level as the source tree (as showned above).

1.3. For Linux systems you must choose the build configuration:
```
$ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="/home/USER/myinstall/terrama2" -DCMAKE_PREFIX_PATH:PATH="/home/USER/mylibs;/home/USER/mylibs/terralib5/lib/cmake" ../codebase/build/cmake
```

1.4. Building (with 4 process in parallel):
```
$ make -j 4
```

1.5. Installing:
```
$ make install
```

1.6. Uninstalling:
```
$ make uninstall
```

**Notes:**

- Some Linux flavours with different versions of GNU gcc and Boost will need more parameters such as:
```
  -DCMAKE_INCLUDE_PATH:PATH="/usr/local;/opt/include"
  -DCMAKE_LIBRARY_PATH:PATH="/usr/local;/opt/lib"
  -DCMAKE_PROGRAM_PATH:PATH="/usr/local/bin;/opt/bin"
  -DBOOST_ROOT:PATH="/opt/boost"
```

- Boost can also be indicated by *BOOST_INCLUDEDIR*:
```
  -DBOOST_INCLUDEDIR:PATH="/usr/local/include"
```

- The parameter -lpthread must be informed only if your Boost was not built as a shared library:
```
  -DCMAKE_CXX_FLAGS:STRING="-lpthread"
```

- For building with Qt5 you can provide the *Qt5_DIR* variable as:
```
  -DQt5_DIR:PATH="/usr/local/lib/cmake/Qt5"
```

- For generating a debug version set *CMAKE_BUILD_TYPE* as:
```
  -DCMAKE_BUILD_TYPE:STRING="Debug"
```

### Package

1.1 Change the current directory to that folder:
```
$ cd /home/USER/mydevel/terrama2/codebase/packages/deb-package
```

1.2. In the shell command line, call the script *deb-terrama2.sh* to build and generate automatically the TerraMA² package:
```
$ ./deb-terrama2.sh
```

**Note:** the script will assume that you installed terralib (and dependencies) at `/opt/terralib/5.2.2`.

1.3. When finished the generated package will be located in the `build-package` folder in the same directory as your codebase.

1.4. To install TerraMA² debian package in the shell command line change the current directory to the `build-package` folder and run the following command:
```
$ sudo dpkg -i TerraMA2-4.0.1-release-linux-x64-Ubuntu-16.04.deb
```

### Quick Notes for Developers

- For Linux, you can set the following variable:
```
$ export LD_LIBRARY_PATH=/home/USER/mylibs/lib
```

- If you want to use QtCreator on Linux you can install it through the following command:
```
$ sudo apt-get install qtcreator
```

## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.

For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
