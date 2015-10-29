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
#include "../../../core/Utils.hpp"

namespace terrama2
{

  namespace ws
  {

  namespace collector
    {

      namespace core
      {

      /*!
        \brief Method to convert a gSOAP struct DataProvider to a terrama2::core::DataProvider.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)

        \return terrama2::core::DataProvider that contains the data in gSOAP struct DataProvider passed.
      */
        template<typename T1> terrama2::core::DataProvider Struct2DataProvider(T1 struct_dataprovider);


      /*!
        \brief Method to convert a terrama2::core::DataProvider to a gSOAP struct DataProvider.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)

        \return a gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
      */
        template<typename T1> T1 DataProvider2Struct(terrama2::core::DataProvider dataProvider);


      /*!
        \brief Method to convert a gSOAP struct DataSet to a terrama2::core::DataSet.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)
        \param T2 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)

        \return terrama2::core::DataSet that contains the data in gSOAP struct DataSet passed.
      */
        template <typename T1, typename T2> terrama2::core::DataSet Struct2DataSet(T1 struct_dataset);


      /*!
        \brief Method to convert a terrama2::core::DataProvider to a gSOAP struct DataProvider.

        \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)
        \param T2 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)

        \return A gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
      */
        template<typename T1, typename T2> T1 DataSet2Struct(terrama2::core::DataSet dataSet);


      /*!
        \brief Method to convert a terrama2::core::DataSetItem to a gSOAP struct DataSetItem.

        \param T1 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)

        \return A gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
      */
       template<typename T1> std::vector< T1 > DataSetItem2Struct(std::vector< terrama2::core::DataSetItem >& dataset_items);


      /*!
        \brief Method to convert a gSOAP struct DataSetItem to a terrama2::core::DataSetItem.

        \param T1 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)

        \return A gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
      */
       template<typename T1> std::vector< terrama2::core::DataSetItem > Struct2DataSetItem(std::vector< T1 >& struct_dataset_items);

      }
    }
  }
}


template <typename T1>
terrama2::core::DataProvider terrama2::ws::collector::core::Struct2DataProvider(T1 struct_dataprovider)
{
  terrama2::core::DataProvider dataProvider(struct_dataprovider.name, terrama2::core::ToDataProviderKind(struct_dataprovider.kind), struct_dataprovider.id);
  dataProvider.setDescription(struct_dataprovider.description);
  dataProvider.setUri(struct_dataprovider.uri);
  dataProvider.setStatus((terrama2::core::DataProvider::Status)struct_dataprovider.status);

  return dataProvider;
}


template<typename T1>
T1 terrama2::ws::collector::core::DataProvider2Struct(terrama2::core::DataProvider dataProvider)
{
  T1 struct_dataprovider = T1{
      dataProvider.id(),
      dataProvider.name(),
      dataProvider.description(),
      (uint32_t) dataProvider.kind(),
      dataProvider.uri(),
      (uint32_t) dataProvider.status()
};

  return struct_dataprovider;
}


template <typename T1, typename T2>
terrama2::core::DataSet terrama2::ws::collector::core::Struct2DataSet(T1 struct_dataSet)
{
  terrama2::core::DataSet dataSet(struct_dataSet.name, terrama2::core::ToDataSetKind(struct_dataSet.kind), struct_dataSet.id, struct_dataSet.data_provider_id);

  dataSet.setDescription(struct_dataSet.description);
  dataSet.setStatus((terrama2::core::DataSet::Status)struct_dataSet.status);

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string(struct_dataSet.data_frequency));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string(struct_dataSet.schedule));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string(struct_dataSet.schedule_retry));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string(struct_dataSet.schedule_timeout));

  dataSet.setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataSet.setSchedule(te::dt::TimeDuration(schedule));
  dataSet.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataSet.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

  std::vector< terrama2::core::DataSetItem > dataSetItems = Struct2DataSetItem< T2 >(struct_dataSet.dataset_items);

  for(int i = 0; i < dataSetItems.size(); i++)
  {
    dataSet.add(dataSetItems.at(i));
  }

  return dataSet;
}


template<typename T1, typename T2>
T1 terrama2::ws::collector::core::DataSet2Struct(terrama2::core::DataSet dataSet)
{
  T1 struct_dataSet;

  struct_dataSet.data_provider_id =  dataSet.provider();
  struct_dataSet.id = dataSet.id();
  struct_dataSet.name = dataSet.name();
  struct_dataSet.kind = (int) dataSet.kind();
  struct_dataSet.description = dataSet.description();
  struct_dataSet.status = (int) dataSet.status();
  struct_dataSet.data_frequency = dataSet.dataFrequency().toString();
  struct_dataSet.schedule = dataSet.schedule().toString();
  struct_dataSet.schedule_retry = dataSet.scheduleRetry().toString();
  struct_dataSet.schedule_timeout = dataSet.scheduleTimeout().toString();

  struct_dataSet.dataset_items = DataSetItem2Struct< T2 >(dataSet.dataSetItems());

  return struct_dataSet;
}


template<typename T1>
std::vector< T1 > terrama2::ws::collector::core::DataSetItem2Struct(std::vector<terrama2::core::DataSetItem>& dataset_items)
{
  std::vector< T1 > struct_dataset_items;

  for(int i = 0; i < dataset_items.size(); i++)
  {
    T1 struct_dataset_item;

    struct_dataset_item.id = dataset_items.at(i).id();
    struct_dataset_item.dataset = dataset_items.at(i).dataset();
    struct_dataset_item.status = (int) dataset_items.at(i).status();
    struct_dataset_item.mask = dataset_items.at(i).mask();
    struct_dataset_item.kind = (int) dataset_items.at(i).kind();
    struct_dataset_item.timezone = dataset_items.at(i).timezone();
    //struct_dataset_item.filter = dataset_items.at(i).filter();
    //struct_dataset_item.storageMetadata = dataset_items.at(i).storageMetadata();

    struct_dataset_items.push_back(struct_dataset_item);
  }
  // VINICIUS: work with filter and storageMetadata
  return struct_dataset_items;
}


template<typename T1>
std::vector< terrama2::core::DataSetItem > terrama2::ws::collector::core::Struct2DataSetItem(std::vector< T1 >& struct_dataset_items)

{
  std::vector< terrama2::core::DataSetItem > dataset_items;

  for(int i = 0; i < dataset_items.size(); i++)
  {
    terrama2::core::DataSetItem dataset_item;

    dataset_item.setId(struct_dataset_items.at(i).id);
    dataset_item.setDataSet(struct_dataset_items.at(i).dataset);
    dataset_item.setStatus((terrama2::core::DataSetItem::Status) struct_dataset_items.at(i).status);
    dataset_item.setMask(struct_dataset_items.at(i).mask);
    dataset_item.setKind((terrama2::core::DataSetItem::Kind) struct_dataset_items.at(i).kind);
    dataset_item.setTimezone(struct_dataset_items.at(i).timezone);
    //struct_dataset_item.setFilter(dataset_items.at(i).filter());
    //struct_dataset_item.setStorageMetadata(dataset_items.at(i).storageMetadata());

    dataset_items.push_back(dataset_item);
  }
  // VINICIUS: work with filter and storageMetadata
  return dataset_items;
}

#endif // __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
