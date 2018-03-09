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
#include "../Config.hpp"
#include "../Shared.hpp"
#include "DataRetriever.hpp"
#include "DataSetSeries.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/Filter.hpp"
#include "../utility/FileRemover.hpp"

//TerraLib
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/memory/DataSet.h>

#include <functional>

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
    typedef std::string DataAccessorType;
    /*!
    \class DataAccessor
    \brief DataAccessor provides an interface for accesing the data from a DataSeries.

    ## Accessing data ##

    Once you get a DataAccessor the getSeries() will return a map of
    DataSet and DataSetSeries, that will offer direct access to the data
    through a [Terralib DataSet](http://www.dpi.inpe.br/terralib5/wiki/doku.php?id=wiki:designimplementation:dataaccess&s[]=dataset#dataset).

    Once the getSeries() is called the latest timestamp from the data found in the file is stored
    and can be retrieved by lastDateTime().

    \warning lastDateTime() will return the latest data timestamp of the last time getSeries()
    was called, **not the latest from sequential calls**.

    \note The best way to get a DataAccessor is from a DataAccessorFactory,
    the DataAccessorFactory::make will return a DataAccessor from the right type.

    ## Derived classes ##

    Derived classes are responsible for the whole data access process,
    from downloading, when necessary, to accessing and filtering the raw data.

    Derived classes should also be able read the format data from the dataset format map.

    \warning Derived classes must have virtual inheritance.

    */
    class TMCOREEXPORT DataAccessor
    {
      public:
        /*!
          \brief Returns the last data timestamp found on last access.

          \sa getSeries()
        */
        virtual std::shared_ptr< te::dt::TimeInstantTZ > lastDateTime() const {return lastDateTime_; }

        //! Returns the DataSeriesSemantics of the DataSeries.
        DataSeriesSemantics semantics() const { return dataSeries_->semantics; }

        /*!
          \brief Get access to the filtered data of a DataSeries

          This method will return a map of DataSetSeries per DataSet of the DataSeries.

          If the data is in a remote file server, it will be downloaded and unpacked if the case, and accessed via TerraLib driver.
          Any temporary folder will be removed after the process.

          \note The data will be converted by the data type driver based on the DataSeriesSemantics of the DataSeries.

          \param filter Filter data applied to accessed data, if empty, all data is returned.

          \exception DataProviderException Raised if internal DataProvider isn't active.
          \exception DataProviderException Raised if the Filter date result in an empty time range.
        */
        virtual std::unordered_map<DataSetPtr,DataSetSeries > getSeries(const Filter& filter, std::shared_ptr<FileRemover> remover) const;
        virtual std::unordered_map<DataSetPtr,DataSetSeries > getSeries(const std::map<DataSetId, std::string> uriMap, const Filter& filter, std::shared_ptr<FileRemover> remover) const;

        std::map<DataSetId, std::string> getFiles(const Filter& filter, std::shared_ptr<FileRemover> remover) const;
        void getSeriesCallback(const Filter& filter, std::shared_ptr<FileRemover> remover, std::function<void(const DataSetId&, const std::string& /*uri*/)> processFile) const;

        //! Utility function for converting string to double in the te::da::DataSet construction.
        te::dt::AbstractData* stringToDouble(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const;

        //! Utility function for converting string to int32 in the te::da::DataSet construction.
        te::dt::AbstractData* stringToInt(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/) const;

        //! Recover projection information from dataset
        Srid getSrid(DataSetPtr dataSet, bool logErrors = true) const;

        //! Default destructor.
        virtual ~DataAccessor() = default;
        //! Default copy constructor
        DataAccessor(const DataAccessor& other) = default;
        //! Default move constructor
        DataAccessor(DataAccessor&& other) = default;
        //! Default const assignment operator
        virtual DataAccessor& operator=(const DataAccessor& other) = default;
        //! Default assignment operator
        virtual DataAccessor& operator=(DataAccessor&& other) = default;

        /*!
          \brief Get name of the timestamp property

          The name of the property may come from the semantics metadata or the dataset format,
          the semantics has priority.
        */
        virtual std::string getTimestampPropertyName(DataSetPtr dataSet, const bool logErrors = true) const;

        virtual std::string getInputTimestampPropertyName(DataSetPtr dataSet) const;

        /*!
          \brief Get name of the geometry property

          The name of the property may come from the semantics metadata or the dataset format,
          the semantics has priority.
        */
        virtual std::string getGeometryPropertyName(DataSetPtr dataSet) const;

        /*!
          \brief Get name of the output geometry property

          The name of the output property come from the semantics metadata or the dataset format.
        */
        virtual std::string getInputGeometryPropertyName(DataSetPtr dataSet) const;

        //! Recover timezone information from dataset
        virtual std::string getTimeZone(DataSetPtr dataSet, bool logErrors = true) const;

      protected:

        /*!
          \brief Constructor

          \exception DataAccessorException Raised if any of the parameters is NULL.
        */
        DataAccessor(DataProviderPtr dataProvider, DataSeriesPtr dataSeries);

        /*!
           \brief Prefix specification for drivers.

           Some drivers may need specification to access a datasource,
           GDAL, for example, need 'CSV:' before the uri for csv files with //txt// extension.

         */
        virtual std::string typePrefix() const { return ""; }

        /*!
           \brief Driver that must be used to access a DataSource
         */
        virtual std::string dataSourceType() const = 0;

        /*!
           \brief Creates a converter to make the necessary transformation of data type.

           This converter will convert strings to Data/Time formats or double,
           create coordinate point class from lat long string and other cases

           \param dataSet Raw dataset
           \param datasetType DataSetType from dataSet
           \return Converter
         */
        virtual std::shared_ptr<te::da::DataSetTypeConverter> getConverter(DataSetPtr dataSet, const std::shared_ptr<te::da::DataSetType>& datasetType) const;

        /*!
           \brief Add original attributes to the converter without converting
         */
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const;

        /*!
           \brief Add adapted attributes to the converter
         */
        virtual void adapt(DataSetPtr /*dataSet*/, std::shared_ptr<te::da::DataSetTypeConverter> /*converter*/) const { }

        /*!
           \brief Retrieve data from server.

           Retrieved data is subject to filter.

         */
        virtual std::string retrieveData(const DataRetrieverPtr dataRetriever,
                                         DataSetPtr dataSet, const Filter& filter, std::shared_ptr<FileRemover> remover) const = 0;

     /*!
        \brief Retrieve data from server.

        Retrieved data is subject to filter.
        The processFile callback takes the uri of the temporary folder as a parameter
      */
        virtual void retrieveDataCallback(const DataRetrieverPtr dataRetriever,
                                          DataSetPtr dataset,
                                          const Filter& filter,
                                          std::shared_ptr<FileRemover> remover,
                                          std::function<void(const std::string& /*uri*/)> processFile) const = 0;

        /*!
           \brief Get a memory dataset do core::DataSet.
           \param uri Uri to the dataset storage
           \param filter Filter applyed to the dataset
           \note Updates lastDateTime
           \return Filtered dataset
         */
        virtual DataSetSeries getSeries(const std::string& uri, const Filter& filter, DataSetPtr dataSet, std::shared_ptr<terrama2::core::FileRemover> remover) const = 0;

        /*!
          \brief Verifies if the DataSet intersects the Filter area.

          Default behavior is true.
        */
        virtual bool intersects(DataSetPtr /*dataset*/, const Filter& /*filter*/) const { return true; }

        DataProviderPtr dataProvider_;//!< DataProvider with information of the server where the data is stored.
        DataSeriesPtr dataSeries_;//!< DataSeries with the DataSet list with data information.

        std::shared_ptr< te::dt::TimeInstantTZ > lastDateTime_;//!< Last data Date/Time
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_HPP__
