#
#  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of TerraMA2 - a free and open source computational
#  platform for analysis, monitoring, and alert of geo-environmental extremes.
#
#  TerraMA2 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraMA2 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraMA2. See LICENSE. If not, write to
#  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
#
#
#  Description: Main CMake script for the TerraMA2 build system.
#
#  Author: Gilberto Ribeiro de Queiroz
#          Paulo R. M. Oliveira
#

cmake_minimum_required(VERSION 2.8.12)

project(terrama2)

#
# main variables for the build script
#

# variables that control the system versioning
set(TERRAMA2_VERSION_MAJOR 4)
set(TERRAMA2_VERSION_MINOR 0)
set(TERRAMA2_VERSION_PATCH 0)
set(TERRAMA2_VERSION_STATUS "rc")
set(TERRAMA2_VERSION_STATUS_FULL "-rc3")
set(TERRAMA2_VERSION_STRING "${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}.${TERRAMA2_VERSION_PATCH}${TERRAMA2_VERSION_STATUS_FULL}")
set(TERRAMA2_VERSION "0x040000")

# tells that 3rd-party libraries should be tracked and installed with TerraMA2
if(NOT DEFINED TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES)
  set(TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES 0 CACHE BOOL "If \"on\" it will search for 3rd-party libraries and install them when building the install target (e.g.: make install)")
endif()

# tells that TerraMA2 should be built as a bundle
if(NOT DEFINED TERRAMA2_BUILD_AS_BUNDLE)
  set(TERRAMA2_BUILD_AS_BUNDLE 0 CACHE BOOL "If on, tells that the build will generate a bundle")
endif()

# set the bundle items as writable before install_name_tool tries to change them (for APPLE platforms)
if(APPLE AND TERRAMA2_BUILD_AS_BUNDLE AND TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES AND NOT DEFINED BU_CHMOD_BUNDLE_ITEMS)
  set(BU_CHMOD_BUNDLE_ITEMS ON CACHE BOOL "If ON, set the bundle items as writable")
endif()

# variables storing absolute paths
set(TERRAMA2_ABSOLUTE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# variables that control paths
if(NOT DEFINED TERRAMA2_DIR_VAR_NAME)
  set(TERRAMA2_DIR_VAR_NAME "TERRAMA2_HOME" CACHE STRING "Name of an environment variable with the base installation path of TerraMA2")
endif()

if(APPLE AND TERRAMA2_BUILD_AS_BUNDLE AND NOT DEFINED TERRAMA2_BASE_DESTINATION_DIR)
  set(CMAKE_MACOSX_BUNDLE ON)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
  set(CMAKE_INSTALL_NAME_DIR "@executable_path/../lib")
  set(TERRAMA2_BASE_DESTINATION_DIR "terrama2_service.app/Contents/")
  set(TERRALIB_BASE_DESTINATION_DIR ${TERRAMA2_BASE_DESTINATION_DIR}/)

elseif(UNIX AND NOT DEFINED TERRAMA2_BASE_DESTINATION_DIR)
  set(TERRAMA2_BASE_DESTINATION_DIR ${CMAKE_INSTALL_PREFIX}/)

elseif(NOT DEFINED TERRAMA2_BASE_DESTINATION_DIR)
  set(TERRAMA2_BASE_DESTINATION_DIR .)
endif()

# variable that defines the path to data directory used in the unittests
if(NOT DEFINED TERRAMA2_DATA_DIR)
  set(TERRAMA2_DATA_DIR "${CMAKE_BINARY_DIR}/data" CACHE PATH "Path to a directory containing the data used in the examples and/or in the unit test")
endif()


# variable that defines the database username to be used in unittests and examples
if(NOT DEFINED TERRAMA2_DATABASE_USERNAME)
  set(TERRAMA2_DATABASE_USERNAME "postgres" CACHE PATH "Database username to be used in unittests and examples")
endif()


# variable that defines the database name to be used in unittests and examples
if(NOT DEFINED TERRAMA2_DATABASE_DBNAME)
    set(TERRAMA2_DATABASE_DBNAME "terrama2" CACHE PATH "Database name to be used in unittests and examples")
endif()

# variable that defines the database host to be used in unittests and examples
if(NOT DEFINED TERRAMA2_DATABASE_HOST)
  set(TERRAMA2_DATABASE_HOST "localhost" CACHE PATH "Database host to be used in unittests and examples")
endif()

# variable that defines the database port to be used in unittests and examples
if(NOT DEFINED TERRAMA2_DATABASE_PORT)
  set(TERRAMA2_DATABASE_PORT "5432" CACHE PATH "Database port to be used in unittests and examples")
endif()

# variable that defines the database port to be used in unittests and examples
if(NOT DEFINED TERRAMA2_DATABASE_PASSWORD)
  set(TERRAMA2_DATABASE_PASSWORD "postgres" CACHE PATH "Database password to be used in unittests and examples")
endif()

# variables that control install locations
if(NOT DEFINED TERRAMA2_DESTINATION_PLUGINS)
  set(TERRAMA2_DESTINATION_PLUGINS "${TERRAMA2_BASE_DESTINATION_DIR}share/terrama2/plugins")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_EXAMPLES)
  set(TERRAMA2_DESTINATION_EXAMPLES "${TERRAMA2_BASE_DESTINATION_DIR}share/terrama2/examples")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_UNITTEST)
  set(TERRAMA2_DESTINATION_UNITTEST "${TERRAMA2_BASE_DESTINATION_DIR}share/terrama2/unittest")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_LIBRARY)
  set(TERRAMA2_DESTINATION_LIBRARY "${TERRAMA2_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_RUNTIME)
  set(TERRAMA2_DESTINATION_RUNTIME "${TERRAMA2_BASE_DESTINATION_DIR}bin")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_ARCHIVE)
  set(TERRAMA2_DESTINATION_ARCHIVE "${TERRAMA2_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_HEADERS)
  set(TERRAMA2_DESTINATION_HEADERS "${TERRAMA2_BASE_DESTINATION_DIR}include")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION_SHARE)
  set(TERRAMA2_DESTINATION_SHARE "${TERRAMA2_BASE_DESTINATION_DIR}share")
