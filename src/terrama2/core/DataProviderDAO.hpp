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
#include <vector>
#include <memory>

namespace te
{
  namespace da
  {
    class DataSourceTransactor;
  }
}

namespace terrama2
{
  namespace core
  {

    class DataProvider;
    typedef std::shared_ptr<DataProvider> DataProviderPtr;

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
          \brief Persists a given data provider.

          The identifier of the data provider must be zero.

          It will persist also the datasets existents in this data provider, the id of the datasets must be zero.

          \exception InvalidDataProviderIdError, InvalidDataSetIdError

          \param DataProviderPtr The data provider to persist.
       */
        static void save(DataProviderPtr dataProvider, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Updates a given data provider.

          The data provider must have a valid identifier.

          \exception InvalidDataProviderIdError

          \param DataProviderPtr The data provider to update.
       */
        static void update(DataProviderPtr dataProvider, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Removes a given data provider.

          \pre The data provider must have a valid identifier.

          It will remove all datasets that belong to this data provider.
          In case there is an analysis that uses one these dataset an exception is thrown.

          \exception InvalidDataProviderIdError, DataSetInUseError

          \param DataProviderPtr The data provider to update.
       */
        static void remove(DataProviderPtr dataProvider, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieves the data provider with the given id.

          \exception InvalidDataProviderIdError

          In case there is no data provider in the database with the given id it will return an empty smart pointer.

          \param id The data provider identifier.

          \return DataProviderPtr A smart pointer to the data provider

       */
        static DataProviderPtr find(const uint64_t id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieves all data provider in the database.

          In case there is no data provider in the database it will return an empty vector.

          \return std::vector<DataProviderPtr> A list with all data providers.

       */
        static std::vector<DataProviderPtr> list(te::da::DataSourceTransactor& transactor);

      private:

        //! Not instantiable.
        DataProviderDAO();

        //! Not instantiable.
        ~DataProviderDAO();

    };

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_DATAPROVIDERDAO_HPP__
