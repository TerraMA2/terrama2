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
  \file terrama2/core/DataProviderDAO.hpp

  \brief Persistense layer to a data provider class.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATAPROVIDERDAO_HPP__
#define __TERRAMA2_CORE_DATAPROVIDERDAO_HPP__

// STL
#include <memory>
#include <vector>

// Forward declaration
namespace te { namespace da { class DataSourceTransactor; } }

namespace terrama2
{
  namespace core
  {
// Forward declaration
    class DataProvider;

    /*!
      \class DataProviderDAO

      \brief Persistense layer to a data provider class.

      Contains methods to persist and retrieve information about a data provider.

      It uses the connection to the database given by the application controller.
     */
    class DataProviderDAO
    {
      public:

        /*!
          \brief Load from database all registered data providers.

          In case there is no data provider in the database it will return an empty vector.

          \param transactor The transactor to be used to perform the operation.

          \return A list with all data providers.

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static std::vector<std::unique_ptr<DataProvider> >
        load(te::da::DataSourceTransactor& transactor);

        /*!
          \brief Insert the given data provider in the database.

          \param provider    The data provider to be inserted into the database.
          \param transactor  The data source transactor to be used to perform the insert operation.
          \param shallow     If true it will persist the datasets in this data provider.

          \pre The data provider must have an identifier equals to 0 (considered invalid).
          \pre Each dataset must have an identifier equals to 0 (considered invalid).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void save(DataProvider& provider, te::da::DataSourceTransactor& transactor,
                         const bool shallow = true);

        /*!
          \brief Update the given data provider in the database.

          \param dataProvider  The data provider to be updated.
          \param transactor The data source transactor to be used to perform the operation.
          \param shallowSave If true it will update the datasets in this data provider.

          \pre The data provider must have an identifier different than 0 (considered valid).
          \pre Each dataset must have an identifier equals to 0 (considered invalid).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void update(DataProvider& provider, te::da::DataSourceTransactor& transactor,
                           const bool shallowSave = true);

        /*!
          \brief Remove the given data provider from the database.

          It will remove all datasets from this provider.

          \param id  The data provider identifier to be removed.
          \param transactor The data source transactor to be used to perform the operation.

          \pre The identifier must be different than 0 (considered valid).
          \pre Should not exist an analysis using one of the datasets from this provider.

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void remove(const uint64_t id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieve the data provider with the given identifier from the database.

          \param id  The data provider identifier to be loaded.
          \param transactor The data source transactor to be used to perform the operation.

          \pre The identifier must be different than 0 (considered valid).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static std::unique_ptr<DataProvider> load(const uint64_t id, te::da::DataSourceTransactor& transactor);

      private:

        //! Not instantiable.
        DataProviderDAO();

        //! Not instantiable.
        ~DataProviderDAO();
    };

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_DATAPROVIDERDAO_HPP__
