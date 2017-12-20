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
  \file terrama2/services/alert/core/AlertExecutor.hpp

  \brief

  \author Jano Simas
          Vinicius Campanha
*/

// TerraMA2
#include "../../../core/Shared.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/TeDataSetFKJoin.hpp"
#include "../../../core/utility/CurlWrapperHttp.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-model/Risk.hpp"
#include "utility/NotifierFactory.hpp"
#include "utility/DocumentFactory.hpp"
#include "AlertExecutor.hpp"
#include "Alert.hpp"
#include "Report.hpp"
#include "Notifier.hpp"
#include "Utils.hpp"
#include "Exception.hpp"


// Terralib
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/ForeignKey.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/Band.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/RasterFactory.h>

#include <terralib/rp/Functions.h>

// Qt
#include <QObject>

// STL
#include <limits>

#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm/binary_search.hpp>

terrama2::services::alert::core::AlertExecutor::AlertExecutor()
{
  qRegisterMetaType<uint32_t>("size_t");
  qRegisterMetaType<std::shared_ptr<te::dt::TimeInstantTZ>>("std::shared_ptr<te::dt::TimeInstantTZ>");
}

std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, terrama2::services::alert::core::comparatorAbstractData>
terrama2::services::alert::core::AlertExecutor::getResultMap(terrama2::core::LegendPtr risk,
                                                             size_t pos,
                                                             te::dt::Property* idProperty,
                                                             std::string datetimeColumnName,
                                                             std::shared_ptr<te::da::DataSet> teDataset,
                                                             std::vector<std::shared_ptr<te::dt::DateTime> > vecDates)
{
  std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, comparatorAbstractData> riskResultMap;
  teDataset->moveBeforeFirst();
  // Get the risk for data
  while(teDataset->moveNext())
  {
    std::shared_ptr<te::dt::DateTime> executionDate = teDataset->getDateTime(datetimeColumnName);

    auto it = std::find_if(vecDates.begin(), vecDates.end(),
                           [&executionDate](std::shared_ptr<te::dt::DateTime> const& current)
    {
              return *current == *executionDate;
  });

    // Only process the risk of data in stored dates
    if(it == vecDates.end())
      continue;

    std::shared_ptr<te::dt::AbstractData> identifierValue = teDataset->getValue(idProperty->getName());

    // process risk level
    uint32_t riskLevel = 0;
    std::string riskName;
    std::string attributeValue;

    // create a getRisk function
    auto getRisk = createGetRiskFunction(risk, teDataset);
    std::tie(riskLevel, riskName, attributeValue) = getRisk(pos);

    auto& resultMap = riskResultMap[identifierValue];
    std::shared_ptr<te::dt::AbstractData> attrValue = teDataset->getValue(pos);
    auto pair = std::make_pair(attrValue, riskLevel);
    assert(attrValue.get());
    resultMap[executionDate->toString()] = pair;
  }

  return riskResultMap;
}

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::AlertExecutor::populateMonitoredObjectAlertDataset( std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                                                       std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, comparatorAbstractData> riskResultMap,
                                                                                                                       AlertPtr alertPtr,
                                                                                                                       te::dt::Property* fkProperty,
                                                                                                                       std::shared_ptr<te::da::DataSetType> alertDataSetType)
{
  std::shared_ptr<te::mem::DataSet> alertDataSet = std::make_shared<te::mem::DataSet>(alertDataSetType.get());

  alertDataSet->moveBeforeFirst();
  for(auto& item : riskResultMap)
  {
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(alertDataSet.get());
    auto value = item.first;
    const auto& resultMap = item.second;

    auto currentDate = *vecDates.rbegin();

    std::string currentRiskProperty = dateTimeToString(currentDate);

    dsItem->setValue(fkProperty->getName(), value->clone());

    auto attrValue = resultMap.at(currentDate->toString()).first;
    dsItem->setValue(alertPtr->riskAttribute, attrValue->clone());

    auto currentRisk = resultMap.at(currentDate->toString()).second;
    dsItem->setInt32(currentRiskProperty, static_cast<int>(currentRisk));

    if(vecDates.size() > 1)
    {
      auto previousDate = *(vecDates.rbegin()+1);

      auto pastRisk = resultMap.at(previousDate->toString()).second;

      int comparisonResult = 0;
      if(currentRisk < pastRisk)
        comparisonResult = -1;
      else if(currentRisk > pastRisk)
        comparisonResult = 1;

      std::string pastRiskProperty = dateTimeToString(previousDate);

      dsItem->setInt32(pastRiskProperty, static_cast<int>(pastRisk));
      dsItem->setInt32(COMPARISON_PROPERTY_NAME, comparisonResult);

      if(vecDates.size() > 2)
      {
        for(auto itDate = vecDates.rbegin()+2; itDate != vecDates.rend(); ++itDate)
        {
          std::string property = dateTimeToString(*itDate);
          auto risk = resultMap.at((*itDate)->toString()).second;
          dsItem->setInt32(property, static_cast<int>(risk));
        }
      }
    }

    alertDataSet->add(dsItem);
  }

  return alertDataSet;
}

