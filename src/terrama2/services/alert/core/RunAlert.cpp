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
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"

#include "RunAlert.hpp"
#include "Alert.hpp"

#include <QObject>

#include <limits>

#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/ForeignKey.h>

//FIXME: Test code, should be removed
#include <QUrl>
void testStore(terrama2::core::DataSetPtr dataSet,
               std::shared_ptr<te::mem::DataSet> teDataSet,
               std::shared_ptr<te::da::DataSetType> teDataSetType)
{
  QUrl uri;
  uri.setScheme("postgis");
  uri.setHost("localhost");
  uri.setPort(5432);
  uri.setUserName("postgres");
  uri.setPassword("postgres");
  uri.setPath("/basedeteste");

  //DataProvider information
  terrama2::core::DataProvider* dataProviderPostGis = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPostGisPtr(dataProviderPostGis);
  dataProviderPostGis->uri = uri.url().toStdString();

  dataProviderPostGis->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
  dataProviderPostGis->dataProviderType = "POSTGIS";
  dataProviderPostGis->active = true;

  //DataSeries information
  terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);

  terrama2::core::DataSet* dataSetOutput = new terrama2::core::DataSet();
  terrama2::core::DataSetPtr dataSetOutputPtr(dataSetOutput);
  dataSetOutput->active = true;
  dataSetOutput->format.emplace("table_name", "alert");
  dataSetOutput->format.emplace("timestamp_column", "execution_date");

  terrama2::core::DataSetSeries dataSeries;
  dataSeries.dataSet = dataSetOutputPtr;
  dataSeries.syncDataSet = terrama2::core::SynchronizedDataSetPtr(new terrama2::core::SynchronizedDataSet(teDataSet));
  dataSeries.teDataSetType = teDataSetType;

  auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(dataProviderPostGisPtr);
  dataStorager->store(dataSeries, dataSetOutputPtr);
}

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

  try
  {
    auto alertId = alertInfo.first;
    RegisterId logId = 0;
    if(logger.get())
      logId = logger->start(alertId);

    TERRAMA2_LOG_DEBUG() << QObject::tr("Starting alert generation");

    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    auto alertPtr = dataManager->findAlert(alertId);

    // input data
    auto inputDataSeries = dataManager->findDataSeries(alertPtr->risk.dataSeriesId);
    auto inputDataProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    // analysing data

    auto filter = alertPtr->filter;
    auto risk = alertPtr->risk;

    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);
    auto dataMap = dataAccessor->getSeries(filter);
    if(dataMap.empty())
    {
      if(logger.get())
        logger->done(nullptr, logId);
      TERRAMA2_LOG_WARNING() << QObject::tr("No data to available.");
      return;
    }

    for(const auto& data : dataMap)
    {
      auto dataset = data.first;
      auto dataSeries = data.second;

      const std::string dataSetAlertName = "alert_"+std::to_string(alertPtr->id)+"_"+std::to_string(dataset->id);
      auto alertDataSetType = std::make_shared<te::da::DataSetType>(dataSetAlertName);

      const std::string alertLevelName = "alert_level";
      te::dt::SimpleProperty* alertProp = new te::dt::SimpleProperty(alertLevelName, te::dt::INT32_TYPE);
      alertDataSetType->add(alertProp);

      const std::string executionDateName = "execution_date";
      te::dt::DateTimeProperty* dateProp = new te::dt::DateTimeProperty(executionDateName, te::dt::TIME_INSTANT_TZ, true);
      alertDataSetType->add(dateProp);

      const std::string identifierFk = terrama2::services::alert::core::getIdentifierPropertyName(dataset, inputDataSeries);
      const std::string identifier = identifierFk+"_fk";
      auto teDataSetType = dataSeries.teDataSetType;
      te::dt::Property* identifierProp = teDataSetType->getProperty(identifierFk);

      if(identifierProp)
      {
        te::dt::Property* foreignProp = identifierProp->clone();
        foreignProp->setName(identifier);
        alertDataSetType->add(foreignProp);

        std::string nameUk = dataSetAlertName+ "_uk";
        te::da::UniqueKey* uniqueKey = new te::da::UniqueKey(nameUk, alertDataSetType.get());
        uniqueKey->add(foreignProp);
        uniqueKey->add(dateProp);
      }
      else
      {
        throw;//TODO: no identifier property
      }

      auto alertDataSet = std::make_shared<te::mem::DataSet>(alertDataSetType.get());

      auto teDataset = dataSeries.syncDataSet->dataset();

      auto dataSetType = dataSeries.teDataSetType;
      auto pos = dataSetType->getPropertyPosition(risk.attribute);
      if(pos == std::numeric_limits<decltype(pos)>::max())
      {
        //TODO: warning
        continue;
      }

      teDataset->moveBeforeFirst();
      alertDataSet->moveBeforeFirst();

      std::function<int(size_t pos)> getRisk = terrama2::services::alert::core::createGetRiskFunction(risk, teDataset);

      while(teDataset->moveNext())
      {
        alertDataSet->moveNext();

        te::mem::DataSetItem* item = new te::mem::DataSetItem(alertDataSet.get());

        int riskLevel = getRisk(pos);
        item->setInt32(alertLevelName, riskLevel);

        auto id = teDataset->getValue(identifierFk);
        item->setValue(identifier, id.release());

        auto executionData = alertInfo.second;
        item->setDateTime(executionDateName, static_cast<te::dt::DateTime*>(executionData->clone()));

        alertDataSet->add(item);
      }

      //FIXME: how should the alert be stored?
      // testStore(dataset, alertDataSet, alertDataSetType);
    }

    if(logger.get())
      logger->done(alertInfo.second, logId);
  }
  catch(...)
  {
    //TODO: Catch in run analysis
  }
}

std::function<int(size_t pos)> terrama2::services::alert::core::createGetRiskFunction(terrama2::core::DataSeriesRisk risk, std::shared_ptr<te::da::DataSet> teDataSet)
{
  if(risk.riskType == terrama2::core::RiskType::NUMERIC)
  {
    return [risk, teDataSet](size_t pos){
                                          const auto& value = teDataSet->getDouble(pos);
                                          return risk.riskLevel(value);
                                        };
  }
  else
  {
    return [risk, teDataSet](size_t pos){
                                          const auto& value = teDataSet->getString(pos);
                                          return risk.riskLevel(value);
                                        };
  }
}

std::string terrama2::services::alert::core::getIdentifierPropertyName(terrama2::core::DataSetPtr dataSet, terrama2::core::DataSeriesPtr dataSeries)
{
  return getProperty(dataSet, dataSeries, "identifier");
}
