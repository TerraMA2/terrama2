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
  \file terrama2/core/data-access/DataAccessorOccurrenceWfp.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_WFP_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_WFP_HPP__

// TerraMA2
#include "DataAccessorFile.hpp"
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessorOccurrence.hpp"

namespace terrama2
{
  namespace core
  {
    struct Filter;
    /*!
      \class DataAccessorOccurrenceWfp
      \brief DataAccessor for the INPE's Wildiland Fire Program - www.inpe.br/queimadas

    */
    class DataAccessorOccurrenceWfp : public DataAccessorOccurrence, public DataAccessorFile
    {
      public:
        DataAccessorOccurrenceWfp(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);
        virtual ~DataAccessorOccurrenceWfp() {}

        static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
        {
          return std::make_shared<DataAccessorOccurrenceWfp>(dataProvider, dataSeries);
        }

        static DataAccessorType dataAccessorType(){ return "OCCURRENCE-wfp"; }

      protected:
        virtual std::string dataSourceType() const override;
        virtual std::string typePrefix() const override;

        virtual void adapt(DataSetPtr dataSet, std::shared_ptr<te::da::DataSetTypeConverter> converter) const override;
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter>, const std::shared_ptr<te::da::DataSetType>&) const override;

        // WFP file may have delayed data that should not be filtered
        virtual bool isValidTimestamp(std::shared_ptr<SynchronizedDataSet> /*dataSet*/,
                                      size_t /*index*/,
                                      const Filter& /*filter*/,
                                      size_t /*dateColumn*/) const override {return true;}

      private:
        //! Name of column with latitude information
        std::string getLatitudePropertyName(DataSetPtr dataSet) const;
        //! Name of column with longitude information
        std::string getLongitudePropertyName(DataSetPtr dataSet) const;
        /*!
          \brief Convert string to TimeInstantTZ.

          \note Format recognized:  YYYY-mm-dd HH:MM:SS"

        */
        te::dt::AbstractData* stringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/,
                                                const std::string& timezone) const;

        //! Convert string to Geometry
        te::dt::AbstractData* stringToPoint(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType, const Srid& srid) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_OCCURRENCE_WFP_HPP__
