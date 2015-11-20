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
#include <cmath>

// Boost
#include "boost/date_time/posix_time/posix_time.hpp"

// TerraLib
#include <terralib/geometry/Geometry.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/geometry/WKTReader.h>

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
        template<typename T1, typename T2, typename StructDataSetCollectRule> terrama2::core::DataSet Struct2DataSet(T1 struct_dataset);


        /*!
                \brief Method to convert a terrama2::core::DataProvider to a gSOAP struct DataProvider.

                \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)
                \param T2 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)

                \return A gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
              */
        template<typename T1, typename T2, typename StructDataSetCollectRule> T1 DataSet2Struct(terrama2::core::DataSet dataSet);


        /*!
          \brief

          \param

          \return
        */
        template<typename StructDataSetCollectRule> std::vector< StructDataSetCollectRule > DataSetCollectRules2Struct(std::vector< terrama2::core::DataSet::CollectRule >& collectorRules);


        /*!
         \brief

         \param

         \return
       */
        template<typename StructDataSetCollectRule> std::vector< terrama2::core::DataSet::CollectRule > Struct2DataSetCollectRules(std::vector< StructDataSetCollectRule >& struct_collectorRules);


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


template<typename T1, typename T2, typename StructDataSetCollectRule>
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

  dataSet.setCollectRules(Struct2DataSetCollectRules< StructDataSetCollectRule >(struct_dataSet.dataset_collectRules));

  std::map< std::string, std::string > metadata;

  for(int i = 0; i < struct_dataSet.metadata_keys.size(); i++)
  {
    metadata[struct_dataSet.metadata_keys.at(i)] = struct_dataSet.metadata_values.at(i);
  }

  dataSet.setMetadata(metadata);

  std::vector< terrama2::core::DataSetItem > dataSetItems = Struct2DataSetItem< T2 >(struct_dataSet.dataset_items);

  for(int i = 0; i < dataSetItems.size(); i++)
  {
    dataSet.add(dataSetItems.at(i));
  }

  return dataSet;
}


template<typename T1, typename T2, typename StructDataSetCollectRule>
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

  struct_dataSet.dataset_collectRules = DataSetCollectRules2Struct< StructDataSetCollectRule >(dataSet.collectRules());

  std::map< std::string, std::string > metadata(dataSet.metadata());

  for(auto& x: metadata)
  {
    struct_dataSet.metadata_keys.push_back(x.first);
    struct_dataSet.metadata_values.push_back(x.second);
  }

  struct_dataSet.dataset_items = DataSetItem2Struct< T2 >(dataSet.dataSetItems());

  return struct_dataSet;
}


template<typename StructDataSetCollectRule>
std::vector< StructDataSetCollectRule > terrama2::ws::collector::core::DataSetCollectRules2Struct(std::vector< terrama2::core::DataSet::CollectRule >& collectRules)
{
  std::vector< StructDataSetCollectRule > struct_dataset_colletRules;

  for(int i = 0; i < collectRules.size(); i++)
  {
    StructDataSetCollectRule struct_collectRule;

    struct_collectRule.id = collectRules.at(i).id;
    struct_collectRule.script = collectRules.at(i).script;
    struct_collectRule.datasetId = collectRules.at(i).datasetId;

   struct_dataset_colletRules.push_back(struct_collectRule);
  }

  return struct_dataset_colletRules;
}

template<typename StructDataSetCollectRule>
std::vector< terrama2::core::DataSet::CollectRule > terrama2::ws::collector::core::Struct2DataSetCollectRules(std::vector< StructDataSetCollectRule >& struct_collectRules)
{
  std::vector< terrama2::core::DataSet::CollectRule > collectRules;

  for(int i = 0; i < struct_collectRules.size(); i++)
  {
    terrama2::core::DataSet::CollectRule collectRule;

    collectRule.id = struct_collectRules.at(i).id;
    collectRule.script = struct_collectRules.at(i).script;
    collectRule.datasetId = struct_collectRules.at(i).datasetId;

    collectRules.push_back(collectRule);
  }

  return collectRules;
}

