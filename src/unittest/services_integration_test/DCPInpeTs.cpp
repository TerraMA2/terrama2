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
  \file src/unittest/services_integration_test/DCPInpeTs.cpp
  \brief test services_integration DCPInpeTs
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
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

//Interpreter
#include <terrama2/core/interpreter/InterpreterFactory.hpp>

#include <terrama2/Config.hpp>

#include "DCPInpeTs.hpp"

//QT
#include <QString>
#include <QCoreApplication>


void DCPInpeTs::collect()
{

  auto dataManagerCollector = std::make_shared<terrama2::services::collector::core::DataManager>();

  auto serviceCollector = utilsTS::collector::gmockAndServicesCollector(dataManagerCollector);

  utilsTS::collector::addInputCollect(dataManagerCollector, utilsTS::typecollectoranalysis::dcp);

  utilsTS::collector::addResultCollector(dataManagerCollector, utilsTS::typecollectoranalysis::dcp);

  std::shared_ptr<terrama2::services::collector::core::Collector> collector = std::make_shared<terrama2::services::collector::core::Collector>();
  collector->id = 777;
  collector->projectId = 0;
  collector->serviceInstanceId = 1;

  collector->inputDataSeries = 1;
  collector->outputDataSeries = 2;

  // picinguaba input_id: 1 output_id: 315
  collector->inputOutputMap.emplace(1, 325);

  // itanhaem input_id: 2 output_id: 316
  collector->inputOutputMap.emplace(2, 326);

  // ubatuba input_id: 3 output_id: 317
  collector->inputOutputMap.emplace(3, 327);

  // itutinga input_id: 4 output_id: 318
  collector->inputOutputMap.emplace(4, 328);

  // cunha input_id: 2 output_id: 319
  collector->inputOutputMap.emplace(5, 329);

  dataManagerCollector->add(collector);


  serviceCollector->addToQueue(collector->id, terrama2::core::TimeUtils::nowUTC());

  utilsTS::timerCollectorAndAnalysis();
}

/*!
 * \brief addAnalysisBase
 * \param dataManagerAnalysis
 * \param scriptAnalysis
 * \return
 *
 * \warning Won't add the analysis to the datamanager!
 */
std::shared_ptr<terrama2::services::analysis::core::Analysis> addAnalysisBase(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string scriptAnalysis)
{
    auto dataSeriesDcp = utilsTS::analysis::addInputDataSeriesAnalysis(dataManagerAnalysis, utilsTS::typecollectoranalysis::dcp);

    auto dataSeriesResult = utilsTS::analysis::addResultAnalysis(dataManagerAnalysis, utilsTS::typecollectoranalysis::dcp, dataSeriesDcp);

    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    std::string script = scriptAnalysis;
    analysis->id = 1;
    analysis->name = "Min DCP";
    analysis->script = script;
    analysis->scriptLanguage = terrama2::services::analysis::core::ScriptLanguage::PYTHON;
    analysis->type = terrama2::services::analysis::core::AnalysisType::DCP_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = dataSeriesResult->id;
    analysis->outputDataSetId = dataSeriesResult->datasetList.front()->id;
    analysis->serviceInstanceId = 1;

    terrama2::services::analysis::core::AnalysisDataSeries dcpADS;
    dcpADS.id = 1;
    dcpADS.dataSeriesId = dataSeriesDcp->id;
    dcpADS.type = terrama2::services::analysis::core::AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    dcpADS.metadata["identifier"] = "table_name";

    std::vector<terrama2::services::analysis::core::AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(dcpADS);
    analysis->analysisDataSeriesList = analysisDataSeriesList;

    return analysis;
}

std::shared_ptr<const terrama2::services::analysis::core::Analysis> addAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string scriptAnalysis)
{
  auto analysis = addAnalysisBase(dataManagerAnalysis, scriptAnalysis);
  dataManagerAnalysis->add(analysis);

  return analysis;
}

/*!
 * \brief analysisHistory
 *
 * \warning operator dcp.value not return the same value the table reference
 */
void DCPInpeTs::analysisHistory()
{

    //init gmock and services.

    auto dataManagerAnalysis = std::make_shared<terrama2::services::analysis::core::DataManager>();


    auto serviceAnalysis = utilsTS::analysis::gmockAndServicesAnalysis(dataManagerAnalysis);

    //call the script analysis history

    std::string scriptAnHistory = R"z(moBuffer = Buffer(BufferType.Out_union, 2., "km")
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

    auto analysis = addAnalysis(dataManagerAnalysis, scriptAnHistory);
    dataManagerAnalysis->add(analysis);

    serviceAnalysis->addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2017-12-12T18:23:23.077+00", terrama2::core::TimeUtils::webgui_timefacet));

    utilsTS::timerCollectorAndAnalysis();

    utilsTS::database::compareCollectAndAnalysis(utilsTS::typecollectoranalysis::dcp_history);

}


