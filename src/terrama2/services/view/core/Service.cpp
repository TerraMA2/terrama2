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
  \file terrama2/services/view/core/Service.hpp

  \brief Class for the view configuration.

  \author Vinicius Campanha
*/

// TerraMA2
#include "Service.hpp"
#include "View.hpp"
#include "MemoryDataSetLayer.hpp"
#include "Utils.hpp"

#include "data-access/Geoserver.hpp"

#include "../../../core/Shared.hpp"
#include "../../../core/utility/TimeUtils.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../impl/DataAccessorFile.hpp"
#include "../../../impl/DataAccessorPostGIS.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"

// Qt
#include <QUrl>
#include <QJsonArray>

terrama2::services::view::core::Service::Service(std::weak_ptr<terrama2::services::view::core::DataManager> dataManager)
  : dataManager_(dataManager)
{
  connectDataManager();
}

bool terrama2::services::view::core::Service::hasDataOnQueue() noexcept
{
  return !viewQueue_.empty();
}

bool terrama2::services::view::core::Service::processNextData()
{
  // check if there is View to build
  if(viewQueue_.empty())
    return false;

  // get first data
  const auto& viewId = viewQueue_.front();

  // prepare task for View building
  prepareTask(viewId);

  // remove from queue
  viewQueue_.pop_front();

  // is there more data to process?
  return !viewQueue_.empty();
}

void terrama2::services::view::core::Service::prepareTask(ViewId viewId)
{
  try
  {
    taskQueue_.emplace(std::bind(&Service::viewJob, this, viewId, std::dynamic_pointer_cast<terrama2::services::view::core::ViewLogger>(logger_), dataManager_));
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::view::core::Service::addToQueue(ViewId viewId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);
    TERRAMA2_LOG_DEBUG() << tr("View %1 added to queue.").arg(viewId);

    auto datamanager = dataManager_.lock();
    auto view = datamanager->findView(viewId);

    const auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    auto serviceInstanceId = serviceManager.instanceId();

    // Check if this view should be executed in this instance
    if(view->serviceInstanceId != serviceInstanceId)
      return;

    if(std::find(processingQueue_.begin(), processingQueue_.end(), viewId) == processingQueue_.end())
    {
      processingQueue_.push_back(viewId);
      viewQueue_.push_back(viewId);
      mainLoopCondition_.notify_one();
    }
    else
    {
      waitQueue_[viewId].push(startTime);
    }

  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::services::view::core::Service::connectDataManager()
{
  auto dataManager = dataManager_.lock();
  connect(dataManager.get(), &terrama2::services::view::core::DataManager::viewAdded, this,
          &terrama2::services::view::core::Service::addProcessToSchedule);
  connect(dataManager.get(), &terrama2::services::view::core::DataManager::viewRemoved, this,
          &terrama2::services::view::core::Service::removeView);
  connect(dataManager.get(), &terrama2::services::view::core::DataManager::viewUpdated, this,
          &terrama2::services::view::core::Service::updateView);
}

void terrama2::services::view::core::Service::removeView(ViewId viewId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing view %1.").arg(viewId);

    auto it = timers_.find(viewId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(viewId);
      timer->disconnect();
      timers_.erase(viewId);
    }

    // remove from queue
    viewQueue_.erase(std::remove(viewQueue_.begin(), viewQueue_.end(), viewId), viewQueue_.end());


    TERRAMA2_LOG_INFO() << tr("View %1 removed successfully.").arg(viewId);
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove view: %1.").arg(viewId);
  }
  catch(boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove view: %1.").arg(viewId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove view: %1.").arg(viewId);
  }
}

void terrama2::services::view::core::Service::updateView(ViewPtr view) noexcept
{
  removeView(view->id);
  addProcessToSchedule(view);
}

