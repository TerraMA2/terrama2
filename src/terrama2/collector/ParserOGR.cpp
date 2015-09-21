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

//QT
#include <QDir>

//STD
#include <memory>

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>

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

  //create a datasource and open
  std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR").release());//FIXME: releasing auto_ptr to create a shared_ptr
  std::map<std::string, std::string> connInfo;
  connInfo["URI"] = uri;
  datasource->setConnectionInfo(connInfo);
  datasource->open(); //FIXME: close? where?

  if(!datasource->isOpened())
  {
    //TODO: throw
  }

// get a transactor to interact to the data source
  te::da::DataSourceTransactor* transactor = (datasource->getTransactor()).release();

  try
  {
    assert(names.size() == 1);//TODO: remove this!
    std::shared_ptr<te::da::DataSet> dataSet(datasource->getDataSet(names.front().toStdString()).release());//FIXME: releasing auto_ptr to create a shared_ptr

    return dataSet;
  }
  catch(...)
  {
    //TODO: error getting dataset...
    return std::auto_ptr<te::da::DataSet>(nullptr);
  }
}
