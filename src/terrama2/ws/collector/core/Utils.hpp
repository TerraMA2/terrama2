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

         It has methods to convert data between TerraMA2 and structs, it will allow to pass the data trough
         gSOAP Web Service.

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__

// STL
#include <memory>
#include <cmath>
#include <strstream>

// Boost
#include "boost/date_time/posix_time/posix_time.hpp"

// TerraLib
#include <terralib/geometry/Polygon.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/geometry/WKTReader.h>

// TerraMA2
#include "../../../core/DataManager.hpp"
#include "../../../core/DataProvider.hpp"
#include "../../../core/DataSet.hpp"
#include "../../../core/Intersection.hpp"
#include "../../../core/Utils.hpp"
#include "../../../collector/Utils.hpp"

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

          \param T1 MUST be a gSOAP struct DataProvider, defined in soapStub.h(gSOAP generated file)

          \return terrama2::core::DataProvider that contains the data in gSOAP struct DataProvider passed.
        */
        template<typename T1> terrama2::core::DataProvider Struct2DataProvider(T1 struct_dataprovider);


        /*!
          \brief Method to convert a terrama2::core::DataProvider to a gSOAP struct DataProvider.

          \param T1 MUST be a gSOAP struct DataProvider, defined in soapStub.h(gSOAP generated file)

          \return a gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
        */
        template<typename T1> T1 DataProvider2Struct(terrama2::core::DataProvider dataProvider);


        /*!
          \brief Method to convert a gSOAP struct DataSet to a terrama2::core::DataSet.

          \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)
          \param T2 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)
          \param StructIntersection MUST be a gSOAP Intersection struct, defined in soapStub.h(gSOAP generated file)

          \return terrama2::core::DataSet that contains the data in gSOAP struct DataSet passed.
        */
        template<typename T1, typename T2, typename StructIntersection> terrama2::core::DataSet Struct2DataSet(T1 struct_dataset);


        /*!
          \brief Method to convert a terrama2::core::DataProvider to a gSOAP struct DataProvider.

          \param T1 MUST be a gSOAP struct DataSet, defined in soapStub.h(gSOAP generated file)
          \param T2 MUST be a gSOAP struct DataSetItem, defined in soapStub.h(gSOAP generated file)
          \param StructIntersection MUST be a gSOAP struct Intersection, defined in soapStub.h(gSOAP generated file)

          \return A gSOAP struct DataProvider that contains the data in terrama2::core::DataProvider passed.
        */
        template<typename T1, typename T2, typename StructIntersection> T1 DataSet2Struct(terrama2::core::DataSet dataSet);


        /*!
          \brief Method to convert a terrama2::core::Intersection into a gSOAP struct Intersection.

          \param StructIntersection MUST be a gSOAP struct Intersection, defined in soapStub.h(gSOAP generated file)

          \return A gSOAP Intersection struct that contains the data in terrama2::core::Intersection passed.
        */
        template<typename StructIntersection> StructIntersection DataSetIntersection2Struct(terrama2::core::Intersection intersection);


        /*!
          \brief Method to convert a struct Intersection into a gSOAP terrama2::core::Intersection.

          \param StructIntersection MUST be a gSOAP struct Intersection, defined in soapStub.h(gSOAP generated file)

          \return A terrama2::core::Intersection that contains the data in the gSOAP struct Intersection passed.
        */
        template<typename StructIntersection> terrama2::core::Intersection Struct2DataSetIntersection(StructIntersection structIntersection);


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
  dataProvider.setOrigin(terrama2::core::ToDataProviderOrigin(struct_dataprovider.origin));

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
      (uint32_t) dataProvider.origin(),
      dataProvider.uri(),
      (uint32_t) dataProvider.status()
};

  return struct_dataprovider;
}


template<typename T1, typename T2, typename StructIntersection>
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

  dataSet.setIntersection(Struct2DataSetIntersection <StructIntersection> (struct_dataSet.intersection));

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


