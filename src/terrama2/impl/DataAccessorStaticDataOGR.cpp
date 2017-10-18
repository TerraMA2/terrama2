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
  \file terrama2/core/impl/DataAccessorStaticDataOGR.cpp

  \brief DataAccessor class for static data accessed via OGR driver.

  \author Paulo R. M. Oliveira
 */

//TerraMA2
#include "DataAccessorStaticDataOGR.hpp"

#include "../core/Exception.hpp"
#include "../core/utility/Logger.hpp"

// QT
#include <QObject>

terrama2::core::DataAccessorStaticDataOGR::DataAccessorStaticDataOGR(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
: DataAccessor(dataProvider, dataSeries),
  DataAccessorGeometricObject(dataProvider, dataSeries),
  DataAccessorFile(dataProvider, dataSeries)
{
 if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
 {
   QString errMsg = QObject::tr("Wrong DataSeries semantics.");
   TERRAMA2_LOG_ERROR() << errMsg;
   throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
 }
}

terrama2::core::DataAccessorStaticDataOGR::~DataAccessorStaticDataOGR()
{

}

std::string terrama2::core::DataAccessorStaticDataOGR::dataSourceType() const
{
  return "OGR";
}

std::string terrama2::core::DataAccessorStaticDataOGR::getTimeZone(DataSetPtr dataSet, bool /*logErrors*/) const
{
  return terrama2::core::DataAccessorFile::getTimeZone(dataSet, false);
}
