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

  \brief The implementation of gui utils module

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "Utils.hpp"
#include "../Exception.hpp"
#include "../../core/DataSet.hpp"
#include "ConfigManager.hpp"

// QT
#include <QDir>
#include <QUrl>
#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>


void terrama2::gui::core::saveTerraMA2File(QMainWindow* appFocus, const QString& destination, const QJsonObject& json)
{
  if (destination.isEmpty())
  {
    throw terrama2::gui::FileException() << terrama2::ErrorDescription(QObject::tr("Error while saving...."));
  }

  QDir dir(destination);
  if (dir.exists())
    throw terrama2::gui::DirectoryException() << terrama2::ErrorDescription(QObject::tr("Invalid directory typed"));

  QString path(destination);
  if (!path.endsWith(".terrama2"))
    path.append(".terrama2");

  QFile file(path);
  file.open(QIODevice::WriteOnly);
  QJsonDocument document = QJsonDocument(json);
  file.write(document.toJson());
  file.close();
}

std::map<std::string, std::string> terrama2::gui::core::makeStorageMetadata(const terrama2::core::DataSet &dataset, const terrama2::gui::core::ConfigManager& configuration)
{
  std::map<std::string, std::string> storageMetadata;

  if (dataset.kind() == terrama2::core::DataSet::GRID_TYPE)
  {
    storageMetadata["PATH"] = configuration.getCollection()->dirPath_.toStdString();
    storageMetadata["KIND"] = "FILE";
  }
  else // postgis
  {
    storageMetadata["PG_HOST"] = configuration.getDatabase()->host_.toStdString();
    storageMetadata["PG_PORT"] = configuration.getDatabase()->port_;
    storageMetadata["PG_USER"] = configuration.getDatabase()->user_.toStdString();
    storageMetadata["PG_PASSWORD"] = configuration.getDatabase()->password_.toStdString();
    storageMetadata["PG_DB_NAME"] = configuration.getDatabase()->name_.toStdString();
    storageMetadata["PG_CLIENT_ENCODING"] = "UTF-8";
    storageMetadata["KIND"] = "POSTGIS";
  }

  return storageMetadata;
}

QMenu *terrama2::gui::core::makeMaskHelpers()
{
  QMenu* menuMask = new QMenu(QObject::tr("Máscaras"));
  menuMask->addAction(QObject::tr("%a - ano com dois digitos"));
  menuMask->addAction(QObject::tr("%A - ano com quatro digitos"));
  menuMask->addAction(QObject::tr("%d - dia com dois digitos"));
  menuMask->addAction(QObject::tr("%M - mes com dois digitos"));
  menuMask->addAction(QObject::tr("%h - hora com dois digitos"));
  menuMask->addAction(QObject::tr("%m - minuto com dois digitos"));
  menuMask->addAction(QObject::tr("%s - segundo com dois digitos"));
  menuMask->addAction(QObject::tr("%. - um caracter qualquer"));

  return menuMask;
}
