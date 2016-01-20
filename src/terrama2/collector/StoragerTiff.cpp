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
#include "../core/Logger.hpp"

#include "../core/DataManager.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/rp/Functions.h>

//Qt
#include <QDebug>

//boost
#include <boost/date_time/local_time/local_date_time.hpp>

terrama2::collector::StoragerTiff::StoragerTiff(const std::map<std::string, std::string>& storageMetadata)
  : Storager(storageMetadata)
{

}

void terrama2::collector::StoragerTiff::store(std::vector<TransferenceData>& transferenceDataVec)
{
  if(transferenceDataVec.empty())
    return;

  try
  {
    //get filename
    std::map<std::string, std::string>::const_iterator dataSetNameIt = metadata_.find("STORAGE_NAME");
    if(dataSetNameIt != metadata_.end())
    {
      std::string dataSetName = dataSetNameIt->second;
      std::string tif = ".tif", tiff = ".tiff";
      //if does not have tiff extension, append ".tiff"
      if(!(std::equal(tif.rbegin(), tif.rend(), dataSetName.rbegin()) || std::equal(tiff.rbegin(), tiff.rend(), dataSetName.rbegin())))
         dataSetName.append(tif);

      if(dataSetName.find('%'))
      {
        //for each name...
      }
    }
    else
    {
      core::DataSet dataset = transferenceDataVec.at(0).dataSet;

      for(TransferenceData& transferenceData : transferenceDataVec)
      {
        std::ostringstream timeStream;
        boost::posix_time::time_facet * f = new boost::posix_time::time_facet("%Y%m%d_%H%M");
        timeStream.imbue(std::locale(timeStream.getloc(),f));

        if(transferenceData.dateData)
          timeStream << transferenceData.dateData->getTimeInstantTZ().local_time();
        else if(transferenceData.dateCollect)
          timeStream << transferenceData.dateCollect->getTimeInstantTZ().local_time();
        else
          timeStream << "00000000_0000";

        std::string destinationDataSetName = "terrama2.";
        destinationDataSetName.append(dataset.name());
        destinationDataSetName.append("_");
        destinationDataSetName.append(timeStream.str());
        destinationDataSetName.append(".tif");

        //TODO: Terrama default dir
        std::string terrama2DefaultDir = "/tmp/";

        std::string storageURI = terrama2DefaultDir + destinationDataSetName;
        //TODO: verify if has raster?
        //TODO: Verify if it's compatible?
        std::unique_ptr<te::rst::Raster> raster(transferenceData.teDataSet->getRaster("raster"));
        te::rp::Copy2DiskRaster(*raster, storageURI);

        transferenceData.uriStorage = storageURI;
      }//for each dataset end
    }//else: no name set
  }
  catch(terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
  }
  catch(te::common::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}
