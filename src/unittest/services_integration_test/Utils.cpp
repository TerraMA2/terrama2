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
  \file src/unittest/services_integration_test/Utils.cpp
  \brief test services_integration Utils
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
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


//Extra
#include <extra/data/OccurrenceWFP.hpp>
#include <extra/data/ResultAnalysisPostGis.hpp>
#include <extra/data/StaticPostGis.hpp>
#include <extra/data/DCPSerramarInpe.hpp>



//Interpreter
#include <terrama2/core/interpreter/InterpreterFactory.hpp>

#include "Utils.hpp"

#include <terrama2/Config.hpp>

#include <QString>
#include <QCoreApplication>
#include <QTimer>

void utilsTS::database::interpreterScriptPy(std::string path)
{
  std::string scriptPath = terrama2::core::FindInTerraMA2Path(path);
  std::string script = terrama2::core::readFileContents(scriptPath);
  auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
  interpreter->setString("dbname","test");
  interpreter->runScript(script);
}




int utilsTS::database::compareCollector(std::string type)
{
   std::string scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/compare-collector.py");
   std::string script = terrama2::core::readFileContents(scriptPath);

   auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
   interpreter->setString("dbname", "test");
   interpreter->setString("typeAnalysis", type);
   interpreter->runScript(script);

   boost::optional<double> countCollector = interpreter->getNumeric("count");

   if(countCollector)
   {
     int qntdTables = int (*countCollector);
     return qntdTables;
   }
   return 0;
}
void utilsTS::database::restoreDB(std::string typeAnalysis)
{

  std::string scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/restore-db.py");

  std::string script = terrama2::core::readFileContents(scriptPath);

  auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
  interpreter->setString("dbname","test");

  if(typeAnalysis == utilsTS::typecollectoranalysis::dcp_history || typeAnalysis == utilsTS::typecollectoranalysis::operator_dcp || typeAnalysis == utilsTS::typecollectoranalysis::operator_history_interval)
  {
    if(typeAnalysis == utilsTS::typecollectoranalysis::dcp_history)
      interpreter->setString("namefile", TERRAMA2_DATA_DIR + "/operator_dcp_history_ref.backup");
    else if(typeAnalysis == utilsTS::typecollectoranalysis::operator_dcp)
      interpreter->setString("namefile", TERRAMA2_DATA_DIR + "/operator_dcp_ref.backup");
    else if(typeAnalysis ==  utilsTS::typecollectoranalysis::operator_history_interval)
      interpreter->setString("namefile", TERRAMA2_DATA_DIR + "/operator_history_interval_ref.backup");
  }
  else if(typeAnalysis == utilsTS::typecollectoranalysis::states)
    interpreter->setString("namefile", TERRAMA2_DATA_DIR + "/estados.backup");
  else if (typeAnalysis == utilsTS::typecollectoranalysis::occurence)
    interpreter->setString("namefile", TERRAMA2_DATA_DIR + "/test_occ.backup");
  else if(typeAnalysis == utilsTS::typecollectoranalysis::counties_tocantins)
    interpreter->setString("namefile", TERRAMA2_DATA_DIR + "/counties_tocantins.backup");

  interpreter->runScript(script);
}



int utilsTS::database::compareAnalysis(std::string typeAnalysis)
{

  std::string scriptPath = terrama2::core::FindInTerraMA2Path("share/terrama2/scripts/compare-analysis.py");

  std::string script = terrama2::core::readFileContents(scriptPath);

  auto interpreter = terrama2::core::InterpreterFactory::getInstance().make("PYTHON");
  interpreter->setString("dbname","test");
  interpreter->setString("typeAnalysis", typeAnalysis);


  interpreter->runScript(script);

  boost::optional<double> statusAnalysis = interpreter->getNumeric("status");

  if(statusAnalysis)
  {
    int qntdTables = int (*statusAnalysis);
    return qntdTables;
  }

  return 0;
}


void utilsTS::database::deleteDB()
{
  interpreterScriptPy("share/terrama2/scripts/delete-db.py");
}

void utilsTS::database::createDB()
{
  deleteDB();
  interpreterScriptPy("share/terrama2/scripts/create-db.py");
}


