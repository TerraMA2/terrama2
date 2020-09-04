# TerraMA²

Repository Health  | Status
------------------ | -----------------------------------------------------------------
 Build             | [![Build Status](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-build-master/badge/icon)](http://www.dpi.inpe.br/jenkins/view/TerraMA%C2%B2/job/terrama2-build-master/)


TerraMA² is a free and open source computational platform for early warning systems.

You can build applications to monitor, analyze and issue early warnings related to air quality, water quality, pipelines, tailings dams for mining, forest fires, landslides and debris-mud flows, floods and droughts.

Through a service-oriented architecture, TerraMA² can integrates geospatial data from different web services.

It takes advantage of the datasets available on the Internet to have access to real-time geo-environmental data (meteorological, climatic, atmospheric, hydrological, geotechnical and socio-demographic).

Besides that, it provides support for building models to several application domains through scriptable languages.

The computational platform is based on a service-oriented architecture, which is open and provides the technological infrastructure required to develop and implement operating systems to monitor early warnings of environmental risks.


## Source Code Instructions

In the root directory of TerraMA² codebase (the source code tree) there are some text files explaining the details of the codebase:

- **[CHANGELOG:](CHANGELOG.md)** List of changes in TerraMA² source code.

- **[LICENSE:](LICENSE)** Licence statement in plain txt format.

- **[README:](README.md)** Contains instructions about how to build and how is organized TerraMA² plataform source code.


## Source Code Organization

- **[build/cmake:](build/cmake)** Contains the CMake scripts with commands, macros and functions used to build the environment for compiling libraries and executables in different platforms using the CMake tool.

- **[examples:](src/examples)** Some examples on how to use TerraMA² API and GUI.

- **[install:](install)** Bash scripts for helping building and installing TerraMA².

- **[licenses](LICENSE):** Copyright notices of third-party libraries used by TerraMA².

- **[share:](share)** XML Schema (.xsd), JSON files, plugin manifest files, translations files and OGC specifications that is shared and installed with TerraMA².

- **[src:](src)** Contains the source code of TerraMA² and its automatic test system.

- **[tool:](tools)** Contains some helper tools for managig TerraMA² code.


## C++ Dependencies

If you want to build yourself TerraMA² then you need to install some third-party libraries. Below we show the list of third-party libraries dependencies and its versions:

- **CMake (Mandatory):** You should use at least CMake version 2.8.12 for building TerraMA². Older versions than this may not work properly. You can download it from: https://cmake.org/download/

- **Boost (Mandatory):** TerraMA² is built on top of Boost libraries. You will need to have them installed in order to build TerraMA². Make sure to have at least version 1.54.0 installed. If you prefer to install from source, download it from: http://www.boost.org.

- **Qt (Mandatory):** Make sure you have an installed Qt version 5.2.1 or later. Linux users may use any package manager to perform an easy installation. Mac OS X can use package managers such as Homebrew (http://brew.sh) or MacPorts (http://www.macports.org) in order to have an easy installation. If you prefer to install from source, download it from: http://qt-project.org/downloads.

- **TerraLib (Mandatory):** TerraMA² is built on top of TerraLib, a free and open source library for building GIS enabled applications. Make sure to have at least TerraLib version 5.3.3. You can download it from: http://www.dpi.inpe.br/terralib5.

- **libCURL (Mandatory):** libcurl is a multiprotocol file transfer library. Make sure you have at least version 7.42.1 in your system. You can download it from: http://curl.haxx.se/libcurl.

- **Google Test (Optional):** Google Test is the Google's C++ test framework. You can download it from: https://github.com/google/googletest.

All the *web application* dependencies and install steps you can find in: https://github.com/TerraMA2/terrama2/tree/master/webapp#terrama-webapp.


## Cloning TerraMA² Repository

- Open the shell command line.

- Make a new folder to host TerraMA² source code:
```
$ mkdir -p /home/${USER}/mydevel/terrama2/codebase
```

- Change the current directory to that new folder:
```
$ cd /home/${USER}/mydevel/terrama2/codebase
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

- **b4.0.0-alpha:** This will be the first branch in TerraMA²'s codebase for the generation 4.
- **b4.0.0-alpha2:** Minimal working version with web user interface. Services: Collector and Analysis.
- **b4.0.0-alpha3:** Analysis of monitored objects and grid.
- **b4.0.0-alpha4:** Views service and web-monitoring module
- **b4.0.0-alpha5:** General review of the interface and analysis
- **b4.0.0-alpha6:** Review of grid operators and Monitored Object forecast operators
- **b4.0.0-alpha7:** Analysis validation and filter by static data (Preparing to Beta release)
- **b4.0.0-beta1:** GDAL driver for raster and Generic CSV driver for occurrences and DCP
- **b4.0.0-beta2:** Alert service
- **b4.0.0-beta3:** Improvements to WebMonitor
- **b4.0.0-release-candidate-1:** Analysis DCP and improvements to WebMonitor
- **b4.0.0-release-candidate-2:** Analysis DCP and lots of minor improvements
- **b4.0.0-release-candidate-3:** Stabilization, DCP analysis view, general improvments
- **b4.0.0-release-candidate-4:** Improvements for final release
- **b4.0.0:** Final release for TerraMA² 4.0.0
  - **b4.0.1:** Bug fixes
  - **b4.0.2:** Auto update database, terralib 5.3 and bugfixes
  - **b4.0.3:** Major stability fixes
  - **b4.0.4:** Interpolation service
  - **b4.0.5:** Analysis improvements

For a more complete releases info, check: https://github.com/TerraMA2/terrama2/releases

To switch to one of the branches listed above, use the checkout command and create a local branch to track the remote branch. The syntax of "git checkout" is:
```
$ git checkout -b <local_branch_name> <remote_branch_name without this part "remotes/">
```

In order to switch to branch *develop* you can use the following command:
```
$ git checkout -b develop origin/b4.1.2*
```


## Tags

Also there are tags which usually are originated from a release branch. For instance, tag *b4.1.2* will be originated from branch *b4.1.2*.

To check all tags available, use:
```
$ git tag -l           (list all tag names)
```

If you want to checkout a specific version given by a tag and create a local branch to work on you can use the following git command:
```
$ git checkout -b <local_branch_tag_name> <one_of_tag_name_listed>
```

For instance, to checkout *b4.1.2* you can enter the following command:
```
$ git checkout -b vb4.1.2  vb4.1.2
```

## Build Instructions

The `build/cmake` folder contains a CMake project for building TerraMA².

Until now its build has been tested on:
- Linux Ubuntu 14.04 and Ubuntu 16.04
- Mac OS X El Capitan and Mac OS Sierra

After choosing the right branch or tag to work on, if you want to build TerraMA² and generate packages, first take a look at the sections below and read the right tips for automatically building in your platform:

- **[LINUX:](LINUX.md)** Contains instructions about how to build and generate TerraMA² package on Linux.

- **[MAC:](MAC.md)** Contains instructions about how to build and generate TerraMA² package on Mac OS.

- **[WINDOWS:](WINDOWS.md)** Contains instructions about how to build and generate TerraMA² package on Windows.


## Reporting Bugs

Any problem should be reported to terrama2-team@dpi.inpe.br.

For more information on TerraMA², please, visit its main web page at: http://www.dpi.inpe.br/terrama2.
