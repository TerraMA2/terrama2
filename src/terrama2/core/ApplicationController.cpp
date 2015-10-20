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
  \file terrama2/core/ApplicationController.cpp

  \brief The base API for TerraMA2 application.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "ApplicationController.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// STL
#include <map>
#include <memory>
#include <string>

// Qt
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>


bool terrama2::core::ApplicationController::loadProject(const std::string &configFileName)
{
  configFile_ = configFileName;


  try
  {
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(configFileName);
    
    QJsonObject project = jdoc.object();

    if(project.contains("database"))
    {
      QJsonObject databaseConfig = project["database"].toObject();
      std::map<std::string, std::string> connInfo;
      connInfo["PG_HOST"] = databaseConfig["host"].toString().toStdString();
      connInfo["PG_PORT"] = databaseConfig["port"].toString().toStdString();
      connInfo["PG_USER"] = databaseConfig["user"].toString().toStdString();
      connInfo["PG_PASSWORD"] = databaseConfig["password"].toString().toStdString();
      connInfo["PG_DB_NAME"] = databaseConfig["name"].toString().toStdString();
      connInfo["PG_CLIENT_ENCODING"] = "UTF-8";

      dataSource_ = te::da::DataSourceFactory::make("POSTGIS");
      dataSource_->setConnectionInfo(connInfo);
      dataSource_->open();

      return true;
    }
    else
    {
      return false;
    }
  }
  catch(...)
  {
    return false;
  }

}

std::auto_ptr<te::da::DataSourceTransactor>
terrama2::core::ApplicationController::getTransactor()
{
  try
  {
    if((dataSource_ != nullptr) && (dataSource_->isOpened()))
      return dataSource_->getTransactor();
  }
  //catch(const te::common::Exception& e)
  //{
  //  throw DataAccessError() << ErrorDescription(e.what());
  //}
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not retrieve a data source transactor."));
  }

  throw DataAccessError() << ErrorDescription(QObject::tr("The data source is not valid or it is not open."));
}

std::shared_ptr<te::da::DataSource> terrama2::core::ApplicationController::getDataSource()
{
  return dataSource_;
}

bool terrama2::core::ApplicationController::createDatabase(const std::string &dbName, const std::string &username, const std::string &password, const std::string &host, const int port)
{

  std::map<std::string, std::string> connInfo;

  connInfo["PG_HOST"] = host;
  connInfo["PG_PORT"] = std::to_string(port);
  connInfo["PG_USER"] = username;
  connInfo["PG_DB_NAME"] = "postgres";
  connInfo["PG_CONNECT_TIMEOUT"] = "4";
  connInfo["PG_CLIENT_ENCODING"] = "UTF-8";
  connInfo["PG_NEWDB_NAME"] = dbName;

  std::string dsType = "POSTGIS";


  // Check the data source existence
  connInfo["PG_CHECK_DB_EXISTENCE"] = dbName;
  bool dsExists = te::da::DataSource::exists(dsType, connInfo);

  if(dsExists)
  {
    return false;
  }
  else
  {
    // Closes the previous data source
    if(dataSource_.get())
    {
      dataSource_->close();
    }

    dataSource_ = std::shared_ptr<te::da::DataSource>(te::da::DataSource::create(dsType, connInfo));

    dataSource_->open();

    auto transactor = dataSource_->getTransactor();

    // TODO: Create the database model executing the script

    return true;
  }

}

bool terrama2::core::ApplicationController::checkConnectionDatabase(const std::string& dbName, const std::string& username, const std::string& password, const std::string& host, const int port)
{
    std::map<std::string, std::string> connInfo;

    connInfo["PG_HOST"] = host;
    connInfo["PG_PORT"] = std::to_string(port);
    connInfo["PG_USER"] = username;
    connInfo["PG_DB_NAME"] = dbName;
    connInfo["PG_CONNECT_TIMEOUT"] = "4";
    connInfo["PG_CLIENT_ENCODING"] = "UTF-8";

    std::string dsType = "POSTGIS";

    // Check the data source existence
    connInfo["PG_CHECK_DB_EXISTENCE"] = dbName;
    bool dsExists = te::da::DataSource::exists(dsType, connInfo);

    if(dsExists)
    {
      return true;
    }
    else
    {
      return false;
    }

}
