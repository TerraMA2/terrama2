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
  \file terrama2/services/view/core/View.hpp

  \brief Utility functions for view module.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Utils.hpp"
#include "Exception.hpp"
#include "MapsServerFactory.hpp"
#include "data-access/Geoserver.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/DateTime.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/geometry/Utils.h>


// Qt
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QString>
#include <QUrl>


void terrama2::services::view::core::registerFactories()
{
  MapsServerFactory::getInstance().add(terrama2::services::view::core::GeoServer::mapsServerType(),
                                       terrama2::services::view::core::GeoServer::make);
}

int terrama2::services::view::core::createGeoserverTempMosaic(terrama2::core::DataManagerPtr dataManager,
                                                              terrama2::core::DataSetPtr dataset,
                                                              const terrama2::core::Filter& filter,
                                                              const std::string& exhibitionName,
                                                              const std::string& outputFolder)
{


  auto dataSeries = dataManager->findDataSeries(dataset->dataSeriesId);

  if(!dataSeries)
  {
    QString errMsg = QObject::tr("Could not find the data series: %1").arg(dataset->dataSeriesId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

  if(!dataProvider)
  {
    QString errMsg = QObject::tr("Could not find the data provider: %1").arg(dataSeries->dataProviderId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  // Delete file if it exists.
  std::string shpFilename = outputFolder + "/data_series_" + std::to_string(dataSeries->id) + ".shp";
  QFileInfo checkFile(shpFilename.c_str());
  if(checkFile.exists())
  {
    QFile file(shpFilename.c_str());
    file.remove();
  }

  auto dataAccessor = std::dynamic_pointer_cast<terrama2::core::DataAccessorFile>(terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries));
  auto remover = std::make_shared<terrama2::core::FileRemover>();
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > dataMap = dataAccessor->getSeries(filter, remover);

  te::mem::DataSet* ds;
  te::da::DataSetType* dt = new te::da::DataSetType(dataSeries->name);
  int geomSRID = 0;

  for(auto data : dataMap)
  {
    const auto& dataSetSeries = data.second;
    auto rasterProperty = te::da::GetFirstRasterProperty(dataSetSeries.teDataSetType.get());
    if(!rasterProperty)
    {
      QString errMsg = QObject::tr("Could a valid raster property for dataset: %1").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    auto datePropertyPos = te::da::GetFirstPropertyPos(dataSetSeries.syncDataSet->dataset().get(), te::dt::DATETIME_TYPE);

    for(unsigned int row = 0; row < dataSetSeries.syncDataSet->size(); ++row)
    {
      auto raster = dataSetSeries.syncDataSet->getRaster(row, rasterProperty->getId());

      // If is the first row, configure dataSet
      if(row == 0)
      {
        geomSRID = raster->getSRID();

        te::dt::SimpleProperty* filenameProp = new te::dt::SimpleProperty("filename", te::dt::STRING_TYPE, true);

        te::gm::GeometryProperty* geomProp = new te::gm::GeometryProperty("geom", 0, te::gm::PolygonType, true);
        geomProp->setSRID(geomSRID);

        te::dt::DateTimeProperty* timestampProp = new te::dt::DateTimeProperty("timestamp", te::dt::TIME_INSTANT, true);

        dt->add(filenameProp);
        dt->add(geomProp);
        dt->add(timestampProp);

        ds = new te::mem::DataSet(dt);
      }

      auto date = dataSetSeries.syncDataSet->getDateTime(row, datePropertyPos);
      std::shared_ptr<te::dt::TimeInstantTZ> tiTz(dynamic_cast<te::dt::TimeInstantTZ*>(date->clone()));
      auto boostTiTz = tiTz->getTimeInstantTZ();

      auto geom = te::gm::GetGeomFromEnvelope(raster->getExtent(), raster->getSRID());

      te::mem::DataSetItem* dsItem01 = new te::mem::DataSetItem(ds);
      QFileInfo info(QString::fromStdString(dataSetSeries.syncDataSet->getString(row, "filename")));
      dsItem01->setString(0, info.fileName().toStdString());
      dsItem01->setGeometry(1, geom);
      dsItem01->setDateTime(2, new te::dt::TimeInstant(boostTiTz.local_time()));

      ds->add(dsItem01);
    }
  }

  std::shared_ptr<te::da::DataSource> dsOGR = te::da::DataSourceFactory::make("OGR", "file://"+shpFilename);
  dsOGR->open();

  ds->moveBeforeFirst();

  te::da::Create(dsOGR.get(), dt, ds);

  dsOGR->close();

  createGeoserverPropertiesFile(outputFolder, exhibitionName, dataSeries->id);

  return geomSRID;
}

void terrama2::services::view::core::createGeoserverPropertiesFile(const std::string& outputFolder,
                                                                   const std::string& exhibitionName,
                                                                   DataSeriesId dataSeriesId)
{
  std::string propertiesFilename = outputFolder + "/data_series_" + std::to_string(dataSeriesId) + ".properties";

  std::string content = "Levels=0.009999999776482582,0.009999999776482582\n"
                        "Heterogeneous=false\n"
                        "TimeAttribute=timestamp\n"
                        "AbsolutePath=false\n"
                        "Name=" + exhibitionName +"\n"
                        "TypeName=data_series_" + std::to_string(dataSeriesId) +"\n"
                        "Caching=false\n"
                        "ExpandToRGB=false\n"
                        "LocationAttribute=filename\n"
                        "SuggestedSPI=it.geosolutions.imageioimpl.plugins.tiff.TIFFImageReaderSpi\n"
                        "CheckAuxiliaryMetadata=false\n"
                        "LevelsNum=1";

  /* Try and open a file for output */

  QFile outputFile(propertiesFilename.c_str());
  outputFile.open(QIODevice::WriteOnly);

  /* Check it opened OK */
  if(!outputFile.isOpen())
  {
    QString errMsg = QObject::tr("Could not open file: %1").arg(propertiesFilename.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  QTextStream outStream(&outputFile);
  outStream << content.c_str();

  /* Close the file */
  outputFile.close();
}

QFileInfoList terrama2::services::view::core::dataSeriesFileList(const std::vector<terrama2::core::DataSetPtr> datasets,
                                                                 const terrama2::core::DataProviderPtr inputDataProvider,
                                                                 const terrama2::core::Filter filter,
                                                                 const std::shared_ptr<terrama2::core::FileRemover> remover,
                                                                 const std::shared_ptr<terrama2::core::DataAccessorFile> dataAccessor)
{
  QFileInfoList fileInfoList;

  for(auto& dataset : datasets)
  {
    // TODO: mask in folder
    QUrl url;

    url = QUrl(QString::fromStdString(inputDataProvider->uri));

    //get timezone of the dataset
    std::string timezone;
    try
    {
      timezone = dataAccessor->getTimeZone(dataset);
    }
    catch(const terrama2::core::UndefinedTagException& /*e*/)
    {
      //if timezone is not defined
      timezone = "UTC+00";
    }

    QFileInfoList baseUriList, foldersList;

    baseUriList.append(url.toString(QUrl::RemoveScheme));

    try
    {
      foldersList = dataAccessor->getFoldersList(baseUriList, terrama2::core::getFolderMask(dataset, nullptr));
    }
    catch(const terrama2::core::UndefinedTagException& /*e*/)
    {
      foldersList = baseUriList;
    }

    for(auto& folderURI : foldersList)
    {
      QFileInfoList tempFileInfoList = dataAccessor->getDataFileInfoList(folderURI.absoluteFilePath().toStdString(),
                                                                         dataAccessor->getMask(dataset),
                                                                         timezone,
                                                                         filter,
                                                                         remover);

      if(tempFileInfoList.empty())
      {
        TERRAMA2_LOG_WARNING() << QObject::tr("No data in folder: %1").arg(folderURI.absoluteFilePath());
        continue;
      }

      fileInfoList.append(tempFileInfoList);
    }
  }

  return fileInfoList;
}
