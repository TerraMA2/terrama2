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
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Vinicius Campanha <vinicius.campanha_indra@inpe.br>
#

#
# Macro TERRAMA2_GSOAP_SOAPCPP2
#
# Description: Generate the gSoap service files for server or client
#


MACRO(TERRAMA2_GSOAP_SOAPCPP2 filhe_path service_name type GSOAP_HDR_FILES GSOAP_SRC_FILES GSOAP_NSM_FILES)


if(${type} STREQUAL "server")

    set(COMMAND_LINE ${GSOAP_SOAPCPP2_EXECUTABLE} ARGS -S -i -w -x ${filhe_path})
    set(ARCHIVE_TYPE Service)


elseif(${type} STREQUAL "client")

    set(COMMAND_LINE ${GSOAP_SOAPCPP2_EXECUTABLE} ARGS -C -i -w -x ${filhe_path})
    set(ARCHIVE_TYPE Proxy)

endif()

    set(${GSOAP_HDR_FILES}   ${CMAKE_CURRENT_BINARY_DIR}/soap${service_name}${ARCHIVE_TYPE}.h
                           ${CMAKE_CURRENT_BINARY_DIR}/soapH.h
                           ${CMAKE_CURRENT_BINARY_DIR}/soapStub.h)
                        
    set(${GSOAP_SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/soap${service_name}${ARCHIVE_TYPE}.cpp
                           ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp)

    set(${GSOAP_NSM_FILES} ${CMAKE_CURRENT_BINARY_DIR}/collector.nsmap)


    add_custom_command(OUTPUT ${${GSOAP_HDR_FILES}} ${${GSOAP_SRC_FILES}} ${${GSOAP_NSM_FILES}}
                       COMMAND ${COMMAND_LINE}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       COMMENT "Generating gSoap Web Services files." VERBATIM)              

ENDMACRO(TERRAMA2_GSOAP_SOAPCPP2)