template<typename T1, typename T2, typename StructIntersection>
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
  struct_dataSet.intersection = DataSetIntersection2Struct< StructIntersection >(dataSet.intersection());

  std::map< std::string, std::string > metadata(dataSet.metadata());

  for(auto& x: metadata)
  {
    struct_dataSet.metadata_keys.push_back(x.first);
    struct_dataSet.metadata_values.push_back(x.second);
  }

  struct_dataSet.dataset_items = DataSetItem2Struct< T2 >(dataSet.dataSetItems());

  return struct_dataSet;
}


template<typename StructIntersection>
StructIntersection terrama2::ws::collector::core::DataSetIntersection2Struct(terrama2::core::Intersection intersection)
{
  StructIntersection structDatasetIntersection;
  structDatasetIntersection.datasetId = intersection.dataset();

  std::map<uint64_t, std::string> bandMap = intersection.bandMap();
  std::vector<uint64_t> keyVecBand;
  std::vector<std::string> valuesVecBand;
  for(auto it = bandMap.begin(); it != bandMap.end(); ++it)
  {
    keyVecBand.push_back(it->first);
    valuesVecBand.push_back(it->second);
  }

  structDatasetIntersection.bandMap_keys = keyVecBand;
  structDatasetIntersection.bandMap_values = valuesVecBand;

  auto attrMap = intersection.attributeMap();
  std::vector<std::string> keyVecAttr;
  std::vector<std::vector<std::string> > valuesVecAttr;
  for(auto it = attrMap.begin(); it != attrMap.end(); ++it)
  {
    keyVecAttr.push_back(it->first);
    valuesVecAttr.push_back(it->second);
  }

  structDatasetIntersection.attributeMap_keys = keyVecAttr;
  structDatasetIntersection.attributeMap_values = valuesVecAttr;


  return structDatasetIntersection;
}

template<typename StructIntersection> terrama2::core::Intersection terrama2::ws::collector::core::Struct2DataSetIntersection(StructIntersection structIntersection)
{
  terrama2::core::Intersection intersection(structIntersection.datasetId);

  std::map<std::string, std::vector<std::string> > attributeMap;
  for(int i = 0; i < structIntersection.attributeMap_keys.size(); ++i)
  {
    attributeMap[structIntersection.attributeMap_keys[i]] = structIntersection.attributeMap_values[i];
  }
  intersection.setAttributeMap(attributeMap);

  std::map<uint64_t, std::string> bandMap;
  for(int i = 0; i < structIntersection.bandMap_keys.size(); ++i)
  {
    bandMap[structIntersection.bandMap_keys[i]] = structIntersection.bandMap_values[i];
  }
  intersection.setBandMap(bandMap);

  return intersection;
}


