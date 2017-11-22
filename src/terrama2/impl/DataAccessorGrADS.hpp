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
  \file terrama2/core/data-access/DataAccessorGrADS.hpp

  \brief Data accessor for GrADS format

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GRADS_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GRADS_HPP__

//TerraMA2
#include <QtCore/QString>
#include "DataAccessorGDAL.hpp"
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessorGrid.hpp"

// STL
#include <map>
#include <string>

namespace terrama2
{
  namespace core
  {
    struct Filter;


    class GrADSDataDescriptor
    {
      public:
        enum DimensionType
        {
          LINEAR,
          LEVELS,
          GAUST62,
          GAUSR15,
          GAUSR20,
          GAUSR30,
          GAUSR40
        };

        struct ValueDef
        {
            int numValues_;
            DimensionType dimensionType_;
            std::vector<double> values_;

            ValueDef() : numValues_(0)
            {}
        };


        struct TValueDef
        {
            int numValues_;
            DimensionType dimensionType_;
            std::vector<std::string> values_;

            TValueDef() : numValues_(0), dimensionType_(LINEAR)
            {}
        };

        struct Var
        {
            std::string varName_;
            int verticalLevels_;
            std::string additionalCode_;
            std::string units_;
            std::string description_;

            Var() : verticalLevels_(0)
            {}
        };

        struct CHSUB
        {
            int t1_;
            int t2_;
            std::string str_;
        };

        std::string datasetFilename_;
        std::string title_;
        std::vector<std::string> vecOptions_;
        std::vector<CHSUB*> vecCHSUB_;
        double undef_;
        int fileHeaderLength_;
        ValueDef* xDef_;
        ValueDef* yDef_;
        ValueDef* zDef_;
        TValueDef* tDef_;
        std::vector<std::unique_ptr<Var> > vecVars_;
        Srid srid_;
        uint32_t numberOfBands_;
        double valueMultiplier_;

        GrADSDataDescriptor();

        ~GrADSDataDescriptor();

        GrADSDataDescriptor(const GrADSDataDescriptor& rhs);

        GrADSDataDescriptor& operator=(const GrADSDataDescriptor& rhs);

        void addVar(const std::string& strVar);

        void setKeyValue(const std::string& key, const std::string& value);

      protected:

        TValueDef* getTValueDef(const std::string& value);

        ValueDef* getValueDef(const std::string& value, const std::string& dimension);

    };

    /*!
      \brief DataAccessor for GRID DataSeries in GrADS format.
    */
    class DataAccessorGrADS : public DataAccessorGDAL
    {
      public:

        //! Constructor
        DataAccessorGrADS(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, const bool checkSemantics = true);

        //! Default destructor.
        virtual ~DataAccessorGrADS() = default;

        //! Default copy constructor
        DataAccessorGrADS(const DataAccessorGrADS& other) = default;

        //! Default move constructor
        DataAccessorGrADS(DataAccessorGrADS&& other) = default;

        //! Default const assignment operator
        DataAccessorGrADS& operator=(const DataAccessorGrADS& other) = default;

        //! Default assignment operator
        DataAccessorGrADS& operator=(DataAccessorGrADS&& other) = default;

        /*
         \brief Retrieves GrADS CTL and reads the mask from the CTL to retrieve the data files.
         \param dataRetriever Smart pointer to the data retriever for the given dataset.
         \param dataset The DataSet to be retrieved.
         \param filter The filter to be applied.
         \return The URI to retrieved data.
        */
        virtual std::string
        retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataSet, const Filter& filter,
                     std::shared_ptr<FileRemover> remover) const override;

        virtual void retrieveDataCallback(const DataRetrieverPtr dataRetriever,
                                          DataSetPtr dataSet,
                                          const Filter& filter,
                                          std::shared_ptr<FileRemover> remover,
                                          std::function<void(const std::string& /*uri*/)> processFile) const override;
        /*
         \brief Returns a smart pointer to a DataAccessorGrADS.
         \param dataProvider The data provider to be accessed.
         \param dataSeries The data series to be accessed.
         \param filter The filter to be applied.
         \return Smart pointer to the DataAccessorGrADS.
        */
        inline static DataAccessorPtr
        make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
        {
          return std::make_shared<DataAccessorGrADS>(dataProvider, dataSeries);
        }

        //! Returns the data accessor type.
        static DataAccessorType dataAccessorType()
        { return "GRID-grads"; }

        //! Concatenate the given dataset to the complete dataset.
        virtual void addToCompleteDataSet(DataSetPtr dataSet,
                                          std::shared_ptr<te::mem::DataSet> completeDataSet,
                                          std::shared_ptr<te::da::DataSet> teDataSet,
                                          std::shared_ptr<te::dt::TimeInstantTZ> fileTimestamp,
                                          const std::string& filename) const override;

        QString grad2TerramaMask(QString qString) const;

        GrADSDataDescriptor readDataDescriptor(const std::string& filename) const;

        bool hasControlFile() const override;

        bool needToOpenConfigFile() const override;

        std::string getConfigFilename(terrama2::core::DataSetPtr dataSet, const std::string& binaryFilename) const override;

        std::string getControlMask(DataSetPtr dataSet) const;

        //Returns the file mask .
        std::string getControlFileMask(DataSetPtr dataSet) const;

        //Returns the folder mask .
        std::string getControlFileFolderMask(DataSetPtr dataSet) const;

        std::string readControlFileBinaryMask(terrama2::core::DataSetPtr dataSet, const std::string& controlFilename) const;

        std::string extractBinaryFileMaskFromControlFile(terrama2::core::DataSetPtr dataSet,
                                                         const std::string& controlFilename) const;

        std::string extractBinaryFolderPathFromControlFile(terrama2::core::DataSetPtr dataSet,
                                                           const std::string& controlFilename) const;

        std::string getBinaryMask(DataSetPtr dataset) const;

        std::string getBinaryFileMask(terrama2::core::DataSetPtr dataset) const;

        std::string getBinaryFolderMask(terrama2::core::DataSetPtr dataset) const;

        uint32_t getBytesBefore(DataSetPtr dataset) const;

        uint32_t getBytesAfter(DataSetPtr dataset) const;

        uint32_t getNumberOfBands(DataSetPtr dataset) const;

        double getValueMultiplier(DataSetPtr dataset) const;

        std::string getDataType(DataSetPtr dataset) const;

        std::shared_ptr<te::mem::DataSet> generateDataSet(const std::string& uri,
                                                          const terrama2::core::Filter& filter,
                                                          terrama2::core::DataSetPtr dataSet,
                                                          std::shared_ptr<terrama2::core::FileRemover> remover,
                                                          const std::string& timezone,
                                                          DataSetSeries& series,
                                                          std::shared_ptr< te::dt::TimeInstantTZ >& lastFileTimestamp) const override;

      protected:
        //! Returns the data source type.
        virtual std::string dataSourceType() const override;

        //! Multiply the raster for a factor
        void checkRasterMultiply(DataSetPtr dataSet, std::unique_ptr<te::rst::Raster>& raster) const;

        void writeVRTFile(GrADSDataDescriptor descriptor, const std::string& binFilename,
                          const std::string& vrtFilename, DataSetPtr dataset) const;

        void invertRaster(std::unique_ptr<te::rst::Raster>& raster) const;

        mutable bool yReverse_ = false; //! Flag for reverse y-axis.
        mutable GrADSDataDescriptor gradsDescriptor_;
    };


    std::string trim(const std::string& value);
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_GRADS_HPP__
