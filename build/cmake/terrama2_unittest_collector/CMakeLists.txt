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
#  Description: CMake script for TerraMA2's unit test - collector module.
#
#  Author: Jano Simas
#

SET(TERRAMA2_TEST_COLLECTOR_DIR "${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/unittest/collector/")

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_TEST_COLLECTOR_DIR}*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_TEST_COLLECTOR_DIR}*.hpp)

file(GLOB TERRAMA2_TEST_SRC_FILES ${TERRAMA2_TEST_COLLECTOR_DIR}Test*.cpp)
file(GLOB TERRAMA2_TEST_HDR_FILES ${TERRAMA2_TEST_COLLECTOR_DIR}Test*.hpp)

list(REMOVE_ITEM TERRAMA2_SRC_FILES ${TERRAMA2_TEST_SRC_FILES})
list(REMOVE_ITEM TERRAMA2_HDR_FILES ${TERRAMA2_TEST_HDR_FILES})

include_directories (
  ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})

foreach(FILE ${TERRAMA2_TEST_SRC_FILES})

   STRING(REGEX REPLACE ".cpp" "" TEST "${FILE}")
   SET(HEADER "${TEST}.hpp")
   STRING(REGEX REPLACE "${TERRAMA2_TEST_COLLECTOR_DIR}" "" TEST "${TEST}")


   add_executable(${TEST} ${FILE} ${HEADER} ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES})

   target_link_libraries(${TEST} terrama2_mod_core
                                 terrama2_mod_collector
                                 terralib_mod_plugin
                                 terralib_mod_common)
   add_test(${TEST} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST}")

   qt5_use_modules(${TEST} Test)
endforeach()