void terrama2::services::alert::core::AlertExecutor::addAdditionalData(std::shared_ptr<te::mem::DataSet> alertDataSet, const std::vector<AdditionalData>& additionalDataVector, std::unordered_map<std::string, terrama2::core::TeDataSetFKJoin> additionalDataMap)
{
  // list of additional properties
  std::set<std::string> propertyNames;

  //iterate over the alert dataset
  for(size_t i = 0; i < alertDataSet->size(); i++)
  {
    alertDataSet->move(i);

    //iterate over all additional dataset to fill each item
    for(const auto& additionalData : additionalDataVector)
    {
      std::string key = std::to_string(additionalData.dataSeriesId)+"_"+std::to_string(additionalData.dataSetId);
      auto join = additionalDataMap.at(key);
      join.referrerDataSet()->move(i);

      //over each addtional dataSet get each attribute value
      for(const std::string& propertyName : additionalData.attributes)
      {
        auto dataSetType = join.referredDataSetType();
        auto referredProperty = join.getProperty(propertyName);

        std::string newPropertyName = dataSetType->getName()+"_"+propertyName;
        try
        {
          newPropertyName = additionalData.alias.at(propertyName);
        }
        catch(...)
        {
          //no alias for the property
        }

        if(propertyNames.find(newPropertyName) == propertyNames.end() )
        {
          // create the property in the alert dataset
          alertDataSet->add(newPropertyName, referredProperty->getType());
          propertyNames.insert(newPropertyName);
        }

        //set property value
        alertDataSet->setValue(newPropertyName, join.getValue(propertyName).release());
      }
    }
  }
}

