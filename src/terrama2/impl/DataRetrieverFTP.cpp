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

// TerraLib
#include <terralib/core/uri/URI.h>

// Libcurl
#include <curl/curl.h>

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

// QT
#include <QObject>
#include <QDir>
#include <QDebug>
#include <QUrl>

terrama2::core::DataRetrieverFTP::DataRetrieverFTP(DataProviderPtr dataprovider, CurlPtr&& curlwrapper)
  : DataRetriever(dataprovider),
    curlwrapper_(std::move(curlwrapper))
{
  curlwrapper_.init();

  // Verifies that the FTP address is valid
  try
  {
    CURLcode status = curlwrapper_.verifyURL(dataprovider->uri);

    if(status != CURLE_OK)
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

size_t terrama2::core::DataRetrieverFTP::write_response(void* ptr, size_t size, size_t nmemb, void* data)
{
  FILE* writehere = (FILE*)data;
  return fwrite(ptr, size, nmemb, writehere);
}

size_t terrama2::core::DataRetrieverFTP::write_vector(void* ptr, size_t size, size_t nmemb, void* data)
{
  size_t sizeRead = size * nmemb;

  std::string* block = (std::string*) data;
  block->append((char*)ptr, sizeRead);

  return sizeRead;
}

std::string terrama2::core::DataRetrieverFTP::retrieveData(const std::string& mask,
                                                           const Filter& filter,
                                                           std::shared_ptr<terrama2::core::FileRemover> remover,
                                                           const std::string& temporaryFolderUri,
                                                           const std::string& folderPath)
{
  std::string downloadBaseFolderUri = temporaryFolderUri;
  std::string downloadFolderUri = temporaryFolderUri + "/" + folderPath + "/";

  if(temporaryFolderUri.empty())
  {
    boost::filesystem::path tempDir = boost::filesystem::temp_directory_path();
    boost::filesystem::path tempTerrama(tempDir.string()+"/terrama2");
    boost::filesystem::path downloadBaseDir = boost::filesystem::unique_path(tempTerrama.string()+"/%%%%-%%%%-%%%%-%%%%");
    boost::filesystem::path downloadDir(downloadBaseDir.string() + "/" + folderPath + "/");

    // Create the directory where you will download the files.
    QDir dir(QString::fromStdString(downloadDir.string()));
    if(!dir.exists())
      dir.mkpath(QString::fromStdString(downloadDir.string()));

    std::string scheme = "file://";
    downloadBaseFolderUri = scheme+downloadBaseDir.string();
    remover->addTemporaryFolder(downloadBaseFolderUri);

    downloadFolderUri = scheme + downloadDir.string();
  }

  curlwrapper_.init();
  try
  {
    te::core::URI uriInput(dataProvider_->uri + "/" + folderPath + "/");

    // Get a file listing from server
    if(curlwrapper_.fcurl())
    {
      std::string block;

      std::vector<std::string> vectorFiles = curlwrapper_.getListFiles(uriInput.uri(), &terrama2::core::DataRetrieverFTP::write_vector, block);

      std::vector<std::string> vectorNames;
      // filter file names that should be downloaded.
      for(std::string fileName: vectorFiles)
      {
        // FIXME: use timestamp
        std::string timezone = "UTC+00";//FIXME: get timezone from dataset
        std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
        if(terrama2::core::isValidDataSetName(mask,filter, timezone, fileName,timestamp))
          vectorNames.push_back(fileName);
      }

      if(!vectorNames.empty())
      {
        for(std::string file: vectorNames)
        {
          curlwrapper_.init();

          // Performs the download of files in the vectorNames
          if(curlwrapper_.fcurl())
          {
            std::string uriOrigin = uriInput.uri() + file;

            QUrl url(QString::fromStdString(downloadFolderUri));
            std::string filePath = url.path().toStdString()+"/"+file;

            CURLcode res = curlwrapper_.getDownloadFiles(uriOrigin, &terrama2::core::DataRetrieverFTP::write_response, filePath);
            remover->addTemporaryFile(filePath);

            if(res != CURLE_OK)
            {
              QString errMsg = QObject::tr("Error during download of file %1.\n").arg(QString::fromStdString(file));
              errMsg.append(curl_easy_strerror(res));

              TERRAMA2_LOG_ERROR() << errMsg;
              throw DataRetrieverException() << ErrorDescription(errMsg);
            }
          }
        }
      }
      else
      {
        QString errMsg = QObject::tr("No files found.");
        TERRAMA2_LOG_WARNING() << errMsg;
        throw NoDataException() << ErrorDescription(errMsg);
      }
    }
  }
  catch(const NoDataException&)
  {
    throw;
  }
  catch(const DataRetrieverException&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    QString errMsg = QObject::tr("Error during download.\n");
    errMsg.append(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
  catch(...)
  {
    throw DataRetrieverException() << ErrorDescription(QObject::tr("Unknown Error."));
  }

  // returns the absolute path of the folder that contains the files that have been made the download.
  return downloadBaseFolderUri;
}

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverFTP::make(DataProviderPtr dataProvider)
{
  CurlPtr curlwrapper;
  return std::make_shared<DataRetrieverFTP>(dataProvider, std::move(curlwrapper));
}
