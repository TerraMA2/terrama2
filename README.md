# TerraMA2

**NOTE:**
* **Until we reach version 4.0.0 this codebase will be instable and not fully operational.**
* **TerraMA2 is under active development. We are preparing this site to host it!**


TerraMA2 is a free and open source computational platform for early warning systems.

You can build applications to monitor, analyze and issue early warnings related to air quality, water quality, pipelines, tailings dams for mining, forest fires, landslides and debris-mud flows, floods and droughts.

Through a service-oriented architecture, TerraMA2 can integrates geospatial data from different web services.

It takes advantage of the datasets available on the Internet to have access to real-time geo-environmental data (meteorological, climatic, atmospheric, hydrological, geotechnical and socio-demographic).

Besides that, it provides support for building models to several application domains through scriptable languages.

The computational platform is based on a service-oriented architecture, which is open and provides the technological infrastructure required to develop and implement operating systems to monitor early warnings of environmental risks.

## Basic Instructions

In the root directory of TerraMA2 codebase (the source code tree) there are some text files that explain the details of the codebase:

- **[BRANCHES-AND-TAGS:](https://github.com/TerraMA2/terrama2/blob/master/BRANCHES-AND-TAGS)** Notes on how to switch to the right branch to work on or the right tag to get the source code.
- **[BUILD-INSTRUCTIONS:]()** Notes on how to compile and install TerraMA2 for each platform.
- **CHANGELOG:** List of changes in TerraMA2 source code.
- **[DEPENDENCIES:](https://github.com/TerraMA2/terrama2/blob/master/DEPENDENCIES)** The list of third-party library you must install before building TerraMA2.
- **[LICENSE:](https://github.com/TerraMA2/terrama2/blob/master/LICENSE)** Licence statement in plain txt format.
- **[README:](https://github.com/TerraMA2/terrama2/blob/master/README)** Contains instructions about how to build and how is organized TerraMA2 plataform source code.

## Directories

- **build:** Contains the CMake scripts with commands, macros and functions used to build the environment for compiling libraries and executables in different platforms using the CMake tool.

- **examples:** Some examples on how to use TerraMA2 API and GUI.

- **licenses:** Copyright notices of third-party libraries used by TerraMA2.

- **resources:** Fonts, images, sql, and xml files among other resources of general use.

- **share:** XML Schema (.xsd), JSON, plugins, translations files and OGC specifications that is shared and installed with TerraMA2.

- **src:** Contains the source code of TerraMA2 and an automatic test system for it.

## Dependencies

If you want to build yourself TerraMA2 then you need to install some third-party libraries.

For Microsoft Visual C++ users we have prepared a zip file containing all the third-party libraries in a binary format. You can download this package from: http://www.dpi.inpe.br/terrama2-devel. In that folder you will find:
- **terrama2-3rdparty-msvc-2013-win64.zip:** all the third-party libraries for building a 64-bit version of TerraMA2 with Qt 5.4.1 support.
- **terrama2-3rdparty-msvc-2013-win32.zip:** all the third-party libraries for building a 32-bit version of TerraMA2 with Qt 5.4.1 support.

Below is a list of third-party libraries dependencies and its versions:
- **Boost (Mandatory):** TerraMA2 is built on top of Boost libraries. You will need to have them installed in order to build TerraMA2. Make sure to have at least version 1.54.0 installed. If you prefer to install from source, download it from: http://www.boost.org.
- *Qt (Mandatory):** Make sure you have an installed Qt version 5.2.1 or later. Linux users may use any package manager to perform an easy installation. Mac OS X can use package managers such as Homebrew (http://brew.sh) or MacPorts (http://www.macports.org) in order to have an easy installation. If you prefer to install from source, download it from: http://qt-project.org/downloads.
- **gSOAP (Mandatory):** Make sure you have an installed gSOAP version 2.8.23 or later. If you prefer to install from source, download it from: http://www.cs.fsu.edu/~engelen/soap.html.
- **TerraLib (Mandatory):** TerraMA2 is built on top of TerraLib, a free and open source library for building GIS enabled applications. Make sure to have at least TerraLib version 5.1.0. You can download it from: http://www.dpi.inpe.br/terralib5.

**Note:** Microsoft Windows users that had downloaded the auxiliary package (in the TerraMA2 site) must install Qt 5.4.1.

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
- **master:** This is the branch where the development team is working to add new features to future versions of TerraMA2. It may be instable although the codebase is subject to automatic tests (regression and unittests). We don't recommend to generate production versions of TerraMA2 from this branch. Use it for testing new features and get involved with TerraMA2 development.
- **b-4.0.0-alpha:** This will be the first branch in TerraMA2's codebase for the generation 4.

To switch to one of the branches listed above, use the checkout command and create a local branch to track the remote branch. The syntax of "git checkout" is:

`$ git checkout -b <local_branch_name> <remote_branch_name without this part "remotes/">`

In order to switch to branch *b-4.0.0-alpha* you can use the following command:

`$ git checkout -b b-4.0.0-alpha origin/b-4.0.0-alpha`

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

After choosing the right branch or tag to work on, follow the insructions on **DEPENDENCIES** section.

The `build/cmake` folder contains a CMake project for building TerraMA2.

Until now its build has been tested on:
- Linux: Ubuntu 14.04
- Mac OS X: Yosemite
- Microsoft Windows: 7.

You should use at least CMake version 2.8.12 for building TerraMA2. Older versions than this may not work properly.

Make sure you have all the third-party library dependencies listed in the DEPENDENCIES file on the root of the codebase.

Follow the build steps below according to your platform.

### Building on Linux with GNU G++

1.1. Open a Command Prompt (Shell).

1.2. We will assume that the codebase (all the source tree) is located at:

`/home/user/terrama2/codebase`

1.3. Create a folder out of the TerraMA2 source tree, for example:
```
$ cd /home/user/terrama2
$ mkdir build-release
$ cd build-release
```

1.4. For Linux systems you must choose the build configuration:
```
$ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="/usr/local/terrama2" -DCMAKE_PREFIX_PATH:PATH="/usr/local;/opt/external-libraries" ../codebase/build/cmake
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
* To turn on C++11 support:
```
     -DCMAKE_CXX_FLAGS:STRING="-std=c++11"
```
* To turn on the check of undefined macros, add the following flag:
```
     -DCMAKE_CXX_FLAGS:STRING="-Wundef"
```
1.5 Building (with 4 process in parallel):

`$ make -j 4`

1.6 Installing:

`$ make install`

1.7 Uninstalling:

`$ make uninstall`


### Building on Mac OS X Yosemite

**TO BE DONE**


### Building on Microsoft Windows with Visual C++

**TO BE DONE**


### Using CMake-GUI

**TO BE DONE**

## Packing TerraMA2

### Packing TerraMA2 on Linux Ubuntu 14.04

**TO BE DONE**

### Packing TerraMA2 on Mac OS X Yosemite

**TO BE DONE**

### Packing TerraMA2 on Microsoft Windows

**TO BE DONE**

### Quick Notes for Developers

If you have built TerraMA2 in Debug mode and you want to run it inside the build tree, you may need to set some environment variables.

For Mac OS X, you can set the following variables:

`$ export DYLD_FALLBACK_LIBRARY_PATH=/Users/user/MyLibs/lib`
`$ export DYLD_FALLBACK_FRAMEWORK_PATH=/Users/user/MyLibs/lib/`

## Reporting Bugs

Any problem should be reported to terrama2-developers@dpi.inpe.br.


For more information on TerraMA2, please, visit its main web page at: http://www.dpi.inpe.br/terrama2.