std::shared_ptr<te::mem::DataSet>
terrama2::services::alert::core::AlertExecutor::monitoredObjectAlert(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                     std::string datetimeColumnName,
                                                                     std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                     AlertPtr alertPtr,
                                                                     terrama2::core::LegendPtr legend,
                                                                     terrama2::core::Filter filter,
                                                                     terrama2::core::DataSetPtr dataset,
                                                                     std::shared_ptr<te::da::DataSet> teDataset,
                                                                     te::dt::Property* idProperty,
                                                                     const std::vector<AdditionalData>& additionalDataVector,
                                                                     std::unordered_map<DataSeriesId, std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr> > tempAdditionalDataVector,
                                                                     std::shared_ptr<terrama2::core::FileRemover> remover)
{
  //Creat a Join class based on the ForeignKey of the dataset
  std::unordered_map<std::string, terrama2::core::TeDataSetFKJoin> additionalDataMap;
  for(const auto& additionalData : additionalDataVector)
  {
    auto pair = tempAdditionalDataVector.at(additionalData.dataSeriesId);
    auto dataSeries = pair.first;
    auto dataProvider = pair.second;

    // referred data to join to the alert data
    terrama2::core::DataSetSeries referredDataSeries;
    if(dataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::DCP)
    {
      // use the dcp positions table
      // as aditional data
      te::core::URI uri(dataProvider->uri);
      std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("POSTGIS", uri));
      std::string destinationDataSetName = getDCPPositionsTableName(dataSeries);

      auto positionsData = terrama2::core::getDCPPositionsTable(datasourceDestination, destinationDataSetName);

      referredDataSeries.dataSet = nullptr;
      referredDataSeries.syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(positionsData.second);
      referredDataSeries.teDataSetType = positionsData.first;
    }
    else
    {
      auto dataSeries = pair.first;
      auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(pair.second, dataSeries);
      auto dataMap = dataAccessor->getSeries(filter, remover);

      decltype(dataMap.begin()) iter;
      if(additionalData.dataSetId == terrama2::core::InvalidId())
      {
        //if no dataset configured
        iter = dataMap.begin();
      }
      else
      {
        //find selected dataset
        iter = std::find_if(dataMap.begin(), dataMap.end(), [&additionalData](std::pair<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries> pair)
        {
          return pair.first->id == additionalData.dataSetId;
        });
      }

      referredDataSeries = dataMap.at(iter->first);
    }

    terrama2::core::TeDataSetFKJoin join(dataSetType,
                                         teDataset,
                                         additionalData.referrerAttribute,
                                         referredDataSeries.teDataSetType,
                                         referredDataSeries.syncDataSet->dataset(),
                                         additionalData.referredAttribute);

    std::string key = std::to_string(additionalData.dataSeriesId)+"_"+std::to_string(additionalData.dataSetId);
    additionalDataMap.emplace(key, join);
  }

  auto alertDataSetType = createAlertDataSetType(alertPtr, dataset);

  auto fkProperty = idProperty->clone();
  fkProperty->setName(idProperty->getName()+"_fk");
  alertDataSetType->add(fkProperty);

  auto riskAttributeProp = dataSetType->getProperty(alertPtr->riskAttribute)->clone();
  alertDataSetType->add(riskAttributeProp);

  auto pos = dataSetType->getPropertyPosition(alertPtr->riskAttribute);
  if(pos == std::numeric_limits<decltype(pos)>::max())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Risk attribute %1 doesn't exist in dataset %2").arg(QString::fromStdString(alertPtr->riskAttribute)).arg(dataset->id);
    return nullptr;
  }

  // Remove uneccessary oldest dates
  auto lastValues = *filter.lastValues;

  if(vecDates.size() > lastValues)
    vecDates = {vecDates.rbegin(), vecDates.rbegin()+lastValues};

  // Insert the risk properties
  for(size_t i = 0; i < vecDates.size(); i++)
  {
    const std::string riskLevelProperty = dateTimeToString(vecDates.at(i));

    te::dt::SimpleProperty* riskLevelProp = new te::dt::SimpleProperty(riskLevelProperty, te::dt::INT32_TYPE);
    alertDataSetType->add(riskLevelProp);
  }

  if(vecDates.size() > 1)
  {
    te::dt::SimpleProperty* comparisonPreviousProp = new te::dt::SimpleProperty(COMPARISON_PROPERTY_NAME, te::dt::INT32_TYPE);
    alertDataSetType->add(comparisonPreviousProp);
  }

  auto riskResultMap = getResultMap(legend, pos, idProperty, datetimeColumnName, teDataset, vecDates);
  std::shared_ptr<te::mem::DataSet> alertDataSet = populateMonitoredObjectAlertDataset(vecDates, riskResultMap, alertPtr, fkProperty, alertDataSetType);
  addAdditionalData(alertDataSet, additionalDataVector, additionalDataMap);

  // remove fk column
  size_t fkPos = terrama2::core::propertyPosition(alertDataSet.get(), fkProperty->getName());

  if( fkPos != std::numeric_limits<size_t>::max())
  {
    alertDataSet->drop(fkPos);
  }

  return alertDataSet;
}

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::AlertExecutor::gridAlert(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                                            std::string datetimeColumnName,
                                                                                            std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                            AlertPtr alertPtr,
                                                                                            terrama2::core::LegendPtr legend,
                                                                                            terrama2::core::Filter filter,
                                                                                            terrama2::core::DataSetPtr dataset,
                                                                                            std::shared_ptr<te::da::DataSet> teDataset)
{
  // Remove uneccessary oldest dates
  auto lastValues = *filter.lastValues;

  if(vecDates.size() > lastValues)
    vecDates = {vecDates.rbegin(), vecDates.rbegin()+lastValues};

  auto alertDataSet = populateGridAlertDataset(dataset, alertPtr, legend, vecDates, teDataset, dataSetType, datetimeColumnName);

  return alertDataSet;
}

