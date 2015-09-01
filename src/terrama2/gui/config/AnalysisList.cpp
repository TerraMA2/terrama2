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
  \file terrama2/gui/config/AnalysisList.cpp

  \brief Definition of methods in class AnalysisList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class AnalysisList

This is the class responsible to enable the comunication with the analysis module

It inherits from QList to facilitate the query. However, the analysis addition or exclusion must be
made through API to provide the syncronization with the server.

*/

// TerraMA2
#include "AnalysisList.hpp"
#include "Services.hpp"

// STL
#include <assert.h>

//! Constructor
AnalysisList::AnalysisList(Services* manager, ModAnalise* service)
{
  assert(manager && service);
  _manager      = manager;
  _service      = service;
  _numInstances = 0;
}

//! Destructor
AnalysisList::~AnalysisList()
{
}

/*! \brief Load the analysis remote list of analyse module

Return true if it managed to make loads the list. Otherwise, display error message and return false

*/
bool AnalysisList::loadData()
{

#ifdef OFFLINE_TEST

  static wsDBColumn attrTestData1[] = {{"Id", WS_COLTYPE_INT},
                                       {"ClasseSuc", WS_COLTYPE_REAL},
                                       {"Limiar_CP", WS_COLTYPE_REAL},
                                       {"Limiar_CA", WS_COLTYPE_REAL}};
  std::vector<wsDBColumn> vetDBCol1(attrTestData1, attrTestData1 + sizeof(attrTestData1)/sizeof(wsDBColumn));
  static wsDBColumn attrTestData2[] = {{"Id", WS_COLTYPE_INT},
                                       {"attr1", WS_COLTYPE_REAL},
                                       {"attr2", WS_COLTYPE_REAL},
                                       {"attr3", WS_COLTYPE_STRING},
                                       {"attr4", WS_COLTYPE_STRING},
                                       {"attr5", WS_COLTYPE_DATETIME}};
  std::vector<wsDBColumn> vetDBCol2(attrTestData2, attrTestData2 + sizeof(attrTestData2)/sizeof(wsDBColumn));

  static wsProjectionParams projPar1 =  { WS_PROJNAME_Mercator, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =  { WS_PROJNAME_Sinusoidal, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};

  static wsRiskMapTheme baseRMT1 =  { 0, "RiskMapTheme 1", projPar1, vetDBCol1};
  static wsRiskMapTheme baseRMT2 =  { 0, "RiskMapTheme 2", projPar2, vetDBCol2};

  static wsRiskMap riskMap1 = {0, baseRMT1, "Mapa 1", "Autor 1", "Inst 1", 1999, 1, 11, 2000, 1, 21, "Detalhes 1"};
  static wsRiskMap riskMap2 = {1, baseRMT2, "Mapa 2", "Autor 2", "Inst 2", 2000, 2, 12, 2001, 2, 22, "Detalhes 2"};
  static wsRiskMap riskMap3 = {2, baseRMT1, "Mapa 3", "Autor 3", "Inst 3", 2001, 3, 13, 2002, 3, 23, "Detalhes 3"};

  static wsPCD pcd_colPts[] = { { "filename1.txt", -47.1021, -23.1231  },
                                { "filename2.txt", -46.7021, -21.2411  },
                                { "filename3.txt", -45.1351, -24.3251  },
                                { "filename4.txt", -47.2441, -23.2337  } };
  std::vector<wsPCD> vectpcd_colPts(pcd_colPts, pcd_colPts + sizeof(pcd_colPts)/sizeof(wsPCD));

  static wsWeatherDataSource testGridData1[] = {
    {0, 1, WS_WDSGEOM_RASTER, "Grid Clima 1", WS_WDSTYPE_PREDICTED_RAIN,   {}, 10.0, 1.0, "Details 1", "Path 1", "Mask 1", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script01.lua", "nav01.nav", "cltfile01.ctl", WS_GRADSDATATYPE_INTEGER16, 1},
    {1, 1, WS_WDSGEOM_POINTS, "Grid Clima 2", WS_WDSTYPE_ACCUMULATED_RAIN, {}, 11.0, 2.0, "Details 2", "Path 2", "Mask 2", WS_WDSFFMT_PCD,        WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script02.lua", "nav02.nav", "cltfile02.ctl", WS_GRADSDATATYPE_FLOAT32,   2},
    {2, 0, WS_WDSGEOM_RASTER, "Grid Clima 3", WS_WDSTYPE_OTHER,            {}, 12.0, 3.0, "Details 3", "Path 3", "Mask 3", WS_WDSFFMT_TIFF,       WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script03.lua", "nav03.nav", "cltfile03.ctl", WS_GRADSDATATYPE_INTEGER16, 3},
    {3, 0, WS_WDSGEOM_POINTS, "Grid Clima 4", WS_WDSTYPE_PREDICTED_RAIN,   {}, 13.0, 4.0, "Details 4", "Path 4", "Mask 4", WS_WDSFFMT_GrADS,      WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script04.lua", "nav04.nav", "cltfile04.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
  };
  std::vector<wsWeatherDataSource> vet1a(testGridData1, testGridData1 + sizeof(testGridData1)/sizeof(wsWeatherDataSource));

  static wsWeatherDataSource testGridData2[] = {
    {4, 1, WS_WDSGEOM_RASTER, "Grid Clima 5", WS_WDSTYPE_PREDICTED_RAIN,   {}, 10.0, 1.0, "Details 1", "Path 1", "Mask 1", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script05.lua", "nav05.nav", "cltfile05.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
    {5, 1, WS_WDSGEOM_RASTER, "Grid Clima 6", WS_WDSTYPE_ACCUMULATED_RAIN, {}, 11.0, 2.0, "Details 2", "Path 2", "Mask 2", WS_WDSFFMT_PCD,        WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script06.lua", "nav06.nav", "cltfile06.ctl", WS_GRADSDATATYPE_INTEGER16, 2},
    {6, 0, WS_WDSGEOM_POINTS, "Grid Clima 7", WS_WDSTYPE_OTHER,            {}, 12.0, 3.0, "Details 3", "Path 3", "Mask 3", WS_WDSFFMT_TIFF,       WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script07.lua", "nav07.nav", "cltfile07.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
    {7, 0, WS_WDSGEOM_POINTS, "Grid Clima 8", WS_WDSTYPE_PREDICTED_RAIN,   {}, 13.0, 4.0, "Details 4", "Path 4", "Mask 4", WS_WDSFFMT_GrADS,      WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script08.lua", "nav08.nav", "cltfile08.ctl", WS_GRADSDATATYPE_INTEGER16, 2},
  };
  std::vector<wsWeatherDataSource> vet1b(testGridData2, testGridData2 + sizeof(testGridData2)/sizeof(wsWeatherDataSource));

  static wsTheme baseTheme1 = { 0, "BaseTheme 1", projPar1, vetDBCol2,  WS_LAYERGEOM_RASTER, 3, 3, 0.0, 0.0 };
  static wsTheme baseTheme2 = { 1, "BaseTheme 2", projPar2, vetDBCol1,  WS_LAYERGEOM_POLYGONS, 3, 3, 0.0, 0.0 };

  static wsAdditionalMap adtMap1 = { 1, baseTheme1, "AdtMap 1", "Author 1", "Inst 1", 0,0,0, 0,0,0, "Dets 1"};
  static wsAdditionalMap adtMap2 = { 2, baseTheme2, "AdtMap 2", "Author 2", "Inst 2", 0,0,0, 0,0,0, "Dets 2"};
  static wsAdditionalMap adtMap3 = { 3, baseTheme1, "AdtMap 3", "Author 3", "Inst 3", 0,0,0, 0,0,0, "Dets 3"};
  static wsAdditionalMap adtMap4 = { 4, baseTheme2, "AdtMap 4", "Author 4", "Inst 4", 0,0,0, 0,0,0, "Dets 4"};

  struct wsAddMapDisplayConfig admdcMap1[] = 
                                   { { adtMap1, WS_STATUS_VISIBLE_IF_WARNING}, { adtMap2, WS_STATUS_NOT_VISIBLE}, 
                                   { adtMap3, WS_STATUS_VISIBLE_IF_WARNING}, { adtMap4, WS_STATUS_NOT_VISIBLE},
                                   };
  std::vector<wsAddMapDisplayConfig> vet2(admdcMap1, admdcMap1 + sizeof(admdcMap1)/sizeof(wsAddMapDisplayConfig));

  static wsAnalysisRule testData[] = {
    {0, WS_ANALYSISTYPE_RISK,  "Teste 1", "Autor 1", "Inst 1", "Detalhes 1", riskMap2, vet2, vet1a, "111", "Script1", 1, WS_ANALYSISCONDITION_ACTIVE, 0, "Cond Script1"},
    {1, WS_ANALYSISTYPE_RISK,  "Teste 2", "Autor 2", "Inst 2", "Detalhes 2", riskMap2, vet2, vet1b, "222", "Script2", 2, WS_ANALYSISCONDITION_INACTIVE, 0, "Cond Script2"},
    {2, WS_ANALYSISTYPE_MODEL,  "Teste 3", "Autor 3", "Inst 3", "Detalhes 3", riskMap3, vet2, vet1a, "Output3", "Script3", 3, WS_ANALYSISCONDITION_CONDITIONED, 1, "Cond Script3"}
  };
  std::vector<wsAnalysisRule> result(testData, testData + sizeof(testData)/sizeof(wsAnalysisRule));

  _numInstances = 3;
#else
  std::vector<struct wsAnalysisRule> result;
  if(_service->analysis__getAnalysisRuleList(result) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível carregar a lista de regras de análise."));
  if(_service->analysis__numInstances(_numInstances) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível verificar o número de instâncias\n do módulo de análise configuradas no sistema."));
#endif

  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    Analysis* ana = new Analysis(result[i]);
    append(ana);
  }

  return true;
}

/*! \brief Adiciona uma nova análise ao conjunto remoto de análises

Return true if it managed to insert a new analyse

*/
bool AnalysisList::addNewAnalysis(const Analysis* ana)
{
  int newid;

#ifdef OFFLINE_TEST
  static int id_generator = 1000;
  newid = id_generator++;
#else
  unsigned i;
  const wsAnalysisRule& data = ana->data();

  std::vector<wsAddMapIDDisplayConfig> addMapIDDCfg;
  struct wsAddMapIDDisplayConfig iddDispCfg;

  for(i=0; i < data.additionalMaps.size(); i++)
  {
    iddDispCfg.addMapID = data.additionalMaps.at(i).addMap.id;
    iddDispCfg.attributes = data.additionalMaps.at(i).attributes;
    iddDispCfg.status = data.additionalMaps.at(i).status;
    iddDispCfg.background = data.additionalMaps.at(i).background;
    iddDispCfg.printImage = data.additionalMaps.at(i).printImage;
    addMapIDDCfg.push_back(iddDispCfg);
  }

  std::vector<int> weatherGridIds;
  for(i=0; i < data.inputWeatherDataSources.size(); i++)
    weatherGridIds.push_back(data.inputWeatherDataSources[i].id);

  if(data.type == WS_ANALYSISTYPE_RISK)  
  {
    if(_service->analysis__newRiskAnalysisRule(data.name,
                                               data.author,  
                                               data.institution,
                                               data.details, 
                                               data.riskMap.id, 
                                               weatherGridIds,
                                               addMapIDDCfg,
                                               data.luaScript,
                                               data.instance,
                                               data.condition,
                                               data.conditionAnalysisID,
                                               data.conditionScript,
                         data.generateImage,
                                               newid) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível adicionar a análise."));
  }
  else if(data.type == WS_ANALYSISTYPE_MODEL)
  {
    if(_service->analysis__newModelAnalysisRule(data.name,
                                                data.author,
                                                data.institution,
                                                data.details, 
                                                weatherGridIds,
                                                data.gridOutputName,
                                                data.luaScript,
                                                data.instance,
                                                data.condition,
                                                data.conditionAnalysisID,
                                                data.conditionScript,
                                                data.gridOutputConfig,
                                                newid) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível adicionar a análise."));
  }
  else if(data.type == WS_ANALYSISTYPE_PCD)
  {
    if(_service->analysis__newPCDAnalysisRule(data.name,
                                              data.author,
                                              data.institution,
                                              data.details,
                                              data.pcd.id,
                                              data.pcdView.id,
                                              addMapIDDCfg,
                                              data.luaScript,
                                              data.instance,
                                              data.condition,
                                              data.conditionAnalysisID,
                                              data.conditionScript,
                                              data.generateImage,
                                              data.alertIcons,
                                              newid) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível adicionar a análise."));
  }
  else
  {
    if(_service->analysis__newTerraMEAnalysisRule(data.name,
                          data.author,  
                          data.institution,
                          data.details, 
                          weatherGridIds,
                          data.luaScript,
                          data.instance,
                          data.condition,
                          data.conditionAnalysisID,
                          data.conditionScript,
                          data.cellularSpace.id,
                          data.fillOperations,
                          newid) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível adicionar a análise."));
  }
#endif

  Analysis* newana = new Analysis;
  *newana = *ana;
  newana->setId(newid);
  append(newana);
  return true;
}

/*! \brief Update the analyse remote data, identified by list index

Return true if it managed to change the analyse

*/

bool AnalysisList::updateAnalysis(int index, const Analysis* ana)
{
  Analysis* oldana = at(index);
  assert(oldana->id() == ana->id());
  
#ifdef OFFLINE_TEST
#else
  const wsAnalysisRule& data = ana->data();
  int dummy;
  unsigned i;

  std::vector<wsAddMapIDDisplayConfig> addMapIDDCfg;
  struct wsAddMapIDDisplayConfig iddDispCfg;

  for(i=0; i < data.additionalMaps.size(); i++)
  {
    iddDispCfg.addMapID = data.additionalMaps.at(i).addMap.id;
    iddDispCfg.attributes = data.additionalMaps.at(i).attributes;
    iddDispCfg.status = data.additionalMaps.at(i).status;
    iddDispCfg.background = data.additionalMaps.at(i).background;
    iddDispCfg.printImage = data.additionalMaps.at(i).printImage;
    addMapIDDCfg.push_back(iddDispCfg);
  }

  std::vector<int> weatherGridIds;
  for(i=0; i < data.inputWeatherDataSources.size(); i++)
    weatherGridIds.push_back(data.inputWeatherDataSources[i].id);

  std::vector<struct wsFillOperation> fillOps;
  for(i=0; i < data.fillOperations.size(); i++)
    fillOps.push_back(data.fillOperations[i]);

  if(data.type == WS_ANALYSISTYPE_RISK)  
  {
    if(_service->analysis__editRiskAnalysisRule(data.id,    
                                                data.name,
                                                data.author,  
                                                data.institution,
                                                data.details, 
                                                weatherGridIds,
                        addMapIDDCfg,
                                                data.luaScript,
                                                data.instance,
                                                data.condition,
                                                data.conditionAnalysisID,
                                                data.conditionScript,
                        data.generateImage,
                                                dummy) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível atualizar a análise."));
  }
  else if(data.type == WS_ANALYSISTYPE_MODEL)
  {
    if(_service->analysis__editModelAnalysisRule(data.id,    
                                                 data.name,
                                                 data.author,  
                                                 data.institution,
                                                 data.details, 
                                                 weatherGridIds,
                                                 data.gridOutputName,
                                                 data.luaScript,
                                                 data.instance,
                                                 data.condition,
                                                 data.conditionAnalysisID,
                                                 data.conditionScript,
                                                 data.gridOutputConfig,
                                                 dummy) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível atualizar a análise."));
  }
  else if(data.type == WS_ANALYSISTYPE_PCD)
  {
    if(_service->analysis__editPCDAnalysisRule(data.id,
                                               data.name,
                                               data.author,
                                               data.institution,
                                               data.details,
                                               addMapIDDCfg,
                                               data.luaScript,
                                               data.instance,
                                               data.condition,
                                               data.conditionAnalysisID,
                                               data.conditionScript,                                               
                                               data.generateImage,
                                               data.alertIcons,
                                               dummy) != SOAP_OK)
      return _manager->showModAnaliseError(QObject::tr("Não foi possível adicionar a análise."));
  }
  else
  {
    if(_service->analysis__editTerraMEAnalysisRule(data.id,    
                           data.name,
                           data.author,  
                           data.institution,
                           data.details, 
                           weatherGridIds,
                           data.luaScript,
                           data.instance,
                           data.condition,
                           data.conditionAnalysisID,
                           data.conditionScript,
                           fillOps,
                           dummy) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível atualizar a análise."));
  }
#endif

  // Remote analyse successfully saved
  *oldana = *ana;
  emit afterUpdateAnalysis(ana->id(), ana->name());
  return true;
}

/*! \brief Remote the analyse remote, identified by the list index. If the analyse is model based
and the deleteLayers parameter is true, the generated layes also be removed.

*/
bool AnalysisList::deleteAnalysis(int index, bool deleteLayers)
{
  Analysis* ana = at(index);
  
#ifdef OFFLINE_TEST
#else
  int dummy;
  if(_service->analysis__deleteAnalysisRule(ana->id(), deleteLayers, dummy) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível remover a análise."));
#endif

  removeAt(index);
  delete ana;

  return true;
}

/*! \brief Return the analyse index associated with an identifier
Return -=1 if the identifier has not been found

*/
int AnalysisList::findAnalysisIndex(int analysisId) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->id() == analysisId)
      return i;
  }
  return -1;
}

/*! \brief Return the analyse associated with an identifier

Return NULL if the identifier has not been found

*/
Analysis* AnalysisList::findAnalysis(int analysisId) const
{
  int index = findAnalysisIndex(analysisId);
  return (index != -1) ? at(index) : NULL;
}
