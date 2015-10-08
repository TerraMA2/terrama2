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

// QT
#include <QDir>
#include <QUrl>
#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


void terrama2::gui::core::checkServiceConnection(const QString& host, const int& port, const QString& user, const QString& password)
{
  QString address;
  if (!host.startsWith("http://") && !host.startsWith("https://"))
    address.append("http://");
  address.append(host);

  QUrl url(address);
  if (!url.isValid())
    throw terrama2::gui::URLError() << terrama2::ErrorDescription(QObject::tr("Invalid URL address typed"));

  url.setUserName(user);
  url.setPassword(password);
  url.setPort(port);

  QNetworkAccessManager manager;

  QNetworkReply* reply = manager.get(QNetworkRequest(url));

  if (reply->error() != QNetworkReply::NoError)
    throw terrama2::gui::ConnectionError() << terrama2::ErrorDescription(QObject::tr("Invalid connection requested"));
}

void terrama2::gui::core::checkFTPConnection(const QString& host, const int& port, const QString& basepath,
                                             const QString& user, const QString& password)
{
  // TODO
  throw terrama2::gui::URLError() << terrama2::ErrorDescription(QObject::tr("Invalid URL address typed"));
}


void terrama2::gui::core::checkLocalFilesConnection(const QString& path)
{
  QString absolutePath = path;
  absolutePath.append("/");
  QDir directory(absolutePath);

  if (!directory.exists() || directory.currentPath() == "/")
  {
    QString error(QObject::tr("Invalid directory typed: \"%1\""));
    error.arg(absolutePath);
    throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(error);
  }
}


void terrama2::gui::core::saveTerraMA2File(QMainWindow* appFocus, const QJsonObject& json)
{
  QString fileDestination = QFileDialog::getSaveFileName(appFocus, QObject::tr("TerraMA2 Export Data Provider"));
  if (fileDestination.isEmpty())
  {
    throw terrama2::Exception() << terrama2::ErrorDescription(QObject::tr("Error while saving...."));
  }

  QDir dir(fileDestination);
  if (dir.exists())
    throw terrama2::gui::DirectoryError() << terrama2::ErrorDescription(QObject::tr("Invalid directory typed"));

  if (!fileDestination.endsWith(".terrama2"))
      fileDestination.append(".terrama2");

  QFile file(fileDestination);
  file.open(QIODevice::WriteOnly);
  QJsonDocument document = QJsonDocument(json);
  file.write(document.toJson());
  file.close();
}