struct isLesserDate
{
    template <class T, class U>
    bool operator()(const T& a, const U& b){ return *a < *b;}
};

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::AlertExecutor::populateGridAlertDataset(terrama2::core::DataSetPtr dataset,
                                                                                                           AlertPtr alertPtr,
                                                                                                           terrama2::core::LegendPtr legend,
                                                                                                           std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                                           std::shared_ptr<te::da::DataSet> teDataset,
                                                                                                           std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                                                           std::string datetimeColumnName)
{
  // Find raster property position
  std::size_t pos = te::da::GetFirstPropertyPos(teDataset.get(), te::dt::RASTER_TYPE);
  if(pos == std::numeric_limits<std::size_t>::max())
  {
    throw;//TODO: throw here
  }

  // Create an alert te::dataset
  auto alertDataSetType = createAlertDataSetType(alertPtr, dataset);
  // create a raster property in the output alert dataset
  auto rasterProp = dataSetType->getProperty(pos)->clone();
  alertDataSetType->add(rasterProp);
  auto rasterPropName = rasterProp->getName();
  //Create a Timestamp property
  auto timestampProp = dataSetType->getProperty(datetimeColumnName)->clone();
  alertDataSetType->add(timestampProp);

  auto alertDataSet = std::make_shared<te::mem::DataSet>(alertDataSetType.get());
  //get band used for risk
  int riskBand = std::numeric_limits<int>::max();
  try
  {
    riskBand = std::stoi(alertPtr->riskAttribute);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Risk band %1 doesn't exist in dataset %2").arg(QString::fromStdString(alertPtr->riskAttribute)).arg(dataset->id);
    return nullptr;
  }

  // iterate over all raster of the input dataset
  teDataset->moveBeforeFirst();
  while(teDataset->moveNext())
  {
    auto timestamp = teDataset->getDateTime(datetimeColumnName);

    //check if the raster date is in vecDates (current alert dates)
    auto validDate = boost::range::binary_search(vecDates, timestamp, isLesserDate());
    if(!validDate)
      continue;

    //get raster
    auto raster = teDataset->getRaster(pos);

    // create new raster
    auto bandProp = new te::rst::BandProperty(0, te::dt::UINT32_TYPE);
    bandProp->m_blkh = 1;
    bandProp->m_blkw = raster->getNumberOfColumns();
    bandProp->m_nblocksx = 1;
    bandProp->m_nblocksy = raster->getNumberOfRows();
    bandProp->m_noDataValue = std::numeric_limits<uint32_t>::max();

    std::vector<te::rst::BandProperty*> bands;
    bands.push_back(bandProp);

    auto grid = new te::rst::Grid(raster->getNumberOfColumns(), raster->getNumberOfRows(), new te::gm::Envelope(*raster->getExtent()), raster->getSRID());
    std::unique_ptr<te::rst::Raster> alertRaster(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));

    auto band = raster->getBand(riskBand);
    auto alertBand = alertRaster->getBand(0);

    for(size_t r = 0; r < grid->getNumberOfRows(); ++r)
    {
      for(size_t c = 0; c < grid->getNumberOfColumns(); ++c)
      {
        auto noData = band->getProperty()->m_noDataValue;
        double value;
        band->getValue(c, r, value);

        if(value == noData)
        {
          alertBand->setValue(c, r, std::numeric_limits<uint32_t>::max());
        }
        else
        {
          auto riskTuple = legend->riskLevel(value);
          int level = std::get<0>(riskTuple);

          alertBand->setValue(c, r, level);
        }
      }
    }

    std::unique_ptr<te::mem::DataSetItem> item(new te::mem::DataSetItem(alertDataSet.get()));
    item->setDateTime(datetimeColumnName, static_cast<te::dt::DateTime*>(timestamp->clone()));
    item->setRaster(rasterPropName, alertRaster.release());
    alertDataSet->add(item.release());
  }

  return alertDataSet;
}

