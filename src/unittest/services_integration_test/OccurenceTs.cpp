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
#include <terrama2/core/utility/GeoUtils.hpp>



//Collector

#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/mock/MockCollectorLogger.hpp>
#include <terrama2/services/collector/core/Intersection.hpp>



//Analysis
#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

//Interpreter
#include <terrama2/core/interpreter/InterpreterFactory.hpp>


#include "OccurenceTs.hpp"

#include <terrama2/Config.hpp>

#include <QString>
#include <QCoreApplication>


void OccurenceTs::collectIntersectionFile()
{

  utilsTS::database::restoreDB(utilsTS::typecollectoranalysis::counties_tocantins);

  auto dataManagerCollector = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto serviceCollector = utilsTS::collector::gmockAndServicesCollector(dataManagerCollector);

  utilsTS::collector::addInputCollect(dataManagerCollector, utilsTS::typecollectoranalysis::occurence);

  utilsTS::collector::addResultCollector(dataManagerCollector, utilsTS::typecollectoranalysis::occurence);


  std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();
  collector->id = 777;
  collector->projectId = 0;
  collector->serviceInstanceId = 1;

  collector->filter.region = terrama2::core::ewktToGeom("SRID=4326;POLYGON((-52.6 -13.8,-52.6 -4.3,-44.4 -4.3,-44.4 -13.8,-52.6 -13.8))");
  collector->filter.cropRaster = true;
  collector->filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-05-01T00:00:00.000-03:00", terrama2::core::TimeUtils::webgui_timefacet);
  collector->filter.discardAfter =  terrama2::core::TimeUtils::stringToTimestamp("2016-05-02T00:00:00.000-03:00", terrama2::core::TimeUtils::webgui_timefacet);


  utilsTS::collector::addInputCollect(dataManagerCollector, utilsTS::typecollectoranalysis::counties_tocantins);

  std::shared_ptr<terrama2::services::collector::core::Intersection> intersection = std::make_shared<terrama2::services::collector::core::Intersection>();

  intersection->collectorId = collector->id;

  std::vector<terrama2::services::collector::core::IntersectionAttribute> attrVec;
  terrama2::services::collector::core::IntersectionAttribute intersectionAttribute;
  intersectionAttribute.alias = "sigla_munic";
  intersectionAttribute.attribute = "nome";
  attrVec.push_back(intersectionAttribute);
  intersection->attributeMap[13] = attrVec;
  collector->intersection = intersection;

  collector->inputDataSeries = 2;
  collector->outputDataSeries = 3;
  collector->inputOutputMap.emplace(2, 3);

  dataManagerCollector->add(collector);

  serviceCollector->addToQueue(collector->id, terrama2::core::TimeUtils::stringToTimestamp("2016-05-01T00:00:00.000-03:00", terrama2::core::TimeUtils::webgui_timefacet));

  utilsTS::timerCollectorAndAnalysis();
}

void OccurenceTs::collectFTP()
{
  auto dataManagerCollector = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto serviceCollector = utilsTS::collector::gmockAndServicesCollector(dataManagerCollector);

  utilsTS::collector::addInputCollect(dataManagerCollector, utilsTS::typecollectoranalysis::focus_tocantins);

  utilsTS::collector::addResultCollector(dataManagerCollector, utilsTS::typecollectoranalysis::focus_tocantins);

  std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();
  collector->id = 777;
  collector->projectId = 0;
  collector->serviceInstanceId = 1;

  collector->filter.region = terrama2::core::ewktToGeom("SRID=4326;POLYGON((-52.6 -13.8,-52.6 -4.3,-44.4 -4.3,-44.4 -13.8,-52.6 -13.8))");
  collector->filter.cropRaster = true;
  collector->filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2017-12-01T00:00:00.000-02:00", terrama2::core::TimeUtils::webgui_timefacet);
  collector->filter.discardAfter =  terrama2::core::TimeUtils::stringToTimestamp("2017-12-11T00:00:00.000-02:00", terrama2::core::TimeUtils::webgui_timefacet);

  collector->inputDataSeries = 5;
  collector->outputDataSeries = 6;

  //input_id: 5 output_id: 6
  collector->inputOutputMap.emplace(5, 6);

  dataManagerCollector->add(collector);


  serviceCollector->addToQueue(collector->id, terrama2::core::TimeUtils::nowUTC());

  utilsTS::timerCollectorAndAnalysis();
}


