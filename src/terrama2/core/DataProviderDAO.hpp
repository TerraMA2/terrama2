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
       \brief Constructor

       It will retrieve the data source from the application controller in order to get the transactor.

       \pre The project must have been loaded in the application controller.

       \exeption PAULO-TODO: Specify the exception
       */
      DataProviderDAO();

      /*!
       \brief Destructor
       */
      virtual ~DataProviderDAO();

      /*!
       \brief Persists a given data provider.

       It will not persist information about datasets contained in this provider.

       \exeption PAULO-TODO: Specify the exception

       \param DataProviderPtr The data provider to persist.
       */
      void save(DataProviderPtr dataProvider);

      /*!
       \brief Updates a given data provider.

       \pre The data provider must have a valid identifier.

       \exeption PAULO-TODO: Specify the exception

       \param DataProviderPtr The data provider to update.
       */
      void update(DataProviderPtr dataProvider);

      /*!
       \brief Removes a given data provider.

       \pre The data provider must have a valid identifier.

       It will remove all datasets that belong to this data provider.
       In case there is an analysis that uses one these dataset an exception is thrown.

       \exeption PAULO-TODO: Specify the exception

       \param DataProviderPtr The data provider to update.
       */
      void remove(DataProviderPtr dataProvider);

      /*!
       \brief Retrieves the data provider with the given id.

       \exeption PAULO-TODO: Specify the exception

       In case there is no data provider in the database with the given id it will return an empty smart pointer.

       \param id The data provider identifier.

       \return DataProviderPtr A smart pointer to the data provider

       */
      DataProviderPtr find(const uint64_t id) const;

      /*!
       \brief Retrieves all data provider in the database.

       \exeption PAULO-TODO: Specify the exception

       In case there is no data provider in the database it will return an empty vector.

       \return std::vector<DataProviderPtr> A list with all data providers.

       */
      std::vector<DataProviderPtr> list() const;

    protected:

      std::auto_ptr<te::da::DataSourceTransactor> transactor_;

    };

  } // core
} // terrama2

#endif // __TERRAMA2_CORE_DATAPROVIDERDAO_HPP__
