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

void terrama2::collector::ParserOGR::read(DataFilterPtr filter, std::vector<TransferenceData>& transferenceDataVec)
{
  if(transferenceDataVec.empty())
    return;

  dataSetItem_ = transferenceDataVec.at(0).datasetItem;

  try
  {
    std::shared_ptr<te::da::DataSetTypeConverter> converter;
    bool first = true;

    if(transferenceDataVec.empty())
      throw NoDataSetFoundException() << ErrorDescription(QObject::tr("No DataSet Found."));

    for(auto& transferenceData : transferenceDataVec)
    {
      QUrl uri(transferenceData.uri_temporary.c_str());

      QFileInfo nameInfo(uri.path());
      if(uri.scheme() != "file" || !nameInfo.exists() || nameInfo.isDir())
        throw InvalidFolderException() << ErrorDescription(QObject::tr("Invalid folder."));//FIXME: invalid file

      if(!filter->filterName(nameInfo.fileName().toStdString()))
        continue;

      //create a datasource and open
      std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR"));
      std::map<std::string, std::string> connInfo;
      connInfo["URI"] = "CSV:"+nameInfo.absoluteFilePath().toStdString();
      datasource->setConnectionInfo(connInfo);

      //RAII for open/closing the datasource
      OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

      if(!datasource->isOpened())
      {
        throw UnableToReadDataSetException() << ErrorDescription(QObject::tr("ParserOGR::read - DataProvider could not be opened."));
      }

      // get a transactor to interact to the data source
      std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

      if(first)
      {
        converter = getConverter(std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(nameInfo.baseName().toStdString())));
        transferenceData.teDatasetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
        assert(transferenceData.teDatasetType);
        first = false;
      }

      assert(converter);
      std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(nameInfo.baseName().toStdString()));
      assert(datasetOrig);
      std::shared_ptr<te::da::DataSet> dataset(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));

      transferenceData.teDataset = dataset;
    }

    return;
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
    throw UnableToReadDataSetException() << ErrorDescription(QObject::tr("ParserOGR::read - Terralib exception: ") +e.what());
  }
  catch(terrama2::collector::Exception& e)
  {
    throw;
  }
  catch(std::exception& e)
  {
    throw UnableToReadDataSetException() << ErrorDescription(QObject::tr("ParserOGR::read - Std exception.")+e.what());
  }

  return;
}