void OccurenceTs::collectFile()
{
  auto dataManagerCollector = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto serviceCollector = utilsTS::collector::gmockAndServicesCollector(dataManagerCollector);


  utilsTS::collector::addInputCollect(dataManagerCollector, utilsTS::typecollectoranalysis::occurence);

  utilsTS::collector::addResultCollector(dataManagerCollector, utilsTS::typecollectoranalysis::occurence);

  std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();

  collector->id = 778;
  collector->projectId = 0;
  collector->serviceInstanceId = 1;

  collector->inputDataSeries = 2;
  collector->outputDataSeries = 3;
  collector->inputOutputMap.emplace(2, 3);


  dataManagerCollector->add(collector);


  serviceCollector->addToQueue(collector->id, terrama2::core::TimeUtils::nowUTC());

  utilsTS::timerCollectorAndAnalysis();
}

std::shared_ptr<terrama2::services::analysis::core::Analysis> addAnalysisBase(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string scriptAnalysis)
{
    auto dataSeriesStatic = utilsTS::analysis::addInputDataSeriesStatic(dataManagerAnalysis);

    terrama2::services::analysis::core::AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeriesStatic->id;
    monitoredObjectADS.type = terrama2::services::analysis::core::AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "geom";


    auto occurrenceDataSeries = utilsTS::analysis::addInputDataSeriesAnalysis(dataManagerAnalysis, utilsTS::typecollectoranalysis::occurence);
    terrama2::services::analysis::core::AnalysisDataSeries occurrenceADS;
    occurrenceADS.id = 10;
    occurrenceADS.dataSeriesId = occurrenceDataSeries->id;
    occurrenceADS.type = terrama2::services::analysis::core::AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
    occurrenceADS.alias = "occ";


    auto dataSeriesResult = utilsTS::analysis::addResultAnalysis(dataManagerAnalysis,
                                                                 utilsTS::typecollectoranalysis::occurence,
                                                                 dataSeriesStatic);

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

std::shared_ptr<const terrama2::services::analysis::core::Analysis> addAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string scriptAnalysis)
{
  auto analysis = addAnalysisBase(dataManagerAnalysis, scriptAnalysis);
  dataManagerAnalysis->add(analysis);

  return analysis;
}

/*!
 * \brief  Analysis Zonal
 *
 * \warning in development
 */
void OccurenceTs::analysisZonal()
{

    utilsTS::database::restoreDB(utilsTS::typecollectoranalysis::states);

    //init gmock and services.

    auto dataManagerAnalysis = std::make_shared<terrama2::services::analysis::core::DataManager>();


    auto serviceAnalysis = utilsTS::analysis::gmockAndServicesAnalysis(dataManagerAnalysis);

    //call the script analysis history

    std::string scriptZonal = R"z(x = occurrence.zonal.count("Occurrence", "2d")
add_value("count", x))z";

    auto analysis = addAnalysisBase(dataManagerAnalysis, scriptZonal);

    auto reprocessingHistoricalDataPtr = std::make_shared<terrama2::services::analysis::core::ReprocessingHistoricalData>();


    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));

    std::string startDate = "2016-04-30 00:00:00";
    boost::posix_time::ptime startBoostDate(boost::posix_time::time_from_string(startDate));
    boost::local_time::local_date_time lstartDate(startBoostDate.date(), startBoostDate.time_of_day(), zone, true);
    reprocessingHistoricalDataPtr->startDate = std::make_shared<te::dt::TimeInstantTZ>(lstartDate);

    std::string endDate = "2016-05-01 00:00:00";
    boost::posix_time::ptime endBoostDate(boost::posix_time::time_from_string(endDate));
    boost::local_time::local_date_time lendDate(endBoostDate.date(), endBoostDate.time_of_day(), zone, true);
    reprocessingHistoricalDataPtr->endDate = std::make_shared<te::dt::TimeInstantTZ>(lendDate);


    analysis->reprocessingHistoricalData = reprocessingHistoricalDataPtr;

    analysis->schedule.frequency = 3;
    analysis->schedule.frequencyUnit = "h";
    analysis->schedule.id = 1;
    analysis->active = true;

    dataManagerAnalysis->add(analysis);


    serviceAnalysis->addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2016-05-31T00:00:00+00", terrama2::core::TimeUtils::webgui_timefacet));

    utilsTS::timerCollectorAndAnalysis();

    utilsTS::database::compareCollectAndAnalysis(utilsTS::typecollectoranalysis::occurence);

}

