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
  \file terrama2/unittest/collector/TsLog.cpp

  \brief Tests for the collector Log.

 \author Vinicius Campanha
*/

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/memory/Transactor.h>

// TerraMA2
#include "TsLog.hpp"
#include "terrama2/collector/Log.hpp"


void TsLog::initTestCase()
{

}


void TsLog::cleanupTestCase()
{

}


void TsLog::init()
{

}


void TsLog::cleanup()
{

}


void TsLog::TestLog()
{
  //std::shared_ptr< te::da::DataSource > dataSource = std::make_shared< te::da::DataSource >(te::da::DataSourceFactory::make("POSTGIS"));
  //te::mem::Transactor transactor_mem(dataSource);

  //terrama2::collector::Log log(transactor_mem);
}
