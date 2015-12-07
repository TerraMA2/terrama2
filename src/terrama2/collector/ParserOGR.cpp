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

// TerraMA2
#include "DataFilter.hpp"
#include "ParserOGR.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// QT
#include <QDir>
#include <QDebug>
#include <QUrl>

// STL
#include <memory>

// Boost
#include <boost/format/exceptions.hpp>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/common/Exception.h>

te::da::DataSetTypeConverter terrama2::collector::ParserOGR::getConverter(const std::shared_ptr<te::da::DataSetType>& datasetType)
{
  te::da::DataSetTypeConverter converter(datasetType.get());

  for(std::size_t i = 0, size = datasetType->size(); i < size; ++i)
  {
    te::dt::Property* p = datasetType->getProperty(i);

    converter.add(i,p->clone());
  }

  converter.remove("FID");
  adapt(converter);

  return converter;
}

void terrama2::collector::ParserOGR::read(const std::string &uri,
                                          DataFilterPtr filter,
                                          std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                                          std::shared_ptr<te::da::DataSetType>& datasetType)
{
  std::lock_guard<std::mutex> lock(mutex_);

  try
  {
    //create a datasource and open
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR"));
    std::map<std::string, std::string> connInfo;
    QUrl url(uri.c_str());
    connInfo["URI"] = url.path().toStdString();
    datasource->setConnectionInfo(connInfo);

    //RAII for open/closing the datasource
    OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    if(!datasource->isOpened())
    {
      throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                            QObject::tr("DataProvider could not be opened."));
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
    std::vector<std::string> names = transactor->getDataSetNames();
    names = filter->filterNames(names);

    if(names.empty())
    {
      //TODO: throw no dataset found
      return;
    }


    //prepare the converter to adapt the dataset
    te::da::DataSetTypeConverter converter = getConverter(std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(names.at(0))));
    datasetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(converter.getResult()->clone()));


    for(const std::string& name : names)
    {
      std::unique_ptr<te::da::DataSet> datasetOrig = transactor->getDataSet(name);
      std::shared_ptr<te::da::DataSet> dataset = std::shared_ptr<te::da::DataSet>(te::da::CreateAdapter(datasetOrig.release(), &converter, true));

      datasetVec.push_back(dataset);
    }

    return;
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                          QObject::tr("Terralib exception: ") +e.what());
  }
  catch(std::exception& e)
  {
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(QObject::tr("Std exception.")+e.what());
  }

  return;
}
