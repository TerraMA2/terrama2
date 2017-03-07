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
#  Description: Find JWSMTP - find JWSMTP include directory and libraries.
#
#  JWSMTP_INCLUDE_DIR - where to find jwsmtp.h.
#  JWSMTP_LIBRARY     - where to find jwsmtp libraries.
#  JWSMTP_FOUND       - True if jwsmtp found.
#
#  Author: Jano Simas <jano.simas@funcate.org.br>
#

if(UNIX)

  find_path(JWSMTP_INCLUDE_DIR jwsmtp.h
            PATHS /usr
                  /usr/local
                  /usr/local/jwsmtp
            PATH_SUFFIXES include
                          jwsmtp
                          include/jwsmtp)

  find_library(JWSMTP_LIBRARY
               NAMES jwsmtp
               PATHS /usr
                     /usr/local
                     /usr/local/jwsmtp
               PATH_SUFFIXES lib)

elseif(WIN32)

  find_path(JWSMTP_INCLUDE_DIR
            NAMES jwsmtp.h
            PATH_SUFFIXES include
                          jwsmtp
                          include/jwsmtp)

  find_library(JWSMTP_LIBRARY_RELEASE
               NAMES jwsmtp
               PATH_SUFFIXES lib)

  find_library(JWSMTP_LIBRARY_DEBUG
               NAMES jwsmtp_d jwsmtpd
               PATH_SUFFIXES lib)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(jwsmtp DEFAULT_MSG JWSMTP_LIBRARY JWSMTP_INCLUDE_DIR)

mark_as_advanced(JWSMTP_INCLUDE_DIR JWSMTP_LIBRARY)