void terrama2::services::view::core::Service::viewJob(ViewId viewId,
                                                      std::shared_ptr< terrama2::services::view::core::ViewLogger > logger,
                                                      std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    notifyWaitQueue(viewId);
    sendProcessFinishedSignal(viewId, false);
    return;
  }

  if(!logger.get())
  {
    QString errMsg = QObject::tr("Unable to access Logger class in view %1").arg(viewId);
    TERRAMA2_LOG_ERROR() << errMsg;

    notifyWaitQueue(viewId);
    sendProcessFinishedSignal(viewId, false);
    return;
  }

  RegisterId logId = 0;

  QJsonObject jsonAnswer;

  try
  {
    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting view %1 generation.").arg(viewId);

    logId = logger->start(viewId);

    /////////////////////////////////////////////////////////////////////////
    //  aquiring metadata

    auto lock = dataManager->getLock();

    auto viewPtr = dataManager->findView(viewId);

    std::unordered_map< terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr > dataSeriesProviders;
    for(auto dataSeriesId : viewPtr->dataSeriesList)
    {
      terrama2::core::DataSeriesPtr inputDataSeries = dataManager->findDataSeries(dataSeriesId);
      terrama2::core::DataProviderPtr inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

      dataSeriesProviders.emplace(inputDataSeries, inputDataProvider);
    }

    lock.unlock();

    /////////////////////////////////////////////////////////////////////////

    for(auto dataSeriesProvider : dataSeriesProviders)
    {
      terrama2::core::DataSeriesPtr inputDataSeries = dataSeriesProvider.first;
      terrama2::core::DataProviderPtr inputDataProvider = dataSeriesProvider.second;

      DataProviderType dataProviderType = inputDataProvider->dataProviderType;

      if(dataProviderType != "POSTGIS" && dataProviderType != "FILE")
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Data provider not supported: %1.").arg(dataProviderType.c_str());
        continue;
      }

      DataFormat dataFormat = inputDataSeries->semantics.dataFormat;

      // Check if the view can be done by the maps server
      bool mapsServerGeneration = false;

      if(!mapsServerUri_.uri().empty())
      {
        if(dataFormat != "OGR" && dataFormat != "POSTGIS" && dataFormat != "GEOTIFF")
        {
          TERRAMA2_LOG_WARNING() << QObject::tr("Data format not supported in the maps server: %1.").arg(dataFormat.c_str());
        }
        else
        {
          mapsServerGeneration = true;
        }
      }

      if(mapsServerGeneration)
      {
        GeoServer geoserver(mapsServerUri_);

        geoserver.registerWorkspace();

        std::string styleName = "";
        auto itStyle = viewPtr->stylesPerDataSeries.find(inputDataSeries->id);

        if(itStyle != viewPtr->stylesPerDataSeries.end())
        {
          styleName = viewPtr->viewName + "style" + std::to_string(inputDataSeries->id);
          geoserver.registerStyle(styleName, itStyle->second);
        }

        QFileInfoList fileInfoList;
        QJsonArray layersArray;

        terrama2::core::DataAccessorPtr dataAccessor =
            terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

        terrama2::core::Filter filter;

        auto it = viewPtr->filtersPerDataSeries.find(inputDataSeries->id);

        if(it != viewPtr->filtersPerDataSeries.end())
        {
          filter = terrama2::core::Filter(it->second);
        }

        auto remover = std::make_shared<terrama2::core::FileRemover>();

        const std::vector< terrama2::core::DataSetPtr > datasets = inputDataSeries->datasetList;

        if(!datasets.empty())
        {
          if(dataProviderType == "FILE")
          {
            terrama2::core::DataSeriesTemporality temporality = inputDataSeries->semantics.temporality;

            if(temporality == terrama2::core::DataSeriesTemporality::DYNAMIC
               && dataFormat == "GEOTIFF")
            {
              QUrl url(QString::fromStdString(inputDataProvider->uri));

              std::string layerName = "name";

              for(auto& dataset : datasets)
              {
                createGeoserverTempMosaic(dataManager, dataset, filter, layerName, url.path().toStdString());
              }

              geoserver.registerMosaicCoverage(layerName + "coveragestore", url.path().toStdString(), layerName, "4326");

              QJsonObject layer;
              layer.insert("layer", QString::fromStdString(layerName));
              layersArray.push_back(layer);
            }
            else
            {
              // Get the list of layers to register
              auto files = dataSeriesFileList(datasets,
                                              inputDataProvider,
                                              filter,
                                              remover,
                                              std::dynamic_pointer_cast<terrama2::core::DataAccessorFile>(dataAccessor));
              fileInfoList.append(files);

              for(auto& fileInfo : fileInfoList)
              {
                if(dataFormat == "OGR")
                {
                  geoserver.registerVectorFile(viewPtr->viewName + std::to_string(inputDataSeries->id) + "datastore",
                                               fileInfo.absoluteFilePath().toStdString(),
                                               fileInfo.completeSuffix().toStdString());
                }
                else if(dataFormat == "GEOTIFF")
                {
                  geoserver.registerCoverageFile(fileInfo.fileName().toStdString() ,
                                                 fileInfo.absoluteFilePath().toStdString(),
                                                 fileInfo.completeBaseName().toStdString(),
                                                 "geotiff",
                                                 styleName);
                }

                QJsonObject layer;
                layer.insert("layer", fileInfo.completeBaseName());
                layersArray.push_back(layer);
              }
            }
          }
          else if(dataProviderType == "POSTGIS")
          {
            terrama2::core::DataSeriesType dataSeriesType = inputDataSeries->semantics.dataSeriesType;

            QUrl url(inputDataProvider->uri.c_str());
            std::map<std::string, std::string> connInfo
            {
              {"PG_HOST", url.host().toStdString()},
              {"PG_PORT", std::to_string(url.port())},
              {"PG_USER", url.userName().toStdString()},
              {"PG_PASSWORD", url.password().toStdString()},
              {"PG_DB_NAME", url.path().section("/", 1, 1).toStdString()},
              {"PG_CONNECT_TIMEOUT", "4"},
              {"PG_CLIENT_ENCODING", "UTF-8"}
            };

            std::shared_ptr< terrama2::core::DataAccessorPostGIS > dataAccessorPostGis =
                std::dynamic_pointer_cast<terrama2::core::DataAccessorPostGIS>(dataAccessor);

            for(auto& dataset : datasets)
            {
              std::string tableName = dataAccessorPostGis->getDataSetTableName(dataset);
              std::string layerName = tableName;
              std::string timestampPropertyName;
              std::string joinSQL;

              try
              {
                timestampPropertyName = dataAccessorPostGis->getTimestampPropertyName(dataset);
              }
              catch (...)
              {
                /* code */
              }

              if(dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
              {
                const auto& id = dataset->format.find("monitored_object_id");
                const auto& foreing = dataset->format.find("monitored_object_pk");

                if(id == dataset->format.end() || foreing == dataset->format.end())
                {
                  logger->error("Data to join not informed.", logId);
                  TERRAMA2_LOG_ERROR() << QObject::tr("Cannot join data from a different DB source!");
                  continue;
                }

                terrama2::core::DataSeriesPtr monitoredObjectDataSeries = dataManager->findDataSeries(std::stoi(id->second));
                terrama2::core::DataProviderPtr monitoredObjectProvider = dataManager->findDataProvider(monitoredObjectDataSeries->dataProviderId);

                QUrl monitoredObjectUrl(monitoredObjectProvider->uri.c_str());

                if(monitoredObjectUrl.host() != url.host()
                   || monitoredObjectUrl.port() != url.port()
                   || monitoredObjectUrl.path().section("/", 1, 1) != url.path().section("/", 1, 1))
                {
                  logger->error("Data to join is in a different DB.", logId);
                  TERRAMA2_LOG_ERROR() << QObject::tr("Cannot join data from a different DB source!");
                  continue;
                }

                if(monitoredObjectDataSeries->datasetList.empty())
                {
                  logger->error("No join data.", logId);
                  TERRAMA2_LOG_ERROR() << QObject::tr("Cannot join data from a different DB source!");
                  continue;
                }

                const terrama2::core::DataSetPtr monitoredObjectDataset = monitoredObjectDataSeries->datasetList.at(0);

                terrama2::core::DataAccessorPtr monitoredObjectDataAccessor =
                    terrama2::core::DataAccessorFactory::getInstance().make(monitoredObjectProvider, monitoredObjectDataSeries);

                std::shared_ptr< terrama2::core::DataAccessorPostGIS > dataAccessorAnalysisPostGIS =
                    std::dynamic_pointer_cast<terrama2::core::DataAccessorPostGIS>(monitoredObjectDataAccessor);

                std::string joinTableName = dataAccessorAnalysisPostGIS->getDataSetTableName(monitoredObjectDataset);

                joinSQL = "SELECT * from " + tableName + " as t1 , " + joinTableName + " as t2 ";

                joinSQL += "WHERE t1.geom_id = t2." + foreing->second;

                // Change the layer name
                layerName = viewPtr->viewName;
              }

              geoserver.registerPostgisTable(inputDataProvider->name,
                                             connInfo,
                                             layerName,
                                             viewPtr->viewName,
                                             timestampPropertyName,
                                             joinSQL);

              QJsonObject layer;
              layer.insert("layer", QString::fromStdString(layerName));
              layersArray.push_back(layer);
            }

          }
        }
        else
        {
          logger->info("No data to register.", logId);
          TERRAMA2_LOG_WARNING() << tr("No data to register in maps server.");
        }

        // TODO: assuming that only has one dataseries, overwriting answer
        jsonAnswer.insert("class", QString("RegisteredViews"));
        jsonAnswer.insert("process_id",static_cast<int32_t>(viewPtr->id));
        jsonAnswer.insert("maps_server_uri", QString::fromStdString(geoserver.uri().uri()));
        jsonAnswer.insert("workspace", QString::fromStdString(geoserver.workspace()));
        jsonAnswer.insert("style", QString::fromStdString(styleName));
        jsonAnswer.insert("layers_list", layersArray);
      }

      if(!viewPtr->imageName.empty())
      {
        // TODO: create VIEW with TerraLib
      }
    }

    TERRAMA2_LOG_INFO() << tr("View %1 generated successfully.").arg(viewId);

    logger->done(terrama2::core::TimeUtils::nowUTC(), logId);

    sendProcessFinishedSignal(viewId, true, jsonAnswer);
    notifyWaitQueue(viewId);

    return;
  }
  catch(const terrama2::Exception& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    TERRAMA2_LOG_ERROR() << errMsg << std::endl;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->error(errMsg, logId);
  }
  catch(const boost::exception& e)
  {
    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->error(errMsg, logId);
  }
  catch(const std::exception& e)
  {
    std::string errMsg = e.what();
    TERRAMA2_LOG_ERROR() << errMsg;
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->error(errMsg, logId);
  }
  catch(...)
  {
    std::string errMsg = "Unkown error.";
    TERRAMA2_LOG_ERROR() << QObject::tr("Unkown error.");
    TERRAMA2_LOG_INFO() << QObject::tr("Build of view %1 finished with error(s).").arg(viewId);

    if(logId != 0)
      logger->error(errMsg, logId);
  }

  sendProcessFinishedSignal(viewId, false);
  notifyWaitQueue(viewId);
}


