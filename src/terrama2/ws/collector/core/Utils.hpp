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

#include "../../../core/DataProvider.hpp"
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

        //template<typename T1, typename T2> T2 Struct2DataSet(T1);
        //template<typename T1, typename T2> T2 DataSet2Struct(T1);

      }
    }
  }
}

template <typename T1, typename T2>
T2 terrama2::ws::collector::core::Struct2DataProvider(T1 struct_dataprovider)
{
  // VINICIUS: check if a DataProvider constructor that receives (id, name, kind) was implemented
/*
  T2 dataprovider(struct_dataprovider.name, struct_dataprovider.kind, struct_dataprovider.id);
  dataprovider.setDescription(struct_dataprovider.description);
  dataprovider.setUri(struct_dataprovider.uri);
  dataprovider.setStatus(struct_dataprovider.status);

  return dataprovider;
*/
}

template<typename T1, typename T2>
T2 terrama2::ws::collector::core::DataProviderPtr2Struct(T1 dataproviderPtr)
{

  T2 struct_dataprovider = T2{
      dataproviderPtr->id(),
      dataproviderPtr->name(),
      dataproviderPtr->description(),
      dataproviderPtr->kind(),
      dataproviderPtr->uri(),
      dataproviderPtr->status()
};

  return struct_dataprovider;
}

// VINICIUS:
/*
template <typename T1, typename T2>
T2 terrama2::ws::collector::core::Struct2DataSet(T1 st_ds)
{

  terrama2::core::DataProvider dp();
  WebService::findDataProvider(data_set.data_provider_id, dp);

  terrama2::core::DataSet ds(dp, data_set.id, data_set.name, data_set.kind);
  ds.setDescription(data_set.description);
  ds.setStatus(data_set.status);
  ds.setDataFrequency(data_set.data_frequency);
  ds.setSchedule(data_set.schedule);
  ds.setScheduleRetry(data_set.schedule_retry);
  ds.scheduleTimeout(data_set.schedule_timeout);

}
*/

/*
template <typename T1, typename T2>
T2 terrama2::ws::collector::core::DataSet2Struct(T1 ds)
{

  terrama2::core::DataProvider dp;

  data_set.dataProvider();
  DataSet ds;

  ds.data_provider_id = ;
  ds.id = ;
  ds.kind = ;
  ds.description = ;
  ds.status = ;
  ds.data_frequency = ;
  ds.schedule = ;
  ds.shedule_retry = ;
  ds.schedule_timeout = ;

}
*/
#endif // __TERRAMA2_WS_COLLECTOR_CORE_UTILS_HPP__