void utilsTS::database::tryCatchCollector(std::string typeAnalysis,
                                          int numTablesExpected)
{
  try
  {
    int qntTablesCollector = compareCollector(typeAnalysis);
    QCOMPARE(qntTablesCollector, numTablesExpected);
    numTablesExpected = 0;
  }
  catch(const terrama2::core::InterpreterException& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toUtf8().data());
    numTablesExpected = 0;
  }

}

void utilsTS::database::tryCatchAnalysis(std::string  typeAnalysis,
                                         int numTablesExpected)
{
  try
  {
    int qntTablesAnalysis =  compareAnalysis(typeAnalysis);
    QCOMPARE(qntTablesAnalysis, numTablesExpected);
    numTablesExpected = 0;
  }
  catch(const terrama2::core::InterpreterException& e)
  {
    QFAIL(boost::get_error_info<terrama2::ErrorDescription>(e)->toUtf8().data());
  }
}

void utilsTS::database::compareCollectAndAnalysis(std::string typeAnalysis)
{

  restoreDB(typeAnalysis);
  int numTablesExpected = 0;


  if(typeAnalysis == utilsTS::typecollectoranalysis::dcp_history || typeAnalysis == utilsTS::typecollectoranalysis::operator_dcp || typeAnalysis == utilsTS::typecollectoranalysis::operator_history_interval)
  {
    numTablesExpected = 5;
    tryCatchCollector(typeAnalysis, numTablesExpected);


    numTablesExpected = 1;
    tryCatchAnalysis(typeAnalysis, numTablesExpected);


  }
  if(typeAnalysis == utilsTS::typecollectoranalysis::occurence)
  {
    numTablesExpected = 1;
    tryCatchCollector(typeAnalysis, numTablesExpected);


    numTablesExpected = 1;
    tryCatchAnalysis(typeAnalysis, numTablesExpected);
  }



}

std::unique_ptr<terrama2::services::collector::core::Service> utilsTS::collector::gmockAndServicesCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector)
{

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


  std::unique_ptr<terrama2::services::collector::core::Service> serviceCollector(new terrama2::services::collector::core::Service(dataManagerCollector));
  serviceCollector->setLogger(logger);
  serviceCollector->start();

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(te::core::URI(""));


  return serviceCollector;
}


void utilsTS::collector::addInputCollect(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector,
                                         std::string typeCollector)
{
  if(typeCollector == utilsTS::typecollectoranalysis::occurence)
  {
    auto dataProvider = terrama2::occurrencewfp::dataProviderFileOccWFP();
    dataManagerCollector->add(dataProvider);

    auto dataSeries = terrama2::occurrencewfp::dataSeriesOccWFP(dataProvider);
    dataManagerCollector->add(dataSeries);
  }
  else if (typeCollector == utilsTS::typecollectoranalysis::dcp)
  {
    auto dataProvider = terrama2::serramar::dataProviderSerramarInpe();
    dataManagerCollector->add(dataProvider);

    auto dataSeries = terrama2::serramar::dataSeriesDcpSerramar(dataProvider);
    dataManagerCollector->add(dataSeries);
  }
  else if(typeCollector == utilsTS::typecollectoranalysis::focus_tocantins)
  {

    auto dataProvider = terrama2::occurrencewfp::ftp::dataProviderFocusFTP();
    dataManagerCollector->add(dataProvider);

    auto dataSeries = terrama2::occurrencewfp::ftp::dataSeriesFocusFTP(dataProvider);
    dataManagerCollector->add(dataSeries);
  }
  else if(typeCollector == utilsTS::typecollectoranalysis::counties_tocantins)
  {
    auto dataProviderStatic = terrama2::staticpostgis::dataProviderStaticPostGis();
    dataManagerCollector->add(dataProviderStatic);

    auto dataSeriesCounties = terrama2::staticpostgis::dataSeriesCounties(dataProviderStatic);
    dataManagerCollector->add(dataSeriesCounties);
  }

}


