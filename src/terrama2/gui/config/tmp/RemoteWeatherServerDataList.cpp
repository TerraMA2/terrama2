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
  \file terrama2/gui/config/RemoteWeatherServerDataList.cpp

  \brief Definition of methods in class RemoteWeatherServerDataList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


/*!
\class RemoteWeatherServerDataList

É a classe responsável por efetuar a comunicação com o
módulo de coleta para tratar as listas de fomatos de dados.

Herda de QList para facilitar a consulta aos dados, porém 
adição e remoção de formatos de dados devem ser feitas através da 
API própria para garantir a sincronização com o servidor.
*/

// STL
#include <assert.h>

// TerraMA2
#include "RemoteWeatherServerData.hpp"
#include "RemoteWeatherServerDataList.hpp"
#include "services.h"

// QT
#include <QObject>


//! Construtor
RemoteWeatherServerDataList::RemoteWeatherServerDataList(Services* manager, ModColeta* service, int idServer)
{
  assert(manager && service);
  _manager = manager;
  _service = service;
  _idServer = idServer;
}

//! Destrutor
RemoteWeatherServerDataList::~RemoteWeatherServerDataList()
{
}

/*! \brief Carrega lista remota de formatos de dados do módulo de coleta

Retorna true se conseguiu carregar a lista.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool RemoteWeatherServerDataList::loadData(int idServ)
{
  // Efetua chamada remota
#ifdef OFFLINE_TEST
  static wsProjectionParams projPar1 =	{ WS_PROJNAME_LatLong, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =	{ WS_PROJNAME_UTM, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};
  static wsPCD pcd_colPts[] = { { "filename1.txt", -47.1021, -23.1231  },
                                { "filename2.txt", -46.7021, -21.2411  },
                                { "filename3.txt", -45.1351, -24.3251  },
                                { "filename4.txt", -47.2441, -23.2337  } };
  std::vector<wsPCD> vectpcd_colPts(pcd_colPts, pcd_colPts + sizeof(pcd_colPts)/sizeof(wsPCD));

  static wsWeatherDataSource testData[] = {
								{	0, 0, WS_WDSGEOM_POINTS, "Data 1", WS_WDSTYPE_PREDICTED_RAIN,   projPar1, 1.1, 4.1, "Detalhes 1", "Path Data 1", "", WS_WDPIT_RADIUS_TOUCH, -1, "", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script01.lua", "nav01.nav", "cltfile01.ctl", WS_GRADSDATATYPE_INTEGER16, 1 },
								{	1, 1, WS_WDSGEOM_POINTS, "Data 2", WS_WDSTYPE_ACCUMULATED_RAIN, projPar2, 1.2, 4.1, "Detalhes 2", "Path Data 2", "", WS_WDPIT_RADIUS_TOUCH, -1, "", WS_WDSFFMT_PCD,        WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script02.lua", "nav02.nav", "cltfile02.ctl", WS_GRADSDATATYPE_FLOAT32,   2 },
								{	2, 0, WS_WDSGEOM_RASTER, "Data 3", WS_WDSTYPE_OTHER,            projPar1, 1.3, 3.1, "Detalhes 3", "Path Data 3", "", WS_WDPIT_RADIUS_TOUCH, -1, "", WS_WDSFFMT_TIFF,       WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script03.lua", "nav03.nav", "cltfile03.ctl", WS_GRADSDATATYPE_INTEGER16, 3 },
								{	3, 1, WS_WDSGEOM_RASTER, "Data 4", WS_WDSTYPE_ACCUMULATED_RAIN, projPar1, 1.4, 3.1, "Detalhes 4", "Path Data 4", "", WS_WDPIT_RADIUS_TOUCH, -1, "", WS_WDSFFMT_GrADS,      WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script04.lua", "nav04.nav", "cltfile04.ctl", WS_GRADSDATATYPE_FLOAT32,   4 },
                                };

  std::vector<wsWeatherDataSource> result(testData, testData + sizeof(testData)/sizeof(wsWeatherDataSource));
#else
  std::vector<struct wsWeatherDataSource> result;
  if(_service->collection__getWeatherDataSourceListByServer(idServ, result) != SOAP_OK)
  {
	  QString error("Não foi possível carregar a lista de séries de dados do servidor (id = ");
	  error.append(QString::number(idServ) + ").");

      return _manager->showModColetaError(error);
  }
#endif

  // Cria objetos do tipo RemoteWeatherServerData para cada item retornada
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    RemoteWeatherServerData* rwsd = new RemoteWeatherServerData(result[i]);
    append(rwsd);
  }
  return true;
}

/*! \brief Adiciona um novo formato de dados a lista dos formatos do servidor

Retorna true se conseguiu adicionar o formato de dados novo.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

Se a inserção remota foi bem sucedida, inclui na lista uma CÓPIA do formato de dados
recebido, com seu identificador do formato de dados atualizado para refletir o 
identificador remoto
*/
bool RemoteWeatherServerDataList::addNewRemoteWeatherServerData(int idServ, const RemoteWeatherServerData* rwsd, int &newId)
{
#ifdef OFFLINE_TEST
  static int id_generator = 1000;
  newId = id_generator++;
#else
  const wsWeatherDataSource& data = rwsd->data();
  if(_service->collection__newWeatherDataSource(idServ,
                                                data.geometry,
                                                data.name,
                                                data.type,
                                                data.projection,
                                                data.spatialResolution,
                                                data.updateFrequencyMins,
                                                data.details,
                                                data.path,
                                                data.mask,
												data.timezone,
												data.prefix,
												data.unit,
                                                data.pcd_influenceType,
                                                data.pcd_influenceThemeId,
                                                data.pcd_influenceAttribute,
                                                data.format,
                                         	      data.asciiGrid_coordUnit,
                                                data.pcd_collectionPoints,
                                         	      data.pcd_collectionRuleLuaScript,
                                                data.pcd_surfaces,
                                                data.tiff_navigationFile,
                                         	      data.grads_ctlFile,
                                         	      data.grads_dataType,
                                         	      data.grads_multiplier,
                                         	      data.grads_swap,
                                         	      data.grads_numBands,
                                         	      data.grads_bandHeader,
                                         	      data.grads_bandTrailler,
                                         	      data.grads_bandTimeOffset,
											    data.wcs_dummy,
												data.wfs_pcd_id_attr,
												data.wfs_pcd_timestamp_attr,
												data.wfs_pcd_timestamp_mask,
                                                data.filters,
												data.intersection,
												data.auxiliar_id,
                                                newId) != SOAP_OK)
    return _manager->showModColetaError(QObject::tr("Não foi possível adicionar a série de dados."));
#endif

  // Formato de dados salvo remotamente com sucesso.  Atualiza lista de formato de dados
  RemoteWeatherServerData* newRwsd = new RemoteWeatherServerData;
  *newRwsd = *rwsd;
  newRwsd->setId(newId);
  
  std::vector<struct wsWeatherDataPcdSurface> result;
  if(_service->collection__getSurfaceList(newId,result) != SOAP_OK)
	  return _manager->showModColetaError(QObject::tr("Não foi possível adicionar a série de dados."));
  newRwsd->setPcd_collectionSurfaces(result);

  append(newRwsd);

  return true;
}

