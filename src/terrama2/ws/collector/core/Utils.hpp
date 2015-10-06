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
  \file terrama2/ws/collector/core/Utils.hpp

  \brief TerraMA2 Utils to Web Service.

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__

// STL
#include <memory>

// Boost
#include "boost/date_time/posix_time/posix_time.hpp"

// TerraLib
#include <terralib/datatype/TimeDuration.h>

// TerraMA2
#include "../../../core/DataManager.hpp"
#include "../../../core/DataProvider.hpp"
#include "../../../core/DataSet.hpp"

struct DataProvider;

namespace terrama2
{

  namespace ws
  {

  namespace collector
    {

      namespace core
      {

      /*!
        \brief Method to convert a gSOAP struct DataProvider to a terrama2::core::DataProviderPtr.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)

        \return terrama2::core::DataProvider that contains the data in gSOAP struct DataProvider passed.
      */
        template<typename T1> terrama2::core::DataProviderPtr Struct2DataProviderPtr(T1 struct_dataprovider);

      /*!
        \brief Method to convert a terrama2::core::DataProviderPtr to a gSOAP struct DataProvider.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)

        \return a gSOAP struct DataProvider that contains the data in terrama2::core::DataProviderPtr passed.
      */
        template<typename T1> T1 DataProviderPtr2Struct(terrama2::core::DataProviderPtr dataproviderPtr);

      /*!
        \brief Method to convert a gSOAP struct DataSet to a terrama2::core::DataSetPtr.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)

        \return terrama2::core::DataSetPtr that contains the data in gSOAP struct DataSet passed.
      */
        template <typename T1> terrama2::core::DataSetPtr Struct2DataSetPtr(T1 struct_dataset);

      /*!
        \brief Method to convert a terrama2::core::DataProvider to a gSOAP struct DataProvider.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)

        \return A gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
      */
        template<typename T1> T1 DataSetPtr2Struct(terrama2::core::DataSetPtr datasetPtr);

      }
    }
  }
}


template <typename T1>
terrama2::core::DataProviderPtr terrama2::ws::collector::core::Struct2DataProviderPtr(T1 struct_dataprovider)
{
  terrama2::core::DataProvider dataprovider(struct_dataprovider.name, (terrama2::core::DataProvider::Kind)struct_dataprovider.kind, struct_dataprovider.id);

  dataprovider.setDescription(struct_dataprovider.description);
  dataprovider.setUri(struct_dataprovider.uri);
  dataprovider.setStatus((terrama2::core::DataProvider::Status)struct_dataprovider.status);

  return std::make_shared<terrama2::core::DataProvider>(dataprovider);
}


template<typename T1>
T1 terrama2::ws::collector::core::DataProviderPtr2Struct(terrama2::core::DataProviderPtr dataproviderPtr)
{
  T1 struct_dataprovider = T1{
      dataproviderPtr->id(),
      dataproviderPtr->name(),
      dataproviderPtr->description(),
      (uint32_t)dataproviderPtr->kind(),
      dataproviderPtr->uri(),
      (uint32_t)dataproviderPtr->status()
};

  return struct_dataprovider;
}


template <typename T1>
terrama2::core::DataSetPtr terrama2::ws::collector::core::Struct2DataSetPtr(T1 struct_dataset)
{
  auto dataproviderPtr = terrama2::core::DataManager::getInstance().findDataProvider(struct_dataset.data_provider_id);

  terrama2::core::DataSet dataset(dataproviderPtr, struct_dataset.name, (terrama2::core::DataSet::Kind)struct_dataset.kind, struct_dataset.id);

  dataset.setDescription(struct_dataset.description);
  dataset.setStatus((terrama2::core::DataSet::Status)struct_dataset.status);

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string(struct_dataset.data_frequency));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string(struct_dataset.schedule));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string(struct_dataset.schedule_retry));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string(struct_dataset.schedule_timeout));

  dataset.setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataset.setSchedule(te::dt::TimeDuration(schedule));
  dataset.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataset.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

  return std::make_shared<terrama2::core::DataSet>(dataset);
}



template<typename T1>
T1 terrama2::ws::collector::core::DataSetPtr2Struct(terrama2::core::DataSetPtr datasetPtr)
{
  T1 struct_dataset;

  struct_dataset.data_provider_id =  datasetPtr->dataProvider()->id();
  struct_dataset.id = datasetPtr->id();
  struct_dataset.name = datasetPtr->name();
  struct_dataset.kind = (int)datasetPtr->kind();
  struct_dataset.description = datasetPtr->description();
  struct_dataset.status = (int)datasetPtr->status();
  struct_dataset.data_frequency = datasetPtr->dataFrequency().toString();
  struct_dataset.schedule = datasetPtr->schedule().toString();
  struct_dataset.schedule_retry = datasetPtr->scheduleRetry().toString();
  struct_dataset.schedule_timeout = datasetPtr->scheduleTimeout().toString();

  return struct_dataset;
}

#endif // __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
