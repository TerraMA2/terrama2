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
#  Author: Evandro Delatin
#  Author: Raphael Willian
#

include_directories (
  ${Boost_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TERRAMA2_GUI_CORE_PATH ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/gui/core)

file(GLOB TERRAMA2_TRANSLATION_FILES "${TERRAMA2_ABSOLUTE_ROOT_DIR}/share/terrama2/translations/*.ts")
file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_GUI_CORE_PATH}/*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_GUI_CORE_PATH}/*.hpp)
file(GLOB TERRAMA2_UI_FILES ${TERRAMA2_GUI_CORE_PATH}/ui/*.ui)

TERRAMA2_ADD_QT_TRANSLATION(TERRAMA2_QM_FILES "${TERRAMA2_TRANSLATION_FILES}" "${CMAKE_BINARY_DIR}/share/terrama2/translations")

QT5_WRAP_UI(TERRAMA2_GEN_HDR_FILES ${TERRAMA2_UI_FILES})

add_library(terrama2_gui_core SHARED ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES} ${TERRAMA2_GEN_HDR_FILES} ${TERRAMA2_QM_FILES})

qt5_use_modules(terrama2_gui_core Widgets Network)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})
source_group("UI Files"  FILES ${TERRAMA2_UI_FILES})

target_link_libraries(terrama2_gui_core terrama2_mod_core
                                        ${Boost_FILESYSTEM_LIBRARY}
                                        ${Boost_SYSTEM_LIBRARY})

set_target_properties(terrama2_gui_core
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../lib")

install(TARGETS terrama2_gui_core
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
        DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/gui/core COMPONENT devel)

export(TARGETS terrama2_mod_core APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
