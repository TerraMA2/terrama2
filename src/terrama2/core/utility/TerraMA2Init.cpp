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
  \file terrama2/core/utility/TerraMA2Init.cpp
  \brief A Raii initialization class for TerraMA2

  \author Jano Simas
*/

#include "TerraMA2Init.hpp"
#include "Utils.hpp"
#include "SemanticsManager.hpp"

// QT
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

terrama2::core::TerraMA2Init::TerraMA2Init(const std::string& serviceType, const int listeningPort)
{
  terrama2::core::initializeTerralib();

  terrama2::core::initializeLogger(serviceType+"_"+std::to_string(listeningPort)+"_terrama2.log");


  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  //read semantics from json file
  std::string semanticsPath = FindInTerraMA2Path("share/terrama2");
  QDir semanticsDir(QString::fromStdString(semanticsPath));
  QStringList fileList = semanticsDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
  for(const auto& filePath : fileList)
  {
    QFile semantcisFile(filePath);
    semantcisFile.open(QFile::ReadOnly);
    QByteArray bytearray = semantcisFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray);
    auto array = jsonDoc.array();
    for(const auto& json : array)
    {
      auto obj = json.toObject();
      auto jsonProvidersTypes = obj["providers_type_list"].toArray();
      std::vector<DataProviderType> providersTypes;
      for(const auto& providerType : jsonProvidersTypes)
        providersTypes.push_back(providerType.toString().toStdString());

      auto jsonMetadata = obj["metadata"].toObject();
      std::unordered_map<std::string, std::string> metadata;
      for(auto it = jsonMetadata.constBegin(); it != jsonMetadata.constEnd(); ++it)
        metadata.emplace(it.key().toStdString(), it.value().toString().toStdString()) ;

      semanticsManager.addSemantics(obj["code"].toString().toStdString(),
                                    obj["name"].toString().toStdString(),
                                    obj["driver"].toString().toStdString(),
                                    dataSeriesTypeFromString(obj["type"].toString().toStdString()),
                                    dataSeriesTemporalityFromString(obj["temporality"].toString().toStdString()),
                                    obj["format"].toString().toStdString(),
                                    providersTypes,
                                    metadata);
    }
  }


}

terrama2::core::TerraMA2Init::~TerraMA2Init()
{
  terrama2::core::finalizeTerralib();
}
