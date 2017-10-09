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
#  Description: CMake script for the TerraMA2 View Core.
#
#  Author: Vinicius Campanha
#

#if(WIN32)
#  add_definitions(-DTMA_CORE_DLL)
#endif()

file(GLOB_RECURSE TERRAMA2_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/view/*.cpp)
file(GLOB_RECURSE TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/view/*.hpp)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})

include_directories ( SYSTEM
  ${Boost_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${Qt5Core_INCLUDE_DIRS}
)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(terrama2_view_core SHARED ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES})

qt5_use_modules(terrama2_view_core Core)

target_link_libraries(terrama2_view_core terrama2_core
                                        terrama2_impl
                                        terralib_mod_maptools
                                        terralib_mod_qt_widgets
                                        terralib_mod_ws_core
                                        terralib_mod_ws_ogc_wms_client)

set_target_properties(terrama2_view_core
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 )

install(TARGETS terrama2_view_core
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
        DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/view COMPONENT devel)
