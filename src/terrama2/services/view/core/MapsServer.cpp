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
  \file terrama2/services/view/core/MapsServer.cpp

  \brief

  \author Jano Simas
*/

#include "MapsServer.hpp"

#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/ws/core/CurlWrapper.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

bool terrama2::services::view::core::MapsServer::checkConnection() const
{
  te::ws::core::CurlWrapper cURLwrapper;
  try
  {
    cURLwrapper.verifyURL(uri_.uri(), 80);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

QJsonObject terrama2::services::view::core::MapsServer::generateLayers(const ViewPtr viewPtr,
                                                                       terrama2::core::DataSeriesPtr dataSeries,
                                                                       terrama2::core::DataProviderPtr dataProvider,
                                                                       const std::shared_ptr<DataManager> dataManager,
                                                                       std::shared_ptr<ViewLogger> logger,
                                                                       const RegisterId logId)
{
  if(dataProvider->dataProviderType == "POSTGIS" &&
      dataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::DCP)
  {
    // to create a dcp view we need a sql function that retrieve the last data
    // from all dcp tables
    std::shared_ptr< te::da::DataSource > dataSource(te::da::DataSourceFactory::make("POSTGIS", dataProvider->uri));
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(dataSource);
    if(!dataSource->isOpened())
    {
      QString errMsg = QObject::tr("Could not connect to database");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    std::string sqlFunctionFile = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/dcp_last_measures.sql");
    std::string sqlFunction = terrama2::core::readFileContents(sqlFunctionFile);
    dataSource->execute(sqlFunction);
  }

  return generateLayersInternal(viewPtr, std::make_pair(dataSeries, dataProvider), dataManager, logger, logId);
}