/*!
 * \brief Analysis Operator DCP
 *
 * \warning dcp.standard_deviation and dcp.variance operator
 * return null could not compare values ​​in SQL.
 */

void DCPInpeTs::analysisDCP()
{
  //init gmock and services.

  auto dataManagerAnalysis = std::make_shared<terrama2::services::analysis::core::DataManager>();

  auto serviceAnalysis = utilsTS::analysis::gmockAndServicesAnalysis(dataManagerAnalysis);

    std::string scriptDCP = R"z(moBuffer = Buffer(BufferType.Out_union, 2., "km")
ids = dcp.influence.by_rule(moBuffer)
maximum = dcp.max("pluvio", ids)
minimum = dcp.min("pluvio", ids)
me = dcp.mean("pluvio", ids)
su = dcp.sum("pluvio", ids)
med = dcp.median("pluvio", ids)
count  = dcp.count(moBuffer)
va = dcp.value("pluvio")
add_value("max", maximum)
add_value("min", minimum)
add_value("mean", me)
add_value("sum", su)
add_value("median", med)
add_value("count", count)
add_value("value", va))z";

    auto analysis = addAnalysis(dataManagerAnalysis, scriptDCP);
    dataManagerAnalysis->add(analysis);
    serviceAnalysis->addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2017-12-13T10:55:04.518+00" , terrama2::core::TimeUtils::webgui_timefacet));

    utilsTS::timerCollectorAndAnalysis();

    utilsTS::database::compareCollectAndAnalysis(utilsTS::typecollectoranalysis::operator_dcp);
}



/*!
 * \brief  Analysis Operator History Interval
 *
 * \warning in development
 */

void DCPInpeTs::analysisHistoryInterval()
{

    //init gmock and services.

    auto dataManagerAnalysis = std::make_shared<terrama2::services::analysis::core::DataManager>();


    auto serviceAnalysis = utilsTS::analysis::gmockAndServicesAnalysis(dataManagerAnalysis);

    //call the script analysis history

    std::string scriptAnHistory = R"z(moBuffer = Buffer(BufferType.Out_union, 2., "km")
ids = dcp.influence.by_rule(moBuffer)
maximum = dcp.history.interval.max("pluvio", "16h", "10h", ids)
minimum = dcp.history.interval.min("pluvio", "16h", "10h", ids)
me = dcp.history.interval.mean("pluvio", "16h", "10h", ids)
su = dcp.history.interval.sum("pluvio", "16h", "10h", ids)
med = dcp.history.interval.median("pluvio", "16h", "10h", ids)
standard = dcp.history.interval.standard_deviation("pluvio", "16h", "10h", ids)
var = dcp.history.interval.variance("pluvio", "16h", "10h", ids)
add_value("max", maximum)
add_value("min", minimum)
add_value("mean", me)
add_value("sum", su)
add_value("median", med)
add_value("standard_deviation", standard)
add_value("variance", var))z";


    auto analysis = addAnalysisBase(dataManagerAnalysis, scriptAnHistory);


    auto reprocessingHistoricalDataPtr = std::make_shared<terrama2::services::analysis::core::ReprocessingHistoricalData>();


    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("+00"));


    std::string startDate = "2008-02-20 00:00:00";
    boost::posix_time::ptime startBoostDate(boost::posix_time::time_from_string(startDate));
    boost::local_time::local_date_time lstartDate(startBoostDate.date(), startBoostDate.time_of_day(), zone, true);
    reprocessingHistoricalDataPtr->startDate = std::make_shared<te::dt::TimeInstantTZ>(lstartDate);

    std::string endDate = "2008-03-07 00:00:00";
    boost::posix_time::ptime endBoostDate(boost::posix_time::time_from_string(endDate));
    boost::local_time::local_date_time lendDate(endBoostDate.date(), endBoostDate.time_of_day(), zone, true);
    reprocessingHistoricalDataPtr->endDate = std::make_shared<te::dt::TimeInstantTZ>(lendDate);


    analysis->reprocessingHistoricalData = reprocessingHistoricalDataPtr;

    analysis->schedule.frequency = 10;
    analysis->schedule.frequencyUnit = "h";
    analysis->schedule.id = 1;
    analysis->active = true;

    dataManagerAnalysis->add(analysis);

    serviceAnalysis->addProcessToSchedule(analysis);

    utilsTS::timerCollectorAndAnalysis();

   // utilsTS::database::compareCollectAndAnalysis(utilsTS::typecollectoranalysis::operator_history_interval);

}
