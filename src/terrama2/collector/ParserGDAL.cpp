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
  \file terrama2/collector/ParserTiff.hpp

  \brief Parsers postgres/postgis data and create a terralib DataSet.

  \author Jano Simas
*/

#include "ParserGDAL.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

//Qt
#include <QUrl>
#include <QString>
#include <QFileInfo>
#include <QtDebug>

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>

void terrama2::collector::ParserGDAL::read(terrama2::collector::DataFilterPtr filter, std::vector<terrama2::collector::TransferenceData>& transferenceDataVec)
{
  if(transferenceDataVec.empty())
    throw NoDataSetFoundException() << ErrorDescription(QObject::tr("No DataSet Found."));

  dataSetItem_ = transferenceDataVec.at(0).datasetItem;

  try
  {
    for(auto& transferenceData : transferenceDataVec)
    {
      QUrl uri(transferenceData.uri_temporary.empty() ? transferenceData.uri_origin.c_str() : transferenceData.uri_temporary.c_str());

      QFileInfo fileInfo(uri.path());
      if(uri.scheme() != "file" || !fileInfo.exists() || fileInfo.isDir())
        throw InvalidFileException() << ErrorDescription(QObject::tr("Invalid file %1.").arg(fileInfo.fileName()));

      if(!filter->filterName(fileInfo.fileName().toStdString()))
        continue;

      if(!verifyFileName(fileInfo.fileName().toStdString()))
        continue;

      //create a datasource and open
      std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("GDAL"));

      std::map<std::string, std::string> connInfo;
      connInfo["URI"] = fileInfo.absoluteFilePath().toStdString();

      fillConnectionInfo(connInfo);
      datasource->setConnectionInfo(connInfo);


      //RAII for open/closing the datasource
      OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);
      if(!datasource->isOpened())
      {
        throw UnableToReadDataSetException() << ErrorDescription(QObject::tr("ParserTiff::read - DataProvider could not be opened."));
      }

      // get a transactor to interact to the data source
      std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
      transferenceData.teDataset = std::shared_ptr<te::da::DataSet>(transactor->getDataSet(fileInfo.fileName().toStdString()));
      transferenceData.teDatasetType = std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(fileInfo.fileName().toStdString()));
    }
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
    throw UnableToReadDataSetException() << ErrorDescription(QObject::tr("ParserTiff::read - Terralib exception: ") +e.what());
  }
  catch(terrama2::collector::Exception& /*e*/)
  {
    throw;
  }
  catch(std::exception& e)
  {
    throw UnableToReadDataSetException() << ErrorDescription(QObject::tr("ParserTiff::read - Std exception.")+e.what());
  }

  return;
}
