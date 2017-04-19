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
  \file terrama2/services/alert/core/RunAlert.hpp

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
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-model/Risk.hpp"
#include "RunAlert.hpp"
#include "Alert.hpp"
#include "Report.hpp"
#include "Notifier.hpp"
#include "Utils.hpp"
#include "utility/NotifierFactory.hpp"
#include "utility/DocumentFactory.hpp"


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

std::vector<std::shared_ptr<te::dt::DateTime> > terrama2::services::alert::core::getDates(std::shared_ptr<te::da::DataSet> teDataset, std::string datetimeColumnName)
{
  std::vector<std::shared_ptr<te::dt::DateTime> > vecDates;

  teDataset->moveBeforeFirst();
  while(teDataset->moveNext())
  {
    // Retrieve all execution dates of dataset
    std::shared_ptr<te::dt::DateTime> executionDate = teDataset->getDateTime(datetimeColumnName);

    auto it = std::lower_bound(vecDates.begin(), vecDates.end(), executionDate,
                                      [&](std::shared_ptr<te::dt::DateTime> const& first, std::shared_ptr<te::dt::DateTime> const& second)
                                        {
                                           return *first < *second;
                                        });

    if (it != vecDates.end() && **it == *executionDate)
      continue;

    vecDates.insert(it, executionDate);
  }

  return vecDates;
}

std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, terrama2::services::alert::core::comparatorAbstractData>
terrama2::services::alert::core::getResultMap(AlertPtr alertPtr,
                                              size_t pos,
                                              te::dt::Property* idProperty,
                                              std::string datetimeColumnName,
                                              std::shared_ptr<te::da::DataSet> teDataset,
                                              std::vector<std::shared_ptr<te::dt::DateTime> > vecDates)
{
  terrama2::core::Risk risk = alertPtr->risk;
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
    auto getRisk = terrama2::services::alert::core::createGetRiskFunction(risk, teDataset);
    std::tie(riskLevel, riskName, attributeValue) = getRisk(pos);

    auto& resultMap = riskResultMap[identifierValue];
    std::shared_ptr<te::dt::AbstractData> attrValue = teDataset->getValue(pos);
    auto pair = std::make_pair(attrValue, riskLevel);
    assert(attrValue.get());
    resultMap[executionDate->toString()] = pair;
  }

  return riskResultMap;
}

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::populateMonitoredObjectAlertDataset( std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                                        std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, comparatorAbstractData> riskResultMap,
                                                                                                        const std::string comparisonPreviosProperty,
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

    std::string currentRiskProperty = validPropertyDateName(currentDate);

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

      std::string pastRiskProperty = validPropertyDateName(previousDate);

      dsItem->setInt32(pastRiskProperty, static_cast<int>(pastRisk));
      dsItem->setInt32(comparisonPreviosProperty, comparisonResult);
    }

    for(auto itDate = vecDates.rbegin()+2; itDate != vecDates.rend(); ++itDate)
    {
      std::string property = validPropertyDateName(*itDate);
      auto risk = resultMap.at((*itDate)->toString()).second;
      dsItem->setInt32(property, static_cast<int>(risk));
    }

    alertDataSet->add(dsItem);
  }

  return alertDataSet;
}