endif()

if(NOT DEFINED TERRAMA2_DESTINATION)
  set(TERRAMA2_DESTINATION "${TERRAMA2_BASE_DESTINATION_DIR}")
endif()

#
# global definitions, includes and config
#

# Common definitions to all modules
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_UBLAS_TYPE_CHECK=0 -DBOOST_LOG_DYN_LINK)

# Specific definitions per platform
if(WIN32)
  #add_definitions(-D_CRT_SECURE_NO_WARNINGS)

  #if(MINGW)
  #  add_definitions(-DTE_MINGW -D_WIN32_WINNT=0x601 -fpermissive)
  #endif()

elseif(APPLE)
  add_definitions(-ftemplate-depth-1024)
endif()

# For each CMake version we need to check some policies
# Note: automatically link executables with qtmain.lib on Windows when it has Qt dependency.
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER  2.8.9)
  cmake_policy(SET CMP0020 NEW)
endif()

if(${CMAKE_MAJOR_VERSION} GREATER 2)
  cmake_policy(SET CMP0006 OLD)
  cmake_policy(SET CMP0026 OLD)
  cmake_policy(SET CMP0042 NEW)
  cmake_policy(SET CMP0043 NEW)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.1.2")
  cmake_policy(SET CMP0053 OLD)
endif()

# Build will generate files into a single folder
if((CMAKE_GENERATOR STREQUAL Xcode) OR MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Where to find modules (find procedures)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

# Compiler options: warning and C++ 11
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER MATCHES ".*clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11")
endif()

# Turn on multi process compilation on Windows
if(MSVC)
  add_definitions(/MP)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Enable CTest
enable_testing()

# Build tree directory will be needed in all projects.
include_directories(${CMAKE_BINARY_DIR})

#
# including CMake's auxiliary files
#

include(CMakeDependentOption)


#
# including TerraMA2 auxiliary macros and functions
#

include(terrama2_macros.cmake)


#
# first check of libraries
#

find_package(Qt5 5.2 REQUIRED COMPONENTS Core Gui Widgets Network LinguistTools Sql Test PrintSupport)

if(NOT Qt5_FOUND)
  message(FATAL_ERROR "TerraMA2: could not find required Qt 5 libraries!")
endif()

add_definitions(-DQt5_VERSION=${Qt5_VERSION})

if(NOT QT_QCOLLECTIONGENERATOR_EXECUTABLE)
  find_program(QT_QCOLLECTIONGENERATOR_EXECUTABLE
               NAME qcollectiongenerator
               PATHS /usr
                     /usr/local
                     ${Qt5_DIR}/../../..
               PATH_SUFFIXES bin)
endif()

