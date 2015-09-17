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

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>

ConfigManager::ConfigManager(QMainWindow* app)
  : app_(app), collection_(new Collection), database_(new Database)
{

}

ConfigManager::~ConfigManager()
{
  delete collection_;
  delete database_;
}

void ConfigManager::loadConfiguration(QString filepath)
{
  try
  {
    QJsonObject metadata = open(filepath);

    name_ = metadata["name"].toString();

    if (metadata.contains("database"))
    {
      QJsonObject databaseConfig = metadata["database"].toObject();
      database_->dbName_ = databaseConfig["name"].toString();
      database_->host_ = databaseConfig["host"].toString();
      database_->port_ = databaseConfig["port"].toString().toInt();
      database_->user_ = databaseConfig["user"].toString();
      database_->password_ = databaseConfig["password"].toString();
    }
    if (metadata.contains("collector_web_service"))
    {

    }
    else
      throw terrama2::Exception() << terrama2::ErrorDescription(QObject::tr("This TerraMA2 file is not valid."));
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

QJsonObject ConfigManager::open(QString filepath)
{
  QString settings;
  QFile file;
  file.setFileName(filepath);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  settings = file.readAll();
  file.close();

  QJsonDocument document = QJsonDocument::fromJson(settings.toUtf8());
  QJsonObject project = document.object();
  return project;
}

Database* ConfigManager::getDatabase() const
{
  return database_;
}

Collection* ConfigManager::getCollection() const
{
  return collection_;
}

QString ConfigManager::getName() const
{
  return name_;
}
