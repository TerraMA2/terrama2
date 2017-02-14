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
#  Description: CMake script for the TerraMA2 Analysis Core Runtime Module.
#
#  Author: Gilberto Ribeiro de Queiroz
#

#if(WIN32)
#  add_definitions(-DTMA_CORE_DLL)
#endif()

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/*.hpp)

file(GLOB TERRAMA2_UTILITY_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/utility/*.cpp)
file(GLOB TERRAMA2_UTILITY_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/utility/*.hpp)

file(GLOB TERRAMA2_PYTHON_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/python/*.cpp)
file(GLOB TERRAMA2_PYTHON_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/python/*.hpp)

file(GLOB TERRAMA2_DCP_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/*.cpp)
file(GLOB TERRAMA2_DCP_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/*.hpp)

file(GLOB TERRAMA2_DCP_HISTORY_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/history/*.cpp)
file(GLOB TERRAMA2_DCP_HISTORY_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/history/*.hpp)

file(GLOB TERRAMA2_DCP_ZONAL_HISTORY_INTERVAL_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/history/interval/*.cpp)
file(GLOB TERRAMA2_TERRAMA2_DCP_ZONAL_HISTORY_INTERVAL_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/history/interval/*.hpp)

file(GLOB TERRAMA2_DCP_INFLUENCE_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/influence/*.cpp)
file(GLOB TERRAMA2_DCP_INFLUENCE_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/dcp/zonal/influence/*.hpp)

file(GLOB TERRAMA2_OCCURRENCE_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/occurrence/*.cpp)
file(GLOB TERRAMA2_OCCURRENCE_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/occurrence/*.hpp)

file(GLOB TERRAMA2_OCCURRENCE_AGRREGATION_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/occurrence/aggregation/*.cpp)
file(GLOB TERRAMA2_OCCURRENCE_AGGREGATION_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/occurrence/aggregation/*.hpp)

file(GLOB TERRAMA2_GRID_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/*.cpp)
file(GLOB TERRAMA2_GRID_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_FORECAST_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/forecast/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_FORECAST_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/forecast/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_FORECAST_INTERVAL_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/forecast/interval/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_FORECAST_INTERVAL_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/forecast/interval/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_FORECAST_ACCUM_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/forecast/accum/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_FORECAST_ACCUM_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/forecast/accum/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_ACCUM_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/accum/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_ACCUM_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/accum/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_PREC_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/prec/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_PREC_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/prec/*.hpp)

file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_INTERVAL_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/interval/*.cpp)
file(GLOB TERRAMA2_GRID_ZONAL_HISTORY_INTERVAL_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/zonal/history/interval/*.hpp)

file(GLOB TERRAMA2_GRID_HISTORY_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/history/*.cpp)
file(GLOB TERRAMA2_GRID_HISTORY_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/history/*.hpp)

file(GLOB TERRAMA2_GRID_HISTORY_INTERVAL_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/history/interval/*.cpp)
file(GLOB TERRAMA2_GRID_HISTORY_INTERVAL_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/history/interval/*.hpp)

file(GLOB TERRAMA2_GRID_FORECAST_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/forecast/*.cpp)
file(GLOB TERRAMA2_GRID_FORECAST_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/forecast/*.hpp)

file(GLOB TERRAMA2_GRID_FORECAST_INTERVAL_SRC_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/forecast/interval/*.cpp)
file(GLOB TERRAMA2_GRID_FORECAST_INTERVAL_HDR_FILES ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/grid/forecast/interval/*.hpp)



source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})
source_group("Source Files\\utility"  FILES ${TERRAMA2_UTILITY_SRC_FILES})
source_group("Header Files\\utility"  FILES ${TERRAMA2_UTILITY_HDR_FILES})
source_group("Source Files\\python"  FILES ${TERRAMA2_PYTHON_SRC_FILES})
source_group("Header Files\\python"  FILES ${TERRAMA2_PYTHON_HDR_FILES})
source_group("Source Files\\dcp\\zonal"  FILES ${TERRAMA2_DCP_SRC_FILES})
source_group("Header Files\\dcp\\zonal"  FILES ${TERRAMA2_DCP_HDR_FILES})
source_group("Source Files\\dcp\\zonal\\history"  FILES ${TERRAMA2_DCP_HISTORY_SRC_FILES})
source_group("Header Files\\dcp\\zonal\\history"  FILES ${TERRAMA2_DCP_HISTORY_HDR_FILES})
source_group("Source Files\\dcp\\zonal\\history\\interval"  FILES ${TERRAMA2_DCP_ZONAL_HISTORY_INTERVAL_SRC_FILES})
source_group("Header Files\\dcp\\zonal\\history\\interval"  FILES ${TERRAMA2_TERRAMA2_DCP_ZONAL_HISTORY_INTERVAL_HDR_FILES})
source_group("Source Files\\dcp\\zonal\\influence"  FILES ${TERRAMA2_DCP_INFLUENCE_SRC_FILES})
source_group("Header Files\\dcp\\zonal\\influence"  FILES ${TERRAMA2_DCP_INFLUENCE_HDR_FILES})
source_group("Source Files\\occurrence"  FILES ${TERRAMA2_OCCURRENCE_SRC_FILES})
source_group("Header Files\\occurrence"  FILES ${TERRAMA2_OCCURRENCE_HDR_FILES})
source_group("Source Files\\occurrence\\aggregation"  FILES ${TERRAMA2_OCCURRENCE_AGRREGATION_SRC_FILES})
source_group("Header Files\\occurrence\\aggregation"  FILES ${TERRAMA2_OCCURRENCE_AGGREGATION_HDR_FILES})
source_group("Source Files\\grid"  FILES ${TERRAMA2_GRID_SRC_FILES})
source_group("Header Files\\grid"  FILES ${TERRAMA2_GRID_HDR_FILES})
source_group("Source Files\\grid\\zonal"  FILES ${TERRAMA2_GRID_ZONAL_SRC_FILES})
source_group("Header Files\\grid\\zonal"  FILES ${TERRAMA2_GRID_ZONAL_HDR_FILES})
source_group("Source Files\\grid\\zonal\\history"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_SRC_FILES})
source_group("Header Files\\grid\\zonal\\history"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_HDR_FILES})
source_group("Source Files\\grid\\zonal\\forecast"  FILES ${TERRAMA2_GRID_ZONAL_FORECAST_SRC_FILES})
source_group("Header Files\\grid\\zonal\\forecast"  FILES ${TERRAMA2_GRID_ZONAL_FORECAST_HDR_FILES})
source_group("Source Files\\grid\\zonal\\forecast\\interval"  FILES ${TERRAMA2_GRID_ZONAL_FORECAST_INTERVAL_SRC_FILES})
source_group("Header Files\\grid\\zonal\\forecast\\interval"  FILES ${TERRAMA2_GRID_ZONAL_FORECAST_INTERVAL_HDR_FILES})
source_group("Source Files\\grid\\zonal\\forecast\\accum"  FILES ${TERRAMA2_GRID_ZONAL_FORECAST_ACCUM_SRC_FILES})
source_group("Header Files\\grid\\zonal\\forecast\\accum"  FILES ${TERRAMA2_GRID_ZONAL_FORECAST_ACCUM_HDR_FILES})
source_group("Source Files\\grid\\zonal\\history\\accum"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_ACCUM_SRC_FILES})
source_group("Header Files\\grid\\zonal\\history\\accum"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_ACCUM_SRC_FILES})
source_group("Source Files\\grid\\zonal\\history\\prec"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_PREC_SRC_FILES})
source_group("Header Files\\grid\\zonal\\history\\prec"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_PREC_SRC_FILES})
source_group("Source Files\\grid\\zonal\\history\\interval"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_INTERVAL_SRC_FILES})
source_group("Header Files\\grid\\zonal\\history\\interval"  FILES ${TERRAMA2_GRID_ZONAL_HISTORY_INTERVAL_SRC_FILES})
source_group("Source Files\\grid\\history"  FILES ${TERRAMA2_GRID_HISTORY_SRC_FILES})
source_group("Header Files\\grid\\history"  FILES ${TERRAMA2_GRID_HISTORY_HDR_FILES})
source_group("Source Files\\grid\\history\\interval"  FILES ${TERRAMA2_GRID_HISTORY_INTERVAL_SRC_FILES})
source_group("Header Files\\grid\\history\\interval"  FILES ${TERRAMA2_GRID_HISTORY_INTERVAL_HDR_FILES})
source_group("Source Files\\grid\\forecast"  FILES ${TERRAMA2_GRID_HISTORY_SRC_FILES})
source_group("Header Files\\grid\\forecast"  FILES ${TERRAMA2_GRID_HISTORY_HDR_FILES})
source_group("Source Files\\grid\\forecast\\interval"  FILES ${TERRAMA2_GRID_HISTORY_INTERVAL_SRC_FILES})
source_group("Header Files\\grid\\forecast\\interval"  FILES ${TERRAMA2_GRID_HISTORY_INTERVAL_HDR_FILES})

include_directories ( SYSTEM
  ${Boost_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${ThreadPool_INCLUDE_DIRS}
)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(terrama2_analysis_core SHARED ${TERRAMA2_SRC_FILES}
                                          ${TERRAMA2_HDR_FILES}
                                          ${TERRAMA2_UTILITY_SRC_FILES}
                                          ${TERRAMA2_UTILITY_HDR_FILES}
                                          ${TERRAMA2_PYTHON_SRC_FILES}
                                          ${TERRAMA2_PYTHON_HDR_FILES}
                                          ${TERRAMA2_DCP_SRC_FILES}
                                          ${TERRAMA2_DCP_HDR_FILES}
                                          ${TERRAMA2_DCP_HISTORY_SRC_FILES}
                                          ${TERRAMA2_DCP_HISTORY_HDR_FILES}
                                          ${TERRAMA2_DCP_ZONAL_HISTORY_INTERVAL_SRC_FILES}
                                          ${TERRAMA2_TERRAMA2_DCP_ZONAL_HISTORY_INTERVAL_HDR_FILES}
                                          ${TERRAMA2_DCP_INFLUENCE_SRC_FILES}
                                          ${TERRAMA2_DCP_INFLUENCE_HDR_FILES}
                                          ${TERRAMA2_OCCURRENCE_SRC_FILES}
                                          ${TERRAMA2_OCCURRENCE_HDR_FILES}
                                          ${TERRAMA2_OCCURRENCE_AGRREGATION_SRC_FILES}
                                          ${TERRAMA2_OCCURRENCE_AGGREGATION_HDR_FILES_SRC_FILES}
                                          ${TERRAMA2_GRID_SRC_FILES}
                                          ${TERRAMA2_GRID_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_FORECAST_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_FORECAST_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_FORECAST_ACCUM_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_FORECAST_ACCUM_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_FORECAST_INTERVAL_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_FORECAST_INTERVAL_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_ACCUM_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_ACCUM_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_PREC_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_PREC_HDR_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_INTERVAL_SRC_FILES}
                                          ${TERRAMA2_GRID_ZONAL_HISTORY_INTERVAL_HDR_FILES}
                                          ${TERRAMA2_GRID_HISTORY_SRC_FILES}
                                          ${TERRAMA2_GRID_HISTORY_HDR_FILES}
                                          ${TERRAMA2_GRID_HISTORY_INTERVAL_SRC_FILES}
                                          ${TERRAMA2_GRID_HISTORY_INTERVAL_HDR_FILES}
                                          ${TERRAMA2_GRID_FORECAST_SRC_FILES}
                                          ${TERRAMA2_GRID_FORECAST_HDR_FILES}
                                          ${TERRAMA2_GRID_FORECAST_INTERVAL_SRC_FILES}
                                          ${TERRAMA2_GRID_FORECAST_INTERVAL_HDR_FILES})

qt5_use_modules(terrama2_analysis_core Core)



target_link_libraries(terrama2_analysis_core  terrama2_core
                                              terrama2_impl
                                              terralib_mod_dataaccess
                                              terralib_mod_memory
                                              terralib_mod_vp_core
                                              terralib_mod_rp
                                              ${PYTHON_LIBRARIES}
                                              ${Boost_PYTHON_LIBRARY})


PYTHON_ADD_MODULE(terrama2 ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/terrama2/services/analysis/core/python/PythonInterpreter.cpp)

qt5_use_modules(terrama2 Core)


target_link_libraries(terrama2  terrama2_analysis_core
                                terrama2_core
                                terrama2_impl
                                terralib_mod_dataaccess
                                terralib_mod_memory
                                terralib_mod_vp_core
                                terralib_mod_rp
                                ${PYTHON_LIBRARIES}
                                ${Boost_PYTHON_LIBRARY})

set_target_properties(terrama2_analysis_core
                      PROPERTIES VERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 SOVERSION ${TERRAMA2_VERSION_MAJOR}.${TERRAMA2_VERSION_MINOR}
                                 INSTALL_NAME_DIR "@executable_path/../lib")

install(TARGETS terrama2_analysis_core
        EXPORT terrama2-targets
        RUNTIME DESTINATION ${TERRAMA2_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${TERRAMA2_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${TERRAMA2_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${TERRAMA2_HDR_FILES}
DESTINATION ${TERRAMA2_DESTINATION_HEADERS}/terrama2/analysis COMPONENT devel)

export(TARGETS terrama2_analysis_core APPEND FILE ${CMAKE_BINARY_DIR}/terrama2-exports.cmake)