QFileInfoList terrama2::services::view::core::Service::dataSeriesFileList(const std::vector<terrama2::core::DataSetPtr> datasets,
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
      foldersList = dataAccessor->getFoldersList(baseUriList, dataAccessor->getFolderMask(dataset));
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
        TERRAMA2_LOG_WARNING() << tr("No data in folder: %1").arg(folderURI.absoluteFilePath());
        continue;
      }

      fileInfoList.append(tempFileInfoList);
    }
  }

  return fileInfoList;
}


void terrama2::services::view::core::Service::notifyWaitQueue(ViewId viewId)
{
  // Remove from processing queue
  auto pqIt = std::find(processingQueue_.begin(), processingQueue_.end(), viewId);
  if(pqIt != processingQueue_.end())
    processingQueue_.erase(pqIt);


  // Verify if the there is an process waiting for the same view
  if(!waitQueue_[viewId].empty())
  {
    waitQueue_[viewId].pop();

    // Adds to the processing queue
    processingQueue_.push_back(viewId);
    viewQueue_.push_back(viewId);

    //wake loop thread
    mainLoopCondition_.notify_one();
  }

}

void terrama2::services::view::core::Service::updateAdditionalInfo(const QJsonObject& obj) noexcept
{
  if(!obj.contains("maps_server_uri"))
  {
    TERRAMA2_LOG_ERROR() << tr("Missing the Maps Server URI in service additional info!");
  }
  else
  {
    mapsServerUri_ = te::core::URI(obj["maps_server_uri"].toString().toStdString());
  }
}
