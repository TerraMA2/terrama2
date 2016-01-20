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
  \author Evandro Delatin
  \author Jano Simas
*/

// TerraMA2
#include "ApplicationController.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraMA2 Logger
#include "Logger.hpp"

// STL
#include <map>
#include <memory>
#include <string>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

// Qt
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>


//**************************************************************************
//FIXME: temporary, should removed. Only used while not using batch executor

/**
* @brief executeQueriesFromFile Read each line from a .sql QFile
* (assumed to not have been opened before this function), and when ; is reached, execute
* the SQL gathered until then on the query object. Then do this until a COMMIT SQL
* statement is found. In other words, this function assumes each file is a single
* SQL transaction, ending with a COMMIT line.
*/

void executeQueriesFromFile(QFile *file, QSqlQuery *query)
{
  while (!file->atEnd()){
    QByteArray readLine="";
    QString cleanedLine;
    QString line="";
    bool finished=false;
    while(!finished){
      readLine = file->readLine();
      cleanedLine=readLine.trimmed();
      // remove comments at end of line
      QStringList strings=cleanedLine.split("--");
      cleanedLine=strings.at(0);

      // remove lines with only comment, and DROP lines
      if(!cleanedLine.startsWith("--")
         && !cleanedLine.startsWith("DROP")
         && !cleanedLine.isEmpty()){
        line+=cleanedLine;
      }
      if(cleanedLine.endsWith(";")){
        break;
      }
      if(cleanedLine.startsWith("COMMIT")){
        finished=true;
      }
    }

    if(!line.isEmpty()){
      query->exec(line);
    }
    if(!query->isActive()){
      qDebug() << QSqlDatabase::drivers();
      qDebug() <<  query->lastError();
      qDebug() << "test executed query:"<< query->executedQuery();
      qDebug() << "test last query:"<< query->lastQuery();
    }
  }
}
//**************************************************************************


bool terrama2::core::ApplicationController::loadProject(const std::string &configFileName)
{
  configFile_ = configFileName;

  if(dataSource_.get() && dataSource_->isOpened())
  {
    dataSource_->close();
  }

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

      return dataSource_->isOpened();
    }
    else
      return false;
  }
  catch(te::common::Exception& e)
  {
    if (const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e))
      TERRAMA2_LOG_ERROR() << message->toStdString();
    return false;
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << "Unknown error while reading and parsing TerraMA2 file";
    return false;
  }

}

std::auto_ptr<te::da::DataSourceTransactor>
terrama2::core::ApplicationController::getTransactor()
{
  try
  {
    if((dataSource_.get() != nullptr) && (dataSource_->isOpened()))
      return dataSource_->getTransactor();
  }
  //catch(const te::common::Exception& e)
  //{
  //  throw DataAccessException() << ErrorDescription(e.what());
  //}
  catch(const std::exception& e)
  {
    const char* message = e.what();
    TERRAMA2_LOG_ERROR() << message;
    throw DataAccessException() << ErrorDescription(message);
  }
  catch(...)
  {
    const QString message = QObject::tr("Could not retrieve a data source transactor.");
    TERRAMA2_LOG_ERROR() << message;
    throw DataAccessException() << ErrorDescription(message);
  }

  QString message = QObject::tr("The data source is not valid or it is not open.");
  TERRAMA2_LOG_ERROR() << message;

  throw DataAccessException() << ErrorDescription(message);
}

std::shared_ptr<te::da::DataSource> terrama2::core::ApplicationController::getDataSource()
{
  return dataSource_;
}

void terrama2::core::ApplicationController::createDatabase(const std::string &dbName, const std::string &username, const std::string &password, const std::string &host, const int port)
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

  bool dsExists = true;

  try
  {
    dsExists = te::da::DataSource::exists(dsType, connInfo);
  }
  catch(const te::common::Exception& e)
  {
    QString messageError = QObject::tr("Invalid data from the database interface! \n\n Details: \n");
    messageError.append(e.what());
    TERRAMA2_LOG_ERROR() << messageError;

    throw DataAccessException() << ErrorDescription(messageError);
  }
  catch(const std::exception& e)
  {
    QString messageError = QObject::tr("Could not connect to the database! \n\n Details: \n");
    messageError.append(e.what());
    TERRAMA2_LOG_ERROR() << messageError;

    throw DataAccessException() << ErrorDescription(messageError);
  }
  catch(...)
  {
    QString messageError = QObject::tr("Unknown Error, could not connect to the database!");
    throw DataAccessException() << ErrorDescription(messageError);
  }

  if(dsExists)
  {
    //return false;
    QString messageError = QObject::tr("Database exists!");
    TERRAMA2_LOG_ERROR() << messageError;
    throw DataAccessException() << ErrorDescription(messageError);
  }
  else
  {
    // Closes the previous data source
    if(dataSource_.get())
    {
      try
      {
        dataSource_->close();
      }

      catch(const te::common::Exception& e)
      {
        QString messageError = QObject::tr("Could not close the database! \n\n Details: \n");
        messageError.append(e.what());
        TERRAMA2_LOG_ERROR() << messageError;

        throw DataAccessException() << ErrorDescription(messageError);
      }

      catch(...)
      {
        QString messageError = QObject::tr("Unknown Error, could not close the database!");
        TERRAMA2_LOG_ERROR() << messageError;
        throw DataAccessException() << ErrorDescription(messageError);
      }
    }

    try
    {
      dataSource_ = std::shared_ptr<te::da::DataSource>(te::da::DataSource::create(dsType, connInfo));

      //**************************************************************************
      //FIXME: temporary, should be executed with batch executor
      //FIXME: Remove includes from CMakeList (project and module)
      QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
      db.setHostName(host.c_str());
      db.setDatabaseName(dbName.c_str());
      db.setUserName(username.c_str());
      db.setPassword(password.c_str());
      db.open();

      QString scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/sql/terrama2-data-model-pg.sql").c_str();
      QFile sqlScript(scriptPath);
      sqlScript.open(QIODevice::ReadOnly);
      QSqlQuery query(db);

      query.exec("CREATE EXTENSION postgis");

      executeQueriesFromFile(&sqlScript, &query);
      sqlScript.close();
      db.close();
      //**************************************************************************

      // TODO: Create the database model executing the script
    }
    catch(te::common::Exception& e)
    {
      QString messageError = QObject::tr("Could not create the database! \n\n Details: \n");
      messageError.append(e.what());

      TERRAMA2_LOG_ERROR() << messageError;

      throw DataAccessException() << ErrorDescription(messageError);
    }
    catch(...)
    {
      QString messageError = QObject::tr("Unknown Error, could not create the database!");
      throw DataAccessException() << ErrorDescription(messageError);
    }
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

  try
  {
    return te::da::DataSource::exists(dsType, connInfo);
  }
  catch(const te::common::Exception& e)
  {
    QString messageError;

    messageError.append(e.what());
    TERRAMA2_LOG_ERROR() << messageError;
  }

  catch(...)
  {
    QString messageError = QObject::tr("Unknown Error, could not check if database exists!");
    throw DataAccessException() << ErrorDescription(messageError);
  }

  return false;
}
