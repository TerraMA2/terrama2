#!/bin/bash
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
#  Description: Install TerraMa2 Helper on Linux Ubuntu.
#
#  Author: Carolina Galvão dos Santos
#
#
#  Example:
#  $ ./deb-helper.sh
#

export TMVERSION=4.1.1-beta
export DEBNAME=terrama2-doc-${TMVERSION}
export DEBARC=amd64
export HELPER_FOLDER=`pwd`/../../helper
export TM_FOLDER=opt/terrama2/${TMVERSION}

#
# Create folders
#
mkdir -p ${DEBNAME}/DEBIAN
mkdir -p ${DEBNAME}/${TM_FOLDER}
#
# Create de Copyright file
#
cat <<EOF >> ${DEBNAME}/DEBIAN/copyright
Format: http://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: TerraMA2 ${LIBRARYNAME} Library
Upstream-Contact: TerraMA2 Team <terrama2-team@dpi.inpe.br>
Source: https://github.com/TerraMA2/terrama2

Files: *
Copyright: Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil
License: LGPL-3.0
  TerraMA2 is free software, you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, write to TerraMA2 Team at <terrama2-team@dpi.inpe.br>.

EOF
#
# Create de Control file
#
cat <<EOF >> ${DEBNAME}/DEBIAN/control
Package: ${DEBNAME}
Maintainer: TerraMA2 Team <terrama2-team@dpi.inpe.br>
Section: misc
Priority: optional
Version: ${TMVERSION}
Architecture: ${DEBARC}
Description: TerraMA2 Help files, version ${TMVERSION}
EOF
#
# Copy folder to be installed
#
cp -r ${HELPER_FOLDER} `pwd`/${DEBNAME}/${TM_FOLDER}
#
# Build the package
#
dpkg-deb --build ${DEBNAME}
