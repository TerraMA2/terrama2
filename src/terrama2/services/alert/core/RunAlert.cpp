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
*/

// TerraMA2
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-model/DataSeriesRisk.hpp"
#include "RunAlert.hpp"
#include "Alert.hpp"
#include "Report.hpp"
#include "AdditionalDataHelper.hpp"


// Terralib
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/ForeignKey.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/utils/Utils.h>

// Qt
#include <QObject>

// STL
#include <limits>


void terrama2::services::alert::core::runAlert(terrama2::core::ExecutionPackage executionPackage,
                                               std::shared_ptr< AlertLogger > logger,
                                               std::weak_ptr<DataManager> weakDataManager)
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
    auto inputDataSeries = dataManager->findDataSeries(alertPtr->risk.dataSeriesId);
    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

    //retrieve additional data
    std::vector<AdditionalDataHelper> additionalDataVector;
    for(auto additionalData : alertPtr->additionalDataVector)
    {
      additionalDataVector.emplace_back(additionalData, dataManager);
    }

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    // analysing data

    auto filter = alertPtr->filter;
    auto risk = alertPtr->risk;

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

      const std::string dataSetAlertName = "alert_"+std::to_string(alertPtr->id)+"_"+std::to_string(dataset->id);
      auto alertDataSetType = std::make_shared<te::da::DataSetType>(dataSetAlertName);

      auto teDataset = dataSeries.syncDataSet->dataset();
      auto dataSetType = dataSeries.teDataSetType;

      auto idProperty = dataSetType->getProperty(getIdentifierPropertyName(dataset, inputDataSeries));
      if(!idProperty)
      {
        QString errMsg = QObject::tr("Invalid identifier attribute.");
        logger->result(AlertLogger::ERROR, nullptr, executionPackage.registerId);
        logger->log(AlertLogger::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return;
      }
      auto fkProperty = idProperty->clone();
      fkProperty->setName(idProperty->getName()+"_fk");
      alertDataSetType->add(fkProperty);

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

      for(auto iter = additionalDataVector.begin(); iter != additionalDataVector.end(); ++iter)
      {
        iter->prepareData(alertPtr->filter);
        iter->addAdditionalAttributesColumns(alertDataSetType);
      }

      auto riskAttributeProp = dataSetType->getProperty(risk.attribute)->clone();
      alertDataSetType->add(riskAttributeProp);

      const std::string comparisonPreviosProperty = "comparison_previous";
      te::dt::SimpleProperty* comparisonPreviousProp = new te::dt::SimpleProperty(comparisonPreviosProperty, te::dt::INT32_TYPE);
      alertDataSetType->add(comparisonPreviousProp);

      auto pos = dataSetType->getPropertyPosition(risk.attribute);
      if(pos == std::numeric_limits<decltype(pos)>::max())
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Risk attribute %1 doesn't exist in dataset %2").arg(QString::fromStdString(risk.attribute)).arg(dataset->id);
        continue;
      }

      // create a getRisk function
      auto getRisk = terrama2::services::alert::core::createGetRiskFunction(risk, teDataset);

      std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, terrama2::core::RiskLevel> >, comparatorAbstractData> riskResultMap;

      // Store execution dates of dataset
      std::vector<std::shared_ptr<te::dt::DateTime> > vecDates;

      teDataset->moveBeforeFirst();

      for (int i = 0; i < teDataset->size(); ++i)
      {
        teDataset->moveNext();

        // Retrieve all execution dates of dataset
        std::shared_ptr<te::dt::DateTime> executionDate = teDataset->getDateTime(datetimeColumnName);

        auto it = std::find_if(vecDates.begin(), vecDates.end(),
                               [&](std::shared_ptr<te::dt::DateTime> const& current)
                                 {
                                    return *current == *executionDate;
                                 });

        if(it == vecDates.end())
          vecDates.push_back(executionDate);
      }

      // Order ASC
      std::sort(vecDates.begin(),vecDates.end(),
                [](const std::shared_ptr<te::dt::DateTime>& left, const std::shared_ptr<te::dt::DateTime>& right)
                {
                  return *left > *right;
                });

      // Remove unnecessary dates
      while(vecDates.size() > *filter.lastValues)
        vecDates.pop_back();

      // Insert the risk properties
      for(int i = 0; i < *filter.lastValues; i++)
      {
        if(i >= vecDates.size())
          break;

        // TODO: month number instead of abbreviated name
        const std::string riskLevelProperty = terrama2::core::validPropertyName(vecDates.at(i)->toString());

        te::dt::SimpleProperty* riskLevelProp = new te::dt::SimpleProperty(riskLevelProperty, te::dt::INT32_TYPE);
        alertDataSetType->add(riskLevelProp);
      }

      teDataset->moveBeforeFirst();

      // Get the risk for data
      for (int j = 0; j < teDataset->size(); ++j)
      {
        teDataset->moveNext();

        std::shared_ptr<te::dt::DateTime> executionDate = teDataset->getDateTime(datetimeColumnName);

        auto it = std::find_if(vecDates.begin(), vecDates.end(),
                               [&](std::shared_ptr<te::dt::DateTime> const& current)
                                 {
                                    return *current == *executionDate;
                                 });

        // Only process the risk of data in stored dates
        if(it == vecDates.end())
          continue;

        std::shared_ptr<te::dt::AbstractData> identifierValue = teDataset->getValue(idProperty->getName());

        // process risk level
        int riskLevel = 0;
        std::string riskName;
        std::string attributeValue;
        std::tie(riskLevel, riskName, attributeValue) = getRisk(pos);

        terrama2::core::RiskLevel risk;
        risk.level = riskLevel;
        risk.name = riskName;
        risk.textValue = attributeValue;

        auto& resultMap = riskResultMap[identifierValue];
        std::shared_ptr<te::dt::AbstractData> attrValue = teDataset->getValue(pos);
        auto pair = std::make_pair(attrValue, risk);
        resultMap[executionDate->toString()] = pair;
      }

      auto alertDataSet = std::make_shared<te::mem::DataSet>(alertDataSetType.get());

      alertDataSet->moveBeforeFirst();

      for(auto& item : riskResultMap)
      {
        te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(alertDataSet.get());
        auto value = item.first;
        auto& resultMap = item.second;

        std::string currentRiskProperty = terrama2::core::validPropertyName(vecDates[0]->toString());

        dsItem->setValue(fkProperty->getName(), value->clone());

        auto attrValue = resultMap[vecDates[0]->toString()].first;
        dsItem->setValue(risk.attribute, attrValue->clone());

        auto currentRisk = resultMap[vecDates[0]->toString()].second;
        dsItem->setInt32(currentRiskProperty, currentRisk.level);

        if(vecDates.size() > 1)
        {
          auto risk2 = resultMap[vecDates[1]->toString()].second;

          int comparisonResult = 0;
          if(currentRisk.level < risk2.level)
            comparisonResult = -1;
          else if(currentRisk.level > risk2.level)
            comparisonResult = 1;

          std::string pastRiskProperty = terrama2::core::validPropertyName(vecDates[1]->toString());

          dsItem->setInt32(pastRiskProperty, risk2.level);
          dsItem->setInt32(comparisonPreviosProperty, comparisonResult);
        }

        for(int i = 2; i < vecDates.size(); i++)
        {
          std::string property = terrama2::core::validPropertyName(vecDates[i]->toString());
          auto risk = resultMap[vecDates[i]->toString()].second;
          dsItem->setInt32(property, risk.level);
        }

        for(auto iter = additionalDataVector.begin(); iter != additionalDataVector.end(); ++iter)
        {
          iter->addAdditionalValues(dsItem, value->toString());
        }

        alertDataSet->add(dsItem);

      }

      terrama2::services::alert::core::Report report(alertPtr, alertDataSet, alertDataSetType, vecDates);

      std::shared_ptr<te::da::DataSet> filteredDataSet = report.retrieveDataChangedRisk();
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

std::function<std::tuple<int, std::string, std::string>(size_t pos)> terrama2::services::alert::core::createGetRiskFunction(terrama2::core::DataSeriesRisk risk, std::shared_ptr<te::da::DataSet> teDataSet)
{
  if(risk.riskType == terrama2::core::RiskType::NUMERIC)
  {
    return [risk, teDataSet](size_t pos)
    {
      const auto& value = teDataSet->getDouble(pos);
      auto level = risk.riskLevel(value);
      return std::make_tuple(std::get<0>(level), std::get<1>(level), teDataSet->getValue(pos)->toString());
    };
  }
  else
  {
    return [risk, teDataSet](size_t pos)
    {
      const auto& value = teDataSet->getString(pos);
      auto level = risk.riskLevel(value);
      return std::make_tuple(std::get<0>(level), std::get<1>(level), teDataSet->getValue(pos)->toString());
    };
  }
}

std::string terrama2::services::alert::core::getIdentifierPropertyName(terrama2::core::DataSetPtr dataSet, terrama2::core::DataSeriesPtr dataSeries)
{
  return getProperty(dataSet, dataSeries, "identifier");
}
