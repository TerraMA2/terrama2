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
  \file terrama2/impl/DataAccessorTxtFile.hpp

  \brief

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_TXT_FILE_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_TXT_FILE_HPP__

//TerraMA2
#include "DataAccessorFile.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorTxtFile

      \brief Base class for DataAccessor classes that access a text file.

    */
    class DataAccessorTxtFile : public DataAccessorFile
    {
      public:
        DataAccessorTxtFile(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true)
          : DataAccessor(dataProvider, dataSeries, false),
            DataAccessorFile(dataProvider, dataSeries, false) { }

        virtual ~DataAccessorTxtFile() = default;

        virtual std::shared_ptr<te::dt::TimeInstantTZ> readFile(DataSetSeries& series, std::shared_ptr<te::mem::DataSet>& completeDataset, std::shared_ptr<te::da::DataSetTypeConverter>& converter, QFileInfo fileInfo, const std::string& mask, terrama2::core::DataSetPtr dataSet) const override;

        virtual std::string dataSourceType() const override { return "OGR"; }

        static DataAccessorType dataAccessorType(){ return "CSV-generic"; }

        virtual std::string typePrefix() const override { return "CSV:"; }

        static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries);

      protected:
        QFileInfo filterTxt(QFileInfo& fileInfo, QTemporaryFile& tempFile) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_TXT_FILE_HPP__