void utilsTS::collector::addResultCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector,
                                            std::string typeCollector)
{
  if (typeCollector == utilsTS::typecollectoranalysis::occurence)
  {
    auto outputDataProvider = terrama2::occurrencewfp::dataProviderPostGisOccWFP();
    dataManagerCollector->add(outputDataProvider);

    auto outputDataSeries = terrama2::occurrencewfp::dataSeriesOccWFPPostGis(outputDataProvider);
    dataManagerCollector->add(outputDataSeries);
  }
  else if (typeCollector == utilsTS::typecollectoranalysis::dcp)
  {
    auto dataProvider = terrama2::serramar::dataProviderPostGisDCP();
    dataManagerCollector->add(dataProvider);

    auto outputDataSeries = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProvider);
    dataManagerCollector->add(outputDataSeries);
  }
  else if (typeCollector == utilsTS::typecollectoranalysis::focus_tocantins)
  {

    auto dataProvider = terrama2::occurrencewfp::ftp::dataProviderFocusPostGis();
    dataManagerCollector->add(dataProvider);

    auto dataSeries = terrama2::occurrencewfp::ftp::dataSeriesFocusPostGis(dataProvider);
    dataManagerCollector->add(dataSeries);
  }

}


void utilsTS::timerCollectorAndAnalysis()
{
  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  QCoreApplication::exec();
}




terrama2::core::DataSeriesPtr utilsTS::analysis::addInputDataSeriesStatic(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis)
{
  auto dataProviderStatic = terrama2::staticpostgis::dataProviderStaticPostGis();
  dataManagerAnalysis->add(dataProviderStatic);

  auto dataSeriesStatic = terrama2::staticpostgis::dataSeriesEstados2010(dataProviderStatic);
  dataManagerAnalysis->add(dataSeriesStatic);

  return dataSeriesStatic;
}

terrama2::core::DataSeriesPtr utilsTS::analysis::addInputDataSeriesAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis,
                                                                            std::string typeAnalysis)
{

  if (typeAnalysis == utilsTS::typecollectoranalysis::occurence)
  {
    auto dataProvider = terrama2::occurrencewfp::dataProviderPostGisOccWFP();
    dataManagerAnalysis->add(dataProvider);

    auto occInpe = terrama2::occurrencewfp::dataSeriesOccWFPPostGis(dataProvider);
    dataManagerAnalysis->add(occInpe);

    return occInpe;
  }
  else if (typeAnalysis == utilsTS::typecollectoranalysis::dcp)
  {
    auto dataProviderDCP = terrama2::serramar::dataProviderPostGisDCP();
    dataManagerAnalysis->add(dataProviderDCP);

    auto dcpSerramar = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProviderDCP);
    dataManagerAnalysis->add(dcpSerramar);

    return dcpSerramar;
  } else {
    assert(false);
    return nullptr;
  }
}

terrama2::core::DataSeriesPtr utilsTS::analysis::addResultAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis,
                                                                   std::string typeAnalysis,
                                                                   terrama2::core::DataSeriesPtr dataSeries)
{
  auto dataProvider = terrama2::resultanalysis::dataProviderResultAnalysis();
  dataManagerAnalysis->add(dataProvider);


  if(typeAnalysis == utilsTS::typecollectoranalysis::occurence)
  {
    auto dataSeriesResult = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProvider,terrama2::resultanalysis::tablename::occurrence_analysis_result, dataSeries);
    dataManagerAnalysis->add(dataSeriesResult);
    return dataSeriesResult;
  }
  else if (typeAnalysis == utilsTS::typecollectoranalysis::dcp)
  {
    auto dataSeriesResult = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProvider,terrama2::resultanalysis::tablename::analysis_dcp_result, dataSeries);
    dataManagerAnalysis->add(dataSeriesResult);
    return dataSeriesResult;
  } else {
    assert(false);
    return nullptr;
  }

}

std::unique_ptr<terrama2::services::analysis::core::Service> utilsTS::analysis::gmockAndServicesAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis)
{
  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;
  Q_UNUSED(pythonInterpreterInit);

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

  std::unique_ptr<terrama2::services::analysis::core::Service> serviceAnalysis(new terrama2::services::analysis::core::Service(dataManagerAnalysis));
  serviceAnalysis->setLogger(logger);
  serviceAnalysis->start();

  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
  serviceManager.setInstanceId(1);
  serviceManager.setLogger(logger);
  serviceManager.setLogConnectionInfo(te::core::URI(""));

  return serviceAnalysis;
}
