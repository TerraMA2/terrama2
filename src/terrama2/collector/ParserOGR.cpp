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
  \file terrama2/collector/ParserOGR.cpp

  \brief Parsers OGR data and create a terralib DataSet.

  \author Jano Simas
*/

#include "ParserOGR.hpp"
#include "Exception.hpp"

//QT
#include <QDir>
#include <QDebug>

//STD
#include <memory>

//terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/common/Exception.h>

QStringList terrama2::collector::ParserOGR::datasetNames(const std::string &uri) const
{
  QDir dir(uri.c_str());

  return dir.entryList();
}

std::shared_ptr<te::da::DataSet> terrama2::collector::ParserOGR::read(const std::string &uri, const QStringList& names)
{
  if(names.isEmpty())
  {
    //TODO: throw
  }

  try
  {
    //create a datasource and open
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR"));
    std::map<std::string, std::string> connInfo;
    connInfo["URI"] = uri;
    datasource->setConnectionInfo(connInfo);
    datasource->open(); //FIXME: close? where?

    if(!datasource->isOpened())
    {
      throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                            QObject::tr("DataProvider could not be opened."));
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    for(const std::string& name : transactor->getDataSetNames())
      qDebug() << name.c_str();

    assert(names.size() == 1);//TODO: remove this!
    std::shared_ptr<te::da::DataSet> dataSet(transactor->getDataSet(names.front().toStdString()));

    if(!dataSet)
    {
      throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                            QObject::tr("DataSet is null."));
    }

    return dataSet;
  }
  catch(te::common::Exception e)
  {
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                          QObject::tr("Terralib exception: ") + e.what());
  }

}