template<typename T1>
std::vector< T1 > terrama2::ws::collector::core::DataSetItem2Struct(std::vector<terrama2::core::DataSetItem>& dataset_items)
{
  std::vector< T1 > struct_dataset_items;

  for(int i = 0; i < dataset_items.size(); i++)
  {
    // need to initialize the struct to avoid to create non-existent filters
    T1 struct_dataset_item{0,0,0,0,"","","",0,"","","",std::nan(""),0,""};

    struct_dataset_item.id = dataset_items.at(i).id();
    struct_dataset_item.dataset = dataset_items.at(i).dataset();
    struct_dataset_item.status = (int) dataset_items.at(i).status();
    struct_dataset_item.mask = dataset_items.at(i).mask();
    struct_dataset_item.kind = (int) dataset_items.at(i).kind();
    struct_dataset_item.timezone = dataset_items.at(i).timezone();
    struct_dataset_item.path = dataset_items.at(i).path();

    terrama2::core::Filter filter = dataset_items.at(i).filter();

    struct_dataset_item.filter_datasetItem = filter.datasetItem();
    struct_dataset_item.filter_expressionType = (int) filter.expressionType();
    struct_dataset_item.filter_bandFilter = filter.bandFilter();

    if(filter.discardBefore() != nullptr)
      struct_dataset_item.filter_discardBefore = filter.discardBefore()->toString();

    if(filter.discardAfter() != nullptr)
      struct_dataset_item.filter_discardAfter = filter.discardAfter()->toString();

    // VINICIUS: toString() is generating a wrong WKT, need to replace '\n' for ','
    if(filter.geometry() != nullptr)
    {
      std::string geom = filter.geometry()->toString();
      std::replace( geom.begin(), geom.end(), '\n', ',');
      struct_dataset_item.filter_geometry = geom;
    }

    if(filter.value() != nullptr)
      struct_dataset_item.filter_value = *filter.value();
    else
      struct_dataset_item.filter_value = std::nan("");

    std::map< std::string, std::string > storageMetadata(dataset_items.at(i).storageMetadata());

    for(auto& x: storageMetadata)
    {
      struct_dataset_item.storageMetadata_keys.push_back(x.first);
      struct_dataset_item.storageMetadata_values.push_back(x.second);
    }

    struct_dataset_items.push_back(struct_dataset_item);
  }

  return struct_dataset_items;
}


template<typename T1>
std::vector< terrama2::core::DataSetItem > terrama2::ws::collector::core::Struct2DataSetItem(std::vector< T1 >& struct_dataset_items)
{
  std::vector< terrama2::core::DataSetItem > dataset_items;

  for(int i = 0; i < struct_dataset_items.size(); i++)
  {
    terrama2::core::DataSetItem dataset_item;

    dataset_item.setId(struct_dataset_items.at(i).id);
    dataset_item.setDataSet(struct_dataset_items.at(i).dataset);
    dataset_item.setStatus((terrama2::core::DataSetItem::Status) struct_dataset_items.at(i).status);
    dataset_item.setMask(struct_dataset_items.at(i).mask);
    dataset_item.setKind((terrama2::core::DataSetItem::Kind) struct_dataset_items.at(i).kind);
    dataset_item.setTimezone(struct_dataset_items.at(i).timezone);
    dataset_item.setPath(struct_dataset_items.at(i).path);

    terrama2::core::Filter filter(struct_dataset_items.at(i).filter_datasetItem);

    if(struct_dataset_items.at(i).filter_expressionType == 0)
    {
      filter.setExpressionType(terrama2::core::Filter::ExpressionType::NONE_TYPE);
    }
    else
      filter.setExpressionType(terrama2::core::Filter::ExpressionType(struct_dataset_items.at(i).filter_expressionType));

    filter.setBandFilter(struct_dataset_items.at(i).filter_bandFilter);

    if(!struct_dataset_items.at(i).filter_discardBefore.empty())
    {
      te::dt::DateTime* td = new te::dt::TimeInstant(boost::posix_time::ptime(boost::posix_time::time_from_string(struct_dataset_items.at(i).filter_discardBefore)));
      std::unique_ptr< te::dt::DateTime > discardBefore(td);
      filter.setDiscardBefore(std::move(discardBefore));
    }

    if(!struct_dataset_items.at(i).filter_discardAfter.empty())
    {
      te::dt::DateTime* td = new te::dt::TimeInstant(boost::posix_time::ptime(boost::posix_time::time_from_string(struct_dataset_items.at(i).filter_discardAfter)));
      std::unique_ptr< te::dt::DateTime > discardAfter(td);
      filter.setDiscardAfter(std::move(discardAfter));
    }

    if(!std::isnan(struct_dataset_items.at(i).filter_value))
    {
      std::unique_ptr< double > value(new double(struct_dataset_items.at(i).filter_value));
      filter.setValue(std::move(value));
    }

    if(!struct_dataset_items.at(i).filter_geometry.empty())
    {
      std::unique_ptr< te::gm::Geometry > geom(te::gm::WKTReader::read(struct_dataset_items.at(i).filter_geometry.c_str()));
      filter.setGeometry(std::move(geom));
    }

    dataset_item.setFilter(filter);

    std::map< std::string, std::string > storageMetadata;

    for(int j = 0; j < struct_dataset_items.at(i).storageMetadata_keys.size(); j++)
    {
      storageMetadata[struct_dataset_items.at(i).storageMetadata_keys.at(j)] = struct_dataset_items.at(i).storageMetadata_values.at(j);
    }

    dataset_item.setStorageMetadata(storageMetadata);

    dataset_items.push_back(dataset_item);
  }

  return dataset_items;
}

#endif // __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
