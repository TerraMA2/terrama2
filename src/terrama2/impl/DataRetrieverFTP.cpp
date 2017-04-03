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
    CURLcode status = curlwrapper_.verifyURL(dataprovider->uri, dataProvider_->timeout);

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


std::vector<std::string> terrama2::core::DataRetrieverFTP::getFoldersList(const std::vector<std::string>& uris,
                                                                          const std::string& foldersMask)
{
  std::vector<std::string> maskList = splitString(foldersMask, '/');

  if(maskList.empty())
    return uris;

  std::vector<std::string> folders = uris;

  for(auto mask : maskList)
  {
    if(!mask.empty())
      folders = checkSubfolders(folders, mask);
  }

  if(folders.empty())
  {
    QString errMsg = QObject::tr("No directory matches the mask.");
    TERRAMA2_LOG_ERROR() << errMsg;
    return {};
  }

  return folders;
}

std::vector<std::string> terrama2::core::DataRetrieverFTP::checkSubfolders(const std::vector<std::string> baseURIs, const std::string mask)
{
  std::vector<std::string> folders;

  for(const auto& uri : baseURIs)
  {
    curlwrapper_.init();

    std::vector<std::string> dirList = curlwrapper_.getFtpListDir(uri,
                                                                  &terrama2::core::DataRetrieverFTP::write_vector,
                                                                  dataProvider_->timeout);

    if(dirList.empty())
    {
      continue;
    }

    for(const auto& folder : dirList)
    {
      if(!terramaMaskMatch(mask, folder))
        continue;

      folders.push_back(uri + "/" + folder + "/");
    }
  }

  return folders;
}


std::string terrama2::core::DataRetrieverFTP::retrieveData(const std::string& mask,
                                                           const Filter& filter,
                                                           std::shared_ptr<terrama2::core::FileRemover> remover,
                                                           const std::string& temporaryFolderUri,
                                                           const std::string& foldersMask)
{
  std::string downloadBaseFolderUri = temporaryFolderUri + "/";

  if(temporaryFolderUri.empty())
  {
    boost::filesystem::path tempDir = boost::filesystem::temp_directory_path();
    boost::filesystem::path tempTerrama(tempDir.string()+"/terrama2");
    boost::filesystem::path downloadBaseDir = boost::filesystem::unique_path(tempTerrama.string()+"/%%%%-%%%%-%%%%-%%%%");

    // Create the directory where you will download the files.
    QDir dir(QString::fromStdString(downloadBaseDir.string()));
    if(!dir.exists())
      dir.mkpath(QString::fromStdString(downloadBaseDir.string()));

    std::string scheme = "file://";
    downloadBaseFolderUri = scheme + downloadBaseDir.string();
    remover->addTemporaryFolder(downloadBaseFolderUri);

    downloadBaseFolderUri = scheme + downloadBaseDir.string();
  }

  curlwrapper_.init();

  if(!curlwrapper_.fcurl())
  {
    QString errMsg = QObject::tr("Error retrieving data via FTP.\n");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  try
  {
    // find valid directories
    std::vector< std::string > baseUriList;
    baseUriList.push_back(dataProvider_->uri);

    if(!foldersMask.empty())
    {
      auto uriList = getFoldersList(baseUriList, foldersMask);

      if(uriList.empty())
      {
        QString errMsg = QObject::tr("No files found!");
        TERRAMA2_LOG_WARNING() << errMsg;
        throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
      }

      baseUriList = uriList;
    }

    // Get a file listing from server
    for(const auto& uri : baseUriList)
    {
      curlwrapper_.init();

      if(!curlwrapper_.fcurl())
      {
        QString errMsg = QObject::tr("Error retrieving data via FTP.\n");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw DataRetrieverException() << ErrorDescription(errMsg);
      }

      std::vector<std::string> vectorFiles = curlwrapper_.getFtpListFiles(uri, &terrama2::core::DataRetrieverFTP::write_vector, dataProvider_->timeout);

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

      if(vectorNames.empty())
      {
        continue;
      }

      // Create directory struct
      QString saveDir(QString::fromStdString(uri));
      saveDir.replace(QString::fromStdString(dataProvider_->uri), QString::fromStdString(downloadBaseFolderUri));

      QString savePath = QUrl(saveDir).toString(QUrl::RemoveScheme);
      QDir dir(savePath);
      if(!dir.exists())
        dir.mkpath(savePath);

      // Performs the download of files in the vectorNames
      for(const auto& file: vectorNames)
      {
        curlwrapper_.init();

        if(!curlwrapper_.fcurl())
        {
          QString errMsg = QObject::tr("Error retrieving data via FTP.\n");
          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataRetrieverException() << ErrorDescription(errMsg);
        }

        std::string uriOrigin = uri + "/" + file;

        std::string filePath = savePath.toStdString() + "/" + file;

        CURLcode res = curlwrapper_.getDownloadFiles(uriOrigin, &terrama2::core::DataRetrieverFTP::write_response, filePath, dataProvider_->timeout);
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
