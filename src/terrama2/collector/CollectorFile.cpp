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
  \file terrama2/collector/CollectorFile.hpp

  \brief Aquire data from server.

  \author Jano Simas
*/


#include "CollectorFile.hpp"
#include "Filter.hpp"
#include "Exception.hpp"

//QT
#include <QStringList>
#include <QFile>
#include <QDebug>

//STD
#include <cstdio>

terrama2::collector::CollectorFile::CollectorFile(const terrama2::core::DataProviderPtr dataProvider, QObject *parent)
  : Collector(dataProvider, parent)
{
  if(dataProvider->kind() != core::DataProvider::FILE_TYPE)
    throw WrongDataProviderKindException() << terrama2::ErrorDescription(
                                                tr("Wrong DataProvider Kind received in CollectorFile constructor."));

  //TODO: Change to QString?
  dir_.setPath(dataProvider->uri().c_str());
}

bool terrama2::collector::CollectorFile::checkConnection() const
{
  return dir_.exists();
}


bool terrama2::collector::CollectorFile::isOpen() const
{
  return dir_.isReadable();
}

void terrama2::collector::CollectorFile::open()
{
  //nothing to do
}

void terrama2::collector::CollectorFile::close()
{
  //nothing to do
}

void terrama2::collector::CollectorFile::getData(const terrama2::collector::DataProcessorPtr dataProcessor)
{
  QString tempDir = QDir::tempPath()+"/TerraMa2";

  QStringList names = dir_.entryList();

  FilterPtr filter = dataProcessor->filter();
  names = filter->filterNames(names);

  for(const QString& fileName : names)
  {
    qDebug() << dir_.absolutePath()+"/"+fileName;
    QString tempFilePath(tempDir+"/"+std::tmpnam(nullptr));
//    QFile::copy(dir_.absolutePath()+"/"+fileName, tempFilePath);

    //TODO: It's a local file, should it be copied? How to remove the temporary file???

    QString uri("file:");
    uri+tempFilePath;
    dataProcessor->import(uri.toStdString());
  }
}
