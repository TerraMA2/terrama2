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
  \file terrama2/gui/config/AddicionalMapList.cpp

  \brief Definition of methods in class AdditionalMapList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


/*!
\class AdditionalMapList

This class is responsible to make a communication with the analysis module

AdditionalMapLiss inherits from QList to simplify the data query. However, the add and remove
maps must be done through owner API to provide the server syncronization.

*/

// TerraMA2
#include "AdditionalMapList.hpp"
#include "Services.hpp"

// STL
#include <assert.h>

//! Constructor
AdditionalMapList::AdditionalMapList(Services* manager, ServPlanos* service)
{
  assert(manager && service);
  _manager = manager;
  _service = service;
}

//! Destructor
AdditionalMapList::~AdditionalMapList()
{
}

#ifdef OFFLINE_TEST
  static wsDBColumn attrTestDB1[] = {{"Id", WS_COLTYPE_INT},
                                       {"ClasseSuc", WS_COLTYPE_REAL},
                                       {"Limiar_CP", WS_COLTYPE_REAL},
                                       {"Limiar_CA", WS_COLTYPE_REAL}};
  static wsDBColumn attrTestDB2[] = {{"Id", WS_COLTYPE_INT},
                                       {"attr1", WS_COLTYPE_REAL},
                                       {"attr2", WS_COLTYPE_REAL},
                                       {"attr3", WS_COLTYPE_STRING},
                                       {"attr4", WS_COLTYPE_STRING},
                                       {"attr5", WS_COLTYPE_DATETIME}};

  static wsProjectionParams projPar1 =  { WS_PROJNAME_LatLong, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =  { WS_PROJNAME_UTM, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar3 =  { WS_PROJNAME_CylindricalEquidistant, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar4 =  { WS_PROJNAME_LatLong, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};

  std::vector<wsDBColumn> vet1a(attrTestDB1, attrTestDB1 + sizeof(attrTestDB1)/sizeof(wsDBColumn));
  std::vector<wsDBColumn> vet2a(attrTestDB2, attrTestDB2 + sizeof(attrTestDB2)/sizeof(wsDBColumn));

  static wsTheme baseRMT[] =  { { 0, "AdditionalMapTheme 1", projPar1, vet1a, WS_LAYERGEOM_POLYGONS, 2, 4, 2.1, 3.2 },
                                { 1, "AdditionalMapTheme 2", projPar2, vet2a, WS_LAYERGEOM_RASTERFILE, 3, 6, 2.3, 4.3 },
                                { 2, "AdditionalMapTheme 3", projPar3, vet2a, WS_LAYERGEOM_LINES, 3, 6, 2.3, 4.3 },
                                { 3, "AdditionalMapTheme 4", projPar4, vet1a, WS_LAYERGEOM_RASTER, 3, 5, 45.2, 91.2},
                                { 4, "AdditionalMapTheme 5", projPar1, vet2a, WS_LAYERGEOM_SHAPEFILE, 3, 6, 2.3, 4.3 },
                                { 5, "AdditionalMapTheme 6", projPar2, vet2a, WS_LAYERGEOM_RASTERFILE, 9, 2, 283.2, 23.2},
                              };
#endif


/*! \brief Load the accessible remote theme list for additional maps of analysis module

Retrieve true if it managed to load list. Otherwise, display error message and return false

*/
bool AdditionalMapList::loadAdditionalMapThemeData()
{
  // Make remote connection}
#ifdef OFFLINE_TEST
/*  static wsDBColumn attrTestData1[] = {{"Id", WS_COLTYPE_INT},
                                       {"ClasseSuc", WS_COLTYPE_REAL},
                                       {"Limiar_CP", WS_COLTYPE_REAL},
                                       {"Limiar_CA", WS_COLTYPE_REAL}};
  std::vector<wsDBColumn> vet1(attrTestData1, attrTestData1 + sizeof(attrTestData1)/sizeof(wsDBColumn));
  static wsDBColumn attrTestData2[] = {{"Id", WS_COLTYPE_INT},
                                       {"attr1", WS_COLTYPE_REAL},
                                       {"attr2", WS_COLTYPE_REAL},
                                       {"attr3", WS_COLTYPE_STRING},
                                       {"attr4", WS_COLTYPE_STRING},
                                       {"attr5", WS_COLTYPE_DATETIME}};
  std::vector<wsDBColumn> vet2(attrTestData2, attrTestData2 + sizeof(attrTestData2)/sizeof(wsDBColumn));

  static wsProjectionParams projPar1 =  { WS_PROJNAME_Albers, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =  { WS_PROJNAME_Miller, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};

  static wsTheme baseRMT[] =  { { 0, "AdditionalMapTheme 1", projPar1, vet1, WS_LAYERGEOM_POLYGONS, 2, 4, 2.1, 3.2 },
                                { 1, "AdditionalMapTheme 2", projPar2, vet2, WS_LAYERGEOM_LINES, 3, 6, 2.3, 4.3 },
                                { 2, "AdditionalMapTheme 3", projPar1, vet1, WS_LAYERGEOM_RASTER, 3, 5, 45.2, 91.2},
                                { 3, "AdditionalMapTheme 4", projPar2, vet2, WS_LAYERGEOM_RASTERFILE, 9, 2, 283.2, 23.2},
                              };*/


  std::vector<wsTheme> result(baseRMT, baseRMT + sizeof(baseRMT)/sizeof(wsTheme));
#else
  std::vector<struct wsTheme> result;
  if (_service->layer__getEligibleThemesForNewAdditionalMap(result) != SOAP_OK)
    return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de temas disponíveis \npara mapas adicionais."));
#endif


//  _additionalMapThemesEligible = result;

  unsigned nPos;
  struct wsTheme theme;
  enum wsLayerGeometry geometry;

  _additionalMapThemesEligibleGrid.clear();
  _additionalMapThemesEligibleVector.clear();

  for (nPos = 0; nPos < result.size(); nPos++)
  {
   theme = result.at(nPos);
   geometry = theme.geometry;

   if ( (geometry == WS_LAYERGEOM_RASTER) || (geometry == WS_LAYERGEOM_RASTERFILE) )
    _additionalMapThemesEligibleGrid.push_back(theme);
   else
        _additionalMapThemesEligibleVector.push_back(theme);

  }

  return true;
}

/*! \brief Load the remote list of additional maps of analysis module

Return true if it managed to load the list. Otherwise, display error message and return false.

*/
bool AdditionalMapList::loadData()
{
  // Make remote connection
#ifdef OFFLINE_TEST
/*  static wsDBColumn attrTestData1[] = {{"Id", WS_COLTYPE_INT},
                                       {"ClasseSuc", WS_COLTYPE_REAL},
                                       {"Limiar_CP", WS_COLTYPE_REAL},
                                       {"Limiar_CA", WS_COLTYPE_REAL}};
  std::vector<wsDBColumn> vet1(attrTestData1, attrTestData1 + sizeof(attrTestData1)/sizeof(wsDBColumn));
  static wsDBColumn attrTestData2[] = {{"Id", WS_COLTYPE_INT},
                                       {"attr1", WS_COLTYPE_REAL},
                                       {"attr2", WS_COLTYPE_REAL},
                                       {"attr3", WS_COLTYPE_STRING},
                                       {"attr4", WS_COLTYPE_STRING},
                                       {"attr5", WS_COLTYPE_DATETIME}};
  std::vector<wsDBColumn> vet2(attrTestData2, attrTestData2 + sizeof(attrTestData2)/sizeof(wsDBColumn));

  static wsProjectionParams projPar1 =  { WS_PROJNAME_PolarStereographic, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =  { WS_PROJNAME_Polyconic, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};

  static wsTheme baseRMT1 = { 0, "AdditionalMapTheme 1", projPar1, vet1, WS_LAYERGEOM_POLYGONS, 2, 4, 2.1, 3.2 };
  static wsTheme baseRMT2 = { 1, "AdditionalMapTheme 2", projPar2, vet2, WS_LAYERGEOM_LINES, 3, 6, 2.3, 4.3 };
  static wsTheme baseRMT3 = { 2, "AdditionalMapTheme 3", projPar1, vet1, WS_LAYERGEOM_RASTER, 3, 5, 45.2, 91.2};
  static wsTheme baseRMT4 = { 3, "AdditionalMapTheme 4", projPar2, vet2, WS_LAYERGEOM_RASTERFILE, 9, 2, 283.2, 23.2};
*/
  static wsAdditionalMap testData[] = {
    {0, baseRMT[0], "Mapa adicional 1", "Autor 1", "Inst 1", 1999, 1, 11, 2000, 1, 21, "Detalhes 1"},
    {1, baseRMT[1], "Mapa adicional 2", "Autor 2", "Inst 2", 2000, 2, 12, 2001, 2, 22, "Detalhes 2"},
    {2, baseRMT[2], "Mapa adicional 3", "Autor 3", "Inst 3", 2000, 2, 12, 2001, 2, 22, "Detalhes 3"},
    {3, baseRMT[3], "Mapa adicional 4", "Autor 4", "Inst 4", 2001, 3, 13, 2002, 3, 23, "Detalhes 4"},
    {4, baseRMT[4], "Mapa adicional 5", "Autor 5", "Inst 5", 2002, 4, 14, 2003, 4, 24, "Detalhes 5"},
    {5, baseRMT[5], "Mapa adicional 6", "Autor 6", "Inst 6", 2003, 5, 15, 2004, 5, 25, "Detalhes 6"}
  };
  std::vector<wsAdditionalMap> result(testData, testData + sizeof(testData)/sizeof(wsAdditionalMap));
#else
  std::vector<struct wsAdditionalMap> result;
  if(_service->layer__getAdditionalMapList(result) != SOAP_OK)
    return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de mapas adicionais do módulo de análise."));
#endif

  // Create objects of additionalMap type for each map retrieved
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    AdditionalMap* map = new AdditionalMap(result[i]);
    append(map);
  }
  return true;
}

/*! \brief Add a new additional map to the remote maps set

Return true if it managed to add the new additional map. Otherwise, display error message and then return false

If the remote insert has been successed then include a copy of additional map received with 
refresed server identifier to set as remote identifier

*/
bool AdditionalMapList::addNewAdditionalMap(const AdditionalMap* rm, bool isGrid)
{
  std::vector<int> newIDnewCollectionID(2);

#ifdef OFFLINE_TEST
  static int id_generator = 1000;
  newId = id_generator++;
#else
  const wsAdditionalMap& data = rm->data();
  if (_service->layer__newAdditionalMap(data.baseTheme.id,
                                   data.name,
                                   data.author,
                                   data.institution,
                                   data.creationYear,
                                   data.creationMonth,
                                   data.creationDay,
                                   data.expirationYear,
                                   data.expirationMonth,
                                   data.expirationDay,
                                   data.details,
                   data.attrProperties,
                   isGrid,
                   newIDnewCollectionID) != SOAP_OK)

      return _manager->showModServPlanosError(QObject::tr("Não foi possível adicionar o Mapa Adicional."));
#endif

  // objeto monitorado salvo remotamente com sucesso.  Atualiza lista de objetos monitorados
  AdditionalMap* newRm = new AdditionalMap;
  *newRm = *rm;
  newRm->setId(newIDnewCollectionID[0]);
  if(isGrid)
  newRm->setDataCollectionId(newIDnewCollectionID[1]);
  else
  newRm->setDataCollectionId(0);

  append(newRm);

  return true;
}

/*! \brief Refresh the remote datas of additional map, identifyng by an index

Return true if it managed to change the additional map. Otherwise, display error message and then
return false. If the remote update has been successed, copy the storaged data to the list

*/
bool AdditionalMapList::updateAdditionalMap(int id, const AdditionalMap* rm)
{
  AdditionalMap* oldRm = findAdditionalMap(id);
  assert(oldRm->id() == rm->id());
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  const wsAdditionalMap& data = rm->data();
  if (_service->layer__editAdditionalMap(data.id,
                                   data.baseTheme.id,
                                   data.name,
                                   data.author,
                                   data.institution,
                                   data.creationYear,
                                   data.creationMonth,
                                   data.creationDay,
                                   data.expirationYear,
                                   data.expirationMonth,
                                   data.expirationDay,
                                   data.details,
                   data.attrProperties, 
                                   dummy) != SOAP_OK)
      return _manager->showModServPlanosError(QObject::tr("Não foi possível atualizar os dados do Mapa Adicional."));
#endif

  // Additional map successfully storaged remotely. Refresh the list of additional maps
  *oldRm = *rm;
  
  emit afterUpdateAdditionalMap(rm->data().dataCollectionId, rm->name());

  return true;
}

/*! \brief Remove an additional map, identifying by own key in database

Return true if it managed to remove the additional map. Otherwise, display error message and return false

*/
bool AdditionalMapList::deleteAdditionalMap(int id)
{
  AdditionalMap* rm = findAdditionalMap(id);
  int index = findAdditionalMapIndex(id);
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  if(_service->layer__deleteAdditionalMap(id, dummy) != SOAP_OK)
    return _manager->showModServPlanosError(QObject::tr("Não foi possível remover o Mapa Adicional."));
#endif

  // We made to remove the additional map of remote server. Remove from list.
  removeAt(index);
  delete rm;
  return true;
}

/*! \brief Return the additional map index associated with an identifier

Return -1 if the identifier has not been found.

*/
int AdditionalMapList::findAdditionalMapIndex(int mapId) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->id() == mapId)
      return i;
  }
  return -1;
}

/*! \brief Return the additional map associated with an identifier

Return NULL if the identifier has not been found.

*/
AdditionalMap* AdditionalMapList::findAdditionalMap(int mapId) const
{
  int index = findAdditionalMapIndex(mapId);
  return (index != -1) ? at(index) : NULL;
}

