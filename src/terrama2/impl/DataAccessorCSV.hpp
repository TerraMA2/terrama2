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
  \file terrama2/impl/DataAccessorCSV.hpp

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
      \class DataAccessorCSV

      \brief Base class for DataAccessor classes that access a text file.

    */
    class DataAccessorCSV : public virtual DataAccessorFile
    {
      public:
        DataAccessorCSV(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true)
          : DataAccessor(dataProvider, dataSeries, false),
            DataAccessorFile(dataProvider, dataSeries, false) { }

        virtual ~DataAccessorCSV() = default;

        virtual std::shared_ptr<te::dt::TimeInstantTZ> readFile(DataSetSeries& series, std::shared_ptr<te::mem::DataSet>& completeDataset, std::shared_ptr<te::da::DataSetTypeConverter>& converter, QFileInfo fileInfo, const std::string& mask, terrama2::core::DataSetPtr dataSet) const override;

        virtual std::string dataSourceType() const override { return "OGR"; }

        virtual std::string typePrefix() const override { return "CSV:"; }

        std::map<std::string, int> dataTypes { {"FLOAT", static_cast<int>(te::dt::DOUBLE_TYPE)},
                                               {"INTEGER", static_cast<int>(te::dt::INT32_TYPE)},
                                               {"TEXT", static_cast<int>(te::dt::STRING_TYPE)},
                                               {"DATETIME", static_cast<int>(te::dt::DATETIME_TYPE)},
                                               {"GEOMETRY_POINT", static_cast<int>(te::dt::GEOMETRY_TYPE)}
                                              };

        bool checkOriginFields(std::shared_ptr<te::da::DataSetTypeConverter> converter,
                             const QJsonArray& fieldsArray) const;

        bool checkProperty(te::da::DataSetType* dataSetType, std::string property) const;

      protected:
        QFileInfo filterTxt(QFileInfo& fileInfo, QTemporaryFile& tempFile, DataSetPtr dataSet) const;

        virtual void checkFields(DataSetPtr dataSet) const = 0;

        QJsonArray getFields(DataSetPtr dataSet) const;

        QJsonObject getFieldObj(const QJsonArray& array, const std::string& fieldName, const int position) const;

        te::dt::AbstractData* stringToTimestamp(te::da::DataSet* dataset,
                                                const std::vector<std::size_t>& indexes,
                                                int /*dstType*/,
                                                const std::string& timezone, std::string& dateTimeFormat) const;

        bool getConvertAll(DataSetPtr dataSet) const;

        //! Convert string to Geometry
        te::dt::AbstractData* stringToPoint(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType, const Srid& srid) const;

        virtual void adapt(DataSetPtr dataset, std::shared_ptr<te::da::DataSetTypeConverter> converter) const override;

        const QString json_latitude_property_name = "latitude_property_name";
        const QString json_longitude_property_name = "longitude_property_name";
        const QString json_latitude_property_position = "latitude_property_position";
        const QString json_longitude_property_position = "longitude_property_position";
        const QString json_property_name = "property_name";
        const QString json_property_position = "property_position";
        const QString json_alias = "alias";
        const QString json_type = "type";
        const QString json_header_size = "header_size";
        const QString json_properties_names_line = "properties_names_line";
        const QString json_convert_all = "convert_all";
        const QString json_fields = "fields";
        const QString json_default_type = "default_type";
        const QString json_format = "format";
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_TXT_FILE_HPP__
