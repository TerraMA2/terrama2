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
  \file terrama2/core/DataSetItemDAO.hpp

  \brief Persistense layer for dataset items.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_DATASETITEMDAO_HPP__
#define __TERRAMA2_CORE_DATASETITEMDAO_HPP__

// TerraMA2
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "Filter.hpp"

// STL
#include <memory>

// Forward declaration
namespace te { namespace da { class DataSourceTransactor; } }

namespace terrama2
{
  namespace core
  {
    
    /*!
      \class DataSetItemDAO

      \brief Persistense layer for dataset items.
     */
    class DataSetItemDAO
    {
      public:

        /*!
          \brief Insert the given dataset item in the database.

          \param item       The dataset item to be inserted into the database.
          \param transactor The data source transactor to be used to perform the insert operation.

          \pre The dataset item must be associated to a valid dataset.
          \pre The dataset item must have an identifier equals to 0 (considered invalid).

          \pos On success the inserted dataset item will have a valid identifier (different from 0).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void save(DataSetItem& item, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Update the dataset item information in the database.

          \param item       The dataset item to be updated.
          \param transactor The data source transactor to be used to perform the update operation.

          \pre The dataset item must be associated to a valid dataset.
          \pre The identifier of the dataset must be valid (a value different from 0).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void update(DataSetItem& item, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Removes the dataset item from the database.

          \param id         The dataset item to be removed from the database.
          \param transactor The data source transactor to be used to perform the delete operation.

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void remove(uint64_t id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Load the list of dataset items for the given dataset.

          \param dataset    The dataset to load its items.
          \param transactor The data source transactor to be used to perform the delete operation.

          \pre The dataset must have a valid identifier (a value different than 0).

          \pos If the load succeed, the items will be added to the dataset.

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void loadItems(DataSet& dataset, te::da::DataSourceTransactor& transactor);

      private:

        //! Not instantiable.
        DataSetItemDAO();

        //! Not instantiable.
        ~DataSetItemDAO();

        /*!
         */
        static void saveStorageMetadata(uint64_t datasetItemId,
                                        const std::map<std::string, std::string>& metadata,
                                        te::da::DataSourceTransactor& transactor);

        /*!
         */
        static void updateStorageMetadata(uint64_t datasetItemId,
                                          std::map<std::string, std::string>& metadata,
                                          te::da::DataSourceTransactor& transactor);

        /*!
         */
        static void removeStorageMetadata(uint64_t datasetItemId, te::da::DataSourceTransactor& transactor);

        /*!
         */
        static void loadStorageMetadata(DataSetItem& item, te::da::DataSourceTransactor& transactor);
      
        /*!
         */
        static void save(uint64_t datasetItemId, FilterPtr f, te::da::DataSourceTransactor& transactor);
      
        /*!
         */
        static void update(FilterPtr f, te::da::DataSourceTransactor& transactor);

        /*!
         */
        static void loadFilter(DataSetItem& item, te::da::DataSourceTransactor& transactor);
    };

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_DATASETITEMDAO_HPP__
