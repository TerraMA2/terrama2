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
  \file terrama2/collector/StoragerTiff.cpp

  \brief Store a temporary terralib DataSet into the permanent Tiff image.

  \author Jano Simas
*/

#include "StoragerTiff.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

//Qt
#include <QDebug>

terrama2::collector::StoragerTiff::StoragerTiff(const std::map<std::string, std::string>& storageMetadata)
  : Storager(storageMetadata)
{

}

std::string terrama2::collector::StoragerTiff::store(const core::DataSetItem& dataSetItem, const std::vector<std::shared_ptr<te::da::DataSet> >& datasetVec, const std::shared_ptr<te::da::DataSetType>& dataSetType)
{
  try
  {
    //connection info
    std::map<std::string, std::string> connInfo;

    //get filename
    std::map<std::string, std::string>::const_iterator dataSetNameIt = storageMetadata_.find("STORAGE_NAME");
    if(dataSetNameIt != storageMetadata_.end())
    {
      std::string dataSetName = dataSetNameIt->second;
      std::string tif = ".tif", tiff = ".tiff";
      //if does not have tiff extension, append ".tiff"
      if(!(std::equal(tif.rbegin(), tif.rend(), dataSetName.rbegin()) || std::equal(tiff.rbegin(), tiff.rend(), dataSetName.rbegin())))
         dataSetName.append(tiff);

      if(dataSetName.find('%'))
      {
        //for each name...
      }
    }
    else
    {
      std::string destinationDataSetName = "terrama2.storager_";
      destinationDataSetName.append(std::to_string(dataSetItem.id()));
      destinationDataSetName.append(".tiff");

      //TODO: Terrama default dir
      std::string terrama2DefaultDir;
      connInfo.at("URI") = terrama2DefaultDir + destinationDataSetName;

      std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("OGR"));
      datasourceDestination->setConnectionInfo(connInfo);
      OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);

      std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
      te::da::ScopedTransaction scopedTransaction(*transactorDestination);

      std::map<std::string, std::string> options;
      std::shared_ptr<te::da::DataSetType> newDataSetType;

      if (!transactorDestination->dataSetExists(destinationDataSetName))
      {
        // create and save datasettype in the datasource destination
        newDataSetType = std::shared_ptr<te::da::DataSetType>(static_cast<te::da::DataSetType*>(dataSetType->clone()));

        newDataSetType->setName(destinationDataSetName);
        transactorDestination->createDataSet(newDataSetType.get(),options);
      }
      else
      {
        newDataSetType = transactorDestination->getDataSetType(destinationDataSetName);
      }

      //Get original geometry to get srid
      te::gm::GeometryProperty* geom = GetFirstGeomProperty(dataSetType.get());
      //configure if there is a geometry property
      if(geom)
      {
        GetFirstGeomProperty(newDataSetType.get())->setSRID(geom->getSRID());
        GetFirstGeomProperty(newDataSetType.get())->setGeometryType(te::gm::GeometryType);
      }

      for(const auto& tempDataSet : datasetVec)
        transactorDestination->add(newDataSetType->getName(), tempDataSet.get(), options);

      scopedTransaction.commit();
    }
  }
  catch(terrama2::Exception& e)
  {
    //TODO: log de erro
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(te::common::Exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
  }
  catch(std::exception& e)
  {
    //TODO: log de erro
    qDebug() << e.what();
  }

  return "";
}
