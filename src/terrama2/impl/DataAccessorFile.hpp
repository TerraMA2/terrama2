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
  \file terrama2/core/data-access/DataAccessorFile.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FILE_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FILE_HPP__

//TerraMA2
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessor.hpp"
#include "../core/data-model/DataSet.hpp"
#include "../core/data-model/Filter.hpp"

// Qt
#include <QFileInfoList>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorFile

      \brief Base class for DataAccessor classes that access a file.

    */
    class DataAccessorFile : public virtual DataAccessor
    {
      public:
        DataAccessorFile(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true)
          : DataAccessor(dataProvider, dataSeries, false)
        {}
        virtual ~DataAccessorFile() = default;

        using terrama2::core::DataAccessor::getSeries;
        // Doc in base class
        virtual std::string retrieveData(const DataRetrieverPtr dataRetriever,
                                         DataSetPtr dataset,
                                         const Filter& filter,
                                         std::shared_ptr<terrama2::core::FileRemover> remover) const override;
        // Doc in base class
        virtual DataSetSeries getSeries(const std::string& uri, const Filter& filter, DataSetPtr dataSet, std::shared_ptr<terrama2::core::FileRemover> remover) const override;

        //! Recover file mask
        virtual std::string getMask(DataSetPtr dataset) const;

        virtual QFileInfoList getFoldersList(const QFileInfoList& uris, const std::string& foldersMask) const;

        /*!
         * \brief Search in a folder and return a list of files that match the mask and filter
         * \param folderURI The folder path to do the search
         * \param mask The files mask
         * \param timezone Timezone of the data
         * \param filter DataSet Filter
         * \param remover
         * \return A QFileInfoList with  all files that match the mask and filter
         */
        static QFileInfoList getDataFileInfoList(const std::string& uri,
                                                  const std::string& mask,
                                                  const std::string& timezone,
                                                  const Filter& filter,
                                                  std::shared_ptr<terrama2::core::FileRemover> remover);

      protected:
        virtual std::shared_ptr<te::mem::DataSet> createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const;
        virtual std::shared_ptr<te::mem::DataSet> internalCreateCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType, bool enableFileName, bool enableFileTimestamp) const final;
        virtual void addToCompleteDataSet(std::shared_ptr<te::mem::DataSet> completeDataSet,
                                          std::shared_ptr<te::da::DataSet> dataSet, std::shared_ptr< te::dt::TimeInstantTZ >, const std::string&) const;
        virtual std::shared_ptr<te::da::DataSet> getTerraLibDataSet(std::shared_ptr<te::da::DataSourceTransactor> transactor, const std::string& dataSetName, std::shared_ptr<te::da::DataSetTypeConverter> converter) const;

        /*!
          \brief Filter dataset based on Filter
        */
        virtual void filterDataSet(std::shared_ptr<te::mem::DataSet> completeDataSet, const Filter& filter) const;

        /*!
          \brief Crop raster with the geometry filter.
        */
        void cropRaster(std::shared_ptr<te::mem::DataSet> completeDataSet, const Filter& filter) const;

        /*!
          \brief Filter the dataset by the last timestamp found.

          \param lastTimestamp Last timestamp found in dataset.
        */
        void filterDataSetByLastValue(std::shared_ptr<te::mem::DataSet> completeDataSet,
                                      const Filter& filter,
                                      std::shared_ptr<te::dt::TimeInstantTZ> lastTimestamp) const;

        /*!
          \brief Filter dataset by timestamp range

          This method will check if the DateTime in the dateColumn is greater then
           Filter discardBefore attribute and lesser than the discardAfter.
           If they are not set, will return true.

          will automatically return true if
           - no date/time column is found
           - Filter has no discardBefore AND no discardAfter set
           - DateTime attribute is null (will be logged)

        */
        virtual bool isValidTimestamp(std::shared_ptr<te::mem::DataSet> dataSet, const Filter& filter, size_t dateColumn) const;
        /*!
          \brief Filter dataset by geometry

          This method will check if the Geometry in the geomColumn intersects the
          Filter region attribute.

          will automatically return true if
           - no geometry column is found
           - Filter has no region set
           - Geometry attribute is null (will be logged)

        */
        virtual bool isValidGeometry(std::shared_ptr<te::mem::DataSet> dataSet, const Filter&  filter, size_t geomColumn) const;

        /*!
          \brief Filter dataset by raster envelope

          This method will check if the raster in the rasterColumn intersects the
          Filter region attribute.

          will automatically return true if
           - no geometry column is found
           - Filter has no region set
           - Raster attribute is null (will be logged)

        */
        virtual bool isValidRaster(std::shared_ptr<te::mem::DataSet> dataSet, const Filter&  filter, size_t rasterColumn) const;

        std::shared_ptr< te::dt::TimeInstantTZ > getDataLastTimestamp(DataSetPtr dataSet, std::shared_ptr<te::da::DataSet> teDataSet) const;

    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FILE_HPP__
