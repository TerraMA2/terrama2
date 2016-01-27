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

// Qt
#include <QXmlStreamReader>

// TerraLib
#include <terralib/qt/plugins/datasource/wcs/WCSConnector.h>

// TerraMA2
#include "CurlOpener.hpp"
#include "DataRetrieverWCS.hpp"
#include "Exception.hpp"
#include "FileOpener.hpp"


terrama2::collector::DataRetrieverWCS::DataRetrieverWCS(const core::DataProvider& dataprovider)
  : DataRetriever(dataprovider)
{
  folder_ = "/tmp/";
}


bool terrama2::collector::DataRetrieverWCS::isRetrivable() const noexcept
{
  return true;
}


void terrama2::collector::DataRetrieverWCS::open()
{

}


size_t write_xml_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t sizeRead = size * nmemb;

  std::string* block = (std::string*) data;
  block->append((char *)ptr, sizeRead);

  return sizeRead;
}


size_t write_file_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}


bool terrama2::collector::DataRetrieverWCS::isOpen()
{
  try
  {
    CurlOpener curl;
    curl.init();

    CURLcode status;

    std::string xml;

    // Verifies that the WCS address is valid
    if(curl.fcurl())
    {
      // VINICIUS: WCS version on uri
      std::string url = dataprovider_.uri() + "&REQUEST=GetCapabilities";
      curl_easy_setopt(curl.fcurl(), CURLOPT_URL, url.c_str());

      // Get data to be written
      curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEFUNCTION, write_xml_callback);
      // Set a pointer to our xml string
      curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEDATA, (void*)&xml);
      /* Perform the request, status will get the return code */
      status = curl_easy_perform(curl.fcurl());

      // Check for errors
      if(status != CURLE_OK)
        return false;

      // Validate the xml response
      QXmlStreamReader xml_stream(xml.c_str());

      if(xml_stream.hasError())
        return false;

      if(!xml_stream.readNextStartElement())
        return false;

      if(xml_stream.name().compare("Capabilities") != 0)
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
   std::string filePath = "";

   try
   {
     CurlOpener curl;
     curl.init();

     CURLcode status;

     if(curl.fcurl())
     {
       std::string url = dataprovider_.uri();

       for(auto metadataItem : datasetitem.metadata())
       {
         url += "&" + metadataItem.first + "=" + metadataItem.second;
       }

       filePath = folder_ + std::to_string(datasetitem.dataset()) + "_" + std::to_string(datasetitem.id());
       FileOpener opener(filePath.c_str(),"wb");

       curl_easy_setopt(curl.fcurl(), CURLOPT_URL, url.c_str());

       // Get data to be written
       curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEFUNCTION, write_file_callback);
       // Set a pointer to our xml string
       curl_easy_setopt(curl.fcurl(), CURLOPT_WRITEDATA, opener.file());
       /* Perform the request, status will get the return code */
       status = curl_easy_perform(curl.fcurl());

       // Check for errors
       if(status != CURLE_OK)
         return "";

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

   return filePath;
 }
