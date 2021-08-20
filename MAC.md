# TerraMA² - Build and Generate Package in Mac OS

## Dependencies that require manual installation on Mac OS

First of all make sure that your machine has the following installed requirements, before proceeding:
* [Git](https://git-scm.com/downloads)
* [CMake (preferably version 3.0.2)](https://cmake.org/download/)
* [Qt (preferably version 5.4.1)](https://download.qt.io/archive/qt/)
* [Postgresql](https://postgresapp.com)
* Xcode Command Line Tools (call in shell command line):
```
$ xcode-select --install
```


## Build and Install TerraLib

You should build and install TerraLib required by TerraMA², according to the link below:
 - [Build and install TerraLib](http://www.dpi.inpe.br/terralib5/wiki/doku.php?id=wiki:documentation:devguide#downloading_the_source_code_and_building_instructions).

## Cloning TerraMA² Repository

1.1. Open the shell command line.

1.2. Make a new folder to host TerraMA² source code:
```
$ mkdir -p /Users/USER/mydevel/terrama2/codebase
```

1.3. Change the current directory to that new folder:
```
$ cd /Users/USER/mydevel/terrama2/codebase
```

1.4. Make a local copy of TerraMA² repository:
```
$ git clone https://github.com/terrama2/terrama2.git .
```

1.5. Then change to the branch you want to build, if necessary:
```
$ git checkout -b master origin/master
```

We will assume that the codebase (all the source tree) is located at: `/Users/USER/mydevel/terrama2/codebase`

## Bash script for building all dependencies on Mac OS

We have prepared a special bash script for building and installing the dependencies on Mac OS. This script can be found in TerraMA² source tree under *install* folder. Follow the steps below:

1.1. Download the third-party libraries source used by the b4.1.2ment team:
  - [quazip-0.7.3.tar.gz](https://sourceforge.net/projects/quazip/files/latest/download).
  - [vv0.9.2.tar.gz](https://github.com/kisli/vmime/archive/v0.9.2.tar.gz).
  
1.2. Open the shell command line in the same folder you have downloaded the third-party sources.

1.3. Make sure your Qt and CMake environment can be found in your PATH:
```
$ export PATH=$PATH:/Users/USER/Qt5.4.1/5.4/clang_64/bin:/Applications/CMake.app/Contents/bin
```

1.4. In the shell command line, call the script *install-3rdparty-macos-sierra.sh* (for Sierra and El-Capitan OS versions) setting the target to install all the stuffs from these third-party libraries and tools:
```
$ TERRAMA2_DEPENDENCIES_DIR=/Users/USER/mylibs  /Users/USER/mydevel/terrama2/codebase/install/install-3rdparty-macos-sierra.sh 
```

**Note:** Don't choose as target location a system folder such as */usr* or */usr/local*. Try some user specific folder. Replace the folder named *USER* by your user name.

## Build Instructions

After choosing the right branch or tag to work on, make sure you have all the third-party library dependencies needed before trying to build TerraMA².

The `build/cmake` folder contains a CMake project for building TerraMA².

You should use at least CMake version 2.8.12 for building TerraMA². Older versions than this may not work properly.

### Development Environment

1.1. Open a Command Prompt (Shell).

1.2. Create a folder out of the TerraMA² source tree to generate the build system, for example:
```
$ cd /Users/USER/mydevel/terrama2
$ mkdir build-release
$ cd build-release
```
**Note:** for the sake of simplicity create this directory in the same level as the source tree (as showned above).

1.3. For Mac OS systems you must choose the build configuration:
```
$ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="/Users/USER/mylibs/terrama2" -DCMAKE_PREFIX_PATH:PATH="/Users/USER/mylibs;/Users/USER/mylibs/terralib5/lib/cmake;/Users/USER/Qt5.4.1/5.4/clang_64/lib/cmake" ../codebase/build/cmake
```

**Note:** Please, in the cmake call above, take special attention to the key *CMAKE_PREFIX_PATH* and Qt location.

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

- You have to specify valid paths for *CMAKE_PREFIX_PATH*. If you have a Qt version installed as a framework in your home directory, you have to tell CMake where to locate its CMake support. For instance, if you have Qt version 5.4.1 installed, you have to add to *CMAKE_PREFIX_PATH* the following directory:
```
/Users/USER/Qt5.4.1/5.4/clang_64/lib/cmake
```
- You have also to tell where TerraLib? CMake support is located. Add to *CMAKE_PREFIX_PATH* where TerraLib? is installed, for example:
```
/Users/USER/mylibs/terralib/lib/cmake
```
- You can also generate an Xcode project by using the "Xcode generator" option:
```
-G "Xcode"
```
- There are some useful variables that can be set inside Xcode in order to run an application. The following environment variable can be set:
```
DYLD_FALLBACK_LIBRARY_PATH
DYLD_FALLBACK_FRAMEWORK_PATH
```

### Package

1.1. Change the current directory to that folder:
```
$ cd /Users/USER/mydevel/terrama2/codebase/packages/dmg-package
```

1.2. Open the *dmg-terrama2.sh* script, with the code editor of your choice.

1.3. Change the variables according to your environment:
* `TL_PATH`: path to installed terralib (path/to/terralib/lib/cmake/terralib-version/).
* `TL_THIRD_DIR`: path to TerraLib third-party.
* `TM_THIRD_DIR`: path to TerraMA² third-party (if not installed with TerraLib's third-party).
* `PATH`: should add Qt and CMake path to system's path.

1.4. After check the script variables, in the shell command line, call the script *dmg-terrama2.sh* to build and generate automatically the TerraMA² package:
```
$ ./dmg-terralib.sh
```

1.5. When finished the generated package will be located in the `build-package` folder in the same directory as your codebase.

1.6. To install the Drag-N-Drop package just double click it, accept the terms and drag the *terrama2-service* app to `Applications`.

### Quick Notes for Developers



## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.

For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
