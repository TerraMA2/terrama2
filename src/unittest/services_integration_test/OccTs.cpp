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
  \file src/unittest/services_integration_test/OccTs.cpp
  \brief test services_integration OccTs
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


#include "OccTs.hpp"

#include <terrama2/Config.hpp>

#include <QString>
#include <QCoreApplication>

std::shared_ptr<terrama2::services::collector::core::Collector> addCollectorOcc(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector)
{

  std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();

  collector->id = 778;
  collector->projectId = 0;
  collector->serviceInstanceId = 1;

  collector->inputDataSeries = 2;
  collector->outputDataSeries = 3;
  collector->inputOutputMap.emplace(2, 3);


  dataManagerCollector->add(collector);

  return collector;
}

void OccTs::collect()
{
  auto dataManagerCollector = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto serviceCollector = utilsTS::collector::gmockAndServicesCollector(dataManagerCollector);


  utilsTS::collector::addInputCollect(dataManagerCollector, utilsTS::typecollectoranalysis::occ);

  utilsTS::collector::addResultCollector(dataManagerCollector, utilsTS::typecollectoranalysis::occ);

  auto collector = addCollectorOcc(dataManagerCollector);

  serviceCollector->addToQueue(collector->id, terrama2::core::TimeUtils::nowUTC());

  utilsTS::timerCollectorAndAnalysis();
}


std::shared_ptr<terrama2::services::analysis::core::Analysis> addAnalysisOcc(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string scriptAnalysis)
{

    auto dataSeriesStatic = utilsTS::analysis::addInputDataSeriesStatic(dataManagerAnalysis);
    terrama2::services::analysis::core::AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeriesStatic->id;
    monitoredObjectADS.type = terrama2::services::analysis::core::AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "geom";


    auto occurrenceDataSeries = utilsTS::analysis::addInputDataSeriesAnalysis(dataManagerAnalysis, utilsTS::typecollectoranalysis::occ);
    terrama2::services::analysis::core::AnalysisDataSeries occurrenceADS;
    occurrenceADS.id = 10;
    occurrenceADS.dataSeriesId = occurrenceDataSeries->id;
    occurrenceADS.type = terrama2::services::analysis::core::AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
    occurrenceADS.alias = "occ";


    auto dataSeriesResult = utilsTS::analysis::addResultAnalysis(dataManagerAnalysis, dataSeriesStatic, utilsTS::typecollectoranalysis::occ);

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    analysis->id = 1;
    analysis->name = "Occ";
    analysis->script = scriptAnalysis;
    analysis->scriptLanguage = terrama2::services::analysis::core::ScriptLanguage::PYTHON;
    analysis->type = terrama2::services::analysis::core::AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = dataSeriesResult->id;
    analysis->outputDataSetId = dataSeriesResult->datasetList.front()->id;
    analysis->serviceInstanceId = 1;


    std::vector<terrama2::services::analysis::core::AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysisDataSeriesList.push_back(occurrenceADS);

    analysis->analysisDataSeriesList = analysisDataSeriesList;

    dataManagerAnalysis->add(analysis);

    return analysis;
}

void OccTs::analysisZonal()
{

    utilsTS::database::restoreDB(utilsTS::typecollectoranalysis::estados);

    //init gmock and services.

    auto dataManagerAnalysis = std::make_shared<terrama2::services::analysis::core::DataManager>();


    auto serviceAnalysis = utilsTS::analysis::gmockAndServicesAnalysis(dataManagerAnalysis);

    //call the script analysis history

    std::string scriptAnHistory = R"z(x = occurrence.zonal.count("Occurrence", "2d")
add_value("count", x))z";

    auto analysis = addAnalysisOcc(dataManagerAnalysis, scriptAnHistory);

    serviceAnalysis->addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2016-04-30T20:15:00-03", terrama2::core::TimeUtils::webgui_timefacet));

    utilsTS::timerCollectorAndAnalysis();

    utilsTS::database::compareCollectAndAnalysis(utilsTS::typecollectoranalysis::occ);

}

