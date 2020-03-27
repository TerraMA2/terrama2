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
#include <terralib/core/Exception.h>

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

terrama2::core::DataRetrieverFTP::DataRetrieverFTP(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperFtp>&& curlwrapper)
  : DataRetriever(dataprovider),
    curlwrapper_(std::move(curlwrapper))
{
  //Set FTP mode
  try
  {
    auto activeMode = dataProvider_->options.at("active_mode");
    curlwrapper_->setActiveMode(activeMode == "true", "-");
  }
  catch(const std::out_of_range&)
  {
    //if not set, set to false
    curlwrapper_->setActiveMode(false);
  }

  // Verifies that the FTP address is valid
  try
  {
    curlwrapper_->verifyURL(dataprovider->uri, dataProvider_->timeout);
  }
  catch(const te::Exception& e)
  {
    QString errMsg = QObject::tr("FTP address is invalid! \n\n Details: \n");
    auto errStr = boost::get_error_info<te::ErrorDescription>(e);
    if(errStr)
      errMsg.append(QString::fromStdString(*errStr));
    errMsg.append(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
  catch(...)
  {
    throw DataRetrieverException() << ErrorDescription(QObject::tr("Unknown Error!"));
  }

}

bool terrama2::core::DataRetrieverFTP::isRetrivable() const noexcept
{
  return true;
}

std::vector<std::string> terrama2::core::DataRetrieverFTP::getFoldersList(const std::vector<std::string>& uris,
                                                                          const std::string& foldersMask,
                                                                          const std::string& timezone,
                                                                          const Filter &filter) const
{
  std::vector<std::string> maskList = splitString(foldersMask, '/');

  if(maskList.empty())
    return uris;

  std::vector<std::string> folders = uris;

  for(const auto& mask : maskList)
  {
    if(!mask.empty())
      folders = checkSubfolders(folders, mask, timezone, filter);
  }

  if(folders.empty())
  {
    QString errMsg = QObject::tr("No directory matches the mask.");
    TERRAMA2_LOG_ERROR() << errMsg;
    return {};
  }

  return folders;
}

std::vector<std::string> terrama2::core::DataRetrieverFTP::checkSubfolders(const std::vector<std::string> baseURIs, const std::string mask, const std::string& timezone, const Filter &filter) const
{
  std::vector<std::string> folders;

  for(const auto& uri : baseURIs)
  {
    std::vector<std::string> dirList = curlwrapper_->listFiles(te::core::URI(uri));

    if(dirList.empty())
    {
      continue;
    }

    for(const auto& folder : dirList)
    {
      std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
//      if(!terramaMaskMatch(mask, folder))
      if(!isValidDataSetName(mask, filter, timezone, folder, timestamp))
        continue;

      folders.push_back(uri + "/" + folder + "/");
    }
  }

  return folders;
}


std::string terrama2::core::DataRetrieverFTP::retrieveData(const std::string& mask,
                                                           const Filter& filter,
                                                           const std::string& timezone,
                                                           std::shared_ptr<terrama2::core::FileRemover> remover,
                                                           const std::string& temporaryFolderUri,
                                                           const std::string& foldersMask) const
{
  std::string downloadBaseFolderUri = getTemporaryFolder(remover, temporaryFolderUri);
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
      std::vector<std::string> vectorFiles = curlwrapper_->listFiles(te::core::URI(uri));

      std::vector<std::string> vectorNames;
      // filter file names that should be downloaded.
      for(const std::string& fileName: vectorFiles)
      {
        // FIXME: use timestamp
        std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
        if(terrama2::core::isValidDataSetName(mask,filter, timezone.empty() ? "UTC+00" : timezone, fileName,timestamp))
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
        std::string uriOrigin = uri + "/" + file;
        std::string filePath = savePath.toStdString() + "/" + file;

        try
        {
          curlwrapper_->downloadFile(uriOrigin, filePath);
        }
        catch(const te::Exception& e)
        {
          QString errMsg = QObject::tr("Error during download of file %1.\n").arg(QString::fromStdString(file));
          auto errStr = boost::get_error_info<te::ErrorDescription>(e);
          if(errStr)
            errMsg.append(QString::fromStdString(*errStr));
          errMsg.append(e.what());

          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataRetrieverException() << ErrorDescription(errMsg);
        }

        remover->addTemporaryFile(filePath);
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
  catch(const te::Exception& e)
  {
    QString errMsg = QObject::tr("Error during download.\n");
    errMsg.append(boost::get_error_info<terrama2::ErrorDescription>(e));
    errMsg.append(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
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
  std::unique_ptr<CurlWrapperFtp> curlwrapper(new CurlWrapperFtp());
  return std::make_shared<DataRetrieverFTP>(dataProvider, std::move(curlwrapper));
}

void terrama2::core::DataRetrieverFTP::retrieveDataCallback(const std::string& mask,
                                                            const Filter& filter,
                                                            const std::string& timezone,
                                                            std::shared_ptr<terrama2::core::FileRemover> remover,
                                                            const std::string& temporaryFolderUri,
                                                            const std::string& foldersMask,
                                                            std::function<void(const std::string &, const std::string &, const std::string&)> processFile) const
{
  try
  {
    // find valid directories
    std::vector< std::string > baseUriList;
    baseUriList.push_back(dataProvider_->uri);

    auto tz = timezone.empty() ? "UTC+00" : timezone;

    if(!foldersMask.empty())
    {
      auto uriList = getFoldersList(baseUriList, foldersMask, tz, filter);

      if(uriList.empty())
      {
        QString errMsg = QObject::tr("No files found!");
        TERRAMA2_LOG_WARNING() << errMsg;
        throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
      }

      baseUriList = uriList;
    }

    // Build URI to get PATH fragment
    te::core::URI dataProviderURI(dataProvider_->uri);
    // Set temporary directory. When empty, creates a new pointing to dataProvider Path.
    // In this way, we will have something like "temporaryDir/dataProviderPath"
    // It is important due the folder may contains temporal mask
    std::string temporaryDataDir = temporaryFolderUri;
    if (temporaryFolderUri.empty())
      temporaryDataDir = getTemporaryFolder(remover, temporaryFolderUri) + dataProviderURI.path();

    // flag if there is any files for the dataset
    bool hasData = false;
    // Get a file listing from server
    for(const auto& uri : baseUriList)
    {
      std::vector<std::string> vectorFiles = curlwrapper_->listFiles(normalizeURI(uri));

      std::vector<std::string> vectorNames;
      // filter file names that should be downloaded.
      for(const std::string& fileName: vectorFiles)
      {
        // FIXME: use timestamp
        std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
        if(terrama2::core::isValidDataSetName(mask,filter, tz, fileName,timestamp))
          vectorNames.push_back(fileName);
      }

      if(vectorNames.empty())
      {
        continue;
      }

      hasData = true;

      te::core::URI u(uri);
      std::string uriPath = QString::fromStdString(u.path()).replace(dataProviderURI.path().c_str(), "/").toStdString();

      // Performs the download of files in the vectorNames
      for(const auto& file: vectorNames)
      {
        // Create directory struct
        QString saveDir(QString::fromStdString(temporaryDataDir+ "/" + uriPath));
        QString savePath = QUrl(saveDir).toLocalFile();
        QDir dir(savePath);
        if(!dir.exists())
          dir.mkpath(savePath);

        std::string uriOrigin = uri + "/" + file;
        std::string filePath = savePath.toStdString() + "/" + file;

        remover->addTemporaryFolder(temporaryDataDir);
        remover->addTemporaryFile(filePath);

        try
        {
          curlwrapper_->downloadFile(normalizeURI(uriOrigin).uri(), filePath);
          TERRAMA2_LOG_WARNING() << QObject::tr("Finished downloading file: %1").arg(QString::fromStdString(file));
          processFile(temporaryDataDir, file, uriPath);
        }
        catch(const te::Exception& e)
        {
          QString errMsg = QObject::tr("Error during download of file %1.\n").arg(QString::fromStdString(file));
          auto errStr = boost::get_error_info<te::ErrorDescription>(e);
          if(errStr)
            errMsg.append(QString::fromStdString(*errStr));
          errMsg.append(e.what());

          TERRAMA2_LOG_ERROR() << errMsg;
          throw DataRetrieverException() << ErrorDescription(errMsg);
        }
      }
    }

    if(!hasData)
    {
      QString errMsg = QObject::tr("No data in the remote server.");
      TERRAMA2_LOG_WARNING() << QObject::tr("Not find data for %1.").arg(QString::fromStdString(mask));
      NoDataException() << ErrorDescription(errMsg);
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
  catch(const te::Exception& e)
  {
    QString errMsg = QObject::tr("Error during download.\n");
    errMsg.append(boost::get_error_info<terrama2::ErrorDescription>(e));
    errMsg.append(e.what());

    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
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
}
