#
#  Copyright (C) 2008-2014 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of the TerraLib - a Framework for building GIS enabled applications.
#
#  TerraLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraLib. See COPYING. If not, write to
#  TerraLib Team at <terralib-team@terralib.org>.
#
#  Description: TerraLib CMake cache options for building TerraLib on Windows environments.
#
#  Before start, please read the "PACKAGE-INSTRUCTIONS".
#

set (CMAKE_INSTALL_PREFIX "$ENV{TERRAMA2_INSTALL_PATH}" CACHE PATH "Where to install package?" FORCE)
set (CMAKE_PREFIX_PATH "$ENV{TERRAMA2_DEPENDENCIES_DIR};$ENV{TERRALIB_DEPENDENCIES_DIR};$ENV{QMAKE_FILEPATH}/.." CACHE PATH "Where to find libraries?" FORCE)

set (terralib_DIR "$ENV{TERRALIB_INSTALL_DIR}" CACHE PATH "Where to find TerraLib?" FORCE)
set (QUAZIP_LIBRARIES "$ENV{TERRAMA2_DEPENDENCIES_DIR}/lib/quazip5.lib" CACHE PATH "Where to find TerraLib?" FORCE)
set (QUAZIP_LIBRARY_DIR "$ENV{TERRAMA2_DEPENDENCIES_DIR}/lib" CACHE PATH "Where to find TerraLib?" FORCE)
set (VMIME_LIBRARY "$ENV{TERRAMA2_DEPENDENCIES_DIR}/lib/vmime.lib" CACHE PATH "Where to find TerraLib?" FORCE)
set (VMIME_INCLUDE_DIR "$ENV{TERRAMA2_DEPENDENCIES_DIR}/include" CACHE PATH "Where to find TerraLib?" FORCE)

# Turn off/on TerraMa2 CMake variables:
set (TERRAMA2_BUILD_EXAMPLES_ENABLED OFF CACHE BOOL "Build Examples?" FORCE)
set (TERRAMA2_BUILD_UNITTEST_ENABLED OFF CACHE BOOL "Build Unittest?" FORCE)
set (TERRAMA2_BUILD_AS_BUNDLE ON CACHE BOOL "Build As Bundle?" FORCE)
set (TERRAMA2_TRACK_3RDPARTY_DEPENDENCIES ON CACHE BOOL "Build Track 3rdparty?" FORCE)

# Set the package type to be generated:
set (CPACK_BINARY_NSIS ON CACHE BOOL "Generate DEB?" FORCE)
set (CPACK_SOURCE_7Z OFF CACHE BOOL "Generate STGZ?" FORCE)
set (CPACK_SOURCE_ZIP OFF CACHE BOOL "Generate TGZ?" FORCE)
