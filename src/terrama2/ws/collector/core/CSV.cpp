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
  \file terrama2/ws/collector/core/CSV.cpp

  \brief Implementation of a collector for the CSV format.

  \author Paulo R. M. Oliveira
*/

#include "CSV.hpp"
#include "terrama2/ws/collector/core/StorageStrategy.hpp"

// STL
#include <stdlib.h>
#include <memory>

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>

terrama2::ws::collector::core::CSV::CSV()
{

}


terrama2::ws::collector::core::CSV::~CSV()
{

}


void terrama2::ws::collector::core::CSV::collect(const std::string &file)
{
  std::auto_ptr<te::da::DataSource> dsOGR = te::da::DataSourceFactory::make("OGR");

  std::map<std::string, std::string> connInfo;

  connInfo["SOURCE"] = file;

  dsOGR->setConnectionInfo(connInfo);
  dsOGR->open();

  std::auto_ptr<te::da::DataSourceTransactor> ogrTransactor = dsOGR->getTransactor();
  std::auto_ptr<te::da::DataSet> ogrDataSet = dsOGR->getDataSet("csv_teste");
  std::auto_ptr<te::da::DataSetType>  dtype = dsOGR->getDataSetType("csv_teste");
  /* PostGIS database to gets the DataSource Capabilities */
  std::auto_ptr<te::da::DataSource> dsPGIS = te::da::DataSourceFactory::make("POSTGIS");




}
