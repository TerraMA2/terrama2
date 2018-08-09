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


#include <terralib/Exception.h>
#include <algorithm>
#include <memory>

#include "../../Exception.hpp"
#include "../Exception.hpp"
#include "../data-access/DataSetSeries.hpp"
#include "../data-model/DataManager.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-access/DataStorager.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSet.hpp"
#include "../utility/DataStoragerFactory.hpp"
#include "../utility/Logger.hpp"
#include "StoragerManager.hpp"

terrama2::core::StoragerManager::StoragerManager(terrama2::core::DataManagerPtr dataManager) : dataManager_(dataManager)
{

}

void terrama2::core::StoragerManager::store(terrama2::core::DataSetSeries series, terrama2::core::DataSetPtr outputDataSet)
{
  if(!outputDataSet)
  {
    QString errMsg = QObject::tr("Invalid dataset %1.").arg(outputDataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataSeries = dataManager_->findDataSeries(outputDataSet->dataSeriesId);
  if(!dataSeries)
  {
    QString errMsg = QObject::tr("Invalid data series %1.").arg(outputDataSet->dataSeriesId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager_->findDataProvider(dataSeries->dataProviderId);
  if(!dataProvider)
  {
    QString errMsg = QObject::tr("Invalid data provider %1.").arg(dataProvider->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataStorager = terrama2::core::DataStoragerFactory::getInstance().make(dataSeries, dataProvider);

  std::string uri = dataStorager->getCompleteURI(outputDataSet);

  while(true)
  {
    {
      std::unique_lock<std::mutex> ul(mutex_);

      // Wait until a dataset with the same URI is not on the processing queue
      conditionVariable_.wait(ul, [this, uri]{ return std::find(vecURIs_.begin(), vecURIs_.end(), uri) == vecURIs_.end();});

      vecURIs_.push_back(uri);

      ul.unlock();
    }

    try
    {
      dataStorager->store(series, outputDataSet);
    }
    catch(const terrama2::Exception& /*e*/)
    {
      removeUriFromQueue(uri);
      throw;
    }
    catch(const te::Exception& e)
    {
      removeUriFromQueue(uri);
      auto errMsg = boost::get_error_info<te::ErrorDescription>(e);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataStoragerException() << ErrorDescription(QString::fromStdString(*errMsg));
    }
    catch(const boost::exception& e)
    {
      removeUriFromQueue(uri);
      auto errMsg = boost::diagnostic_information(e);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataStoragerException() << ErrorDescription(QString::fromStdString(errMsg));
    }
    catch(const std::exception& e)
    {
      removeUriFromQueue(uri);
      TERRAMA2_LOG_ERROR() << e.what();
      throw DataStoragerException() << ErrorDescription(e.what());
    }
    catch(...)
    {
      removeUriFromQueue(uri);
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not store the dataset");
      throw DataStoragerException() << ErrorDescription(QObject::tr("Could not store the dataset"));
    }

    removeUriFromQueue(uri);
    break;
  }

}

void terrama2::core::StoragerManager::removeUriFromQueue(const std::string& uri)
{
  std::lock_guard<std::mutex> lockGuard(mutex_);

  auto it = std::find(vecURIs_.begin(), vecURIs_.end(), uri);
  vecURIs_.erase(it);
  conditionVariable_.notify_all();
}
