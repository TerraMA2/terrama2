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
  \file terrama2/gui/core/ConfigManager.cpp

  \brief This source file handling the terrama2 configuration file.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigManager.hpp"
#include "../Exception.hpp"
#include "../../core/Utils.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/DataManager.hpp"

// Qt
#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>
#include <QString>


terrama2::gui::core::ConfigManager::ConfigManager(QMainWindow* app)
  : app_(app), collection_(new Collection), database_(new Database)
{

}

terrama2::gui::core::ConfigManager::~ConfigManager()
{
  delete collection_;
  delete database_;
}

void terrama2::gui::core::ConfigManager::loadConfiguration(QString filepath)
{
  try
  {
    QJsonDocument jdoc = terrama2::core::ReadJsonFile(filepath.toStdString());

    QJsonObject metadata = jdoc.object();

    name_ = metadata["name"].toString();
    metadata.insert("path",filepath);
    fileList_.insert(name_, metadata);
    setDataForm(metadata);

  }
  catch (const terrama2::Exception& e)
  {
    const QString* msg = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::critical(app_, "TerraMA2", *msg);
  }
  catch (const std::exception& e)
  {
    QMessageBox::information(app_, "TerraMA2", e.what());
  }
}

void terrama2::gui::core::ConfigManager::insertFile(QString newname, QJsonObject metatada)
{
 fileList_.insert(newname,metatada);
 setDataForm(metatada);
}

void terrama2::gui::core::ConfigManager::renameFile(QString selectedName, QString newname)
{
 fileList_.insert(newname,fileList_[selectedName]);
 fileList_.remove(selectedName);
}

void terrama2::gui::core::ConfigManager::removeFile(QString selectedName)
{
 fileList_.remove(selectedName);
}

void terrama2::gui::core::ConfigManager::setDataForm(QJsonObject metadata)
{

    if (metadata.contains("database"))
    {
      QJsonObject databaseConfig = metadata["database"].toObject();
      database_->dbName_ = databaseConfig["name"].toString();
      database_->host_ = databaseConfig["host"].toString();
      database_->port_ = databaseConfig["port"].toString().toInt();
      database_->user_ = databaseConfig["user"].toString();
      database_->password_ = databaseConfig["password"].toString();
      database_->driver_ = databaseConfig["driver"].toString();
      database_->study_ = metadata["is_study"].toString();
      database_->name_ = metadata["name"].toString();
      database_->version_ = metadata["version"].toString();
    }
    if (metadata.contains("collector_web_service"))
    {
      QJsonObject collectConfig = metadata["collector_web_service"].toObject();

      collection_->dirPath_ = collectConfig["data_path"].toString();
      collection_->logFile_ = collectConfig["log_file"].toString();
      collection_->timeout_ = collectConfig["connection_timeout"].toInt();
      collection_->address_ = collectConfig["address"].toString();
      collection_->servicePort_ = collectConfig["port"].toString().toInt();
      collection_->cmd_ = collectConfig["command"].toString();
      collection_->params_ = collectConfig["parameters"].toString();
    }
    else
      throw terrama2::Exception() << terrama2::ErrorDescription(QObject::tr("This TerraMA2 file is not valid."));

}

QMap<QString, QJsonObject> terrama2::gui::core::ConfigManager::getfiles() const
{
  return fileList_;
}

void terrama2::gui::core::ConfigManager::setDatabase(QJsonObject dbase)
{
 database_->dbName_ = dbase["name"].toString();
 database_->driver_ = dbase["driver"].toString();
 database_->host_ = dbase["host"].toString();
 database_->port_ = dbase["port"].toString().toInt();
 database_->user_ = dbase["user"].toString();
 database_->password_ = dbase["password"].toString();
}

terrama2::gui::core::Database* terrama2::gui::core::ConfigManager::getDatabase() const
{
  return database_;
}

terrama2::gui::core::Collection* terrama2::gui::core::ConfigManager::getCollection() const
{
  return collection_;
}

QString terrama2::gui::core::ConfigManager::getName() const
{
  return name_;
}
