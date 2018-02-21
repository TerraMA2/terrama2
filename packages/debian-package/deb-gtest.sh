#!/bin/bash
#
#  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.
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
#  Description: Generate a debia package of GTest for TerraMA2
#
#  Author: Vinicius Campanha
#
#
#  Example:
#  $ ./deb-gtest.sh
#
#
# Set Package Info
#

export UBUNTUVERSION=`lsb_release -rs`

export TMVERSION=4.0.3
export DEBNAME=terrama2-gtest
export DEBVERSION=1.8.0
export DEBARC=amd64
export LIBRARYNAME=GTest
export FILENAME=googletest-release-1.8.0.tar.gz
export FOLDERNAME=googletest-release-1.8.0
export DOWNLOAD_LINK=https://github.com/google/googletest/archive/release-1.8.0.tar.gz

#
# Valid parameter val or abort script
#
function valid()
{
  if [ $1 -ne 0 ]; then
    echo $2
    echo ""
    exit
  fi
}

#
# Donwload Source
#
# if [ ! -f "${FILENAME}" ]; then
#     wget "${DOWNLOAD_LINK}" -O ${FILENAME}
# fi

if [ ! -d "${FOLDERNAME}" ]; then
    tar xzvf  ${FILENAME}
    valid $? "Could not extract VMime!"
fi

if [ ! -d "${FOLDERNAME}" ]; then
    echo "Could not find VMime folder!"
    exit
fi

cd ${FOLDERNAME}

export ROOT_DIR=`pwd`

#
# Create debian folder
#
rm -rf debian
mkdir -p debian
valid $? "Could not create debian folder!"
#
# Create the copyright file
#
cat > debian/copyright <<EOF
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
# Create the changelog (no messages needed)
#
dch --create -v ${DEBVERSION}-ubuntu${UBUNTUVERSION} --distribution unstable --package ${DEBNAME} ""
#
# Create control file
#
cat > debian/control <<EOF
Source: ${DEBNAME}
Maintainer: TerraMA2 Team <terrama2-team@dpi.inpe.br>
Section: misc
Priority: optional
Standards-Version: 3.9.2
Build-Depends: debhelper (>= 7)

Package: ${DEBNAME}
Architecture: ${DEBARC}
Depends: \${shlibs:Depends}, \${misc:Depends}
Description: TerraMA2 ${LIBRARYNAME} Library, version ${DEBVERSION}
EOF
#
# Create rules file
#
cat > debian/rules <<EOF
#!/usr/bin/make -f
%:
	dh \$@
override_dh_auto_configure:
	mkdir -p debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty
        cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="`pwd`/debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty" -DCMAKE_INSTALL_RPATH:PATH="`pwd`/debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty/lib"
override_dh_auto_build:
	PREFIX=`pwd`/debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty make -j 4
override_dh_auto_test:
override_dh_auto_install:
	PREFIX=`pwd`/debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty make install -j 4
EOF
#
# Create some misc files
#
echo "8" > debian/compat
mkdir -p debian/source
echo "3.0 (quilt)" > debian/source/format

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="`pwd`/debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty" -DCMAKE_INSTALL_RPATH:PATH="`pwd`/debian/${DEBNAME}/opt/terrama2/${TMVERSION}/3rdparty/lib"

valid $? "Error at cmake"
#
# Build the package
#
# To add a GnuPG key just uncomment the "-k" and change the following key
nice -n19 ionice -c3 debuild -us -uc -b #-kFBC36213
