# TerraMA² - Build and Generate Package in Windows

## Dependencies that require manual installation on Windows

First of all make sure that your machine has the following installed requirements, before proceeding:
* [Git](https://git-scm.com/downloads)
* [CMake (versions newer than 3.0.0)](https://cmake.org/download/)
* [Qt (versions newer than 5.4.0)](https://download.qt.io/archive/qt/)
* [Visual Studio 2015](https://www.visualstudio.com/pt-br/)
**Note:** There is no support for Visual Studio 2017 yet, comming soon.

Optional installations (necessary only after package installation):
* [Postgresql](https://postgresapp.com)
* [NodeJs](https://nodejs.org/en/)


## Build and Install TerraLib

You should build and install TerraLib required by TerraMA², according to the link below:
 - [Build and install TerraLib](http://www.dpi.inpe.br/terralib5/wiki/doku.php?id=wiki:documentation:devguide#downloading_the_source_code_and_building_instructions).


## Cloning TerraMA² Repository

1.1. Open the prompt command line.

1.2. Make a new folder to host TerraMA² source code:
```
$ mkdir -p C:\mydevel\terrama2\codebase
```

1.3. Change the current directory to that new folder:
```
$ cd C:\mydevel\terrama2\codebase
```

1.4. Make a local copy of TerraMA² repository:
```
$ git clone https://github.com/terrama2/terrama2.git .
```

1.5. Then change to the branch you want to build, if necessary:
```
$ git checkout -b master origin/master
```

We will assume that the codebase (all the source tree) is located at: `C:\mydevel\terrama2\codebase`

## Batch script for building all dependencies on Windows

1.1. Download and extract the third-party libraries used by the development team:
  - [terrama2-3rdparty-msvc-2015-win64.tar.gz](terrama2-3rdparty-msvc-2015-win64).
  
## Build Instructions

After choosing the right branch or tag to work on, make sure you have all the third-party library dependencies needed before trying to build TerraMA².

The `build/cmake` folder contains a CMake project for building TerraMA².

You should use at least CMake version 3.0.0 for building TerraMA². Older versions than this may not work properly.

### Development Environment

1.1. Open a Command Prompt.

1.2. Create a folder out of the TerraMA² source tree to generate the build system, for example:
```
$ cd C:\mydevel\terrama2
$ mkdir build-release
$ cd build-release
```
**Note:** for the sake of simplicity create this directory in the same level as the source tree (as showned above).

1.3. Make sure your Qt and CMake environment can be found in your system PATH:
```
$ setx path "%path%;C:\Qt\5.10.0\msvc2015_64\bin;C:\Program Files\CMake\bin"
```

1.4. Call this command to configure your Visual Studio environment:
```
call "%PROGRAMFILES(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat x64
```

1.5. For Windows systems you must choose the build configuration:
```
$ cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="C:\mylibs-terrama2\terrama2" -DCMAKE_PREFIX_PATH:PATH="C:\terralib5-3rdparty-msvc-2017-win64;C:\terrama2-3rdparty-msvc-2015-win64;C:\Qt\5.10.0\msvc2015_64\lib\cmake" ..\codebase\build\cmake
```

1.6. Building TerraMA² in prompt:
```
$ msbuild /m ALL_BUILD.vcxproj /p:Configuration=Debug
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
- You can set inside Visual Studio some environment variables to run an application. For examples, where to find your libs:
```
PATH=C:\terralib5-3rdparty-msvc-2017-win64\lib;C:\terrama2-3rdparty-msvc-2015-win64\lib;C:\Qt\Qt5.10.0\5.10.0\msvc2017_64\bin;${Path}
```

### Package

1.1. Change the current directory to that folder:
```
$ cd C:\mydevel\terrama2\codebase\packages\nsis-package
```

1.2. Open the *nsis-terrama2-msvc14.bat* script, with the code editor of your choice.

1.3. Change the variables according to your environment:
* `TERRALIB_INSTALL_DIR`: path to installed terralib.
* `TERRALIB_DEPENDENCIES_DIR`: path to TerraLib third-party.
* `TERRAMA2_DEPENDENCIES_DIR`: path to TerraMA² third-party (if not installed with TerraLib's third-party).
* `QMAKE_FILEPATH`: path to Qt executables.
* `VCVARS_FILEPATH`: path to "vcvarsall.bat". 
* `CMAKE_FILEPATH`: path to CMake executables.

1.4. After check the script variables, in the prompt command line, call the script *nsis-terrama2-msvc14.bat* to build and generate automatically the TerraMA² package:
```
$ .\nsis-terrama2-msvc14.bat
```

1.5. When finished the generated package will be located in the `build-package` folder in the same directory as your codebase.

1.6. To install the package just double click it and accept the terms.

### Quick Notes for Developers



## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.

For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