void terrama2::services::alert::core::addAdditionalData(std::shared_ptr<te::mem::DataSet> alertDataSet, AlertPtr alertPtr, std::unordered_map<std::string, terrama2::core::TeDataSetFKJoin> additionalDataMap)
{
  // list of additional properties
  std::set<std::string> propertyNames;

  //iterate over the alert dataset
  for(size_t i = 0; i < alertDataSet->size(); i++)
  {
    alertDataSet->move(i);

    //iterate over all additional dataset to fill each item
    for(auto additionalData : alertPtr->additionalDataVector)
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

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::monitoredObjectAlert(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                                                 std::string datetimeColumnName,
                                                                                                 std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                                 AlertPtr alertPtr,
                                                                                                 terrama2::core::Filter filter,
                                                                                                 terrama2::core::DataSetPtr dataset,
                                                                                                 std::shared_ptr<te::da::DataSet> teDataset,
                                                                                                 te::dt::Property* idProperty,
                                                                                                 std::unordered_map<DataSeriesId, std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr> > tempAdditionalDataVector,
                                                                                                 std::shared_ptr<terrama2::core::FileRemover> remover)
{
  //Creat a Join class based on the ForeignKey of the dataset
  std::unordered_map<std::string, terrama2::core::TeDataSetFKJoin> additionalDataMap;
  for(auto additionalData : alertPtr->additionalDataVector)
  {
    auto pair = tempAdditionalDataVector.at(additionalData.dataSeriesId);
    auto dataSeries = pair.first;
    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(pair.second, dataSeries);
    auto dataMap = dataAccessor->getSeries(filter, remover);

    auto iter = std::find_if(dataMap.begin(), dataMap.end(), [&additionalData](std::pair<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries> pair)
                                                                              {
                                                                                return pair.first->id == additionalData.dataSetId;
                                                                              });
    auto referredDataSeries = dataMap.at(iter->first);

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

  const std::string comparisonPreviosProperty = "comparison_previous";
  te::dt::SimpleProperty* comparisonPreviousProp = new te::dt::SimpleProperty(comparisonPreviosProperty, te::dt::INT32_TYPE);
  alertDataSetType->add(comparisonPreviousProp);

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
    // TODO: month number instead of abbreviated name
    const std::string riskLevelProperty = validPropertyDateName(vecDates.at(i));

    te::dt::SimpleProperty* riskLevelProp = new te::dt::SimpleProperty(riskLevelProperty, te::dt::INT32_TYPE);
    alertDataSetType->add(riskLevelProp);
  }

  auto riskResultMap = getResultMap(alertPtr, pos, idProperty, datetimeColumnName, teDataset, vecDates);
  std::shared_ptr<te::mem::DataSet> alertDataSet = populateMonitoredObjectAlertDataset(vecDates, riskResultMap, comparisonPreviosProperty, alertPtr, fkProperty, alertDataSetType);
  addAdditionalData(alertDataSet, alertPtr, additionalDataMap);

  return alertDataSet;
}

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::gridAlert(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                                      std::string datetimeColumnName,
                                                                                      std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                      AlertPtr alertPtr,
                                                                                      terrama2::core::Filter filter,
                                                                                      terrama2::core::DataSetPtr dataset,
                                                                                      std::shared_ptr<te::da::DataSet> teDataset)
{
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

  // Remove uneccessary oldest dates
  auto lastValues = *filter.lastValues;

  if(vecDates.size() > lastValues)
    vecDates = {vecDates.rbegin(), vecDates.rbegin()+lastValues};

  auto alertDataSet = populateGridAlertDataset(dataset, alertPtr, vecDates, teDataset, dataSetType, datetimeColumnName);

  return alertDataSet;
}

struct isLesserDate
{
   template <class T, class U>
   bool operator()(const T& a, const U& b){ return *a < *b;}
};

std::shared_ptr<te::mem::DataSet> terrama2::services::alert::core::populateGridAlertDataset(terrama2::core::DataSetPtr dataset,
                                                                                            AlertPtr alertPtr,
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

  auto risk = alertPtr->risk;
  int riskBand = std::stoi(alertPtr->riskAttribute);

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
          auto riskTuple = risk.riskLevel(value);
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

void terrama2::services::alert::core::runAlert(terrama2::core::ExecutionPackage executionPackage,
                                               std::shared_ptr< AlertLogger > logger,
                                               std::weak_ptr<DataManager> weakDataManager,
                                               const std::map<std::string, std::string>& serverMap)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    return;
  }

  try
  {
    auto alertId = executionPackage.processId;

    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting alert generation");

    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    AlertPtr alertPtr = dataManager->findAlert(alertId);

    // input data
    auto inputDataSeries = dataManager->findDataSeries(alertPtr->dataSeriesId);
    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

    //retrieve additional data
    std::unordered_map<DataSeriesId, std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr> > tempAdditionalDataVector;
    for(auto additionalData : alertPtr->additionalDataVector)
    {
      auto dataSeries = dataManager->findDataSeries(additionalData.dataSeriesId);
      auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

      tempAdditionalDataVector.emplace(additionalData.dataSeriesId, std::make_pair(dataSeries, dataProvider));
    }

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    // analysing data

    auto filter = alertPtr->filter;

    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataMap = dataAccessor->getSeries(filter, remover);
    if(dataMap.empty())
    {
      logger->result(AlertLogger::DONE, nullptr, executionPackage.registerId);
      logger->log(AlertLogger::WARNING_MESSAGE, QObject::tr("No data to available.").toStdString(), executionPackage.registerId);
      TERRAMA2_LOG_WARNING() << QObject::tr("No data to available.");
      return;
    }

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
      catch(const terrama2::core::UndefinedTagException /*e*/)
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
        logger->log(AlertLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return;
      }

      // Store execution dates of dataset, ASC order
      std::vector<std::shared_ptr<te::dt::DateTime> > vecDates = getDates(teDataset, datetimeColumnName);

      std::shared_ptr<te::mem::DataSet> alertDataSet;
      if(inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
      {
        if(dataSetType->getNumberOfForeignKeys() != 1)
        {
          QString errMsg = QObject::tr("Invalid number of identifier attribute.");
          logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
          logger->log(AlertLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return;
        }

        auto tempProperties = dataSetType->getForeignKey(0)->getProperties();
        auto idProperty = tempProperties.front();
        if(tempProperties.size() != 1 || !idProperty)
        {
          QString errMsg = QObject::tr("Invalid identifier attribute.");
          logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
          logger->log(AlertLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return;
        }

        alertDataSet = monitoredObjectAlert(dataSetType,
                                            datetimeColumnName,
                                            vecDates,
                                            alertPtr,
                                            filter,
                                            dataset,
                                            teDataset,
                                            idProperty,
                                            tempAdditionalDataVector,
                                            remover);
      }
      else if (inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
      {
        alertDataSet = gridAlert( dataSetType,
                                  datetimeColumnName,
                                  vecDates,
                                  alertPtr,
                                  filter,
                                  dataset,
                                  teDataset);
      }

      ReportPtr reportPtr = std::make_shared<Report>(alertPtr, inputDataSeries, alertDataSet, vecDates);

      NotifierPtr notifierPtr = NotifierFactory::getInstance().make("EMAIL", serverMap, reportPtr);

      for(const auto& recipient : alertPtr->notifications)
        notifierPtr->send(recipient);

    }

    logger->result(AlertLogger::DONE, executionPackage.executionDate, executionPackage.registerId);

    TERRAMA2_LOG_INFO() << QObject::tr("Alert '%1' generated successfully").arg(alertPtr->name.c_str());
  }
  catch(const terrama2::Exception& e)
  {
    logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString(), executionPackage.registerId);
    TERRAMA2_LOG_DEBUG() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
  }
  catch(boost::exception& e)
  {
    logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::ERROR_MESSAGE, boost::diagnostic_information(e), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(std::exception& e)
  {
    QString errMsg(e.what());
    logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::ERROR_MESSAGE, e.what(), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << errMsg;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Unknown exception");
    logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
    logger->log(AlertLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    TERRAMA2_LOG_ERROR() << errMsg;
  }
}

std::function<std::tuple<int, std::string, std::string>(size_t pos)> terrama2::services::alert::core::createGetRiskFunction(terrama2::core::Risk risk, std::shared_ptr<te::da::DataSet> teDataSet)
{
  return [risk, teDataSet](size_t pos)
  {
    const auto& value = teDataSet->getDouble(pos);
    auto level = risk.riskLevel(value);
    return std::make_tuple(std::get<0>(level), std::get<1>(level), teDataSet->getValue(pos)->toString());
  };
}

std::string terrama2::services::alert::core::getIdentifierPropertyName(terrama2::core::DataSetPtr dataSet, terrama2::core::DataSeriesPtr dataSeries)
{
  return getProperty(dataSet, dataSeries, "identifier");
}

std::shared_ptr<te::da::DataSetType> terrama2::services::alert::core::createAlertDataSetType(AlertPtr alertPtr, terrama2::core::DataSetPtr dataset)
{
      const std::string dataSetAlertName = "alert_"+std::to_string(alertPtr->id)+"_"+std::to_string(dataset->id);
      return std::make_shared<te::da::DataSetType>(dataSetAlertName);
}
