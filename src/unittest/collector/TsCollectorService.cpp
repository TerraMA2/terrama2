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
  \file unittest/collector/TsCollectorService.cpp

  \brief Test Collector Service...

  \author Paulo R. M. Oliveira
*/

#include "TsCollectorService.hpp"

#include <terrama2/collector/CollectorService.hpp>

//QT
#include <QApplication>

// STL
#include <memory>

//terralib
#include <terralib/postgis/Utils.h>
#include <terralib/postgis/Connection.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>

#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

void TsCollectorService::initTestCase()
{

}

void TsCollectorService::cleanupTestCase()
{

}

void TsCollectorService::init()
{

}

void TsCollectorService::cleanup()
{

}


//QTEST_MAIN(TsCollectorService)
#include "TsCollectorService.moc"
