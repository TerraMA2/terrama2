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
#  Description: CMake script for collector client
#
#  Author: Paulo R. M. Oliveira
#          Vinicius Campanha
#

include_directories (
  ${GSOAP_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${TERRAMA2_ABSOLUTE_ROOT_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/generated
)

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/ws/collector/server/*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/ws/collector/server/*.hpp)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


TERRAMA2_GSOAP_SOAPCPP2(${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/ws/collector/core/WebService.hpp
                        collector server
                        GSOAP_HDR_FILES GSOAP_SRC_FILES GSOAP_NSM_FILES)


source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})
source_group("GSOAP Files"   FILES ${GSOAP_HDR_FILES} ${GSOAP_SRC_FILES} ${GSOAP_NSM_FILES})

add_library(terrama2_mod_ws_collector_server
            SHARED
            ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES}
            ${GSOAP_HDR_FILES} ${GSOAP_SRC_FILES} ${GSOAP_NSM_FILES})

target_link_libraries(terrama2_mod_ws_collector_server terrama2_mod_ws_collector_core
                                                       terrama2_mod_core
                                                       ${GSOAP_CPP_LIBRARY}
                                                       ${Boost_LOG_LIBRARY})

qt5_use_modules(terrama2_mod_ws_collector_server Widgets)

set_target_properties(terrama2_mod_ws_collector_server
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../lib")

install(TARGETS terrama2_mod_ws_collector_server
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
        DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/ws/collector/server COMPONENT devel)

export(TARGETS terrama2_mod_ws_collector_server APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
