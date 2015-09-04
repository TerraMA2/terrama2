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
#  Description: Find gSOAP include directory,libraries and tools.
#
#  GSOAP_INCLUDE_DIR           - where to find stdsoap2.h.
#  GSOAP_CPP_LIBRARY           - the gsoap++ library link.
#  GSOAP_SOAPCPP2_EXECUTABLE   - the soapcpp2 application.
#  GSOAP_FOUND                 - True if gSOAP found.
#
#  Author: Gilberto Ribeiro de Queiroz <gribeiro@dpi.inpe.br>
#          Vinicius Campanha <vinicius.campanha_indra@inpe.br>
#

find_path(GSOAP_INCLUDE_DIR
          NAMES stdsoap2.h
          PATHS /usr
                /usr/local
          PATH_SUFFIXES include)

if(UNIX)

  find_library(GSOAP_CPP_LIBRARY
               NAMES gsoap++
               PATHS /usr
                     /usr/local
               PATH_SUFFIXES lib)
elseif(WIN32)

  find_library(GSOAP_CPP_LIBRARY_RELEASE
               NAMES gsoap++
               PATH_SUFFIXES lib)

  find_library(GSOAP_CPP_LIBRARY_DEBUG
               NAMES gsoap++_d gsoap++d gsoap++
               PATH_SUFFIXES lib)

  if(GSOAP_CPP_LIBRARY_RELEASE AND GSOAP_CPP_LIBRARY_DEBUG)
    set(GSOAP_CPP_LIBRARY optimized ${GSOAP_CPP_LIBRARY_RELEASE} debug ${GSOAP_CPP_LIBRARY_DEBUG})
  elseif(GSOAP_CPP_LIBRARY_RELEASE)
    set(GSOAP_CPP_LIBRARY optimized ${GSOAP_CPP_LIBRARY_RELEASE} debug ${GSOAP_CPP_LIBRARY_RELEASE})
  elseif(GSOAP_CPP_LIBRARY_DEBUG)
    set(GSOAP_CPP_LIBRARY optimized ${GSOAP_CPP_LIBRARY_DEBUG} debug ${GSOAP_CPP_LIBRARY_DEBUG})
  endif()

endif()

find_program(GSOAP_SOAPCPP2_EXECUTABLE
             NAMES soapcpp2
             PATHS /usr
                   /usr/local
             PATH_SUFFIXES bin)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(gSOAP DEFAULT_MSG GSOAP_CPP_LIBRARY GSOAP_INCLUDE_DIR GSOAP_SOAPCPP2_EXECUTABLE)

mark_as_advanced(GSOAP_INCLUDE_DIR GSOAP_CPP_LIBRARY GSOAP_SOAPCPP2_EXECUTABLE)