/*! \brief Atualiza os dados remotos de um formato de dado, identificada por sua
           chave (id) no banco

Retorna true se conseguiu alterar o banco.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. Se a atualização remota
foi bem sucedida, copia dados salvos para a lista. 

\param id identificador do dado no banco
\param rwsd   Dados a serem salvos
*/
bool RemoteWeatherServerDataList::updateRemoteWeatherServerData(int idServ, int id, RemoteWeatherServerData* rwsd)
{
  RemoteWeatherServerData* oldRwsd = findServerDataById(id);
  assert(oldRwsd->id() == rwsd->id());

#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  const wsWeatherDataSource& data = rwsd->data();
  if(_service->collection__editWeatherDataSource(id,
                                                 idServ,
                                                 data.name,
                                                 data.type,
                                                 data.projection,
                                                 data.spatialResolution,
                                                 data.updateFrequencyMins,
                                                 data.details,
                                                 data.path,
                                                 data.mask,
												 data.timezone,
												 data.prefix,
												 data.unit,
                                                 data.pcd_influenceType,
                                                 data.pcd_influenceThemeId,
                                                 data.pcd_influenceAttribute,
                                                 data.format,
                                         	       data.asciiGrid_coordUnit,
                                         	       data.pcd_collectionPoints,
                                         	       data.pcd_collectionRuleLuaScript,
                                                 data.pcd_surfaces,
                                                 data.tiff_navigationFile,
                                         	       data.grads_ctlFile,
                                         	       data.grads_dataType,
                                         	       data.grads_multiplier,
                                         	       data.grads_swap,
                                         	       data.grads_numBands,
                                         	       data.grads_bandHeader,
                                         	       data.grads_bandTrailler,
                                         	       data.grads_bandTimeOffset,
											     data.wcs_dummy,
												 data.wfs_pcd_id_attr,
												 data.wfs_pcd_timestamp_attr,
												 data.wfs_pcd_timestamp_mask,
                                                 data.filters,
												 data.intersection,
												 data.auxiliar_id,
                                                 dummy) != SOAP_OK)
    return _manager->showModColetaError(QObject::tr("Não foi possível atualizar a série de dados."));
#endif

  std::vector<struct wsWeatherDataPcdSurface> result;
  if(_service->collection__getSurfaceList(rwsd->id(),result) != SOAP_OK)
	  return _manager->showModColetaError(QObject::tr("Não foi possível adicionar a série de dados."));
  rwsd->setPcd_collectionSurfaces(result);

  // Fonte de dados salva remotamente com sucesso.  Atualiza lista de fontes de dados
  *oldRwsd = *rwsd;

  emit afterUpdateRWSData(rwsd->id(), rwsd->name());
  return true;
}

/*! \brief Remove uma formado de dados remota, identificada por sua 
           chave (id) no banco

Retorna true se conseguiu remover o formato de dados.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 
*/
bool RemoteWeatherServerDataList::deleteRemoteWeatherServerData(int id, bool deleteLayers)
{
  RemoteWeatherServerData* rwsd = findServerDataById(id);
  int index = findIndexServerDataById(id);
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  if(_service->collection__deleteWeatherDataSource(id, deleteLayers, dummy) != SOAP_OK)
    return _manager->showModColetaError(QObject::tr("Não foi possível remover a série de dados."));
#endif

  // Conseguimos apagar a análise do servidor remoto.  Remove da lista
  removeAt(index);
  delete rwsd;

  return true;
}

int RemoteWeatherServerDataList::findIndexServerDataById(int id )
{
	int ret = -1;

	for (int i=0; i< count() && ret == -1; i++)
	{
		RemoteWeatherServerData *ptr;

		ptr = value(i);

		if (id == ptr->id())
			ret = i;
	}

	return ret;
}

RemoteWeatherServerData* RemoteWeatherServerDataList::findServerDataById( int id )
{
	RemoteWeatherServerData* rwServerData = NULL;
	int index = findIndexServerDataById(id);

	if (index != -1)
		rwServerData = value(index);

	return rwServerData;
}

