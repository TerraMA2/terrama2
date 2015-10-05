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
  \file terrama2/core/DataSetDAO.hpp

  \brief DataProvider DAO...

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATASETDAO_HPP__
#define __TERRAMA2_CORE_DATASETDAO_HPP__

// TerraMA2
#include "DataSet.hpp"
#include "Filter.hpp"

// STL
#include <vector>
#include <memory>

namespace te
{
  namespace da
  {
    class DataSource;
    class DataSourceTransactor;
  }
}

namespace terrama2
{
  namespace core
  {
    class DataSetDAO
    {
      public:

        /*!
          \brief Persists a given dataset in the database, the generated id will be set in the dataset.
          In case the data provider identifier is 0, it will also save the data provider.         *

          \param dataset Dataset to save.
          \param transactor Data source transactor.

          \exception InvalidDataSetError, InvalidDataSetIdError
         */
        static void save(DataSetPtr dataset, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Update a given dataset in the database.
          The identifier of the dataset must be valid.

          \param dataset Dataset to update.
          \param transactor Data source transactor.

          \exception InvalidDataSetError, InvalidDataSetIdError
         */
        static void update(DataSetPtr dataset, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Removes the dataset with the given id.
          The identifier of the dataset must be valid.
          In case there is an analysis configured to use this dataset, the dataset will not be removed.

          \param id Identifier of the dataset.
          \param transactor Data source transactor.

          \exception InvalidDataSetIdError, DataSetInUseError
         */
        static void remove(uint64_t id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Search for a dataset with the given id
          In case none is found it will return an empty smart pointer.

          \param id Identifier of the dataset.
          \param transactor Data source transactor.
          \return A smart pointer to the dataset.

          \exception InvalidDataSetIdError
         */
        static DataSetPtr find(uint64_t id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieve all datasets from the database.
          In case none is found it will return an empty vector.

          \param transactor Data source transactor.
          \return Vector with all datasets.
         */
        static std::vector<DataSetPtr> list(te::da::DataSourceTransactor& transactor);

      private:

        /*!
          \brief Retrieve the list of collect rules of a given dataset.

          \param dataSetId Identifier of the dataset.
          \param transactor Data source transactor.
          \return The list of collect rules.
         */
        static std::vector<terrama2::core::DataSet::CollectRule> getCollectRules(uint64_t dataSetId, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Persists the collect rules of a given dataset.

           \param dataSet Dataset with the collect rules.
           \param transactor Data source transactor.
         */
        static void saveCollectRules(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieve a map with the metadata for a given dataset.

          In case there is no metadata, it will return an empty map.

          \param dataSetId Identifier of the dataset.
          \param transactor Data source transactor.
          \return The map with the dataset metadata.
         */
        static std::map<std::string, std::string> getMetadata(uint64_t dataSetId, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Persists the collect rules of a given dataset.
           \param dataSet Dataset with the metadata.
           \param transactor Data source transactor.
         */
        static void saveMetadata(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieve a vector of dataset item from a given dataset ID.

          In case there is no dataset item, it will return an empty vector.

          \param dataSetId Identifier of the dataset.
          \param transactor Data source transactor.
          \return The vector of data.
         */
        static std::vector<terrama2::core::DataSetItemPtr> getDataSetItemList(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Persists the list of dataset item from a given dataset.
           \param dataSetId Identifier of the dataset.
           \param dataList List with the data to persist.
           \param transactor Data source transactor.
         */
        static void saveDataSetItemList(const int64_t dataSetId, const std::vector<DataSetItemPtr>& dataList, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Updates the list of dataset item from a given dataset.
           \param dataSet The dataset to update.
           \param transactor Data source transactor.
         */
        static void updateDataSetItemList(terrama2::core::DataSetPtr dataSet, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieve the filter from a given dataset ID.

          \param dataSetItem The dataset item.
          \param transactor Data source transactor.
          \return The filter from the dataset item.
         */
        static terrama2::core::FilterPtr getFilter(terrama2::core::DataSetItemPtr dataSetItem, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Persists the given filter.
           \param filter The filter to persist.
           \param transactor Data source transactor.
         */
        static void saveFilter(const uint64_t dataSetItemId, terrama2::core::FilterPtr filter, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Updates the given filter.
           \param filter The filter to update.
           \param transactor Data source transactor.
         */
        static void updateFilter(terrama2::core::FilterPtr filter, te::da::DataSourceTransactor& transactor);


        /*!
          \brief Retrieve a the storageg strategy metadata from a given dataset ID.

          \param dataSetItemId The dataset item identifier.
          \param transactor Data source transactor.
          \return The map with the storage strategy metadata.
         */
        static std::map<std::string, std::string> getStorageMetadata(const uint64_t dataSetItemId, te::da::DataSourceTransactor& transactor);

        /*!
           \brief Persists the storage strategy metadata of a dataset item.
           \param dataSetItemId The dataset item identifier.
           \param storageMetadata The map with the storage strategy metadata.
           \param transactor Data source transactor.
         */
        static void saveStorageMetadata(const uint64_t dataSetItemId, const std::map<std::string, std::string>& storageMetadata, te::da::DataSourceTransactor& transactor);


        //! Not instantiable.
        DataSetDAO();

        //! Not instantiable.
        ~DataSetDAO();
    };

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_DATASETDAO_HPP__
