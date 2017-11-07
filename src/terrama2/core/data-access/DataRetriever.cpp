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
  \file terrama2/core/data-access/DataRetriever.cpp

  \brief

  \author Jano Simas
 */

#include "DataRetriever.hpp"
#include "../utility/Logger.hpp"
#include "../Exception.hpp"

//Qt
#include <QString>
#include <QObject>

#include <boost/filesystem/operations.hpp>

terrama2::core::DataRetriever::DataRetriever(DataProviderPtr dataProvider)
  : dataProvider_(dataProvider)
{
  if(!dataProvider_.get())
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataRetriever::retrieveData(const std::string& /*query*/,
                                                        const Filter& /*filter*/,
                                                        const std::string& /*timezone*/,
                                                        std::shared_ptr<terrama2::core::FileRemover> /*remover*/,
                                                        const std::string& /*temporaryFolder*/,
                                                        const std::string& /*folderPath*/) const
{
  QString errMsg = QObject::tr("Non retrievable DataRetriever.");
  throw NotRetrivableException() << ErrorDescription(errMsg);
}

void terrama2::core::DataRetriever::retrieveDataCallback(const std::string& mask,
                                                         const terrama2::core::Filter& filter,
                                                         const std::string& timezone,
                                                         std::shared_ptr<terrama2::core::FileRemover> remover,
                                                         const std::string& temporaryFolderUri,
                                                         const std::string& foldersMask,
                                                         std::function<void (const std::string&)> processFile) const
{
  retrieveDataCallback(mask, filter, timezone, remover, temporaryFolderUri, foldersMask, [processFile](const std::string& uri, const std::string&){processFile(uri);});
}

void terrama2::core::DataRetriever::retrieveDataCallback(const std::string&,
                                                         const terrama2::core::Filter&,
                                                         const std::string&,
                                                         std::shared_ptr<terrama2::core::FileRemover>,
                                                         const std::string&,
                                                         const std::string&,
                                                         std::function<void (const std::string&, const std::string&)>) const
{
  QString errMsg = QObject::tr("Non retrievable DataRetriever.");
  throw NotRetrivableException() << ErrorDescription(errMsg);
}

te::dt::TimeInstantTZ terrama2::core::DataRetriever::lastDateTime() const
{
  QString errMsg = QObject::tr("Non retrievable DataRetriever.");
  throw NotRetrivableException() << ErrorDescription(errMsg);
}

bool terrama2::core::DataRetriever::isRetrivable() const
{
  return false;
}
terrama2::core::DataRetriever* terrama2::core::DataRetriever::make(DataProviderPtr dataProvider)
{
  return new DataRetriever(dataProvider);
}

std::string terrama2::core::DataRetriever::getTemporaryFolder(std::shared_ptr<terrama2::core::FileRemover> remover, const std::string& oldTempTerraMAFolder) const
{
    std::string tempTerraMAFolder = oldTempTerraMAFolder + "/";

    if(oldTempTerraMAFolder.empty())
    {
      boost::filesystem::path systemTempDir = boost::filesystem::temp_directory_path();
      boost::filesystem::path tempTerrama(systemTempDir.string()+"/terrama2");
      boost::filesystem::path tempDataDir = boost::filesystem::unique_path(tempTerrama.string()+"/%%%%-%%%%-%%%%-%%%%");

      // Create the directory where you will download the files.
      QDir dir(QString::fromStdString(tempDataDir.string()));
      if(!dir.exists())
        dir.mkpath(QString::fromStdString(tempDataDir.string()));

      std::string scheme = "file://";
      tempTerraMAFolder = scheme + tempDataDir.string();
      remover->addTemporaryFolder(tempTerraMAFolder);
    }

    return tempTerraMAFolder;
}

