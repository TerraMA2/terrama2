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
  \file terrama2/impl/DataRetrieverFTP.cpp

  \brief Data Retriever FTP.

  \author Evandro Delatin
*/

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

// TerraMA2
#include "DataRetrieverFTP.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/Utils.hpp"

// Libcurl
#include <curl/curl.h>

// Boost
#include <boost/algorithm/string.hpp>

// QT
#include <QObject>
#include <QDir>
#include <QDebug>

terrama2::core::DataRetrieverFTP::DataRetrieverFTP(DataProviderPtr dataprovider, CurlPtr&& curlwrapper)
  : DataRetriever(dataprovider),
    curlwrapper_(std::move(curlwrapper))
{
  temporaryFolder_ = "/tmp/terrama2-download/";
  scheme_ = "file://";

  // Create the directory where you will download the files.
  QDir dir(temporaryFolder_.c_str());
  if (!dir.exists())
    dir.mkpath(temporaryFolder_.c_str());

  CURLcode status;
  curlwrapper_.init();

  // Verifies that the FTP address is valid
  try
  {
    std::string url = dataprovider->uri.c_str();

    status = curlwrapper_.verifyURL(url);

    if (status != CURLE_OK)
    {
      QString errMsg = QObject::tr("FTP address is invalid. \n\n");
      errMsg.append(curl_easy_strerror(status));

      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataRetrieverException() << ErrorDescription(errMsg);
    }
  }
  catch(const std::exception& e)
  {
    QString errMsg = QObject::tr("FTP address is invalid! \n\n Details: \n");
    errMsg.append(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
  catch(...)
  {
    throw DataRetrieverException() << ErrorDescription(QObject::tr("Unknown Error, FTP address is invalid!"));
  }

}

bool terrama2::core::DataRetrieverFTP::isRetrivable() const noexcept
{
  return true;
}

terrama2::core::DataRetrieverFTP::~DataRetrieverFTP()
{

}

size_t terrama2::core::DataRetrieverFTP::write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}

size_t terrama2::core::DataRetrieverFTP::write_vector(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t sizeRead = size * nmemb;

  std::string* block = (std::string*) data;
  block->append((char *)ptr, sizeRead);

  return sizeRead;
}

std::string terrama2::core::DataRetrieverFTP::retrieveData(const std::string& mask, const terrama2::core::Filter& filter)
{
  std::string uriOrigin;
  std::string uriInput;

  std::vector<std::string> vectorFiles;
  std::string block;

  curlwrapper_.init();

  try
  {
    // Get a file listing from server
    if(curlwrapper_.fcurl())
    {
      uriInput = dataProvider_->uri+"/";

      vectorFiles = curlwrapper_.getListFiles(uriInput, &terrama2::core::DataRetrieverFTP::write_vector, block);

      // filter file names that should be downloaded.
      for (std::string fileName: vectorFiles)
      {
        // FIXME: use timestamp
        std::string timezone = "UTC+00";//FIXME: get timezone from dataset
        std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
        if (terrama2::core::isValidDataSetName(mask,filter, timezone, fileName,timestamp))
          vectorNames_.push_back(fileName);
      }

      if (!vectorNames_.empty())
      {
        for (std::string file: vectorNames_)
        {
          CURLcode res;

          curlwrapper_.init();

          // Performs the download of files in the vectorNames
          if(curlwrapper_.fcurl())
          {
            uriOrigin = dataProvider_->uri +"/"+file;
            std::string filePath = temporaryFolder_+"/"+file;

            res = curlwrapper_.getDownloadFiles(uriOrigin, &terrama2::core::DataRetrieverFTP::write_response, filePath);

            if (res != CURLE_OK)
            {
              QString errMsg = QObject::tr("Could not perform the download! \n\n");
              errMsg.append(curl_easy_strerror(res));

              TERRAMA2_LOG_ERROR() << errMsg;
              throw DataRetrieverException() << ErrorDescription(errMsg);
            }
          }
        }
      }
      else
      {
        QString errMsg = QObject::tr("Could not found files! \n\n");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw DataRetrieverException() << ErrorDescription(errMsg);
      }
    }
  }
  catch(const std::exception& e)
  {
    QString errMsg = QObject::tr("Could not perform the download files! \n\n Details: \n");
    errMsg.append(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  catch(...)
  {
    throw DataRetrieverException() << ErrorDescription(QObject::tr("Unknown Error, Could not perform the download files!"));
  }

  // returns the absolute path of the folder that contains the files that have been made the download.
  return scheme_+temporaryFolder_;
}

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverFTP::make(DataProviderPtr dataProvider)
{
  CurlPtr curlwrapper;
  return std::make_shared<DataRetrieverFTP>(dataProvider, std::move(curlwrapper));
}
