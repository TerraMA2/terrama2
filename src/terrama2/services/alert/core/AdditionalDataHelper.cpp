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
  \file terrama2/services/alert/core/AdditionalDataHelper.cpp

  \brief

  \author Jano Simas
*/

#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"
#include "../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataSetMapper.hpp"
#include "AdditionalDataHelper.hpp"
#include "DataManager.hpp"
#include "Exception.hpp"

#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/memory/DataSetItem.h>

terrama2::services::alert::core::AdditionalDataHelper::AdditionalDataHelper(AdditionalData additionalData, DataManagerPtr dataManager)
  : additionalData_(additionalData)
{
  dataSeries_ = dataManager->findDataSeries(additionalData.id);
  dataProvider_ = dataManager->findDataProvider(dataSeries_->dataProviderId);
  remover_ = std::make_shared<terrama2::core::FileRemover>();
}

bool terrama2::services::alert::core::AdditionalDataHelper::prepareData(terrama2::core::Filter filter)
{
  auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider_, dataSeries_);
  dataMap_ = dataAccessor->getSeries(filter, remover_);
  if(dataMap_.empty())
  {
    TERRAMA2_LOG_WARNING() << QObject::tr("No data to available in dataseries %1.").arg(additionalData_.id);
    return false;
  }

  for(const auto& data : dataMap_)
  {
    const auto& dataSetSeries = data.second;
    auto mapper = std::make_shared<terrama2::core::DataSetMapper>(dataSetSeries.syncDataSet->dataset(), additionalData_.identifier);
    mapperMap_.emplace(data.first, mapper);
  }

  isDataReady_ = true;
  return true;
}

void terrama2::services::alert::core::AdditionalDataHelper::addAdditionalAttributesColumns(std::shared_ptr<te::da::DataSetType> alertDataSetType) const
{
  if(!isDataReady_)
  {
    throw AdditionalDataException() << ErrorDescription(QObject::tr("Data not loaded.\nCall AdditionalDataHelper::prepareData."));
  }

  for(const auto& data : dataMap_)
  {
    const auto& dataSet = data.first;
    const auto& dataSetSeries = data.second;

    for(const auto& attribute : additionalData_.attributes)
    {
      try
      {
        std::string name = dataSeries_->name+"_"+attribute+"_"+std::to_string(dataSet->id);
        auto teDataSetType = dataSetSeries.teDataSetType;
        auto property = teDataSetType->getProperty(attribute);
        auto newProperty = property->clone();
        newProperty->setName(name);
        alertDataSetType->add(newProperty);
      }
      catch(...)
      {
        TERRAMA2_LOG_WARNING() << QObject::tr("Attribute %1 not found in dataset %2").arg(QString::fromStdString(attribute)).arg(dataSet->id);
      }
    }
  }
}

void terrama2::services::alert::core::AdditionalDataHelper::addAdditionalValues(te::mem::DataSetItem* item, const std::string& fkValue) const
{
  if(!isDataReady_)
  {
    throw AdditionalDataException() << ErrorDescription(QObject::tr("Data not loaded.\nCall AdditionalDataHelper::prepareData."));
  }

  for(auto data : dataMap_)
  {
    const auto& dataSet = data.first;
    auto dataSetSeries = data.second;

    auto mapper = mapperMap_.at(dataSet);


    for(const auto& attribute : additionalData_.attributes)
    {
      auto value = mapper->getValue(fkValue, attribute);

      std::string columnName = dataSeries_->name+"_"+attribute+"_"+std::to_string(dataSet->id);
      if(value.get())
        item->setValue(columnName, value->clone());
      else
        item->setValue(columnName, nullptr);
    }
  }
}
