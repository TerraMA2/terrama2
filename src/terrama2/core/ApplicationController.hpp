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
  \file terrama2/core/ApplicationController.hpp

  \brief The base API for TerraMA2 application.

  \author Paulo R. M. Oliveira
  \author Evandro Delatin
  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_APPLICATION_CONTROLLER_HPP__
#define __TERRAMA2_CORE_APPLICATION_CONTROLLER_HPP__

//TerraLib
#include <terralib/common/Singleton.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

//STL
#include <memory>
#include <string>

//Qt
#include <QJsonObject>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Singleton class to provide a connection to the database and read all project configuration.
     */
    class ApplicationController : public te::common::Singleton<ApplicationController>
    {
      /*! In order to use the singleton template from TerraLib the ApplicationController must be a friend class of the te::common::Singleton. */
      friend class te::common::Singleton<ApplicationController>;

      public:

        /*!
          \brief Loads project configuration from the given file and creates a data source for the database configuration.

          \param configFileName Path to the configuration file

          \return Returns if it was possible to read the file and read all the configurations.
         */
        bool loadProject(const QJsonObject& project);

        /*!
          \brief Returns a datasource transactor.

          \return Returns a auto pointer to the datasource transactor.

          \exception DataAccessException If it is not possible to get a new data source transactor an exception is raisen.
        */
        std::shared_ptr<te::da::DataSourceTransactor> getTransactor();

        /*!
          \brief Returns a datasource.

          \return Returns a shared pointer to the datasource.
        */
        std::shared_ptr<te::da::DataSource> getDataSource();

        /*!
          \brief Creates a database with TerraMA2's data model.

          \param dbName - database name.
          \param username - user name database.
          \param password - Password database
          \param host - Address access to the database. Ex. localhost.
          \param port - database service port.

          \return Returns true if the database was create successfully.
          \exception DataAccessException when invalid data from the database interface.
          \exception DataAccessException when could not connect to the database.
          \exception DataAccessException when unknown error, could not connect to the database.
          \exception DataAccessException when database exists.
          \exception DataAccessException when could not close the database.
          \exception DataAccessException when unknown error, could not close the database.
          \exception DataAccessException when could not create the database.
          \exception DataAccessException when unknown error, could not create the database.
        */
        void createDatabase(const std::string& dbName, const std::string& username, const std::string& password, const std::string& host, const int port);

        /*!
          \brief Check if TerraMA2's database exists.

          \param dbName - database name.
          \param username - user name database.
          \param password - Password database
          \param host - Address access to the database. Ex. localhost.
          \param port - database service port.

          \return Returns true if the database exists.
          \exception DataAccessException when Unknown Error, could not check if database exists.
        */
        bool checkConnectionDatabase(const std::string& dbName, const std::string& username, const std::string& password, const std::string& host, const int port);

      protected:

        QJsonObject project_;                          //!< Configuration information.
        std::shared_ptr<te::da::DataSource> dataSource_;  //!< Smart pointer to the data source.
    };
  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_APPLICATION_CONTROLLER_HPP__
