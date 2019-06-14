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
  \file terrama2/core/data-access/DataAccessorGeometricObjectPostGIS.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorWFS.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Utils.hpp"

//TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

//QT
#include <QUrl>
#include <QObject>

terrama2::core::DataAccessorWFS::DataAccessorWFS(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics)
 : DataAccessor(dataProvider, dataSeries),
   DataAccessorFile(dataProvider, dataSeries)
{
  if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
  }
}

void terrama2::core::DataAccessorWFS::retrieveDataCallback(const terrama2::core::DataRetrieverPtr dataRetriever,
                                                           terrama2::core::DataSetPtr dataset,
                                                           const terrama2::core::Filter &filter,
                                                           std::shared_ptr<terrama2::core::FileRemover> remover,
                                                           std::function<void (const std::string &, const std::string &)> processFile) const
{
  auto features = terrama2::core::getProperty(dataset, nullptr, "mask");

  std::string tempFolderToDownload;

  dataRetriever->retrieveDataCallback(features, filter, "", remover, "", tempFolderToDownload,
                                      [processFile](const std::string& uri, const std::string& file, const std::string& folderMatched) {
    processFile(uri, folderMatched);

    const QUrl urlToFile(QString::fromStdString(uri + "/" + folderMatched + "/" + file.c_str()));
    // Remove files to free disk
    QFile removeFile(urlToFile.toString(QUrl::NormalizePathSegments | QUrl::RemoveScheme));
    removeFile.remove();
  });
}

std::string terrama2::core::DataAccessorWFS::dataSourceType() const
{
  return "POSTGIS";
}
