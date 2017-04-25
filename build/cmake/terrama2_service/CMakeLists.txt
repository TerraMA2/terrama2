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
#  Description: CMake script for the TerraMA2 Collector Module.
#
#  Author: Jano Simas
#

#if(WIN32)
#  add_definitions(-DTMA_CORE_DLL)
#endif()

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/appserver/*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/appserver/*.hpp)


source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})

include_directories (
  ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src
)

include_directories ( SYSTEM
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${Boost_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(terrama2_service ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES})

qt5_use_modules(terrama2_service Core)

target_link_libraries(terrama2_service terrama2_core
                                       terrama2_impl
                                       ${PYTHON_LIBRARIES}
                                       ${Boost_PROGRAM_OPTIONS_LIBRARY})

IF(TERRAMA2_COLLECTOR_ENABLED)
  target_link_libraries(terrama2_service terrama2_collector_core)
ENDIF()

IF(TERRAMA2_ANALYSIS_CORE_ENABLED)
  target_link_libraries(terrama2_service terrama2_analysis_core)
ENDIF()

IF(TERRAMA2_VIEW_ENABLED)
  target_link_libraries(terrama2_service terrama2_view_core)
ENDIF()

IF(TERRAMA2_ALERT_ENABLED)
  target_link_libraries(terrama2_service terrama2_alert_core)
ENDIF()

set_target_properties(terrama2_service
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                               INSTALL_NAME_DIR "@executable_path/../bin")

install(TARGETS terrama2_service
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/services COMPONENT devel)

install(CODE "execute_process(COMMAND sudo ln -sf ${CMAKE_INSTALL_PREFIX}/bin/terrama2_service /usr/bin/terrama2_service)")

export(TARGETS terrama2_service APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
