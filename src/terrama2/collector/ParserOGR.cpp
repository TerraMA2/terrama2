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
  \author Evandro Delatin
*/

#include "ParserOGR.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"


//QT
#include <QDir>
#include <QDebug>

//STD
#include <memory>

#include <boost/format/exceptions.hpp>

//terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/common/Exception.h>

#include "terralib/dataaccess/dataset/DataSet.h"
#include "terralib/dataaccess/dataset/DataSetAdapter.h"
#include "terralib/dataaccess/dataset/DataSetTypeConverter.h"



std::vector<std::string> terrama2::collector::ParserOGR::datasetNames(const std::string &uri) const
{
  QDir dir(uri.c_str());

  QFileInfoList entryList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Readable);
  std::vector<std::string> names;
  for(const QFileInfo& file : entryList)
    names.push_back(file.baseName().toStdString());

  return names;
}


void terrama2::collector::ParserOGR::datasetupdate(std::shared_ptr<te::da::DataSet> dataset, std::shared_ptr<te::da::DataSetType> datasetTypeVec)
{
  te::da::DataSetAdapter* adaptedDataSet = new te::da::DataSetAdapter(dataset.get());

  te::da::DataSetTypeConverter converter(datasetTypeVec.get());
  const std::vector<std::vector<std::size_t> >& indexes = converter.getConvertedPropertyIndexes();
  const std::vector<te::da::AttributeConverter>& funcs = converter.getConverters();

  for(std::size_t i = 0; i < datasetTypeVec->size(); ++i)
    {
      te::dt::Property* p = datasetTypeVec->getProperty(i);
      if (p->getName() != "FID")
         adaptedDataSet->add(p->getName(), p->getType(), indexes[i], funcs[i]);
    }
}

void terrama2::collector::ParserOGR::read(const std::string &uri,
                                          DataFilterPtr filter,
                                          std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                                          std::shared_ptr<te::da::DataSetType>& datasetTypeVec)
{

  std::vector<std::string> allNames = datasetNames(uri);
  std::vector<std::string> names = filter->filterNames(allNames);

  if(names.empty())
  {
    //TODO: throw
  }

  std::lock_guard<std::mutex> lock(mutex_);

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

    for(const std::string& name : names)
    {
      std::shared_ptr<te::da::DataSet> dataSet(transactor->getDataSet(name));
      datasetTypeVec = std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(name));

      if(!dataSet || !datasetTypeVec)
      {
        throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                              QObject::tr("DataSet: %1 is null.").arg(name.c_str()));
      }

      datasetupdate(dataSet, datasetTypeVec);

      datasetVec.push_back(dataSet);
    }

    return;
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                          QObject::tr("Terralib exception: ") + boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }
  catch(...)
  {
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(QObject::tr("Unknown exception."));
  }

  return;
}
