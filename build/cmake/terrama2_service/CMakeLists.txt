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
file(GLOB TERRAMA2_UI_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/appserver/*.ui)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})
source_group("UI Files"      FILES ${TERRAMA2_UI_FILES})

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

QT5_WRAP_UI(TERRAMA2_GEN_HDR_FILES ${TERRAMA2_UI_FILES})


if(APPLE AND TERRAMA2_BUILD_AS_BUNDLE)

  find_library(TERRAMA2_COREFOUNDATION_LIBRARY
               NAMES CoreFoundation
               PATHS "/")
  
  set(MACOSX_BUNDLE_BUNDLE_NAME "terrama2-service")
  set(MACOSX_BUNDLE_SIGNATURE "terrama2-service")
  set(MACOSX_BUNDLE_EXECUTABLE_NAME "terrama2.sh")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2001-2015")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "http://www.terrama2.org")
  set(MACOSX_BUNDLE_INFO_STRING "System for manipulating geographical data.")
  set(MACOSX_BUNDLE_ICON_FILE "../share/terrama2/icons/app/terrama2_logo2.icns")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING ${TERRAMA2_STRING_VERSION})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${TERRAMA2_PKG_VERSION})
  set(MACOSX_BUNDLE_BUNDLE_VERSION ${TERRAMA2_STRING_VERSION})

  configure_file(${CMAKE_ROOT}/Modules/MacOSXBundleInfo.plist.in ${CMAKE_BINARY_DIR}/Info.plist)
  set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist)

endif()

add_executable(terrama2_service ${TERRAMA2_SRC_FILES} ${TERRAMA2_HDR_FILES} ${TERRAMA2_GEN_HDR_FILES})

qt5_use_modules(terrama2_service Core Widgets)

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
  target_link_libraries(terrama2_service terrama2_alert_core
                                         terrama2_alert_impl)
ENDIF()

install(TARGETS terrama2_service
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime
        BUNDLE DESTINATION . COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/services COMPONENT devel)

if(UNIX AND NOT APPLE)
  install(CODE "execute_process(COMMAND sudo ln -sf ${CMAKE_INSTALL_PREFIX}/bin/terrama2_service /usr/bin/terrama2_service)")
endif()

if(APPLE AND TERRAMA2_BUILD_AS_BUNDLE)

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
     "[Paths]\nBinaries = MacOS\nPlugins = qtplugins")

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
          DESTINATION ${TERRAMA2_BASE_DESTINATION_DIR}/Resources COMPONENT runtime)
  
elseif(WIN32)
    
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
    "[Paths]\nBinaries = .\nPlugins = ../qtplugins")
         
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt.conf
          DESTINATION bin COMPONENT runtime)          
endif()

export(TARGETS terrama2_service APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
