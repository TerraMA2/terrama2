# TerraMA² - Build and Generate Package in Mac OS

## Build and Install TerraLib

First of all you should build and install TerraLib required by TerraMA², according to the link below:
 - [Build and install TerraLib](http://www.dpi.inpe.br/terralib5/wiki/doku.php?id=wiki:documentation:devguide#downloading_the_source_code_and_building_instructions).

## Cloning TerraMA² Repository

- Open the shell command line.

- Make a new folder to host TerraMA² source code:
```
$ mkdir -p /Users/USER/mydevel/terrama2/codebase
```

- Change the current directory to that new folder:
```
$ cd /Users/USER/mydevel/terrama2/codebase
```

- Make a local copy of TerraMA² repository:
```
$ git clone https://github.com/terrama2/terrama2.git .
```

- Then change to the branch you want to build, if necessary:
```
$ git checkout -b master origin/master
```

### Bash script for building all dependencies on Mac OS

We have prepared a special bash script for building and installing the dependencies on Mac OS. This script can be found in TerraMA² source tree under *install* folder. Follow the steps below:

- Download the third-party libraries package used by the development team:
  - [terrama2-3rdparty-macos.tar.gz](http://www.dpi.inpe.br/terrama2-devel/terrama2-3rdparty-macos-sierra.tar.gz).
  
- Open the shell command line in the same folder you have downloaded the 3rd-party package.

- Make sure your Qt and CMake environment can be found in your PATH:
```
$ export PATH=$PATH:/Users/USER/Qt5.4.1/5.4/clang_64/bin:/Applications/CMake.app/Contents/bin
```

- In the shell command line, call the script *install-3rdparty-macos-sierra.sh* (for Sierra and El-Capitan OS versions) setting the target to install all the stuffs from these third-party libraries and tools:
```
$ TERRAMA2_DEPENDENCIES_DIR=/Users/USER/mylibs  /Users/USER/mydevel/terrama2/codebase/install/install-3rdparty-macos-sierra.sh 
```

**Note:** Don't choose as target location a system folder such as */usr* or */usr/local*. Try some user specific folder. Replace the folder named *USER* by your user name.


## Build Instructions

After choosing the right branch or tag to work on, make sure you have all the third-party library dependencies needed before trying to build TerraMA².

The `build/cmake` folder contains a CMake project for building TerraMA².

You should use at least CMake version 2.8.12 for building TerraMA². Older versions than this may not work properly.

We will assume that the codebase (all the source tree) is located at: `/Users/USER/mydevel/terrama2/codebase`

- Change the current directory to that folder:
```
$ cd /Users/USER/mydevel/terrama2/codebase/packages/dmg-package
```

- Open the *dmg-terrama2.sh* script, with the code editor of your choice.

- Change the variables according to your environment:
* `TL_PATH`: path to installed terralib (path/to/terralib/lib/cmake/terralib-version/).
* `TL_THIRD_DIR`: path to TerraLib third-party.
* `TM_THIRD_DIR`: path to TerraMA² third-party (if not installed with TerraLib's third-party).
* `PATH`: should add Qt and CMake path to system's path.

- After check the script variables, in the shell command line, call the script *dmg-terrama2.sh* to build and generate automatically the TerraMA² package:
```
$ ./dmg-terralib
```

- When finished the generated package will be located in the `build-package` folder in the same directory as your codebase.

- To install the Drag-N-Drop package just double click it, accept the terms and drag the *terrama2-service* app to `Applications`.

### Quick Notes for Developers

If you have experienced  any problem building any of the third-party tool on Mac OS, try to install Xcode command line tools:
```
$ xcode-select --install
```

## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.

For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
