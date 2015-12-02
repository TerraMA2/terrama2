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

#include "DataFilter.hpp"
#include "ParserOGR.hpp"
#include "Exception.hpp"
#include "Utils.hpp"


//QT
#include <QDir>
#include <QDebug>
#include <QUrl>

//STD
#include <memory>

#include <boost/format/exceptions.hpp>

//terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/common/Exception.h>

void terrama2::collector::ParserOGR::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType)
{
  for(std::size_t i = 0, size = datasetType->size(); i < size; ++i)
  {
    te::dt::Property* p = datasetType->getProperty(i);

    converter->add(i,p->clone());
  }
}

std::shared_ptr<te::da::DataSetTypeConverter> terrama2::collector::ParserOGR::getConverter(const std::shared_ptr<te::da::DataSetType>& datasetType)
{
  std::shared_ptr<te::da::DataSetTypeConverter> converter(new te::da::DataSetTypeConverter(datasetType.get()));

  addColumns(converter, datasetType);

  converter->remove("FID");
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
    QUrl url(uri.c_str());
    QDir dir(url.path());
    QStringList localEntryList = dir.entryList(QDir::Files);
    std::vector<std::string> names(localEntryList.size());
    std::transform(localEntryList.begin(), localEntryList.end(), names.begin(), [](const QString& name){ return name.toStdString(); });

    names = filter->filterNames(names);

    if(names.empty())
    {
      //TODO: throw no dataset found
      return;
    }

    std::shared_ptr<te::da::DataSetTypeConverter> converter;
    bool first = true;
    //prepare the converter to adapt the dataset
    for(const std::string& name : names)
    {
      //create a datasource and open
      std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR"));
      std::map<std::string, std::string> connInfo;
      connInfo["URI"] = "CSV:"+url.path().toStdString()+"/"+name;
      datasource->setConnectionInfo(connInfo);

      //RAII for open/closing the datasource
      OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

      if(!datasource->isOpened())
      {
        throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                              QObject::tr("ParserOGR::read - DataProvider could not be opened."));
      }

      // get a transactor to interact to the data source
      std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

      QFileInfo nameInfo(name.c_str());
      if(first)
      {
        converter = getConverter(std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(nameInfo.baseName().toStdString())));
        datasetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
        assert(datasetType);
        first = false;
      }

      assert(converter);
      std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(nameInfo.baseName().toStdString()));
      assert(datasetOrig);
      std::shared_ptr<te::da::DataSet> dataset(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));

      datasetVec.push_back(dataset);
    }

    return;
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(
                                          QObject::tr("ParserOGR::read - Terralib exception: ") +e.what());
  }
  catch(std::exception& e)
  {
    throw UnableToReadDataSetError() << terrama2::ErrorDescription(QObject::tr("ParserOGR::read - Std exception.")+e.what());
  }

  return;
}
