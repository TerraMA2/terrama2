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
  \file terrama2/core/data-access/DataStoragerPostGIS.cpp

  \brief

  \author Jano Simas
 */

#include "DataStoragerPostGIS.hpp"

#include "../core/data-model/DataProvider.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Utils.hpp"

//terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QUrl>

//STL
#include <algorithm>

terrama2::core::DataStoragerPtr terrama2::core::DataStoragerPostGIS::make(DataSeriesPtr dataSeries, DataProviderPtr dataProvider)
{
  return std::make_shared<DataStoragerPostGIS>(dataSeries, dataProvider);
}

std::string terrama2::core::DataStoragerPostGIS::getCompleteURI(DataSetPtr outputDataSet) const
{
  return dataProvider_->uri;
}

std::string terrama2::core::DataStoragerPostGIS::getDataSetTableName(DataSetPtr dataSet) const
{
  return getProperty(dataSet, dataSeries_, "table_name");
}
