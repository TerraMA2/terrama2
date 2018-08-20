/*
  Copyright (C) 2017 National Institute For Space Research (INPE) - Brazil.

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
  \file terrama2/impl/DataRetrieverWCS.cpp

  \brief Data Retriever WCS.

  \author Ricardo Pontes Bonfiglioli
*/

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

// TerraMA2
#include "DataRetrieverWCS.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/data-model/Filter.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/Utils.hpp"

// TerraLib
#include <terralib/core/uri/Utils.h>
#include <terralib/core/Exception.h>
#include <terralib/ws/core/Exception.h>
#include <terralib/ws/ogc/wcs/client/WCSClient.h>
#include <terralib/ws/ogc/wcs/dataaccess/Exception.h>

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

// QT
#include <QObject>
#include <QDir>
#include <QDebug>
#include <QUrl>

terrama2::core::DataRetrieverWCS::DataRetrieverWCS(DataProviderPtr dataprovider)
  : DataRetriever(dataprovider)
{
}

te::core::URI terrama2::core::DataRetrieverWCS::getConnectionInfo(std::string& usrDataDir) const
{
  std::string strURI("wcs://");

  // Get the server URL
  std::string serviceURL = dataProvider_->uri;//m_ui->m_serverLineEdit->text().trimmed().toUtf8().constData();

  if (serviceURL.empty())
  {
    QString errMsg = QObject::tr("Please define the server address first!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  std::string encodedURL = te::core::URIEncode(serviceURL);

  //std::string usrDataDir = //te::qt::af::AppCtrlSingleton::getInstance().getUserDataDir().toUtf8().data();

  strURI = strURI + "?URI="+ encodedURL + "&VERSION=2.0.1" + "&USERDATADIR=" + usrDataDir;

  te::core::URI uri(strURI);

  return uri;
}

void terrama2::core::DataRetrieverWCS::verifyConnectionInfo(const te::core::URI& uri) const
{
  if(!uri.isValid())
  {
    QString errMsg = QObject::tr("The connection information is invalid!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }

  std::map<std::string, std::string> kvp = te::core::Expand(uri.query());
  std::map<std::string, std::string>::const_iterator it = kvp.begin();
  std::map<std::string, std::string>::const_iterator itend = kvp.end();

  it = kvp.find("URI");
  if (it == itend || it->second.empty())
  {
    QString errMsg = QObject::tr("The connection information is invalid. Missing URI parameter!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
  it = kvp.find("VERSION");
  if (it == itend || it->second.empty())
  {
    QString errMsg = QObject::tr("The connection information is invalid. Missing VERSION parameter!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
  it = kvp.find("USERDATADIR");
  if (it == itend || it->second.empty())
  {
    QString errMsg = QObject::tr("The connection information is invalid. Missing USERDATADIR parameter!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
}


bool terrama2::core::DataRetrieverWCS::isRetrivable() const noexcept
{
  return true;
}

terrama2::core::DataRetrieverWCS::~DataRetrieverWCS()
{

}

std::string terrama2::core::DataRetrieverWCS::retrieveData(const std::string& mask,
                                                           const Filter& filter,
                                                           const std::string& timezone,
                                                           std::shared_ptr<terrama2::core::FileRemover> remover,
                                                           const std::string& temporaryFolderUri,
                                                           const std::string& foldersMask)
{
  std::string downloadBaseFolderUri = temporaryFolderUri + "/";

  std::string userDataDir = downloadBaseFolderUri;

  if(temporaryFolderUri.empty())
  {
    boost::filesystem::path tempDir = boost::filesystem::temp_directory_path();
    boost::filesystem::path tempTerrama(tempDir.string()+"/terrama2");
    boost::filesystem::path downloadBaseDir = boost::filesystem::unique_path(tempTerrama.string()+"/%%%%-%%%%-%%%%-%%%%");

    // Create the directory where you will download the files.
    QDir dir(QString::fromStdString(downloadBaseDir.string()));
    if(!dir.exists())
      dir.mkpath(QString::fromStdString(downloadBaseDir.string()));

    userDataDir = downloadBaseDir.string();

    std::string scheme = "FILE:/";
    downloadBaseFolderUri = scheme + downloadBaseDir.string();
    remover->addTemporaryFolder(downloadBaseFolderUri);

    downloadBaseFolderUri = scheme + downloadBaseDir.string();
  }

  te::core::URI uri = this->getConnectionInfo(userDataDir);

  verifyConnectionInfo(uri);

  // instantiate WCSClient
  try
  {
    std::map<std::string, std::string> kvp = te::core::Expand(uri.query());

    wcs_.reset(new te::ws::ogc::WCSClient(kvp["USERDATADIR"], kvp["URI"], kvp["VERSION"]));

    wcs_->updateCapabilities();
  }
  catch(const te::ws::core::Exception& e)
  {
    throw;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Unknow error!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }


  // get coverages
  std::vector< std::string > coverages = wcs_->getCapabilities().coverages;

  for(unsigned int i = 0; i < coverages.size(); i ++)
  {
    te::ws::ogc::wcs::CoverageRequest coverageRequest = te::ws::ogc::wcs::CoverageRequest();
    coverageRequest.coverageID = coverages.at(i);
    coverageRequest.format = "image/tiff";

    std::string coveragePath = wcs_->getCoverage(coverageRequest);
  }


  // returns the absolute path of the folder that contains the files that have been made the download.
  return downloadBaseFolderUri;
}

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverWCS::make(DataProviderPtr dataProvider)
{
  return std::make_shared<DataRetrieverWCS>(dataProvider);
}
