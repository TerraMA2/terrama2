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
  \file terrama2/core/data-access/DataAccessorDcpSingleTable.hpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorDCPSingleTable.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"

#include <QObject>

terrama2::core::DataAccessorDcpSingleTable::DataAccessorDcpSingleTable(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
 :  DataAccessor(dataProvider, dataSeries),
    DataAccessorDcpPostGIS(dataProvider, dataSeries, false)
{
  if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
  }
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorDcpSingleTable::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorDcpSingleTable>(dataProvider, dataSeries);
}

std::string terrama2::core::DataAccessorDcpSingleTable::getDCPId(terrama2::core::DataSetPtr dataSet) const
{
  return std::to_string(dataSet->id);
}

std::string terrama2::core::DataAccessorDcpSingleTable::getDCPIdPorperty(terrama2::core::DataSetPtr /*dataSet*/) const
{
  return "dcp_id";
}

std::string terrama2::core::DataAccessorDcpSingleTable::getDataSetTableName(terrama2::core::DataSetPtr /*dataSet*/) const
{
  return "dcp_data_"+std::to_string(dataSeries_->id);
}

void terrama2::core::DataAccessorDcpSingleTable::addExtraConditions(terrama2::core::DataSetPtr dataSet, std::vector<std::string>& whereConditions) const
{
  std::string id = getDCPId(dataSet);
  std::string idProperty = getDCPIdPorperty(dataSet);

  whereConditions.push_back(idProperty+" = '"+id+"'");
}
