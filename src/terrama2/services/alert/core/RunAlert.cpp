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

#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/data-access/DataAccessor.hpp"

#include "RunAlert.hpp"
#include "Alert.hpp"
#include "Report.hpp"
#include "ReportFactory.hpp"
#include "AdditionalDataHelper.hpp"

#include <QObject>

#include <limits>

#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/ForeignKey.h>

void terrama2::services::alert::core::runAlert(std::pair<AlertId, std::shared_ptr<te::dt::TimeInstantTZ> > alertInfo,
                                               std::shared_ptr< AlertLogger > logger,
                                               std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Unable to access DataManager");
    return;
  }

  RegisterId logId = 0;
  try
  {
    auto alertId = alertInfo.first;

    logId = logger->start(alertId);

    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting alert generation");

    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    auto alertPtr = dataManager->findAlert(alertId);

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
    filter.lastValue = true;
    auto risk = alertPtr->risk;

    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataMap = dataAccessor->getSeries(filter, remover);
    if(dataMap.empty())
    {
      logger->result(AlertLogger::DONE, nullptr, logId);
      logger->log(AlertLogger::WARNING_MESSAGE, QObject::tr("No data to available.").toStdString(), logId);
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
      auto fkProperty = idProperty->clone();
      fkProperty->setName(idProperty->getName()+"_fk");
      alertDataSetType->add(fkProperty);

      for(auto iter = additionalDataVector.begin(); iter != additionalDataVector.end(); ++iter)
      {
        iter->prepareData(alertPtr->filter);
        iter->addAdditionalAttributesColumns(alertDataSetType);
      }

      te::dt::SimpleProperty* riskAttributeProp = new te::dt::SimpleProperty(risk.attribute, te::dt::STRING_TYPE);
      alertDataSetType->add(riskAttributeProp);

      const std::string riskLevelProperty = "risk_level";
      te::dt::SimpleProperty* riskLevelProp = new te::dt::SimpleProperty(riskLevelProperty, te::dt::STRING_TYPE);
      alertDataSetType->add(riskLevelProp);

      auto alertDataSet = std::make_shared<te::mem::DataSet>(alertDataSetType.get());

      auto pos = dataSetType->getPropertyPosition(risk.attribute);
      if(pos == std::numeric_limits<decltype(pos)>::max())
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Risk attribute %1 doesn't exist in dataset %2").arg(QString::fromStdString(risk.attribute)).arg(dataset->id);
        continue;
      }

      // create a getRisk function
      auto getRisk = terrama2::services::alert::core::createGetRiskFunction(risk, teDataset);

      teDataset->moveBeforeFirst();
      alertDataSet->moveBeforeFirst();
      while(teDataset->moveNext())
      {
        alertDataSet->moveNext();

        te::mem::DataSetItem* item = new te::mem::DataSetItem(alertDataSet.get());
        //fk value
        auto fkValue = teDataset->getValue(idProperty->getName());
        item->setValue(fkProperty->getName(), fkValue->clone());

        // risk level
        int riskLevel = 0;
        std::string riskName;
        std::string attributeValue;
        std::tie(riskLevel, riskName, attributeValue) = getRisk(pos);
        item->setString(riskLevelProperty, std::to_string(riskLevel)+"("+riskName+")");
        item->setString(risk.attribute, attributeValue);

        for(auto iter = additionalDataVector.begin(); iter != additionalDataVector.end(); ++iter)
        {
          iter->addAdditionalValues(item, fkValue->toString());
        }

        alertDataSet->add(item);
      }

      auto& factory = ReportFactory::getInstance();
      auto report = factory.make(alertPtr->reportMetadata.at(ReportTags::TYPE), alertPtr->reportMetadata);
      report->process(alertPtr, dataset, alertInfo.second, alertDataSet);
    }

    logger->result(AlertLogger::DONE, alertInfo.second, logId);
  }
  catch(const terrama2::Exception& e)
  {
    logger->result(AlertLogger::ERROR, nullptr, logId);
    logger->log(AlertLogger::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString(), logId);
    TERRAMA2_LOG_DEBUG() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    throw;//re-throw
  }
  catch(boost::exception& e)
  {
    logger->result(AlertLogger::ERROR, nullptr, logId);
    logger->log(AlertLogger::ERROR_MESSAGE, boost::diagnostic_information(e), logId);
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(std::exception& e)
  {
    QString errMsg(e.what());
    logger->result(AlertLogger::ERROR, nullptr, logId);
    logger->log(AlertLogger::ERROR_MESSAGE, e.what(), logId);
    TERRAMA2_LOG_ERROR() << errMsg;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Unknown exception");
    logger->result(AlertLogger::ERROR, nullptr, logId);
    logger->log(AlertLogger::ERROR_MESSAGE, errMsg.toStdString(), logId);
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
