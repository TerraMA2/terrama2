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
#  Description: CMake script for the TerraMA2 Alert Module.
#
#  Author: Jano Simas
#

#if(WIN32)
#  add_definitions(-DTMA_CORE_DLL)
#endif()

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/alert/impl/*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/alert/impl/*.hpp)


source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})

include_directories ( SYSTEM
  ${Boost_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${Qt5Core_INCLUDE_DIRS}
  ${VMIME_INCLUDE_DIR}
)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(terrama2_alert_impl SHARED ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES})

qt5_use_modules(terrama2_alert_impl Core)

target_link_libraries(terrama2_alert_impl terrama2_core
                                          terrama2_alert_core
                                          terralib_mod_dataaccess)

IF(VMIME_FOUND)
  target_link_libraries(terrama2_alert_impl ${VMIME_LIBRARY})
ENDIF()

IF(Qt5_VERSION VERSION_LESS "5.3")
  add_definitions(-DQt5_BELLOW_5_3)
ENDIF()

set_target_properties(terrama2_alert_impl
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 )

install(TARGETS terrama2_alert_impl
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/alert COMPONENT devel)

export(TARGETS terrama2_alert_impl APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
