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

#include "ApplicationController.hpp"

//STL
#include <string>
#include <memory>
#include <map>

//Qt
#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QString>

//TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>


bool terrama2::core::ApplicationController::loadProject(const std::string &configFileName)
{
  configFileName_ = configFileName;

  QString settings;
  QFile file;
  file.setFileName(configFileName.c_str());
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  settings = file.readAll();
  file.close();

  QJsonDocument document = QJsonDocument::fromJson(settings.toUtf8());
  QJsonObject project = document.object();

  if(project.contains("database"))
  {
    QJsonObject databaseConfig = project["database"].toObject();
    std::map<std::string, std::string> connInfo;
    connInfo["PG_HOST"] = databaseConfig["hostName"].toString().toStdString();
    connInfo["PG_PORT"] = databaseConfig["port"].toString().toStdString();
    connInfo["PG_USER"] = databaseConfig["user"].toString().toStdString();
    connInfo["PG_PASSWORD"] = databaseConfig["password"].toString().toStdString();
    connInfo["PG_DB_NAME"] = databaseConfig["dbName"].toString().toStdString();
    connInfo["PG_CLIENT_ENCODING"] = "CP1252";

    dataSouce_ = te::da::DataSourceFactory::make("POSTGIS");
    dataSouce_->setConnectionInfo(connInfo);
    dataSouce_->open();

    return true;
  }
  else
  {
    return false;
  }


}

std::auto_ptr<te::da::DataSourceTransactor> terrama2::core::ApplicationController::getTransactor()
{

  std::auto_ptr<te::da::DataSourceTransactor> ptr;
  if(dataSouce_.get())
  {
    if(dataSouce_->isOpened())
    {
      ptr = dataSouce_->getTransactor();
    }
  }

  return ptr;
}
