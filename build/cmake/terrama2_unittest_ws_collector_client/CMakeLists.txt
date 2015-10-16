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
#  Description: CMake script for TerraMA2's unit test - collector client module.
#
#  Author: Vinicius Camapanha
#

include_directories (
  ${Boost_INCLUDE_DIR}
  ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src
  ${GSOAP_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/gsoap)

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/unittest/ws/collector/client/*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/unittest/ws/collector/client/*.hpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(COMMAND_LINE ${GSOAP_SOAPCPP2_EXECUTABLE} ARGS -T -i -w -x -I${GSOAP_IMPORT_DIR} -I${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/ws/collector/core ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/ws/collector/core/WebService.hpp)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/gsoap)

set(GSOAP_HDR_FILES ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapWebService.h
                    ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapWebProxy.h
                    ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapH.h
                    ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapStub.h)

set(GSOAP_SRC_FILES ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapWebService.cpp
                    ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapWebProxy.cpp
                    ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapC.cpp)

set(GSOAP_NSM_FILES ${CMAKE_CURRENT_BINARY_DIR}/gsoap/Web.nsmap)

set(GSOAP_TEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/gsoap/soapTester.cpp)

add_custom_command(OUTPUT ${GSOAP_HDR_FILES} ${GSOAP_SRC_FILES} ${GSOAP_NSM_FILES} ${GSOAP_TEST_FILE}
                   COMMAND ${COMMAND_LINE}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gsoap
                   COMMENT "Generating gSoap Web Service Files for Tests." VERBATIM USES_TERMINAL)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})
source_group("gSOAP Files"   FILES ${GSOAP_HDR_FILES} ${GSOAP_SRC_FILES} ${GSOAP_NSM_FILES})

add_definitions("-DSOAP_DEFMAIN=NOTMAIN")

add_executable(terrama2_unittest_ws_collector_client ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES} ${GSOAP_HDR_FILES} ${GSOAP_SRC_FILES} ${GSOAP_NSM_FILES} ${GSOAP_TEST_FILE})
#add_executable(terrama2_unittest_ws_collector_server_autotest ${GSOAP_HDR_FILES} ${GSOAP_SRC_FILES} ${GSOAP_TEST_FILE} ${GSOAP_NSM_FILES})

target_link_libraries(terrama2_unittest_ws_collector_client terrama2_mod_ws_collector_client
                                                            terrama2_mod_core
                                                            terralib_mod_plugin
                                                            terralib_mod_common
                                                            ${GSOAP_CPP_LIBRARY})

#target_link_libraries(terrama2_unittest_ws_collector_server_autotest ${GSOAP_CPP_LIBRARY})

qt5_use_modules(terrama2_unittest_ws_collector_client Test)
#qt5_use_modules(terrama2_unittest_ws_collector_server_autotest Test)
