/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.
  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.
  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.
  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file src/unittest/collector/IntRasterTs.cpp
  \brief Integration test for raster data
  \author Jano Simas
*/

#ifndef __TERRAMA2_UNITTEST_COLLECTOR_INTEGRATION_RASTER_HPP__
#define __TERRAMA2_UNITTEST_COLLECTOR_INTEGRATION_RASTER_HPP__


#include <QtTest>


class IntRasterTs: public QObject
{
    Q_OBJECT

  private slots:

    void initTestCase(){} // Run before all tests
    void cleanupTestCase(){} // Run after all tests

    void init(){ } //run before each test
    void cleanup(){ } //run before each test

    //******Test functions********

    /*!
    \brief Collect from ftp, crop and store some grads raster files as tiff.

    The data is collected from the DSA ftp server as GrADS, the area is filtered and crop by the Brazilian Amazonas state and the time is between 2016-11-25T06:00:00.000-02:00 and 2016-11-25T12:00:00.000-02:00

    Bounding box of the region: SRID=4326;POLYGON((-73.8036991603083 -9.81412714740936,-73.8036991603083 2.24662115728613,-56.097053202293 2.24662115728613,-56.097053202293 -9.81412714740936,-73.8036991603083 -9.81412714740936))
    */
    void CollectAndCropRaster();
};

#endif //__TERRAMA2_UNITTEST_COLLECTOR_INTEGRATION_RASTER_HPP__
