# TerraMA²

Repository Health  | Status
------------------ | -----------------------------------------------------------------
 Build             | [![Build Status](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-build-master/badge/icon)](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-build-master/)
 Cppcheck          | [![Build Status](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-cppcheck-linux-ubuntu-14.04/badge/icon)](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-cppcheck-linux-ubuntu-14.04/)
 JSHint            | [![Build Status](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-jscheck-linux-ubuntu-14.04/badge/icon)](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-jscheck-linux-ubuntu-14.04/)


**NOTE:**
* **Until we reach version 4.0.0 this codebase will be unstable and not fully operational.**
* **TerraMA² is under active development. We are preparing this site to host it!**
* **If you want to try the old version, please, look at http://www.dpi.inpe.br/terrama2.**
* **If you have any question, please, send us an e-mail at: terrama2-team@dpi.inpe.br.**

TerraMA² is a free and open source computational platform for early warning systems.

You can build applications to monitor, analyze and issue early warnings related to air quality, water quality, pipelines, tailings dams for mining, forest fires, landslides and debris-mud flows, floods and droughts.

Through a service-oriented architecture, TerraMA² can integrates geospatial data from different web services.

It takes advantage of the datasets available on the Internet to have access to real-time geo-environmental data (meteorological, climatic, atmospheric, hydrological, geotechnical and socio-demographic).

Besides that, it provides support for building models to several application domains through scriptable languages.

The computational platform is based on a service-oriented architecture, which is open and provides the technological infrastructure required to develop and implement operating systems to monitor early warnings of environmental risks.

## Source Code Instructions

In the root directory of TerraMA² codebase (the source code tree) there are some text files explaining the details of the codebase:

- **[BRANCHES-AND-TAGS:](https://github.com/TerraMA2/terrama2/blob/master/BRANCHES-AND-TAGS)** Notes on how to switch to the right branch to work on or the right tag to get the source code.

- **[BUILD-INSTRUCTIONS:](https://github.com/TerraMA2/terrama2/blob/master/BUILD-INSTRUCTIONS)** Notes on how to compile and install TerraMA² for each platform.

- **CHANGELOG:** List of changes in TerraMA² source code. **Not available yet!**

- **[DEPENDENCIES:](https://github.com/TerraMA2/terrama2/blob/master/DEPENDENCIES)** The list of third-party library you must install before building TerraMA².

- **[LICENSE:](https://github.com/TerraMA2/terrama2/blob/master/LICENSE)** Licence statement in plain txt format.

- **[README:](https://github.com/TerraMA2/terrama2/blob/master/README)** Contains instructions about how to build and how is organized TerraMA² plataform source code.

If you want to build TerraMA² from source, first take a look at the section **Dependencies** (below in this document) and read the right tip for automatically building the dependencies in your platform.

## Source Code Organization

- **[build/cmake:](https://github.com/TerraMA2/terrama2/tree/master/build/cmake)** Contains the CMake scripts with commands, macros and functions used to build the environment for compiling libraries and executables in different platforms using the CMake tool.

- **[examples:](https://github.com/TerraMA2/terrama2/tree/master/examples)** Some examples on how to use TerraMA² API and GUI.

- **[install:](https://github.com/TerraMA2/terrama2/tree/master/install)** Bash scripts for helping building and installing TerraMA².

- **licenses:** Copyright notices of third-party libraries used by TerraMA². **Not available yet!**

- **[resources:](https://github.com/TerraMA2/terrama2/tree/master/resources)** Fonts, images, sql, and xml files among other resources of general use.

- **[share:](https://github.com/TerraMA2/terrama2/tree/master/share)** XML Schema (.xsd), JSON files, plugin manifest files, translations files and OGC specifications that is shared and installed with TerraMA².

- **[src:](https://github.com/TerraMA2/terrama2/tree/master/src)** Contains the source code of TerraMA² and its automatic test system.

- **[tool:](https://github.com/TerraMA2/terrama2/tree/master/tools)** Contains some helper tools for managig TerraMA² code.

## C++ Dependencies

The file named **[DEPENDENCIES](https://github.com/TerraMA2/terrama2/blob/master/DEPENDENCIES)** in the root of TerraMA² source tree contains the official list of third-party libraries and tools that you must install before building TerraMA² from source.

If you want to build yourself TerraMA² then you need to install some third-party libraries. Below we show the list of third-party libraries dependencies and its versions:
- **Boost (Mandatory):** TerraMA² is built on top of Boost libraries. You will need to have them installed in order to build TerraMA². Make sure to have at least version 1.54.0 installed. If you prefer to install from source, download it from: http://www.boost.org.

- **Qt (Mandatory):** Make sure you have an installed Qt version 5.2.1 or later. Linux users may use any package manager to perform an easy installation. Mac OS X can use package managers such as Homebrew (http://brew.sh) or MacPorts (http://www.macports.org) in order to have an easy installation. If you prefer to install from source, download it from: http://qt-project.org/downloads.

- **TerraLib (Mandatory):** TerraMA² is built on top of TerraLib, a free and open source library for building GIS enabled applications. Make sure to have at least TerraLib version 5.1.0. You can download it from: http://www.dpi.inpe.br/terralib5.

- **libCURL (Mandatory):** libcurl is a multiprotocol file transfer library. Make sure you have at least version 7.42.1 in your system. You can download it from: http://curl.haxx.se/libcurl.

- **Google Test (Optional):** Google Test is the Google's C++ test framework. You can download it from: https://github.com/google/googletest.

## Web Application and Components Dependencies

All the web application dependencies and install steps you can find in: https://github.com/TerraMA2/terrama2/tree/master/webapp#terrama-webapp.

The file named **[DEPENDENCIES](https://github.com/TerraMA2/terrama2/blob/master/DEPENDENCIES)** in the root of TerraMA² source tree contains the official list of third-party libraries and tools that you must install before building TerraMA² from source.

### Bash script for building all dependencies on Linux Ubuntu 14.04

We have prepared a special bash script for building and installing the dependencies on Linux Ubuntu 14.04. This script can be found in TerraMA² source tree under *install* folder. Follow the steps below:

- Download the third-party libraries package used by the development team: [terrama2-3rdparty-linux-ubuntu-14.04.tar.gz](http://www.dpi.inpe.br/terrama2-devel/terrama2-3rdparty-linux-ubuntu-14.04.tar.gz).

- Copy the script [install-3rdparty-linux-ubuntu-14.04.sh](https://raw.githubusercontent.com/TerraMA2/terrama2/master/install/install-3rdparty-linux-ubuntu-14.04.sh) to the same folder you have downloaded the *terrama2-3rdparty-linux-ubuntu-14.04.tar.gz* package.

- Open the shell command line and call the script *install-3rdparty-linux-ubuntu-14.04.sh* setting the target to install all the stuffs from these third-party libraries and tools:
```
$ ./install-3rdparty-linux-ubuntu-14.04.sh /home/USER/MyLibs /home/USER/MyDevel/terrama2/codebase
```

**Note:** Don't choose as target location a system folder such as */usr* or */usr/local*. Try some user specifiic folder. The best suggestion is to replace the folder named *user* by your user name.

### Bash script for building all dependencies on Mac OS X

We have prepared a special bash script for building and installing the dependencies on Mac OS X. This script can be found in TerraMA² source tree under *install* folder. Follow the steps below:

- Download the third-party libraries package used by the development team:
  - [terrama2-3rdparty-macosx-yosemite.tar.gz](http://www.dpi.inpe.br/terrama2-devel/terrama2-3rdparty-macosx-yosemite.tar.gz).
  - [terrama2-3rdparty-macosx-el-capitan.tar.gz](http://www.dpi.inpe.br/terrama2-devel/terrama2-3rdparty-macosx-el-capitan.tar.gz).

- Copy one of the scripts to the same folder you have downloaded the 3rd-party package:
  - [install-3rdparty-macosx-yosemite.sh](https://raw.githubusercontent.com/TerraMA2/terrama2/master/install/install-3rdparty-macosx-yosemite.sh)
  - [install-3rdparty-macosx-el-capitan.sh](https://raw.githubusercontent.com/TerraMA2/terrama2/master/install/install-3rdparty-macosx-el-capitan.sh)

- Open the shell command line.

- Make sure your Qt and CMake environment can be found in your PATH:
```
$ export PATH=$PATH:/Users/user/Qt5.4.1/5.4/clang_64/bin:/Applications/CMake.app/Contents/bin
```

- In the shell command line, call the script *install-3rdparty-macosx-yosemite.sh* (or the El-Capitan one) setting the target to install all the stuffs from these third-party libraries and tools:
```
$ ./install-3rdparty-macosx-yosemite.sh /home/USER/MyLibs /home/USER/MyDevel/terrama2/codebase
```

**Note:** Don't choose as target location a system folder such as */usr* or */usr/local*. Try some user specifiic folder. The best suggestion is to replace the folder named *USER* by your user name.

### Prepared dependencies for Microsot Windows

**THIS SECTION IS UNDER DEVELOPMENT**

For Microsoft Visual C++ users we have prepared a zip file containing all the third-party libraries in a binary format. You can download this package from http://www.dpi.inpe.br/terrama2-devel. In that folder you will find:
- **terrama2-3rdparty-msvc-2013-win64.zip:** all the third-party libraries for building a 64-bit version of TerraMA2 with Qt 5.4.1 support.
- **terrama2-3rdparty-msvc-2013-win32.zip:** all the third-party libraries for building a 32-bit version of TerraMA2 with Qt 5.4.1 support.

Microsoft Windows users that had downloaded the auxiliary package (in the TerraMA² site) must install Qt 5.4.1.


## Cloning TerraMA² Repository

- Open the shell command line.

- Make a new folder to host TerraMA² source code:
```
$ mkdir -p /home/user/mydevel/terrama2/codebase
```

- Change the current directory to that new folder:
```
$ cd /home/user/mydevel/terrama2/codebase
```

- Make a local copy of TerraMA² repository:
```
$ git clone https://github.com/terrama2/terrama2.git .
```

## Branches
You can check all branches available (remotes and local) and see the current one (marked with "*"):

`$ git branch -a`

The output of above command will be something like:
```
  * master
  remotes/origin/HEAD -> origin/master
  remotes/origin/master
```

In the above output the "* master" means that the current branch is master.

We have the following branches:
- **master:** This is the branch where the development team is working to add new features to future versions of TerraMA². It may be unstable although the codebase is subject to automatic tests (regression and unittests). We don't recommend to generate production versions of TerraMA² from this branch. Use it for testing new features and get involved with TerraMA² development.

- **b-4.0.0-alpha:** This will be the first branch in TerraMA²'s codebase for the generation 4.
- **b-4.0.0-alpha2:** Minimal working version with web user interface. Services: Collector and Analysis.
- **b-4.0.0-alpha3:** Analysis of monitored objects and grid.
- **b-4.0.0-alpha4:** Views service and web-monitoring module
- **b-4.0.0-alpha5:** General review of the interface and analysis
- **b-4.0.0-alpha6:** Review of grid operators and Monitored Object forecast operators
- **b-4.0.0-alpha7:** Analysis validation and filter by static data (Preparing to Beta release)
- **b-4.0.0-beta1:** 

To switch to one of the branches listed above, use the checkout command and create a local branch to track the remote branch. The syntax of "git checkout" is:

`$ git checkout -b <local_branch_name> <remote_branch_name without this part "remotes/">`

In order to switch to branch *b-4.0.0-alpha* you can use the following command:

`$ git checkout -b b-4.0.0-alpha* origin/b-4.0.0-alpha*`

## Tags

Also there are tags which usually are originated from a release branch. For instance, tag *t-4.0.0-alpha1* will be originated from branch *b-4.0.0-alpha*.

To check all tags available, use:

`$ git tag -l           (list all tag names)`
```
  t-4.0.0-alpha1
  t-4.0.0-alpha2
  t-4.0.0-beta1
  t-4.0.0-rc1
  t-4.0.0
  ...
```

If you want to checkout a specific version given by a tag and create a local branch to work on you can use the following git command:

`$ git checkout -b <local_branch_tag_name> <one_of_tag_name_listed>`

For instance, to checkout *t-4.0.0-alpha1* you can enter the following command:

`$ git checkout -b t-4.0.0-alpha1  t-4.0.0-alpha1`

## Build Instructions

After choosing the right branch or tag to work on, follow the insructions on **DEPENDENCIES** section. Make sure you have all the third-party library dependencies listed in this section before trying to build TerraMA².

The `build/cmake` folder contains a CMake project for building TerraMA².

Until now its build has been tested on:
- Linux Ubuntu 14.04
- Mac OS X Yosemite
- Microsoft Windows 7.

You should use at least CMake version 2.8.12 for building TerraMA². Older versions than this may not work properly.

Follow the build steps below according to your platform.

### Building on Linux with GNU G++

1.1. Open a Command Prompt (Shell).

1.2. We will assume that the codebase (all the source tree) is located at:

`/home/user/mydevel/terrama2/codebase`

1.3. Create a folder out of the TerraMA² source tree to generate the build system, for example:
```
$ cd /home/user/mydevel/terrama2
$ mkdir build-release
$ cd build-release
```
**Note:** for the sake of simplicity create this directory in the same level as the source tree (as showned above).

1.4. For Linux systems you must choose the build configuration:
```
$ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="/home/user/myinstall/terrama2" -DCMAKE_PREFIX_PATH:PATH="/home/user/mylibs;/home/user/mylibs/terralib5/lib/cmake" ../codebase/build/cmake
```

1.5 Building (with 4 process in parallel):
```
$ make -j 4
```

1.6 Installing:
```
$ make install
```

1.7 Uninstalling:
```
$ make uninstall
```

Notes:
* Some Linux flavours with different versions of GNU gcc and Boost will need more parameters such as:
```
  -DCMAKE_INCLUDE_PATH:PATH="/usr/local;/opt/include"
  -DCMAKE_LIBRARY_PATH:PATH="/usr/local;/opt/lib"
  -DCMAKE_PROGRAM_PATH:PATH="/usr/local/bin;/opt/bin"
  -DBOOST_ROOT:PATH="/opt/boost"
```
* Boost can also be indicated by BOOST_INCLUDEDIR (note: without an '_' separating INCLUDE and DIR):
```
  -DBOOST_INCLUDEDIR:PATH="/usr/local/include"
```
* The parameter -lpthread must be informed only if your Boost was not built as a shared library:
```
  -DCMAKE_CXX_FLAGS:STRING="-lpthread"
```
* For building with Qt5 you can provide the Qt5_DIR variable as:
```
  -DQt5_DIR:PATH="/usr/local/lib/cmake/Qt5"
```
* For generating a debug version set CMAKE_BUILD_TYPE as:
```
  -DCMAKE_BUILD_TYPE:STRING="Debug"
```

### Building on Mac OS X Yosemite

1.1 Open a Command Prompt (Shell).

1.2. We will assume that the codebase (all the source tree) is located at:
```
/Users/user/mydevel/terrama2/codebase
```

1.3. Create a folder out of the TerraMA² source tree to generate the build system, for example:
```
$ cd /Users/user/mydevel/terrama2
$ mkdir build-release
$ cd build-release
```
**Note:** for the sake of simplicity create this directory in the same level as the source tree (as showned above).

1.4. For Mac OS X systems you must choose the build configuration:
```
$ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="/Users/user/myinstall/terrama2" -DCMAKE_PREFIX_PATH:PATH="/Users/user/mylibs;/Users/user/mylibs/terralib5/lib/cmake;/Users/user/Qt5.4.1/5.4/clang_64/lib/cmake" ../codebase/build/cmake
```

**Note:** Please, in the cmake call above, take special attention to the key *CMAKE_PREFIX_PATH* and Qt location.

1.5. Building (with 4 process in parallel):
```
$ make -j 4
```

1.6. Installing:
```
$ make install
```
1.7. Uninstalling:
```
$ make uninstall
```

Notes:

* You have to specify valid paths for *CMAKE_PREFIX_PATH*. If you have a Qt version installed as a framework in your home directory, you have to tell CMake where to locate its CMake support. For instance, if you have Qt version 5.4.1 installed, you have to add to *CMAKE_PREFIX_PATH* the following directory:
```
/Users/user/Qt5.4.1/5.4/clang_64/lib/cmake
```

* You have also to tell where TerraLib? CMake support is located. Add to CMAKE_PREFIX_PATH where TerraLib? is installed, for example:
```
/Users/user/MyLibs/terralib/lib/cmake
```

* You can also generate an Xcode project by using the "Xcode generator" option:
```
-G "Xcode"
```

* There are some useful variables that can be set inside Xcode in order to run an application. The following environment variable can be set:
```
DYLD_FALLBACK_LIBRARY_PATH
DYLD_FALLBACK_FRAMEWORK_PATH
```

### Building on Microsoft Windows with Visual C++

**TO BE DONE**


### Quick Notes for Developers

If you have built TerraMA² in Debug mode and you want to run it inside the build tree, you may need to set some environment variables:
* For Mac OS X, you can set the following variables:
```
$ export DYLD_FALLBACK_LIBRARY_PATH=/Users/user/mylibs/lib
$ export DYLD_FALLBACK_FRAMEWORK_PATH=/Users/user/mylibs/lib/
```

* For Linux, you can set the following variable:
```
$ export LD_LIBRARY_PATH=/home/user/mylibs/lib
```

If you want to use QtCreator on Linux Ubuntu 14.04 you can install it through the following command:
```
$ sudo apt-get install qtcreator
```

On Linux Ubuntu 14.04 you can install git through the following command:
```
$ sudo apt-get install git
```

If you have experienced  any problem building any of the third-party tool on Mac OS X, try to install Xcode command line tools:
```
$ xcode-select --install
```

## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.


For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
