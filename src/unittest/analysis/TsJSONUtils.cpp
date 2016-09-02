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
  \file unittest/analysis/TsJSONUtils.hpp

  \brief Tests for JSON Utility class

  \author Paulo R. M. Oliveira
*/


#include "TsJSONUtils.hpp"

//TerraMA2
#include <terrama2/services/analysis/core/JSonUtils.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>

using namespace terrama2::services::analysis::core;

void TsJSONUtils::testJSON()
{
  std::string script = "x = dcp.min(\"Serra do Mar\", \"pluvio\", 2, Buffer.OBJECT_PLUS_EXTERN)\n"
                       "add_value(\"min\", x)\n"
                       "x = dcp.max(\"Serra do Mar\", \"pluvio\", 2, Buffer.OBJECT_PLUS_EXTERN)\n"
                       "add_value(\"max\", x)\n"
                       "x = dcp.mean(\"Serra do Mar\", \"pluvio\", 2, Buffer.OBJECT_PLUS_EXTERN)\n"
                       "add_value(\"mean\", x)\n"
                       "x = dcp.median(\"Serra do Mar\", \"pluvio\", 2, Buffer.OBJECT_PLUS_EXTERN)\n"
                       "add_value(\"median\", x)\n"
                       "x = dcp.standardDeviation(\"Serra do Mar\", \"pluvio\", 2, Buffer.OBJECT_PLUS_EXTERN)\n"
                       "add_value(\"standardDeviation\", x)\n";

  AnalysisPtr analysis;
  analysis->id = 1;
  analysis->name = "Min DCP";
  analysis->script = script;
  analysis->scriptLanguage = PYTHON;
  analysis->type = MONITORED_OBJECT_TYPE;
  analysis->active = true;

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = 1;
  monitoredObjectADS.type = DATASERIES_MONITORED_OBJECT_TYPE;


  AnalysisDataSeries dcpADS;
  dcpADS.id = 2;
  dcpADS.dataSeriesId = 2;
  dcpADS.type = ADDITIONAL_DATA_TYPE;
  dcpADS.metadata["INFLUENCE_TYPE"] = "RADIUS_CENTER";
  dcpADS.metadata["RADIUS"] = "50";

  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(dcpADS);
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysis->analysisDataSeriesList = analysisDataSeriesList;

  QJsonObject jsonObj = toJson(analysis);
  Analysis decodedAnalysis = fromAnalysisJson(jsonObj);

  QVERIFY(analysis->id == decodedAnalysis.id);
  QVERIFY(analysis->name == decodedAnalysis.name);
  QVERIFY(analysis->script == decodedAnalysis.script);
  QVERIFY(analysis->scriptLanguage == decodedAnalysis.scriptLanguage);
  QVERIFY(analysis->type == decodedAnalysis.type);
  QVERIFY(analysis->active == decodedAnalysis.active);
  QVERIFY(analysis->analysisDataSeriesList.size() == decodedAnalysis.analysisDataSeriesList.size());

  for (int i = 0; i < analysis->analysisDataSeriesList.size(); ++i)
  {
    QVERIFY(analysis->analysisDataSeriesList[i].id == decodedAnalysis.analysisDataSeriesList[i].id);
    QVERIFY(analysis->analysisDataSeriesList[i].type == decodedAnalysis.analysisDataSeriesList[i].type);
    QVERIFY(analysis->analysisDataSeriesList[i].dataSeriesId == decodedAnalysis.analysisDataSeriesList[i].dataSeriesId);
    QVERIFY(analysis->analysisDataSeriesList[i].alias == decodedAnalysis.analysisDataSeriesList[i].alias);
    QVERIFY(analysis->analysisDataSeriesList[i].metadata == decodedAnalysis.analysisDataSeriesList[i].metadata);
  }
}
