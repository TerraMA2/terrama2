#
#  Copyright (C) 2008-2014 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of the TerraLib - a Framework for building GIS enabled applications.
#
#  TerraLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  TerraLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with TerraLib. See COPYING. If not, write to
#  TerraLib Team at <terralib-team@terralib.org>.
#
#
#  Description: Find VMIME - find VMIME include directory and libraries.
#
#  VMIME_INCLUDE_DIR - where to find vmime.h.
#  VMIME_LIBRARY     - where to find vmime libraries.
#  VMIME_FOUND       - True if vmime found.
#
#  Author: Jano Simas <jano.simas@funcate.org.br>
#

if(UNIX)

  find_path(VMIME_INCLUDE_DIR vmime/vmime.hpp
            PATHS /usr
                  /usr/local
                  /opt/terrama2/4.0.0/3rdparty
            PATH_SUFFIXES include
                          vmime)

  find_library(VMIME_LIBRARY
               NAMES vmime
               PATHS /usr
                     /usr/local
                     /usr/local/vmime
                     /opt/terrama2/4.0.0/3rdparty
               PATH_SUFFIXES lib)

elseif(WIN32)

  find_path(VMIME_INCLUDE_DIR
            NAMES vmime.h
            PATH_SUFFIXES include
                          vmime
                          include/vmime)

  find_library(VMIME_LIBRARY_RELEASE
               NAMES vmime
               PATH_SUFFIXES lib)

  find_library(VMIME_LIBRARY_DEBUG
               NAMES vmime_d vmimed
               PATH_SUFFIXES lib)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(vmime DEFAULT_MSG VMIME_LIBRARY VMIME_INCLUDE_DIR)

mark_as_advanced(VMIME_INCLUDE_DIR VMIME_LIBRARY)