void terrama2::services::alert::core::AlertExecutor::runAlert(terrama2::core::ExecutionPackage executionPackage,
                                                              std::shared_ptr< AlertLogger > logger,
                                                              std::weak_ptr<DataManager> weakDataManager,
                                                              const std::map<std::string, std::string>& serverMap)
{
  auto alertId = executionPackage.processId;

  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    emit alertFinished(alertId, executionPackage.executionDate, false);
    return;
  }


  //////////////////////////////////////////////////////////
  //  aquiring metadata
  auto lock = dataManager->getLock();

  AlertPtr alertPtr = dataManager->findAlert(alertId);
  terrama2::core::LegendPtr legend = dataManager->findLegend(alertPtr->riskId);
  if(!legend.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to find legend with id: %1").arg(alertPtr->riskId);
    emit alertFinished(alertId, executionPackage.executionDate, false);
    return;
  }

  // input data
  auto inputDataSeries = dataManager->findDataSeries(alertPtr->dataSeriesId);
  auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

  //retrieve additional data
  std::vector<AdditionalData> additionalDataVector = alertPtr->additionalDataVector;
  std::unordered_map<DataSeriesId, std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr> > tempAdditionalDataVector;
  for(auto additionalData : additionalDataVector)
  {
    auto dataSeries = dataManager->findDataSeries(additionalData.dataSeriesId);
    auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

    tempAdditionalDataVector.emplace(additionalData.dataSeriesId, std::make_pair(dataSeries, dataProvider));
  }

  // dataManager no longer in use
  lock.unlock();

  try
  {
    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting alert generation");

    /////////////////////////////////////////////////////////////////////////
    // analysing data

    auto processingStartTime = terrama2::core::TimeUtils::nowUTC();

    auto filter = alertPtr->filter;

    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataMap = dataAccessor->getSeries(filter, remover);
    if(dataMap.empty())
    {
      logger->result(AlertLogger::Status::DONE, nullptr, executionPackage.registerId);
      logger->log(AlertLogger::MessageType::WARNING_MESSAGE, QObject::tr("No data to available.").toStdString(), executionPackage.registerId);
      TERRAMA2_LOG_WARNING() << QObject::tr("No data to available.");
      emit alertFinished(alertId, executionPackage.executionDate, false);
      return;
    }

    // Flag to check if at least one alert was generated
    bool alertGenerated = false;
    // flag to notify the wemmonitor
    bool notify = false;

    for(const auto& data : dataMap)
    {
      auto dataset = data.first;
      auto dataSeries = data.second;

      auto teDataset = dataSeries.syncDataSet->dataset();
      auto dataSetType = dataSeries.teDataSetType;

      // Get the datetime column name in teDataSet for filters
      std::string datetimeColumnName = "";

      try
      {
        datetimeColumnName = dataAccessor->getTimestampPropertyName(dataset, false);
      }
      catch(const terrama2::core::UndefinedTagException& /*e*/)
      {
        // do nothing
      }

      if(datetimeColumnName.empty())
      {
        auto property = dataSetType->findFirstPropertyOfType(te::dt::DATETIME_TYPE);

        if(property)
        {
          datetimeColumnName = property->getName();
        }
      }

      if(datetimeColumnName.empty())
      {
        QString errMsg = QObject::tr("Unable to identify timestamp column.");
        logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return;
      }

      // Store execution dates of dataset, ASC order
      std::vector<std::shared_ptr<te::dt::DateTime> > vecDates = terrama2::core::getAllDates(teDataset.get(),
                                                                                             datetimeColumnName);

      std::shared_ptr<te::mem::DataSet> alertDataSet;
      if(inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
      {
        if(dataSetType->getNumberOfForeignKeys() != 1)
        {
          QString errMsg = QObject::tr("Invalid number of identifier attribute.");
          logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return;
        }

        auto tempProperties = dataSetType->getForeignKey(0)->getProperties();
        auto idProperty = tempProperties.front();
        if(!idProperty)
        {
          QString errMsg = QObject::tr("Invalid identifier attribute.");
          logger->result(AlertLogger::Status::ERROR, nullptr, executionPackage.registerId);
          logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return;
        }

        ////////////////////////////////////
        // Include identifier attribute to alert aditional data
        {
          AdditionalData indentifierData;
          try
          {
            indentifierData.dataSeriesId = static_cast<DataSeriesId>(std::stoi(dataset->format.at("monitored_object_id")));
            auto attribute = dataset->format.at("monitored_object_pk");
            indentifierData.attributes.push_back(attribute);
            indentifierData.alias.emplace(attribute, attribute);
          }
          catch(const std::out_of_range&)
          {
            QString errMsg("Internal error.\nIncomplete monitored object information.");
            TERRAMA2_LOG_ERROR() << errMsg;
            throw AdditionalDataException() << ErrorDescription(errMsg);
          }

          indentifierData.referrerAttribute = idProperty->getName();
          indentifierData.referredAttribute = idProperty->getName();

          additionalDataVector.push_back(indentifierData);

          auto dataSeries = dataManager->findDataSeries(indentifierData.dataSeriesId);
          auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

          tempAdditionalDataVector.emplace(indentifierData.dataSeriesId, std::make_pair(dataSeries, dataProvider));
        }
        ////////////////////////////////////

        alertDataSet = monitoredObjectAlert(dataSetType,
                                            datetimeColumnName,
                                            vecDates,
                                            alertPtr,
                                            legend,
                                            filter,
                                            dataset,
                                            teDataset,
                                            idProperty,
                                            additionalDataVector,
                                            tempAdditionalDataVector,
                                            remover);
      }
      else if (inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
      {
        alertDataSet = gridAlert( dataSetType,
                                  datetimeColumnName,
                                  vecDates,
                                  alertPtr,
                                  legend,
                                  filter,
                                  dataset,
                                  teDataset);
      }

      if(alertDataSet->isEmpty())
      {
        QString errMsg = QObject::tr("No alert data for %1 data series.").arg(dataset->dataSeriesId);
        logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return;
      }

      te::core::URI imageUri;
      if(alertPtr->view)
      {
        try
        {
          imageUri = generateImage(alertPtr, remover);
        }
        catch (const ImageGenerationException&)
        {
          /* code */
        }
      }

      ReportPtr reportPtr = std::make_shared<Report>(alertPtr, legend, inputDataSeries, alertDataSet, vecDates);
      reportPtr->includeImage(imageUri);
      for(const auto& notification : alertPtr->notifications)
      {
        //check if should emit a notification
        if((notification.notifyOnRiskLevel <= reportPtr->maxRisk())
            || (notification.notifyOnChange && reportPtr->riskChanged()))
        {
          notify = true;
          sendNotification(serverMap, reportPtr, notification, executionPackage, logger);
        }
      }



      alertGenerated = true;
    }

    // check if at least one alert was generated
    if(!alertGenerated)
    {
      QString errMsg = QObject::tr("No alert was generated.");
      logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      logger->result(AlertLogger::Status::ERROR, nullptr, executionPackage.registerId);
      TERRAMA2_LOG_WARNING() << errMsg.toStdString();
      emit alertFinished(alertId, executionPackage.executionDate, false);
      return;
    }

    auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

    logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);
    logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

    logger->result(AlertLogger::Status::DONE, executionPackage.executionDate, executionPackage.registerId);

    TERRAMA2_LOG_INFO() << QObject::tr("Alert '%1' generated successfully").arg(alertPtr->name.c_str());

    QJsonObject obj;
    obj.insert("notify", notify);

    emit alertFinished(alertId, executionPackage.executionDate, true, obj);
  }
  catch(const terrama2::Exception& e)
  {
    logger->result(AlertLogger::Status::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString(), executionPackage.registerId);
    TERRAMA2_LOG_DEBUG() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();

    emit alertFinished(alertId, executionPackage.executionDate, false);
  }
  catch(const boost::exception& e)
  {
    logger->result(AlertLogger::Status::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::MessageType::ERROR_MESSAGE, boost::diagnostic_information(e), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);

    emit alertFinished(alertId, executionPackage.executionDate, false);
  }
  catch(const std::exception& e)
  {
    QString errMsg(e.what());
    logger->result(AlertLogger::Status::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::MessageType::ERROR_MESSAGE, e.what(), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << errMsg;

    emit alertFinished(alertId, executionPackage.executionDate, false);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Unknown exception");
    logger->result(AlertLogger::Status::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << errMsg;

    emit alertFinished(alertId, executionPackage.executionDate, false);
  }
}

te::core::URI terrama2::services::alert::core::AlertExecutor::generateImage(AlertPtr alertPtr,
                                                                            std::shared_ptr<terrama2::core::FileRemover> remover)
{
  const auto& view = *alertPtr->view;
  if(view.views.empty())
  {
    QString errMsg("Empty list of layers.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ImageGenerationException() << ErrorDescription(errMsg);
  }

  //build request
  auto geoserverUri = view.geoserverUri;
  geoserverUri+="/wms?service=WMS&version=1.1.0&request=GetMap&format=image%2Fvnd.jpeg-png";

  // Configure image size
  geoserverUri+= "&width="+std::to_string(view.width)
                 +"&height="+std::to_string(view.height);

  const auto& lowerLeftCorner = view.lowerLeftCorner;
  const auto& topRightCorner = view.topRightCorner;
  // Configure bounding box
  geoserverUri+= "&bbox="+std::to_string(lowerLeftCorner->getX())
                 +","+std::to_string(lowerLeftCorner->getY())
                 +","+std::to_string(topRightCorner->getX())
                 +","+std::to_string(topRightCorner->getY());

  //add layers to request
  geoserverUri+="&layers=";
  for(auto layer = view.views.crbegin(); layer < view.views.crend(); ++layer)
  {
    geoserverUri+=layer->second+":view"+std::to_string(layer->first)+",";
  }
  // remove last comma
  geoserverUri.pop_back();

  //add projection system
  geoserverUri+="&srs=EPSG:"+std::to_string(view.srid);

  //add time restriction
  geoserverUri+="&TIME="+terrama2::core::TimeUtils::getISOString(alertPtr->filter.discardAfter);

  // generate temporary image name
  auto imageName = terrama2::core::simplifyString(alertPtr->name)+".jpg";

  // create temporary folder
  boost::filesystem::path tempDir = boost::filesystem::temp_directory_path();
  boost::filesystem::path tempTerrama(tempDir.string()+"/terrama2");
  boost::filesystem::path downloadBaseDir = boost::filesystem::unique_path(tempTerrama.string()+"/%%%%-%%%%-%%%%-%%%%");
  // Create the directory where you will download the files.
  QDir dir(QString::fromStdString(downloadBaseDir.string()));
  if(!dir.exists())
    dir.mkpath(QString::fromStdString(downloadBaseDir.string()));

  auto imagePath = downloadBaseDir.string()+"/"+imageName;
  remover->addTemporaryFile(imagePath);

// download image
  try
  {
    terrama2::core::CurlWrapperHttp curl;
    curl.downloadFile(geoserverUri, imagePath);

    if(curl.responseCode() != 200)
    {
      QString errMsg("Error retrieving image.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ImageGenerationException() << ErrorDescription(errMsg);
    }
  } catch (const te::core::Exception&) {
    return te::core::URI();
  }

  return te::core::URI("file://"+imagePath);
}

te::core::URI terrama2::services::alert::core::AlertExecutor::makeDocument(ReportPtr reportPtr, const Notification& notification, const terrama2::core::ExecutionPackage& executionPackage, std::shared_ptr< AlertLogger > logger) const
{
  try
  {
    return DocumentFactory::getInstance().makeDocument(notification.includeReport, reportPtr);
  }
  catch(const NotifierException& e)
  {
    QString errMsg("Error: ");
    const auto msg = boost::get_error_info<terrama2::ErrorDescription>(e);
    if (msg != nullptr)
    {
      errMsg.append(msg);
    }

    logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << errMsg;
  }

  return te::core::URI();
}

void terrama2::services::alert::core::AlertExecutor::sendNotification(const std::map<std::string, std::string>& serverMap,
                                                                      ReportPtr reportPtr,
                                                                      const Notification& notification,
                                                                      terrama2::core::ExecutionPackage executionPackage,
                                                                      std::shared_ptr< AlertLogger > logger) const
{
  te::core::URI documentURI = makeDocument(reportPtr, notification, executionPackage, logger);
  try
  {
    if(serverMap.empty())
    {
      QString errMsg("No message server configured");
      logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
      TERRAMA2_LOG_ERROR() << errMsg;
    }
    else
    {
      NotifierPtr notifierPtr = NotifierFactory::getInstance().make("EMAIL", serverMap, reportPtr);
      notifierPtr->send(notification,
                        documentURI);
    }
  }
  catch(const NotifierException& e)
  {
    QString errMsg("Error: ");
    const auto msg = boost::get_error_info<terrama2::ErrorDescription>(e);
    if (msg != nullptr)
    {
      errMsg.append(msg);
    }

    logger->log(AlertLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << errMsg;
  }
}

std::function<std::tuple<int, std::string, std::string>(size_t pos)>
terrama2::services::alert::core::AlertExecutor::createGetRiskFunction(terrama2::core::LegendPtr legend, std::shared_ptr<te::da::DataSet> teDataSet)
{
  return [legend, teDataSet](size_t pos)
  {
    const auto& value = teDataSet->getDouble(pos);
    auto level = legend->riskLevel(value);
    return std::make_tuple(std::get<0>(level), std::get<1>(level), teDataSet->getValue(pos)->toString());
  };
}

std::string terrama2::services::alert::core::AlertExecutor::getIdentifierPropertyName(terrama2::core::DataSetPtr dataSet, terrama2::core::DataSeriesPtr dataSeries)
{
  return getProperty(dataSet, dataSeries, "identifier");
}

std::shared_ptr<te::da::DataSetType> terrama2::services::alert::core::AlertExecutor::createAlertDataSetType(AlertPtr alertPtr, terrama2::core::DataSetPtr dataset)
{
  const std::string dataSetAlertName = "alert_"+std::to_string(alertPtr->id)+"_"+std::to_string(dataset->id);
  return std::make_shared<te::da::DataSetType>(dataSetAlertName);
}
