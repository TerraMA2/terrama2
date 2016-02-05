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
  \file terrama2/core/dao/DataSetDAO.hpp

  \brief Persistence layer for datasets.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_DAO_DATASETDAO_HPP__
#define __TERRAMA2_CORE_DAO_DATASETDAO_HPP__

// TerraMA2
#include "../DataSet.hpp"

// STL
#include <vector>

// Forward declaration
namespace te
{
  namespace da
  {
    class DataSourceTransactor;
    class DataSet;
  }
}

namespace terrama2
{
  namespace core
  {
    namespace dao
    {
      /*!
        \class DataSetDAO

        \brief Persistence layer for datasets.
       */
      class DataSetDAO
      {
        public:

          /*!
            \brief Insert the given dataset in the database.

            \param dataset     The dataset to be inserted into the database.
            \param transactor  The data source transactor to be used to perform the insert operation.
            \param shallow     If true it will save only the dataset attributes into the database. Dataset items will not be saved.

            \pre The dataset must be associated to a valid data provider.
            \pre The dataset must have an identifier equals to 0 (considered invalid).

            \pos On success the inserted dataset will have a valid identifier (different from 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void save(DataSet& dataset,
                           te::da::DataSourceTransactor& transactor,
                           const bool shallow = true);

          /*!
            \brief Update the dataset information in the database.

            \param dataset     The dataset to be updated.
            \param transactor  The data source transactor to be used to perform the update operation.
            \param shallowSave If true it will update only the dataset attributes into the database. Dataset items will not be updated.

            \pre The dataset must be associated to a valid data provider.
            \pre The dataset must have a valid identifier (a value different from 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void update(DataSet& dataset, te::da::DataSourceTransactor& transactor, const bool shallow = true);

          /*!
            \brief Removes the dataset from the database.

            \param id         The dataset to be removed from the database.
            \param transactor The data source transactor to be used to perform the delete operation.

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void remove(uint64_t id, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Load the informed dataset.

            \param id         A dataset identifier.
            \param transactor The data source transactor to be used to perform the delete operation.

            \pre The identifier must be valid identifier (a value different than 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static DataSet load(uint64_t id, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Load the list of datasets for the given data provider.

            \param provider    The data provider to load its datasets.
            \param transactor  The data source transactor to be used to perform the delete operation.

            \pre The data provider must have a valid identifier (a value different than 0).

            \pos If the load succeed, the datasets will be added to the provider.

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static std::vector<DataSet>
          loadAll(uint64_t providerId, te::da::DataSourceTransactor& transactor);

        private:

          //! Not instantiable.
          DataSetDAO();

          //! Not instantiable.
          ~DataSetDAO();


          /*!
            \brief Load the metadata for the given dataset.

            \param dataSet    The dataset to load its metadata.
            \param transactor  The data source transactor to be used to perform the delete operation.

            \pre The data provider must have a valid identifier (a value different than 0).

            \pos If the load succeed, the collect rules will be added to the dataset.

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void loadMetadata(DataSet& dataset, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Insert the metadata of the given dataset in the database.

            \param dataset     The dataset with the metadata to be inserted into the database.
            \param transactor  The data source transactor to be used to perform the insert operation.

            \pre The dataset must have a valid identifier (a value different than 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void saveMetadata(DataSet& dataSet, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Update the metadata of the given dataset in the database.

            \param dataset     The dataset with the metadata to be updated into the database.
            \param transactor  The data source transactor to be used to perform the insert operation.

            \pre The dataset must have a valid identifier (a value different than 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void updateMetadata(DataSet& dataset, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Removes the metadata of a dataset from the database.

            \param id         The identifier of the dataset.
            \param transactor The data source transactor to be used to perform the delete operation.

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void removeMetadata(uint64_t id, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Reads all columns from the current row of the query result and creates a dataset.

            \param queryResult The terralib dataset with the query result.
            \param transactor  The data source transactor to be used to perform the insert operation.

            \return The dataset created for the current row of the query result.

           */
          static DataSet getDataSet(std::shared_ptr<te::da::DataSet> queryResult, te::da::DataSourceTransactor& transactor);
      };
    } // end namespace dao
  }   // end namespace core
}     // end namespace terrama2

#endif // __TERRAMA2_CORE_DAO_DATASETDAO_HPP__
