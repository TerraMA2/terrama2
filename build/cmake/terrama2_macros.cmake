#
#  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.
#
# This file is part of TerraMA2 - a free and open source computational
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
#  CMake scripts for TerraMA2
#
#
#  Description: Auxiliary macros and functions.
#
#  Author: Gilberto Ribeiro de Queiroz
#          Vinicius Campanha
#

#
# Macro TERRAMA2_GSOAP_SOAPCPP2
#
#  Author: Gilberto Ribeiro de Queiroz
#          Vinicius Campanha
#
# Description: Generate the gSoap service files for server or client
#
MACRO(TERRAMA2_GSOAP_SOAPCPP2 file_path service_name type GSOAP_HDR_FILES GSOAP_SRC_FILES GSOAP_NSM_FILES)
  if(${type} STREQUAL "server")
    set(COMMAND_LINE ${GSOAP_SOAPCPP2_EXECUTABLE} ARGS -S -i -w -x ${file_path})
    set(ARCHIVE_TYPE Service)
  elseif(${type} STREQUAL "client")
    set(COMMAND_LINE ${GSOAP_SOAPCPP2_EXECUTABLE} ARGS -C -i -w -x ${file_path})
    set(ARCHIVE_TYPE Proxy)
  endif()

  make_directory(${CMAKE_CURRENT_BINARY_DIR}/generated)
  set(${GSOAP_HDR_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/soap${service_name}${ARCHIVE_TYPE}.h
                         ${CMAKE_CURRENT_BINARY_DIR}/generated/soapH.h
                         ${CMAKE_CURRENT_BINARY_DIR}/generated/soapStub.h)

  set(${GSOAP_SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/soap${service_name}${ARCHIVE_TYPE}.cpp
                         ${CMAKE_CURRENT_BINARY_DIR}/generated/soapC.cpp)

  set(${GSOAP_NSM_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/${service_name}.nsmap)

  add_custom_command(OUTPUT ${${GSOAP_HDR_FILES}} ${${GSOAP_SRC_FILES}} ${${GSOAP_NSM_FILES}}
                     COMMAND ${COMMAND_LINE}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated
                     COMMENT "Generating gSoap Web Services files." VERBATIM)

ENDMACRO(TERRAMA2_GSOAP_SOAPCPP2)


#
# Function TERRAMA2_ADD_QT_TRANSLATION
#
#  Author: Evandro Delatin
#          Gilberto Ribeiro de Queiroz
#          Raphael Willian da Costa
#
# Description: Add the list of .ts files to translation units.
#
# Parameters:
#   qm_files..: output variable.
#   ts_files..: the list of .ts files (with full path).
#   qm_out_dir: where to generate the .qm files.
#
function(TERRAMA2_ADD_QT_TRANSLATION qm_files ts_files qm_out_dir)
  set(ts_files ${ARGV1})
  set(qm_out_dir ${ARGV2})
  foreach(ts_file ${ts_files})
    get_filename_component(ts_file_absolute_path ${ts_file} ABSOLUTE)
    # warning: this will control the output location of generated qm files by macro QT5_ADD_TRANSLATION.
    set_source_files_properties(${ts_file_absolute_path} PROPERTIES OUTPUT_LOCATION "${qm_out_dir}")
    QT5_ADD_TRANSLATION(qt_qm_files ${ts_file_absolute_path})
  endforeach()
  set(${qm_files} ${qt_qm_files} PARENT_SCOPE)
endfunction(TERRAMA2_ADD_QT_TRANSLATION)

