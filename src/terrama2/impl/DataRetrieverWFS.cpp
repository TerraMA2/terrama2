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
  \file terrama2/impl/DataRetrieverWFS.cpp
*/

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <string>

// TerraMA2
#include "DataRetrieverWFS.hpp"
#include "DataAccessorWFS.hpp"
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

terrama2::core::DataRetrieverWFS::DataRetrieverWFS(DataProviderPtr dataprovider,
   std::unique_ptr<CurlWrapperHttp> &&curlwrapper)
  : DataRetriever(dataprovider)
{
}

bool terrama2::core::DataRetrieverWFS::isRetrivable() const noexcept
{
  return true;
}

std::string terrama2::core::DataRetrieverWFS::getOuputfilePath(WFSParameters parameters) const
{
  std::string scriptPath = FindInTerraMA2Path("share/terrama2/scripts/wfsclient/client/main.py");
  std::string script = readFileContents(scriptPath);

  std::map<std::string, std::string> inputParameters;

  inputParameters["host"] = parameters.host;
  inputParameters["version"] = parameters.version;
  inputParameters["feature"] = parameters.feature;
  inputParameters["xmin"] = parameters.xmin;
  inputParameters["ymin"] = parameters.ymin;
  inputParameters["xmax"] = parameters.xmax;
  inputParameters["ymax"] = parameters.ymax;
  inputParameters["projection"] = parameters.projection;
  inputParameters["tempFolder"] = parameters.tempFolder;

  auto outputfilePath = "";

  try
  {
    auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
    interpreter->runScript(script, inputParameters);

//    auto fileNames = interpreter->getString("outputfile_path");

//    outputfilePath = fileNames;
  }
  catch (const InterpreterException& e)
  {
    QString errMsg = "Error listing files:\n";
    errMsg.append(boost::get_error_info<terrama2::ErrorDescription>(e));
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  return outputfilePath;
}

std::string terrama2::core::DataRetrieverWFS::retrieveData(const std::string& mask,
                                                           const Filter& filter,
                                                           const std::string& timezone,
                                                           std::shared_ptr<terrama2::core::FileRemover> remover,
                                                           const std::string& temporaryFolderUri,
                                                           const std::string& foldersMask) const
{
  std::string downloadBaseFolderUri = getTemporaryFolder(remover, temporaryFolderUri);

  try
  {
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

void terrama2::core::DataRetrieverWFS::retrieveDataCallback(const std::string& mask,
                                                            const terrama2::core::Filter& filter,
                                                            const std::string& timezone,
                                                            std::shared_ptr<terrama2::core::FileRemover> remover,
                                                            const std::string& temporaryFolderUri,
                                                            const std::string& foldersMask,
                                                            std::function<void(const std::string &, const std::string &, const std::string &)> processFile) const
{
  try
  {
    if(filter.region == nullptr)
      return;

    auto mbr = filter.region.get()->getEnvelope()->getMBR();

    WFSParameters parameters;
    parameters.host = dataProvider_->uri;
    parameters.version = "1.1.0";
    parameters.feature = mask;
    parameters.xmin = std::to_string(mbr->getLowerLeftX());
    parameters.ymin = std::to_string(mbr->getLowerLeftY());
    parameters.xmax = std::to_string(mbr->getUpperRightX());
    parameters.ymax = std::to_string(mbr->getUpperRightY());
    parameters.projection = "EPSG:31468";

    auto temporaryDataDir = getTemporaryFolder(remover, temporaryFolderUri);

//    parameters.tempFolder = terrama2::core::getTemporaryFolder(remover);
    parameters.tempFolder = temporaryDataDir;

    auto fileName = mask;

//    if(terrama2::core::isValidDataSetName(mask,filter, timezone.empty() ? "UTC+00" : timezone, fileName,timestamp))

    std::string outputfilePath = getOuputfilePath(parameters);

    processFile(temporaryDataDir, fileName+".zip", "");
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

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverWFS::make(DataProviderPtr dataProvider)
{
  std::unique_ptr<CurlWrapperHttp> curlwrapper(new CurlWrapperHttp());
  return std::make_shared<DataRetrieverWFS>(dataProvider, std::move(curlwrapper));
}