template<typename T1>
std::vector< T1 > terrama2::ws::collector::core::DataSetItem2Struct(std::vector<terrama2::core::DataSetItem>& dataset_items)
{
  std::vector< T1 > struct_dataset_items;

  for(int i = 0; i < dataset_items.size(); i++)
  {
    // need to initialize the struct to avoid to create non-existent filters
    T1 struct_dataset_item{0,0,0,0,"","","",0,0,"","","","","","","",0,std::nan(""),0,""};

    struct_dataset_item.id = dataset_items.at(i).id();
    struct_dataset_item.dataset = dataset_items.at(i).dataset();
    struct_dataset_item.status = (int) dataset_items.at(i).status();
    struct_dataset_item.mask = dataset_items.at(i).mask();
    struct_dataset_item.kind = (int) dataset_items.at(i).kind();
    struct_dataset_item.timezone = dataset_items.at(i).timezone();
    struct_dataset_item.path = dataset_items.at(i).path();
    struct_dataset_item.srid = dataset_items.at(i).srid();

    terrama2::core::Filter filter = dataset_items.at(i).filter();

    struct_dataset_item.filter_datasetItem = filter.datasetItem();
    struct_dataset_item.filter_expressionType = (int) filter.expressionType();
    struct_dataset_item.filter_bandFilter = filter.bandFilter();

    if(filter.discardBefore() != nullptr)
    {
      terrama2::collector::BoostLocalDateTime2DateTimeString(filter.discardBefore()->getTimeInstantTZ(), struct_dataset_item.filter_discardBefore_date, struct_dataset_item.filter_discardBefore_time ,struct_dataset_item.filter_discardBefore_timezone);
    }

    if(filter.discardAfter() != nullptr)
      terrama2::collector::BoostLocalDateTime2DateTimeString(filter.discardAfter()->getTimeInstantTZ(), struct_dataset_item.filter_discardAfter_date, struct_dataset_item.filter_discardAfter_time ,struct_dataset_item.filter_discardAfter_timezone);

    // VINICIUS: TERRALIB toString() is generating a wrong WKT, need to replace '\n' for ','
    if(filter.geometry() != nullptr)
    {
      std::string geom = filter.geometry()->toString();
      std::replace( geom.begin(), geom.end(), '\n', ',');
      struct_dataset_item.filter_geometry = geom;
      struct_dataset_item.filter_geometry_srid = filter.geometry()->getSRID();
    }

    if(filter.value() != nullptr)
      struct_dataset_item.filter_value = *filter.value();
    else
      struct_dataset_item.filter_value = std::nan("");

    std::map< std::string, std::string > metadata(dataset_items.at(i).metadata());

    for(auto& x: metadata)
    {
      struct_dataset_item.metadata_keys.push_back(x.first);
      struct_dataset_item.metadata_values.push_back(x.second);
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
    dataset_item.setSrid(struct_dataset_items.at(i).srid);

    terrama2::core::Filter filter(struct_dataset_items.at(i).filter_datasetItem);

    if(struct_dataset_items.at(i).filter_expressionType == 0)
    {
      filter.setExpressionType(terrama2::core::Filter::ExpressionType::NONE_TYPE);
    }
    else
      filter.setExpressionType(terrama2::core::Filter::ExpressionType(struct_dataset_items.at(i).filter_expressionType));

    filter.setBandFilter(struct_dataset_items.at(i).filter_bandFilter);

    if(!struct_dataset_items.at(i).filter_discardBefore_date.empty())
    {
      //Build a te::dt::TimeInstantTZ
      std::unique_ptr< te::dt::TimeInstantTZ > discardBefore(new te::dt::TimeInstantTZ(terrama2::collector::DateTimeString2BoostLocalDateTime(struct_dataset_items.at(i).filter_discardBefore_date, struct_dataset_items.at(i).filter_discardBefore_time, struct_dataset_items.at(i).filter_discardBefore_timezone)));
      filter.setDiscardBefore(std::move(discardBefore));
    }

    if(!struct_dataset_items.at(i).filter_discardAfter_date.empty())
    {
      //Build a te::dt::TimeInstantTZ
      std::unique_ptr< te::dt::TimeInstantTZ > discardAfter(new te::dt::TimeInstantTZ(terrama2::collector::DateTimeString2BoostLocalDateTime(struct_dataset_items.at(i).filter_discardAfter_date, struct_dataset_items.at(i).filter_discardAfter_time, struct_dataset_items.at(i).filter_discardAfter_timezone)));
      filter.setDiscardAfter(std::move(discardAfter));
    }

    if(!std::isnan(struct_dataset_items.at(i).filter_value))
    {
      std::unique_ptr< double > value(new double(struct_dataset_items.at(i).filter_value));
      filter.setValue(std::move(value));
    }

    if(!struct_dataset_items.at(i).filter_geometry.empty())
    {
      std::unique_ptr< te::gm::Polygon > geom(dynamic_cast<te::gm::Polygon*>(te::gm::WKTReader::read(struct_dataset_items.at(i).filter_geometry.c_str())));
      geom->setSRID(struct_dataset_items.at(i).filter_geometry_srid);
      filter.setGeometry(std::move(geom));
    }

    dataset_item.setFilter(filter);

    std::map< std::string, std::string > metadata;

    for(int j = 0; j < struct_dataset_items.at(i).metadata_keys.size(); j++)
    {
      metadata[struct_dataset_items.at(i).metadata_keys.at(j)] = struct_dataset_items.at(i).metadata_values.at(j);
    }

    dataset_item.setMetadata(metadata);

    dataset_items.push_back(dataset_item);
  }

  return dataset_items;
}

#endif // __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