find_package(Boost 1.54.0 REQUIRED system date_time chrono log timer filesystem regex python iostreams program_options)

if(NOT Boost_FOUND)
  message(FATAL_ERROR "TerraMA2: could not find required Boost libraries!")
endif()

find_package(Doxygen)

find_package(terralib REQUIRED)

if(NOT terralib_FOUND)
  message(FATAL_ERROR "TerraMA2: could not find TerraLib libraries!")
endif()

find_package(CURL REQUIRED)

if(NOT CURL_FOUND)
  message(FATAL_ERROR "TerraMA2: could not find required libCURL library!")
endif()

find_package(PythonLibs REQUIRED)
if(NOT PYTHONLIBS_FOUND)
  message(FATAL_ERROR "TerraMA2: could not find required Python library!")
endif()

find_package(GTest)
find_package(GMock)

find_package(QuaZip REQUIRED)
if(NOT QUAZIP_FOUND)
  message(FATAL_ERROR "TerraMA2: could not find required Quazip library!")
endif()

find_package(VMime)
if(VMIME_FOUND)
  add_definitions(-DVMIME_FOUND)
else(NOT VMIME_FOUND)
    message("TerraMA2: could not find vmime library. TerraMA2 won't be able to send email notifications.")
endif()

set(ThreadPool_INCLUDE_DIRS "${TERRAMA2_ABSOLUTE_ROOT_DIR}/third-party/ThreadPool/")



# variables that controls what is built
if(NOT DEFINED TERRAMA2_BUILD_EXAMPLES_ENABLED)
  CMAKE_DEPENDENT_OPTION(TERRAMA2_BUILD_EXAMPLES_ENABLED "Build examples?" ON "GMOCK_FOUND" OFF)
endif()

if(NOT DEFINED TERRAMA2_BUILD_UNITTEST_ENABLED)
  CMAKE_DEPENDENT_OPTION(TERRAMA2_BUILD_UNITTEST_ENABLED "Build unit-test module?" ON "GMOCK_FOUND" OFF)
endif()

#
# build options
#
# warning: please, do not change the order below!
#

if(DOXYGEN_FOUND)
  option(TERRAMA2_DOXYGEN_ENABLED  "Enable API documentation build?" OFF)
endif()

option(TERRAMA2_CORE_ENABLED "Build TerraMA2 Core module?" ON)

#####################################
#            MODULES
CMAKE_DEPENDENT_OPTION(TERRAMA2_COLLECTOR_ENABLED "Build Collector module?" ON "TERRAMA2_CORE_ENABLED" OFF)
if(TERRAMA2_COLLECTOR_ENABLED)
  add_definitions(-DTERRAMA2_COLLECTOR_ENABLED)
endif()

