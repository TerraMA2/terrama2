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
#  Description: CMake script for the TerraMA2 Core Runtime Module.
#
#  Author: Gilberto Ribeiro de Queiroz
#

if (WIN32)
  add_definitions(-DTMCOREDLL -DBOOST_IOSTREAMS_DYN_LINK)
endif()

file(GLOB_RECURSE TERRAMA2_SRC_FILES
         ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/core/*.cpp)
file(GLOB_RECURSE TERRAMA2_HDR_FILES
        ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/core/*.hpp)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})

include_directories ( SYSTEM
  ${Boost_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${QUAZIP_INCLUDE_DIR}
  ${QUAZIP_ZLIB_INCLUDE_DIR}
  ${Qt5Core_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIR}
  ${QUAZIP_INCLUDE_DIR}
  ${VMIME_INCLUDE_DIR}
)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(terrama2_core SHARED ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES})

qt5_use_modules(terrama2_core Core Sql Network)

target_link_libraries(terrama2_core terralib_mod_memory
                                        terralib_mod_dataaccess
                                        terralib_mod_common
                                        terralib_mod_ws_core
                                        ${Boost_FILESYSTEM_LIBRARY}
                                        ${Boost_SYSTEM_LIBRARY}
                                        ${Boost_LOG_LIBRARY}
                                        ${Boost_IOSTREAMS_LIBRARY}
                                        ${Boost_REGEX_LIBRARY}
                                        ${Boost_PYTHON_LIBRARY}
                                        ${PYTHON_LIBRARIES}
                                        ${QUAZIP_LIBRARIES}
                                        ${CURL_LIBRARY})

set_target_properties(terrama2_core
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 )

install(TARGETS terrama2_core
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
        DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/core COMPONENT devel)

export(TARGETS terrama2_core APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
