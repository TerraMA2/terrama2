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
#  Description: CMake script for the TerraMA2 analysis example.
#
#  Author: Jano Simas
#

#if(WIN32)
#  add_definitions(-DTMA_CORE_DLL)
#endif()

SET(TERRAMA2_EXAMPLE_DIR "${TERRAMA2_ABSOLUTE_ROOT_DIR}/src/examples/analysis/")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER MATCHES ".*clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

file(GLOB TERRAMA2_SRC_FILES ${TERRAMA2_EXAMPLE_DIR}*.cpp)
file(GLOB TERRAMA2_HDR_FILES ${TERRAMA2_EXAMPLE_DIR}*.hpp)

include_directories (
  ${TERRAMA2_ABSOLUTE_ROOT_DIR}/src
)

include_directories ( SYSTEM
  ${Boost_INCLUDE_DIR}
  ${terralib_INCLUDE_DIRS}
  ${terralib_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${GTEST_INCLUDE_DIRS}
  ${GMOCK_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

source_group("Source Files"  FILES ${TERRAMA2_SRC_FILES})
source_group("Header Files"  FILES ${TERRAMA2_HDR_FILES})

foreach(FILE ${TERRAMA2_SRC_FILES})
   STRING(REGEX REPLACE ".cpp" "" EXAMPLE "${FILE}")
   STRING(REGEX REPLACE "${TERRAMA2_EXAMPLE_DIR}" "" EXAMPLE "${EXAMPLE}")


   add_executable(${EXAMPLE} ${FILE})

   target_link_libraries(${EXAMPLE} terrama2_core
                                    terrama2_analysis_core
                                    terrama2_impl
                                    terralib_mod_common
                                    ${GTEST_BOTH_LIBRARIES}
                                    ${GMOCK_BOTH_LIBRARIES}
                                    ${PYTHON_LIBRARIES})
endforeach()
