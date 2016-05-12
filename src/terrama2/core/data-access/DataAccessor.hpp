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
  \file terrama2/core/data-access/DataAccessor.hpp

  \brief Base class to access data from a DataSeries.

  \author Jano Simas
  \author Evandro Delatin
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__

//TerraMA2
#include "../../Config.hpp"

#include "../Shared.hpp"

#include "DataRetriever.hpp"
#include "Series.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/Filter.hpp"

//TerraLib
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/memory/DataSet.h>

namespace te
{
  namespace da
  {
    class DataSet;
  } /* da */
} /* te */

namespace terrama2
{
  namespace core
  {
    /*!
    \class DataAccessor
    \brief Base class to access data from a DataSeries.

    Derived classes as responsible for the whole data access process,
    from downloading, when necessary, to accessing and filtering the raw data.

    Derived classes should also be able read the format data from the dataset format map.

    \warning Derived classes should have virtual inheritance.

    */
    class DataAccessor
    {
      public:
        //! Returns the last data timestamp found on last access.
        virtual std::shared_ptr< te::dt::TimeInstantTZ > lastDateTime() const {return lastDateTime_; }
        //! Returns the semantics of the DataSeries.
        DataSeriesSemantics semantics() const { return dataSeries_->semantics; }
        /*!
          \brief Get access to the filtered data of a DataSeries

          This method will return a Series per DataSet of the DataSeries.

          In case the data is in a remote file server it will be downloaded, unpacked if the case, and accessed via TerrLib driver.
          Any temporary folder will be removed in the process.

          The data will be converted by the data type driver based on the DataSeriesSemantics of the DataSeries.

          \param filter Filter data applied to accessed data, if empty, all data is returned.
        */
        virtual std::map<DataSetPtr, Series > getSeries(const Filter& filter) const;

        //! Utility function for converting string to double in the te::da::DataSet contruction.
        te::dt::AbstractData* stringToDouble(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const;

        //! Utility function for converting string to int32 in the te::da::DataSet contruction.
        te::dt::AbstractData* stringToInt(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const;

        //! Default destructor.
        virtual ~DataAccessor() {}

      protected:

        /*!
          \brief Base class nad interface for accesing data.

          Each derived implementation must deal with protocol, format and data semantics.

          \param filter If defined creates a cache for the filtered data.//TODO: no implemented DataAccessor cache
        */
        DataAccessor(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, Filter filter = Filter());

        /*!
           \brief Prefix especification for drivers.

           Some drivers may need especification to access a datasource,
           GDal, for example, need 'CSV:' befor the uri for csv files with txt extension.

         */
        virtual std::string typePrefix() const { return ""; }

        /*!
           \brief Driver that must be used to access a DataSource
         */
        virtual std::string dataSourceType() const = 0;

        /*!
           \brief Creates a converter to make the necessary convertions of data type.

           This converter will convert strings to Data/Time formats or double,
           create coordinate point classe from lat long string and other cases

           \param dataSet Raw dataset
           \param datasetType DataSetType from dataSet
           \return Converter
         */
        virtual std::shared_ptr<te::da::DataSetTypeConverter> getConverter(DataSetPtr dataSet, const std::shared_ptr<te::da::DataSetType>& datasetType) const;

        /*!
           \brief Add original attributes to the converter without convertion
         */
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const;

        /*!
           \brief Add addapted attributes to the converter
         */
        virtual void adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const { }

        /*!
           \brief Retrieve data from server.

           Retrieved data is subjetc to filter.

         */
        virtual std::string retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataSet, const Filter& filter) const = 0;

        /*!
           \brief Get a memory dataset do core::DataSet.
           \param uri Uri to the dataset storage
           \param filter Filter applyed to the dataset
           \note Updates lastDateTime
           \return Filtered dataset
         */
        virtual Series getSeries(const std::string& uri, const Filter& filter, DataSetPtr dataSet) const = 0;

        /*!
          \brief Verifies if the DataSet intersects the Filter area.

          Default behavior is true.
        */
        virtual bool intersects(DataSetPtr dataset, const Filter& filter) const { return true; }

        DataProviderPtr dataProvider_;//!< DataProvider with iformation of the server where the data is stored.
        DataSeriesPtr dataSeries_;//!< DataSeries with the DataSet list with data iformation.
        Filter filter_;//! Filter applied to accessed data.

        std::shared_ptr< te::dt::TimeInstantTZ > lastDateTime_;//!< Last data Date/Time
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__