CMAKE_DEPENDENT_OPTION(TERRAMA2_IMPLEMENTATION_ENABLED "Build Implementation module?" ON "TERRAMA2_CORE_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_ANALYSIS_CORE_ENABLED "Build Analysis core module?" ON "TERRAMA2_CORE_ENABLED" OFF)
if(TERRAMA2_ANALYSIS_CORE_ENABLED)
  add_definitions(-DTERRAMA2_ANALYSIS_CORE_ENABLED)
endif()

CMAKE_DEPENDENT_OPTION(TERRAMA2_VIEW_ENABLED "Build View module?" ON "TERRAMA2_CORE_ENABLED" OFF)
if(TERRAMA2_VIEW_ENABLED)
  add_definitions(-DTERRAMA2_VIEW_ENABLED)
endif()

CMAKE_DEPENDENT_OPTION(TERRAMA2_ALERT_ENABLED "Build Alert module?" ON "TERRAMA2_CORE_ENABLED" OFF)
if(TERRAMA2_ALERT_ENABLED)
  add_definitions(-DTERRAMA2_ALERT_ENABLED)
endif()

CMAKE_DEPENDENT_OPTION(TERRAMA2_ARCHIVER_ENABLED "Build Archiver module?" ON "TERRAMA2_CORE_ENABLED" OFF)
if(TERRAMA2_ARCHIVER_ENABLED)
  add_definitions(-DTERRAMA2_ARCHIVER_ENABLED)
endif()

CMAKE_DEPENDENT_OPTION(TERRAMA2_INTERPOLATOR_ENABLED "Build Interpolator module?" ON "TERRAMA2_CORE_ENABLED" OFF)
if(TERRAMA2_INTERPOLATOR_ENABLED)
  add_definitions(-DTERRAMA2_INTERPOLATOR_ENABLED)
endif()

#####################################

#####################################
#            SERVICES
CMAKE_DEPENDENT_OPTION(TERRAMA2_SERVICE_ENABLED "Build Services?" ON "TERRAMA2_CORE_ENABLED" OFF)

#####################################

#####################################
#            TESTS

CMAKE_DEPENDENT_OPTION(TERRAMA2_UNITTEST_CORE_ENABLED "Build unittest for core module?" ON "TERRAMA2_CORE_ENABLED;TERRAMA2_BUILD_UNITTEST_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_UNITTEST_COLLECTOR_ENABLED "Build unittest for collector module?" ON "TERRAMA2_COLLECTOR_ENABLED;TERRAMA2_BUILD_UNITTEST_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_UNITTEST_ANALYSIS_ENABLED "Build unittest for analysis module?" OFF "TERRAMA2_ANALYSIS_CORE_ENABLED;TERRAMA2_BUILD_UNITTEST_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_UNITTEST_VIEW_ENABLED "Build unittest for view module?" ON "TERRAMA2_VIEW_ENABLED;TERRAMA2_BUILD_UNITTEST_ENABLED" OFF)

######################################


#####################################
#            EXAMPLES
CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_CORE_ENABLED "Build examples for core module?" ON "TERRAMA2_CORE_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_ANALYSIS_ENABLED "Build examples for analysis module?" ON "TERRAMA2_ANALYSIS_CORE_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_COLLECTOR_ENABLED "Build examples for collector module?" ON "TERRAMA2_COLLECTOR_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_ALERT_ENABLED "Build examples for alert module?" ON "TERRAMA2_ALERT_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_VIEW_ENABLED "Build examples for view module?" ON "TERRAMA2_VIEW_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_ARCHIVER_ENABLED "Build examples for archiver module?" ON "TERRAMA2_ARCHIVER_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

CMAKE_DEPENDENT_OPTION(TERRAMA2_EXAMPLES_INTERPOLATOR_ENABLED "Build examples for interpolator module?" ON "TERRAMA2_INTERPOLATOR_ENABLED;TERRAMA2_IMPLEMENTATION_ENABLED;TERRAMA2_BUILD_EXAMPLES_ENABLED" OFF)

######################################

#
# process TerraMA2 configuration files
#

configure_file(${CMAKE_SOURCE_DIR}/terrama2-config-pkg.cmake.in
               ${CMAKE_BINARY_DIR}/pkg/terrama2-config.cmake @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/terrama2-config-version.cmake.in
               ${CMAKE_BINARY_DIR}/terrama2-config-version.cmake @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/terrama2-config.cmake.in
               ${CMAKE_BINARY_DIR}/terrama2-config.cmake @ONLY)

configure_file(${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/Config.hpp.in
               ${CMAKE_BINARY_DIR}/terrama2_config.hpp @ONLY)

configure_file(${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/Version.hpp.in
               ${CMAKE_BINARY_DIR}/terrama2_version.hpp @ONLY)

configure_file(${TERRAMA2_ABSOLUTE_ROOT_DIR}/LICENSE ${CMAKE_BINARY_DIR}/COPYING.txt COPYONLY)


#
# build modules
#

if(TERRAMA2_CORE_ENABLED)
  add_subdirectory(terrama2_core)
endif()

if(TERRAMA2_IMPLEMENTATION_ENABLED)
  add_subdirectory(terrama2_impl)
endif()

if(TERRAMA2_SERVICE_ENABLED)
  add_subdirectory(terrama2_service)
endif()

if(TERRAMA2_COLLECTOR_ENABLED)
  add_subdirectory(terrama2_collector_core)
  add_subdirectory(terrama2_collector_mock)
endif()

if(TERRAMA2_ANALYSIS_CORE_ENABLED)
  add_subdirectory(terrama2_analysis_core)
  add_subdirectory(terrama2_analysis_mock)
endif()

if(TERRAMA2_ALERT_ENABLED)
  add_subdirectory(terrama2_alert_core)
  add_subdirectory(terrama2_alert_impl)
endif()

if(TERRAMA2_INTERPOLATOR_ENABLED)
  add_subdirectory(terrama2_interpolator_core)
endif()

if(TERRAMA2_VIEW_ENABLED)
  add_subdirectory(terrama2_view_core)
endif()

if(TERRAMA2_UNITTEST_CORE_ENABLED)
  add_subdirectory(terrama2_unittest_core)
endif()

if(TERRAMA2_UNITTEST_COLLECTOR_ENABLED)
  add_subdirectory(terrama2_unittest_collector)
endif()

if(TERRAMA2_UNITTEST_ANALYSIS_ENABLED)
  add_subdirectory(terrama2_unittest_analysis)
endif()

if(TERRAMA2_UNITTEST_VIEW_ENABLED)
  add_subdirectory(terrama2_unittest_view)
endif()

if(TERRAMA2_EXAMPLES_CORE_ENABLED)
  add_subdirectory(terrama2_examples_core)
  add_subdirectory(terrama2_examples_data)
endif()

if(TERRAMA2_EXAMPLES_ANALYSIS_ENABLED)
  add_subdirectory(terrama2_examples_analysis)
endif()

if(TERRAMA2_EXAMPLES_COLLECTOR_ENABLED)
  add_subdirectory(terrama2_examples_collector)
endif()

if(TERRAMA2_EXAMPLES_VIEW_ENABLED)
  add_subdirectory(terrama2_examples_view)
endif()

if(TERRAMA2_EXAMPLES_ALERT_ENABLED)
    add_subdirectory(terrama2_examples_alert)
endif()

if(TERRAMA2_EXAMPLES_ARCHIVER_ENABLED)
  add_subdirectory(terrama2_examples_archiver)
endif()

if(TERRAMA2_EXAMPLES_INTERPOLATOR_ENABLED)
  add_subdirectory(terrama2_examples_interpolator)
endif()

if(TERRAMA2_DOXYGEN_ENABLED)
  add_subdirectory(terrama2_doxygen)
endif()


#
# install and targets export
#

file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/*.hpp)

install(FILES ${CMAKE_BINARY_DIR}/terrama2_config.hpp
              ${CMAKE_BINARY_DIR}/terrama2_version.hpp
              ${TERRAMA2_HDR_FILES}
        DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2 COMPONENT devel)

install(FILES ${CMAKE_BINARY_DIR}/pkg/terrama2-config.cmake
              ${CMAKE_BINARY_DIR}/terrama2-config-version.cmake
        DESTINATION ${TERRAMA2_DESTINATION_LIBRARY}/cmake/terrama2-${TERRAMA2_VERSION_STRING} COMPONENT devel)

install(EXPORT terrama2-targets
        DESTINATION ${TERRAMA2_DESTINATION_LIBRARY}/cmake/terrama2-${TERRAMA2_VERSION_STRING}
        FILE terrama2-exports.cmake
        COMPONENT devel)

install(DIRECTORY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/share/terrama2
        DESTINATION ${TERRAMA2_DESTINATION_SHARE} COMPONENT runtime)

install(DIRECTORY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/webapp
        DESTINATION ${TERRAMA2_DESTINATION} COMPONENT runtime)
install(DIRECTORY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/webmonitor
        DESTINATION ${TERRAMA2_DESTINATION} COMPONENT runtime)
install(DIRECTORY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/webcomponents
        DESTINATION ${TERRAMA2_DESTINATION} COMPONENT runtime)
install(DIRECTORY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/third-party
        DESTINATION ${TERRAMA2_DESTINATION} COMPONENT runtime)

#
# Generate a post installation script with the installation folder
#
if(APPLE)

 install(DIRECTORY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/mac/
         DESTINATION ${TERRAMA2_BASE_DESTINATION_DIR}/scripts COMPONENT scripts USE_SOURCE_PERMISSIONS)

  file(COPY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/mac/ DESTINATION ${CMAKE_BINARY_DIR}/scripts FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
  file(COPY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/webapp DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/webmonitor DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/webcomponents DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/third-party DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY ${TERRAMA2_ABSOLUTE_ROOT_DIR}/share DESTINATION ${CMAKE_BINARY_DIR})

elseif(UNIX)
  configure_file(${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/postinst.in
                 ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/postinst @ONLY)
  configure_file(${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/prerm.in
                ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/prerm @ONLY)
  configure_file(${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/postrm.in
                 ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/postrm @ONLY)

  install(FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/remove-config.py
          DESTINATION ${TERRAMA2_DESTINATION_SHARE}/scripts COMPONENT scripts)
  install(FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/postinst
          DESTINATION ${TERRAMA2_DESTINATION_SHARE}/scripts COMPONENT scripts)
  install(FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/prerm
          DESTINATION ${TERRAMA2_DESTINATION_SHARE}/scripts COMPONENT scripts)
  install(FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/scripts/postrm
          DESTINATION ${TERRAMA2_DESTINATION_SHARE}/scripts COMPONENT scripts)
endif()

#
# If generating a deb package the script is executed by the debian post install configuration
# sa: build/cmake/terrama2-cpack-options.cmake.in
#

if(NOT CPACK_BINARY_DEB AND NOT APPLE)
  install(CODE "execute_process(COMMAND bash postinst ${TERRAMA2_BASE_DESTINATION_DIR} WORKING_DIRECTORY ${TERRAMA2_DESTINATION_SHARE}/scripts/ )" COMPONENT scripts)
endif()

#
# Install TerraLib and QtPlugins
#
if(TERRAMA2_BUILD_AS_BUNDLE)
  TeInstallQt5Plugins()

  if(WIN32)
    set(_subfolder bin)
  elseif(APPLE)
    set(_subfolder lib)
  endif()

  TeInstallTerraLibPlugins("${TERRALIB_PLUGINS}" ${TERRAMA2_BASE_DESTINATION_DIR}/${_subfolder})

  if(EXISTS "${terralib_DIR}/CMakeCache.txt")
    set(_TESRC "${terralib_INCLUDE_DIRS}/..")
    if(WIN32)
      set(_TEBIN "${terralib_DIR}/Release")
    elseif(APPLE)
      set(_TEBIN "${terralib_DIR}/bin")
    endif()
  else()
    set(_TESRC "${terralib_DIR}/../../..")
    if(WIN32)
      set(_TEBIN "${_TESRC}/bin")
    elseif(APPLE)
      set(_TEBIN "${_TESRC}/lib")
    endif()
  endif()

  install (
    DIRECTORY ${_TESRC}
    DESTINATION ${TERRAMA2_DESTINATION_SHARE}
    COMPONENT runtime
    PATTERN "*terraview*" EXCLUDE
    PATTERN "*.ado.*" EXCLUDE
    PATTERN "ado-*" EXCLUDE
    PATTERN "*.terralib4.*" EXCLUDE
    PATTERN "*.qt.edit.*" EXCLUDE
    PATTERN "*.qt.mobile.*" EXCLUDE
    PATTERN "*terraview_plugins" EXCLUDE
  )
endif()

#
# Track dependencies (fixbundle)
#
if(TERRAMA2_BUILD_AS_BUNDLE AND TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES)

  include(InstallRequiredSystemLibraries)

  add_subdirectory(fix_bundle)

endif()

#
# CPack
#

# CPack will use this path to generate a tar.gz or equivalent source package

if(CMAKE_SIZEOF_VOID_P MATCHES 8)
  #64 bits compiler detected
  set(TERRAMA2_PKG_ARCHITECTURE "x64")
else()
  if(CMAKE_SIZEOF_VOID_P MATCHES 4)
    #32 bits compiler detected
    set(TERRAMA2_PKG_ARCHITECTURE "x86")
  else()
    set(TERRAMA2_PKG_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
  endif()
endif()

set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${TERRAMA2_ABSOLUTE_ROOT_DIR};/")

configure_file(${terrama2_SOURCE_DIR}/terrama2-cpack-options.cmake.in
               ${CMAKE_BINARY_DIR}/terrama2CPackOptions.cmake
               @ONLY)

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/terrama2CPackOptions.cmake)

include(CPack)

# defining installation types, components and groups
cpack_add_install_type(full DISPLAY_NAME "Complete")
cpack_add_install_type(minimalistic DISPLAY_NAME "Minimalistic")

cpack_add_component_group(general_group
                          DISPLAY_NAME "Applications"
                          DESCRIPTION "TerraMA2 web services and GUI applications"
                          EXPANDED)

cpack_add_component_group(devel_group
                          DISPLAY_NAME "Development Tools"
                          DESCRIPTION "Development Tools: header files, libraries and cmake stuffs"
                          EXPANDED)

cpack_add_component(runtime
                    DISPLAY_NAME "Runtime Libraries and Executables"
                    DESCRIPTION "Install only necessary shared libraries (or DLLs) and executables"
                    GROUP general_group
                    INSTALL_TYPES full minimalistic)

cpack_add_component(devel
                    DISPLAY_NAME "Development package"
                    DESCRIPTION "Install header files, cmake stuffs and additional shared libraries"
                    GROUP devel_group
                    INSTALL_TYPES full minimalistic)

#
# Uninstall target
#

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake -DREMOVE="remove")
add_custom_target(purge COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake -DREMOVE="purge")
