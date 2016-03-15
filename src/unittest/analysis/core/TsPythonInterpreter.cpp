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
  \file unittest/core/TsPythonInterpreter.cpp

  \brief Test for PythonInterpreter functions

  \author Paulo R. M. Oliveira
*/

//TerraMA2
#include "TsPythonInterpreter.hpp"

#include <terrama2/analysis/core/Context.hpp>
#include <terrama2/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/analysis/core/Analysis.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/Logger.hpp>
#include <terrama2/Exception.hpp>


// Python
#include <Python.h>

// STD
#include <thread>

#include <QCoreApplication>
#include <QTimer>


using namespace terrama2::analysis::core;


/*
void TsPythonInterpreter::countPointsAnalysis()
{


  Analysis analysis;

  analysis.setId(1);

  std::string script = "x = countPoints(\"Ocorrencia\", 0, \"2h\", \"\")\nresult(x)";

  analysis.setScript(script);
  analysis.setScriptLanguage(Analysis::PYTHON);
  analysis.setType(Analysis::MONITORED_OBJECT_TYPE);

  terrama2::core::DataProvider provider("Provider", terrama2::core::DataProvider::FILE_TYPE, 1);
  provider.setUri("file:///Users/paulo/Workspace/data/shp/");
  terrama2::core::DataSet dataset("Dataset", terrama2::core::DataSet::STATIC_DATA, 1, 1);
  terrama2::core::DataSetItem item(terrama2::core::DataSetItem::STATIC_DATA, 1, 1);
  item.setMask("UFEBRASIL.shp");
  item.setSrid(4019);

  std::map<std::string, std::string> itemMetadata;
  itemMetadata["identifier"] = "NM_ESTADO";
  item.setMetadata(itemMetadata);

  dataset.add(item);

  terrama2::core::DataManager::getInstance().load(true);
  terrama2::core::DataManager::getInstance().add(provider);
  terrama2::core::DataManager::getInstance().add(dataset);
  analysis.setMonitoredObject(dataset);

  terrama2::core::DataProvider provider1("Provider2", terrama2::core::DataProvider::FILE_TYPE, 2);
  provider1.setUri("file:///Users/paulo/Workspace/data/");
  terrama2::core::DataSet dataset1("Ocorrencia", terrama2::core::DataSet::OCCURENCE_TYPE, 2, 2);
  terrama2::core::DataSetItem item1(terrama2::core::DataSetItem::FIRE_POINTS_TYPE, 2, 2);
  item1.setMask("exporta_20150826_2030.csv");
  item1.setSrid(4326);
  dataset1.add(item1);

  terrama2::core::DataManager::getInstance().add(provider1);
  terrama2::core::DataManager::getInstance().add(dataset1);
  std::vector<terrama2::core::DataSet> staticDataList;
  staticDataList.push_back(dataset1);
  analysis.setAdditionalDataList(staticDataList);

  terrama2::analysis::core::runAnalysis(analysis);

}
*/


void TsPythonInterpreter::sumHistoryPCD()
{

  Analysis analysis;

  analysis.setId(1);

  std::string script = "x = sumHistoryPCD(\"PCD-Angra\", \"pluvio\", 2, \"10h\")\nresult(x)";

  analysis.setScript(script);
  analysis.setScriptLanguage(Analysis::PYTHON);
  analysis.setType(Analysis::MONITORED_OBJECT_TYPE);

  terrama2::core::DataProvider provider("Provider", terrama2::core::DataProvider::FILE_TYPE, 1);
  provider.setUri("file:///Users/paulo/Workspace/data/shp/");
  terrama2::core::DataSet dataset("Dataset", terrama2::core::DataSet::STATIC_DATA, 1, 1);
  terrama2::core::DataSetItem item(terrama2::core::DataSetItem::STATIC_DATA, 1, 1);
  item.setMask("afetados.shp");
  item.setSrid(4618);

  std::map<std::string, std::string> itemMetadata;
  itemMetadata["identifier"] = "NOME";
  item.setMetadata(itemMetadata);

  dataset.add(item);

  terrama2::core::DataManager::getInstance().load(true);
  terrama2::core::DataManager::getInstance().add(provider);
  terrama2::core::DataManager::getInstance().add(dataset);
  analysis.setMonitoredObject(dataset);



  terrama2::core::DataProvider provider1("Provider2", terrama2::core::DataProvider::FILE_TYPE, 2);
  provider1.setUri("file:///Users/paulo/Workspace/data/");
  terrama2::core::DataSet pcdDataset("PCD-Angra", terrama2::core::DataSet::PCD_TYPE, 2, 2);
  terrama2::core::DataSetItem item1(terrama2::core::DataSetItem::PCD_INPE_TYPE, 2, 2);
  item1.setMask("angra.txt");
  item1.setTimezone("-02:00");
  item1.setSrid(4674);

  terrama2::core::PCD dcp(item1);
  te::gm::Point* point = new te::gm::Point(-44.46540, -23.00506, 4674, nullptr);

  dcp.setLocation(point);

  pcdDataset.add(dcp);

  terrama2::core::DataManager::getInstance().add(provider1);
  terrama2::core::DataManager::getInstance().add(pcdDataset);

  std::vector<terrama2::core::DataSet> additionalDataList;
  additionalDataList.push_back(pcdDataset);
  analysis.setAdditionalDataList(additionalDataList);

  terrama2::analysis::core::Analysis::Influence influence;
  influence.type = terrama2::analysis::core::Analysis::InfluenceType::RADIUS_CENTER;
  influence.radius = 50;
  analysis.setInfluence(pcdDataset.id(), influence);

  terrama2::analysis::core::runAnalysis(analysis);

}
