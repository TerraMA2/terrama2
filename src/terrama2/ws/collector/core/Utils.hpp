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
        \brief Method to convert a gSOAP struct DataProvider to a struct terrama2::core::DataProvider .

        \param T1 MUST be a struct DataProvider, defined in soapStub.h(gSOAP generated file)

        \return terrama2::core::DataProvider that contains the data in gSOAP struct DataProvider passed.
      */
        template<typename T1, typename T2> T2 Struct2DataProvider(T1);


        template<typename T1, typename T2> T2 DataProviderPtr2Struct(T1);

        // VINICIUS: check if need to create a Struct2DataProviderPtr

        template <typename T1> terrama2::core::DataSetPtr Struct2DataSetPtr(T1 struct_dataset);

        template<typename T1> T1 DataSetPtr2Struct(terrama2::core::DataSetPtr datasetPtr);

      }
    }
  }
}

template <typename T1, typename T2>
T2 terrama2::ws::collector::core::Struct2DataProvider(T1 struct_dataprovider)
{
  // VINICIUS: check if a DataProvider constructor that receives (id, name, kind) was implemented
  //T2 dataprovider(struct_dataprovider.name, (terrama2::core::DataProvider::Kind)struct_dataprovider.kind, struct_dataprovider.id);
  T2 dataprovider(struct_dataprovider.name, (terrama2::core::DataProvider::Kind)struct_dataprovider.kind);

  dataprovider.setDescription(struct_dataprovider.description);
  dataprovider.setUri(struct_dataprovider.uri);
  dataprovider.setStatus((terrama2::core::DataProvider::Status)struct_dataprovider.status);

  return dataprovider;

}

template<typename T1, typename T2>
T2 terrama2::ws::collector::core::DataProviderPtr2Struct(T1 dataproviderPtr)
{

  T2 struct_dataprovider = T2{
      dataproviderPtr->id(),
      dataproviderPtr->name(),
      dataproviderPtr->description(),
      (int)dataproviderPtr->kind(),
      dataproviderPtr->uri(),
      (int)dataproviderPtr->status()
};

  return struct_dataprovider;
}



template <typename T1>
terrama2::core::DataSetPtr terrama2::ws::collector::core::Struct2DataSetPtr(T1 struct_dataset)
{

  auto dataproviderPtr = terrama2::core::DataManager::getInstance().findDataProvider(struct_dataset.id);

  // VINICIUS: check if the constructor accept the id parameter already
  //terrama2::core::DataSet dataset(dataproviderPtr, struct_dataset.name, (terrama2::core::DataSet::Kind)struct_dataset.kind, struct_dataset.id);
  terrama2::core::DataSet dataset(dataproviderPtr, struct_dataset.name, (terrama2::core::DataSet::Kind)struct_dataset.kind);

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

  auto datasetPtr = std::make_shared<terrama2::core::DataSet>(dataset);

  return datasetPtr;
}



template<typename T1>
T1 terrama2::ws::collector::core::DataSetPtr2Struct(terrama2::core::DataSetPtr datasetPtr)
{
  T1 struct_dataset;

  struct_dataset.data_provider_id =  datasetPtr->dataProvider()->id();
  struct_dataset.id = datasetPtr->id();
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
