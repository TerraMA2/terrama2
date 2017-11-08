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

- **[CHANGELOG:](https://github.com/TerraMA2/terrama2/blob/master/Changelog.md)** List of changes in TerraMA² source code.

- **[DEPENDENCIES:](https://github.com/TerraMA2/terrama2/blob/master/DEPENDENCIES)** The list of third-party library you must install before building TerraMA².

- **[LICENSE:](https://github.com/TerraMA2/terrama2/blob/master/LICENSE)** Licence statement in plain txt format.

- **[README:](https://github.com/TerraMA2/terrama2/blob/master/README)** Contains instructions about how to build and how is organized TerraMA² plataform source code.


## Source Code Organization

- **[build/cmake:](https://github.com/TerraMA2/terrama2/tree/master/build/cmake)** Contains the CMake scripts with commands, macros and functions used to build the environment for compiling libraries and executables in different platforms using the CMake tool.

- **[examples:](https://github.com/TerraMA2/terrama2/tree/master/src/examples)** Some examples on how to use TerraMA² API and GUI.

- **[install:](https://github.com/TerraMA2/terrama2/tree/master/install)** Bash scripts for helping building and installing TerraMA².

- **[licenses](https://github.com/TerraMA2/terrama2/blob/master/LICENSE):** Copyright notices of third-party libraries used by TerraMA².

- **[share:](https://github.com/TerraMA2/terrama2/tree/master/share)** XML Schema (.xsd), JSON files, plugin manifest files, translations files and OGC specifications that is shared and installed with TerraMA².

- **[src:](https://github.com/TerraMA2/terrama2/tree/master/src)** Contains the source code of TerraMA² and its automatic test system.

- **[tool:](https://github.com/TerraMA2/terrama2/tree/master/tools)** Contains some helper tools for managig TerraMA² code.


## C++ Dependencies

The file named **[DEPENDENCIES](https://github.com/TerraMA2/terrama2/blob/master/DEPENDENCIES)** in the root of TerraMA² source tree contains the official list of third-party libraries and tools that you must install before building TerraMA² from source.

If you want to build yourself TerraMA² then you need to install some third-party libraries. Below we show the list of third-party libraries dependencies and its versions:

- **CMake (Mandatory):** You should use at least CMake version 2.8.12 for building TerraMA². Older versions than this may not work properly. You can download it from: https://cmake.org/download/

- **Boost (Mandatory):** TerraMA² is built on top of Boost libraries. You will need to have them installed in order to build TerraMA². Make sure to have at least version 1.54.0 installed. If you prefer to install from source, download it from: http://www.boost.org.

- **Qt (Mandatory):** Make sure you have an installed Qt version 5.2.1 or later. Linux users may use any package manager to perform an easy installation. Mac OS X can use package managers such as Homebrew (http://brew.sh) or MacPorts (http://www.macports.org) in order to have an easy installation. If you prefer to install from source, download it from: http://qt-project.org/downloads.

- **TerraLib (Mandatory):** TerraMA² is built on top of TerraLib, a free and open source library for building GIS enabled applications. Make sure to have at least TerraLib version 5.1.0. You can download it from: http://www.dpi.inpe.br/terralib5.

- **libCURL (Mandatory):** libcurl is a multiprotocol file transfer library. Make sure you have at least version 7.42.1 in your system. You can download it from: http://curl.haxx.se/libcurl.

- **Google Test (Optional):** Google Test is the Google's C++ test framework. You can download it from: https://github.com/google/googletest.

All the *web application* dependencies and install steps you can find in: https://github.com/TerraMA2/terrama2/tree/master/webapp#terrama-webapp.


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
```
$ git branch -a
```

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
- **b-4.0.0-beta1:** GDAL driver for raster and Generic CSV driver for occurrences and DCP
- **b-4.0.0-beta2:** Alert service
- **b-4.0.0-beta3:** Improvements to WebMonitor
- **b-4.0.0-rc1:** Analysis DCP and improvements to WebMonitor
- **b-4.0.0-rc2:** Analysis DCP and lots of minor improvements
- **b-4.0.0-rc3:** Stabilization, DCP analysis view, general improvments
- **b-4.0.0-rc4:** In development...

For a more complete releases info, check: https://github.com/TerraMA2/terrama2/releases

To switch to one of the branches listed above, use the checkout command and create a local branch to track the remote branch. The syntax of "git checkout" is:
```
$ git checkout -b <local_branch_name> <remote_branch_name without this part "remotes/">
```

In order to switch to branch *b-4.0.0-alpha* you can use the following command:
```
$ git checkout -b b-4.0.0-alpha* origin/b-4.0.0-alpha*
```


## Tags

Also there are tags which usually are originated from a release branch. For instance, tag *t-4.0.0-alpha1* will be originated from branch *b-4.0.0-alpha*.

To check all tags available, use:
```
$ git tag -l           (list all tag names)
```
```
  t-4.0.0-alpha1
  t-4.0.0-alpha2
  t-4.0.0-beta1
  t-4.0.0-rc1
  t-4.0.0
  ...
```

If you want to checkout a specific version given by a tag and create a local branch to work on you can use the following git command:
```
$ git checkout -b <local_branch_tag_name> <one_of_tag_name_listed>
```

For instance, to checkout *t-4.0.0-alpha1* you can enter the following command:
```
$ git checkout -b t-4.0.0-alpha1  t-4.0.0-alpha1
```


## Build Instructions

The `build/cmake` folder contains a CMake project for building TerraMA².

Until now its build has been tested on:
- Linux Ubuntu 14.04 and Ubuntu 16.04
- Mac OS X El Capitan and Mac OS Sierra

After choosing the right branch or tag to work on, if you want to build TerraMA² and generate packages, first take a look at the sections below and read the right tips for automatically building in your platform:

- **[LINUX:](https://github.com/TerraMA2/terrama2/blob/master/LINUX.md)** Contains instructions about how to build and generate TerraMA² package on Linux.

- **[MAC:](https://github.com/TerraMA2/terrama2/blob/master/MAC.md)** Contains instructions about how to build and generate TerraMA² package on Mac OS.


## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.

For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
