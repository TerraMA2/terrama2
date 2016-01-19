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
  \file terrama2/collector/ParserPostgis.cpp

  \brief Parsers postgres/postgis data and create a terralib DataSet.

  \author Jano Simas
*/

#include "ParserPostgis.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "../core/Logger.hpp"

//QT
#include <QUrl>
#include <QDebug>
#include <QObject>

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

void terrama2::collector::ParserPostgis::read(terrama2::collector::DataFilterPtr filter,
                                              std::vector<TransferenceData>& transferenceDataVec)
{
  try
  {
    for(auto& tranferenceData : transferenceDataVec)
    {
      QUrl url(tranferenceData.uri_origin.c_str());
      std::map<std::string, std::string> metadata{ {"KIND", "POSTGIS"},
                                                          {"PG_HOST", url.host().toStdString()},
                                                          {"PG_PORT", std::to_string(url.port())},
                                                          {"PG_USER", url.userName().toStdString()},
                                                          {"PG_PASSWORD", url.password().toStdString()},
                                                          {"PG_DB_NAME", url.path().section("/", 1, 1).toStdString()},
                                                          {"PG_CONNECT_TIMEOUT", "4"},
                                                          {"PG_CLIENT_ENCODING", "UTF-8"}
                                                        };


      std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("POSTGIS"));
      datasource->setConnectionInfo(metadata);

      //RAII for open/closing the datasource
      OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

      if(!datasource->isOpened())
      {
        throw UnableToReadDataSetException() << terrama2::ErrorDescription(
                                                  QObject::tr("DataProvider could not be opened."));
      }

      // get a transactor to interact to the data source
      std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
      transactor->begin();

      std::vector<std::string> names = transactor->getDataSetNames();
      names = filter->filterNames(names);
      if(names.empty())
        throw NoDataSetFoundException() << terrama2::ErrorDescription(QObject::tr("No DataSet Found."));

      for(const auto& name : names)
      {
        std::shared_ptr<te::da::DataSet> dataSet(transactor->getDataSet(name));
        tranferenceData.teDatasetType = std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(name));

        if(!dataSet || !tranferenceData.teDatasetType)
        {
          throw UnableToReadDataSetException() << terrama2::ErrorDescription(
                                                    QObject::tr("DataSet: %1 is null.").arg(name.c_str()));
        }

        tranferenceData.teDataset = dataSet;
      }
    }
  }
  catch(te::common::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    throw UnableToReadDataSetException() << terrama2::ErrorDescription(
                                              QObject::tr("Terralib exception: ") +e.what());
  }
  catch(std::exception& e)
  {
    QString message = QObject::tr("Std exception.") + e.what();
    TERRAMA2_LOG_ERROR() << message;
    throw UnableToReadDataSetException() << terrama2::ErrorDescription(message);
  }
}
