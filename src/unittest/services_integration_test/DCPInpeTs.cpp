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
  \file src/unittest/collector/DCPInpeTs.cpp
  \brief test collector DCPInpe
  \author Bianca Maciel
*/

// STL
#include <iostream>

#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/Raii.hpp>


//Collector

#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/mock/MockCollectorLogger.hpp>


//Analysis
#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

//Interpreter
#include <terrama2/core/interpreter/InterpreterFactory.hpp>

//Extra
#include <extra/data/DCPSerramarInpe.hpp>
#include <extra/data/ResultAnalysisPostGis.hpp>

#include "DCPInpeTS.hpp"

#include <terrama2/Config.hpp>

#include <QString>
#include <QCoreApplication>


void interpreterScriptPy(std::string path)
{
  std::string scriptPath = terrama2::core::FindInTerraMA2Path(path);
  std::string script = terrama2::core::readFileContents(scriptPath);

  auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
  interpreter->setString("dbname","test");
  interpreter->runScript(script);
}

void restoreDB()
{

  interpreterScriptPy("share/terrama2/scripts/restore-db.py");
}


int compareCollector()
{
   std::string scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/compare-collector.py");
   std::string script = terrama2::core::readFileContents(scriptPath);

   auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
   interpreter->setString("dbname","test");
   interpreter->runScript(script);

   boost::optional<double> countCollector = interpreter->getNumeric("count");

   if(countCollector)
   {
     int qntdTables = int (*countCollector);
     return qntdTables;
   }

   return 0;
}

int compareAnalysis()
{

  std::string scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/compare-analysis.py");

  std::string script = terrama2::core::readFileContents(scriptPath);

  auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
  interpreter->setString("dbname","test");

  interpreter->runScript(script);

  boost::optional<double> statusAnalysis = interpreter->getNumeric("status");

  if(statusAnalysis)
  {
    int qntdTables = int (*statusAnalysis);
    return qntdTables;
  }

  return 0;
}


void DCPInpeTs::deleteDB()
{
  interpreterScriptPy("share/terrama2/scripts/delete-db.py");
}

void DCPInpeTs::createDB()
{
  deleteDB();
  interpreterScriptPy("share/terrama2/scripts/create-db.py");
}


void timerCollectorAndAnalysis()
{
  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  QCoreApplication::exec();
}

void addInputCollect(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector)
{
    //////////////////////////////////////////////
    //     input
    // DataProvider information
    //////////////////////////////////////////////
    auto dataProvider = terrama2::serramar::dataProviderSerramarInpe();
    dataManagerCollector->add(dataProvider);

    // DataSeries information
    auto dataSeries = terrama2::serramar::dataSeriesDcpSerramar(dataProvider);
    dataManagerCollector->add(dataSeries);
}


void addResultCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector)
{
    ///////////////////////////////////////////////
    //     output
    ///////////////////////////////////////////////

    // DataProvider information
    auto dataProvider = terrama2::serramar::dataProviderPostGisDCP();
    dataManagerCollector->add(dataProvider);

    // DataSeries information
    auto outputDataSeries = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProvider);
    dataManagerCollector->add(outputDataSeries);
}


terrama2::services::collector::core::CollectorPtr addCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector)
{
  std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();
  collector->id = 777;
  collector->projectId = 0;
  collector->serviceInstanceId = 1;

  collector->inputDataSeries = 1;
  collector->outputDataSeries = 2;

  // picinguaba input_id: 1 output_id: 6
  collector->inputOutputMap.emplace(1, 6);

  // itanhaem input_id: 2 output_id: 4
  collector->inputOutputMap.emplace(2, 7);

  // ubatuba input_id: 3 output_id: 8
  collector->inputOutputMap.emplace(3, 8);

  // itutinga input_id: 4 output_id: 9
  collector->inputOutputMap.emplace(4, 9);

  // cunha input_id: 2 output_id: 10
  collector->inputOutputMap.emplace(5, 10);

  dataManagerCollector->add(collector);

  return collector;
}

