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
  \file terrama2/collector/DataRetrieverWCS.cpp

  \brief Data Retriever WCS.

 \author Vinicius Campanha
*/

// LibCurl
#include <curl/curl.h>

// TerraLib
#include <terralib/qt/plugins/datasource/wcs/WCSConnector.h>

// TerraMA2
#include "CurlOpener.hpp"
#include "DataRetrieverWCS.hpp"
#include "Exception.hpp"



bool terrama2::collector::DataRetrieverWCS::isRetrivable() const noexcept
{
  return true;
}

void terrama2::collector::DataRetrieverWCS::open()
{

}

bool terrama2::collector::DataRetrieverWCS::isOpen()
{
  try
  {
    CurlOpener curl;
    curl.init();

    CURLcode status;

    if(curl.fcurl())
    {
      curl_easy_setopt(curl.fcurl(), CURLOPT_URL, "http://flanche.net:9090/rasdaman/ows?service=WCS&request=GetCapabilities");

      /* Perform the request, status will get the return code */
      status = curl_easy_perform(curl.fcurl());

      /* Check for errors */
      if(status != CURLE_OK)
        return false;
    }
  }
  catch(const std::exception& e)
  {
    QString messageError = QObject::tr("Could not perform the files download! \n\n Details: \n");
    messageError.append(e.what());

    throw DataRetrieverWCSException() << ErrorDescription(messageError);
  }
  catch(...)
  {
    throw DataRetrieverWCSException() << ErrorDescription(QObject::tr("Unknown Error, Could not perform the files download!"));
  }


  return true;
}

void terrama2::collector::DataRetrieverWCS::close()
{

}

 std::string terrama2::collector::DataRetrieverWCS::retrieveData(const terrama2::core::DataSetItem& datasetitem,
                                                                 DataFilterPtr filter,
                                                                 std::vector<terrama2::collector::TransferenceData>& transferenceDataVec)
 {
    return "";
 }
