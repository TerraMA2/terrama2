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
  \file terrama2/core/data-access/DataStoragerTable.hpp

  \brief Base class for table dataset

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_TABLE_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_TABLE_HPP__

//TerraMA2
#include "../core/data-access/DataStorager.hpp"

namespace terrama2
{
  namespace core
  {
    class DataStoragerTable : public DataStorager
    {
      public:
        DataStoragerTable(DataSeriesPtr dataSeries, DataProviderPtr outputDataProvider)
                : DataStorager(dataSeries, outputDataProvider) {}
        ~DataStoragerTable() {}

        virtual void store(DataSetSeries series, DataSetPtr outputDataSet) const override;

      protected:
        //! Return the dataset name
        virtual std::string getDataSetName(DataSetPtr dataSet) const = 0;
        virtual std::string getGeometryPropertyName(DataSetPtr dataSet) const;
        /*!
           \brief Check if the two properties have same name and type.
           \exception DataStoragerException Raise if have the same name and different types
        */
        bool isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember) const;

      protected:
        std::unique_ptr<te::dt::Property> copyProperty(te::dt::Property* property) const;
        std::shared_ptr<te::da::DataSetType> copyDataSetType(std::shared_ptr<te::da::DataSetType> dataSetType, const std::string& newDataSetName) const;
        void updateAttributeNames(std::shared_ptr<te::mem::DataSet> dataSet, std::shared_ptr<te::da::DataSetType> dataSetType, DataSetPtr dataset) const;
        virtual std::string driver() const = 0;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_TABLE_HPP__