void DCPInpeTs::collect()
{

  auto dataManagerCollector = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto loggerCopy = std::make_shared<terrama2::core::MockCollectorLogger>();

  EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));

  EXPECT_CALL(*loggerCopy, addInput(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*loggerCopy, addOutput(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());


  auto logger = std::make_shared<terrama2::core::MockCollectorLogger>();

  EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
  EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

  EXPECT_CALL(*logger, addInput(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*logger, addOutput(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());


  terrama2::services::collector::core::Service serviceCollector(dataManagerCollector);
  serviceCollector.setLogger(logger);
  serviceCollector.start();

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(te::core::URI(""));


  addInputCollect(dataManagerCollector);

  addResultCollector(dataManagerCollector);

  auto collector = addCollector(dataManagerCollector);

  serviceCollector.addToQueue(collector->id, terrama2::core::TimeUtils::nowUTC());


  timerCollectorAndAnalysis();
}

terrama2::core::DataSeriesPtr addInputAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis)
{

    auto dataProviderDCP = terrama2::serramar::dataProviderPostGisDCP();
    dataManagerAnalysis->add(dataProviderDCP);

    auto dcpSerramar = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProviderDCP);
    dataManagerAnalysis->add(dcpSerramar);

    return dcpSerramar;
}

terrama2::core::DataSeriesPtr addResultAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis,terrama2::core::DataSeriesPtr dataSeries)
{
    auto dataProvider = terrama2::resultanalysis::dataProviderResultAnalysis();
    dataManagerAnalysis->add(dataProvider);

    auto dataSeriesResult = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProvider,
                                                                                      terrama2::resultanalysis::tablename::analysis_dcp_result,
                                                                                      dataSeries);
    dataManagerAnalysis->add(dataSeriesResult);

    return dataSeriesResult;
}

terrama2::services::analysis::core::AnalysisPtr addAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis)
{

    auto dataSeriesDcp = addInputAnalysis(dataManagerAnalysis);

    auto dataSeriesResult = addResultAnalysis(dataManagerAnalysis, dataSeriesDcp);

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    std::string script = R"z(moBuffer = Buffer()
ids = dcp.influence.by_rule(moBuffer)
maximum = dcp.history.max("pluvio", "6000d")
minimum = dcp.history.min("pluvio", "6000d")
me = dcp.history.mean("pluvio", "6000d")
su = dcp.history.sum("pluvio", "6000d")
med = dcp.history.median("pluvio", "6000d")
standard = dcp.history.standard_deviation("pluvio", "6000d")
var = dcp.history.variance("pluvio", "6000d")
count  = dcp.count(moBuffer)
va = dcp.value("pluvio")
add_value("max", maximum)
add_value("min", minimum)
add_value("mean", me)
add_value("sum", su)
add_value("median", med)
add_value("standard_deviation", standard)
add_value("variance", var)
add_value("count", count)
add_value("value", va))z";

    analysis->id = 1;
    analysis->name = "Min DCP";
    analysis->script = script;
    analysis->scriptLanguage = terrama2::services::analysis::core::ScriptLanguage::PYTHON;
    analysis->type = terrama2::services::analysis::core::AnalysisType::DCP_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = dataSeriesResult->id;
    analysis->outputDataSetId = dataSeriesResult->datasetList.front()->id;
    analysis->serviceInstanceId = 1;

    analysis->metadata["INFLUENCE_TYPE"] = "1";
    analysis->metadata["INFLUENCE_RADIUS"] = "50";
    analysis->metadata["INFLUENCE_RADIUS_UNIT"] = "km";

    terrama2::services::analysis::core::AnalysisDataSeries dcpADS;
    dcpADS.id = 1;
    dcpADS.dataSeriesId = dataSeriesDcp->id;
    dcpADS.type = terrama2::services::analysis::core::AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    dcpADS.metadata["identifier"] = "table_name";

    std::vector<terrama2::services::analysis::core::AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(dcpADS);
    analysis->analysisDataSeriesList = analysisDataSeriesList;

    dataManagerAnalysis->add(analysis);

    return analysis;
}

void DCPInpeTs::analysis()
{
    terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

    auto dataManagerAnalysis = std::make_shared<terrama2::services::analysis::core::DataManager>();

    auto loggerCopy = std::make_shared<terrama2::core::MockAnalysisLogger>();

    EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));

    auto logger = std::make_shared<terrama2::core::MockAnalysisLogger>();

    EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
    EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

    terrama2::services::analysis::core::Service serviceAnalysis(dataManagerAnalysis);
    serviceAnalysis.setLogger(logger);
    serviceAnalysis.start();

    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    serviceManager.setInstanceId(1);
    serviceManager.setLogger(logger);
    serviceManager.setLogConnectionInfo(te::core::URI(""));

    auto analysis = addAnalysis(dataManagerAnalysis);

    //serviceAnalysis.addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2017-11-01T18:29:29.785+00", terrama2::core::TimeUtils::webgui_timefacet));
    serviceAnalysis.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());

    timerCollectorAndAnalysis();

}

void DCPInpeTs::compareCollectAndAnalysis()
{
  restoreDB();

  try
  {
    int qntTablesCollector = compareCollector();
    QCOMPARE(qntTablesCollector, 5);
  }
  catch(const terrama2::core::InterpreterException& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toUtf8().data());
  }

  try
  {
    int qntTablesAnalysis =  compareAnalysis();
    QCOMPARE(qntTablesAnalysis, 1);
  }
  catch(const terrama2::core::InterpreterException& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toUtf8().data());
  }
}
