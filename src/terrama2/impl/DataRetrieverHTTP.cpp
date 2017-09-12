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
  \file terrama2/impl/DataRetrieverHTTP.cpp

  \brief Data Retriever HTTP.

  \author Jean Souza
*/

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

// TerraMA2
#include "DataRetrieverHTTP.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/interpreter/InterpreterFactory.hpp"

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

terrama2::core::DataRetrieverHTTP::DataRetrieverHTTP(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperHttp>&& curlwrapper)
  : DataRetriever(dataprovider),
    curlwrapper_(std::move(curlwrapper))
{
  // Verifies if the HTTP address is valid
  try
  {
    curlwrapper_->verifyURL(dataProvider_->uri, dataProvider_->timeout);
  }
  catch(const te::Exception& e)
  {
    QString errMsg = QObject::tr("HTTP address is invalid! \n\n Details: \n");
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

bool terrama2::core::DataRetrieverHTTP::isRetrivable() const noexcept
{
  return true;
}

terrama2::core::DataRetrieverHTTP::~DataRetrieverHTTP()
{

}

std::vector<std::string> terrama2::core::DataRetrieverHTTP::listFiles(const std::string& uri)
{
  std::vector<std::string> dirList = curlwrapper_->listFiles(te::core::URI(uri));
  std::string httpServerHtml;

  for(auto const& s : dirList) {
    httpServerHtml += s;
  }

  boost::replace_all(httpServerHtml, "\"", "\\\"");
  httpServerHtml.erase(std::remove(httpServerHtml.begin(), httpServerHtml.end(), '\n'), httpServerHtml.end());

  std::string scriptPath = FindInTerraMA2Path("scripts/parse-http-server-html.py");
  std::ifstream scriptIfs(scriptPath);
  std::string script((std::istreambuf_iterator<char>(scriptIfs)), (std::istreambuf_iterator<char>()));
  boost::replace_all(script, "{HTML_CODE}", httpServerHtml);

  std::vector<std::string> vectorFiles;
  try
  {
    auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
    interpreter->runScript(script);
    auto fileNames = interpreter->getString("files");

    if(fileNames)
      boost::split(vectorFiles, *fileNames, [](char c){return c == ',';});
  }
  catch (const InterpreterException& e)
  {
    QString errMsg = "Error listing files:\n";
    errMsg.append(boost::get_error_info<terrama2::ErrorDescription>(e));
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  return vectorFiles;
}

std::string terrama2::core::DataRetrieverHTTP::retrieveData(const std::string& mask,
                                                           const Filter& filter,
                                                           const std::string& timezone,
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
  }

  try
  {
    std::vector<std::string> vectorFiles = listFiles(dataProvider_->uri + "/" + foldersMask);

    std::vector<std::string> vectorNames;
    // filter file names that should be downloaded.
    for(const std::string& fileName: vectorFiles)
    {
      // FIXME: use timestamp
      std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
      if(terrama2::core::isValidDataSetName(mask,filter, timezone.empty() ? "UTC+00" : timezone, fileName,timestamp))
        vectorNames.push_back(fileName);
    }

    if(!vectorNames.empty())
    {
      // Create directory struct
      QString saveDir(QString::fromStdString(downloadBaseFolderUri+ "/" + foldersMask));
      QString savePath = QUrl(saveDir).toString(QUrl::RemoveScheme);
      QDir dir(savePath);
      if(!dir.exists())
        dir.mkpath(savePath);

      // Performs the download of files in the vectorNames
      for(const auto& file: vectorNames)
      {
        std::string uriOrigin = dataProvider_->uri + "/" + foldersMask + "/" + file;
        std::string filePath = savePath.toStdString() + "/" + file;

        te::core::URI uri(uriOrigin);

        std::string user = uri.user();
        std::string password = uri.password();

        if(!user.empty() && !password.empty())
        {
          curlwrapper_->setAuthenticationMethod(te::ws::core::HTTP_BASIC);
          curlwrapper_->setUsername(user);
          curlwrapper_->setPassword(password);
        }

        try
        {
          curlwrapper_->downloadFile(uriOrigin, filePath);
        }
        catch(const te::Exception& e)
        {
          // Creating a lambda to use in the catch block, the purpose of this is to avoid code repetition
          auto downloadException = [&file](const te::Exception& e) {
            QString errMsg = QObject::tr("Error during download of file %1.\n").arg(QString::fromStdString(file));
            auto errStr = boost::get_error_info<te::ErrorDescription>(e);
            if(errStr)
              errMsg.append(QString::fromStdString(*errStr));
            errMsg.append(e.what());

            TERRAMA2_LOG_ERROR() << errMsg;
            throw DataRetrieverException() << ErrorDescription(errMsg);
          };

          if(!user.empty() && !password.empty())
          {
            try
            {
              curlwrapper_->setAuthenticationMethod(te::ws::core::HTTP_DIGEST);
              curlwrapper_->downloadFile(uriOrigin, filePath);
            }
            catch(const te::Exception& e)
            {
              downloadException(e);
            }
          }
          else
          {
            downloadException(e);
          }
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

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverHTTP::make(DataProviderPtr dataProvider)
{
  std::unique_ptr<CurlWrapperHttp> curlwrapper(new CurlWrapperHttp());
  return std::make_shared<DataRetrieverHTTP>(dataProvider, std::move(curlwrapper));
}
