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
  \file terrama2/impl/DataAccessorDcpToa5.hpp

  \brief Parser of DCP TOA5 file.

  \author Evandro Delatin
 */

#ifndef __TERRAMA2_IMPL_DATA_ACCESSOR_DCP_TOA5_HPP__
#define __TERRAMA2_IMPL_DATA_ACCESSOR_DCP_TOA5_HPP__

//TerraMA2
#include "DataAccessorFile.hpp"
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessorDcp.hpp"
#include "../core/data-model/DataSet.hpp"
#include "../core/data-model/Filter.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
     * \brief The DataAccessorDcpToa5 class is responsible for making the parser file in the format TOA5.
     *
    */
    class TMIMPLEXPORT DataAccessorDcpToa5 : public DataAccessorDcp, public DataAccessorFile
    {
      public:
        /*!
         * \brief DataAccessorDcpToa5 Constructor.
         * \param dataProvider It contains information of a DataProvider;
         * \param dataSeries It contains a dataset Ex. set of DCP-TOA5.
         * \param filter It contains the filter data.
         */
        DataAccessorDcpToa5(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);

        /*!
         * \brief ~DataAccessorDcpToa5 Default Destructor.
         */
        virtual ~DataAccessorDcpToa5() {}

        static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries);
        static DataAccessorType dataAccessorType(){ return "DCP-toa5"; }

      protected:
        /*!
         * \brief dataSourceType Driver that must be used to access a DataSource.
         * \return Return driver. Ex. OGR.
         */
        virtual std::string dataSourceType() const override;

        /*!
         * \brief typePrefix Prefix especification for drivers.
         * \return Return expecification for drivers. Ex. CSV:.
         */
        virtual std::string typePrefix() const override;

        /*!
         * \brief adapt method is responsible for manually change data from TOA5 file. This method first performs the remove
         *  of the column "TIMESTAMP" of TOA5 file, then creates a new column or property "DateTime" type dt::DateTimeProperty
         * with value te::dt::TIME_INSTANT_TZ.
         * \param dataset It contains description of a dataset;
         * \param converter DataSetTypeConverter is the type that is a pointer to the DataSetType that will be converted manually.
         * After creating "DateTime" method is used "add" to add the new property in the dataset.
         * The add method has the following parameters:
         * \param PropertyPos is the position of the property will be converted;
         * \param DtProperty is converted property;
         * \param StringToTimestamp is the function that will be used to make the conversion of the property values.
         * \note DataSetType A note can be used to describe a set of data. It lists the attributes of the data set,
         *  including their names and types.
         */
        virtual void adapt(DataSetPtr dataset, std::shared_ptr<te::da::DataSetTypeConverter> converter) const override;

        /*!
         * \brief addColumns Add original attributes to the converter without convertion.
         */
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const override;

        /*!
         * \brief getDataSet Get a memory dataset do core::DataSet.
         * \param uri Uri to the dataset;
         * \param filter Filter applyed to the dataset;
         * \param dataSet It contains description of a dataset. Ex. information of DCP-TOA5;
         */
        virtual DataSetSeries getSeries(const std::string& uri, const Filter& filter, DataSetPtr dataSet, std::shared_ptr<terrama2::core::FileRemover> remover) const override;

      private:

        /*!
         * \brief recordColumn Get record column.
         * \return Return Record Column.
         */
        std::string getRecordPropertyName(DataSetPtr dataSet) const;

        /*!
         * \brief stationColumn Get station column.
         * \return Return Station Column.
         */
        std::string getStationPropertyName(DataSetPtr dataSet) const;

        /*!
         * \brief stringToTimestamp is the function is responsible for handling the date and time fields
            * in the format timestamp TOA5 file.
            * \param dataset te::da::DataSet* dataset is input data set;
            * \param indexes std::vector<std::size_t>& indexes is list with the positions of the modified property on the input data set;
            * \param timezone It contains the timezone;
            * \return te::dt::AbstractData* dt is adding a new data type in the system. Ex dateTime type timestamp.
            * \note Convert string to TimeInstantTZ. Format recognized: YYYY-mm-dd HH:MM:SS".
         */
        te::dt::AbstractData* stringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/, const std::string& timezone) const;


        void readTOA5file(const QFileInfo& fileInfo, const std::string& saveUri) const;
    };
  }
}
#endif // __TERRAMA2_IMPL_DATA_ACCESSOR_DCP_TOA5_HPP__
