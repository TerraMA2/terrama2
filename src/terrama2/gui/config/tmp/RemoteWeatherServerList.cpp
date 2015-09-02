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
  \file terrama2/gui/config/RemoteWeatherServerList.cpp

  \brief Definition of methods in class RemoteWeatherServerList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/

/*!
\class RemoteWeatherServerList

É a classe responsável por efetuar a comunicação com o
módulo de coleta para obter informacoes sobre os servidores.

Herda de QList para facilitar a consulta aos dados, porém 
adição e remoção de servidores devem ser feitas através da 
API própria para garantir a sincronização com o servidor.
*/

// STL
#include <assert.h>

// TerraMA2
#include "RemoteWeatherServerList.hpp"
#include "Services.hpp"


//! Construtor
RemoteWeatherServerList::RemoteWeatherServerList(Services* manager, ModColeta* service)
{
  assert(manager && service);
  _manager = manager;
  _service = service;
}

//! Destrutor
RemoteWeatherServerList::~RemoteWeatherServerList()
{
}

/*! \brief Carrega lista remota de servidores do módulo de coleta

Retorna true se conseguiu carregar a lista.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool RemoteWeatherServerList::loadData()
{
  // Efetua chamada remota
#ifdef OFFLINE_TEST
  static wsRemoteWeatherServer testData[] = {
    {0, "Servidor 1", "Descr Servidor 1", "tsts", 8080, WS_PROTOCOL_FTP, "usr", "pass", true, 2, "caminho 1"},
    {1, "Servidor 2", "Descr Servidor 2", "tsts", 8080, WS_PROTOCOL_FILE, "usr", "pass", true, 2, "caminho 2"},
    {2, "Servidor 3", "Descr Servidor 3", "tsts", 8080, WS_PROTOCOL_FTP, "usr", "pass", true, 2, "caminho 3"},
  };
  std::vector<wsRemoteWeatherServer> result(testData, testData + sizeof(testData)/sizeof(wsRemoteWeatherServer));
#else
  std::vector<struct wsRemoteWeatherServer> result;
  if(_service->collection__getRemoteWeatherServerList(result) != SOAP_OK)
      return _manager->showModColetaError(QObject::tr("Não foi possível carregar a lista de servidores."));
#endif

  // Cria objetos do tipo RemoteWeatherServer para cada item retornada
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    RemoteWeatherServer* rws = new RemoteWeatherServer(result[i]);
	rws->loadServerDataList(_manager, _service);
    append(rws);
  }
  return true;
}

/*! \brief Adiciona um novo servidor ao conjunto remoto de servidores

Retorna true se conseguiu adicionar o novo servidor.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

Se a inserção remota foi bem sucedida, inclui na lista uma CÓPIA do servidor
recebido, com seu identificador do servidor atualizado para refletir o 
identificador remoto
*/
bool RemoteWeatherServerList::addNewRemoteWeatherServer(const RemoteWeatherServer* rws, int &newId)
{
#ifdef OFFLINE_TEST
  static int id_generator = 1000;
  newId = id_generator++;
#else
  const wsRemoteWeatherServer& data = rws->data();
  if(_service->collection__newRemoteWeatherServer(data.name,
	                                              data.details,
												  data.address,
												  data.nPort,
												  data.protocol,
												  data.userName,
												  data.password,
												  data.lActive,
												  data.nIntervalMinutes,
												  data.basePath,
												  newId) != SOAP_OK)
      return _manager->showModColetaError(QObject::tr("Não foi possível adicionar o servidor."));
#endif

  // Servidor salvo remotamente com sucesso.  Atualiza lista de servidores
  RemoteWeatherServer* newRws = new RemoteWeatherServer;
  *newRws = *rws;
  newRws->setId(newId);

  newRws->initDatalist(_manager, _service);

  append(newRws);

  return true;
}

/*! \brief Atualiza os dados remotos de um servidor, identificado por seu 
           índice na lista

Retorna true se conseguiu alterar o servidor.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. Se a atualização remota
foi bem sucedida, copia dados salvos para a lista. 

\param id identificador do servidor no banco
\param rws   Dados a serem salvos
*/
bool RemoteWeatherServerList::updateRemoteWeatherServer(int id, const RemoteWeatherServer* rws)
{
  RemoteWeatherServer* oldRws = findServerById(id);
  assert(oldRws->id() == rws->id());
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  const wsRemoteWeatherServer& data = rws->data();
  if(_service->collection__editRemoteWeatherServer(id,
	                                               data.name,
												   data.details,
												   data.address,
												   data.nPort,
												   data.protocol,
												   data.userName,
												   data.password,
												   data.lActive,
												   data.nIntervalMinutes,
												   data.basePath,
												   dummy) != SOAP_OK)
      return _manager->showModColetaError(QObject::tr("Não foi possível atualizar os dados do servidor."));
#endif

  // Servidor salvo remotamente com sucesso.  Atualiza lista de servidores
  *oldRws = *rws;
  return true;
}

/*! \brief Remove um servidor remoto, identificado por sua
           chave (id) no banco

Retorna true se conseguiu remover o servidor.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 
*/
bool RemoteWeatherServerList::deleteRemoteWeatherServer(int id, bool deleteLayers)
{
  RemoteWeatherServer* rws = findServerById(id);
  int index = findIndexServerById(id);
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  if(_service->collection__deleteRemoteWeatherServer(id, deleteLayers, dummy) != SOAP_OK)
    return _manager->showModColetaError(QObject::tr("Não foi possível remover o servidor."));
#endif

  // Conseguimos apagar o servidor do servidor remoto.  Remove da lista
  removeAt(index);
  delete rws;
  return true;
}

int RemoteWeatherServerList::findIndexServerById(int id )
{
	int ret = -1;

	for (int i=0; i< count() && ret == -1; i++)
	{
		RemoteWeatherServer *ptr;

		ptr = value(i);

		if (id == ptr->id())
			ret = i;
	}

	return ret;
}

RemoteWeatherServer* RemoteWeatherServerList::findServerById(int id )
{
	RemoteWeatherServer* rwServer = NULL;
	int index = findIndexServerById(id);

	if (index != -1)
		rwServer = value(index);

/*	for (int i=0; i< count() && rwServer == NULL; i++)
	{
		RemoteWeatherServer *ptr;

		ptr = value(i);

		if (id == ptr->id())
			rwServer = value(i);
	}*/

	return rwServer;
}

RemoteWeatherServerData* RemoteWeatherServerList::findServerDataById(int srvId, int id )
{
	RemoteWeatherServer *rwServer;
	RemoteWeatherServerData *rwServerData;

	rwServer = RemoteWeatherServerList::findServerById(srvId );

	if ( rwServer !=NULL )
		rwServerData = rwServer->findServerDataById(id );

	return rwServerData;
}

/*! \brief Carrega lista remota de temas possiveis para serem usados como máscara pelos filtros
Retorna true se conseguiu carregar a lista. Caso contrário, MOSTRA mensagem de erro e retorna false.*/
bool RemoteWeatherServerList::loadFilterThemeData()
{
	// Efetua chamada remota
	#ifdef OFFLINE_TEST
		std::vector<wsTheme> result(baseRMT, baseRMT + sizeof(baseRMT)/sizeof(wsTheme));
	#else
		std::vector<struct wsTheme> result;
		if (_service->collection__getEligibleThemesForFilter(result) != SOAP_OK)
			return _manager->showModColetaError(QObject::tr("Não foi possível carregar a lista de temas disponíveis \npara serem usados como máscara pelos filtros."));
	#endif
	
	_filterThemesEligible = result;
	return true;
}

/*! \brief Carrega lista remota de PCDs dado um tema.
Retorna true se conseguiu carregar a lista. Caso contrário, MOSTRA mensagem de erro e retorna false.*/
bool RemoteWeatherServerList::getPCDColPtsByTheme(wsProjectionParams proj, wsPCDInfPlaneAttributes pcdInfPlaneAttributes, std::vector<struct wsPCD> &pcdColPts)
{
	#ifdef OFFLINE_TEST
		;
	#else
	if (_service->collection__getPCDColPtsByTheme(proj,
												  pcdInfPlaneAttributes.themeID, 
												  pcdInfPlaneAttributes.attributeName, 
												  pcdInfPlaneAttributes.mask, 
												  pcdColPts) != SOAP_OK) 
			return _manager->showModColetaError(QObject::tr("Não foi possível carregar a lista de PCDs do tema."));
	#endif

	return true;
}

bool RemoteWeatherServerList::loadPCDThemeData()
{
	// Efetua chamada remota
	#ifdef OFFLINE_TEST
		std::vector<wsPCDTheme> result(baseRMT, baseRMT + sizeof(baseRMT)/sizeof(wsPCDTheme));
	#else
		std::vector<struct wsPCDTheme> result;
		if (_service->collection__getPCDThemes(result) != SOAP_OK)
			return _manager->showModColetaError(QObject::tr("Não foi possível carregar a lista de temas de PCDs."));
	#endif
	
	_pcdThemes = result;
	return true;
}

std::vector<struct wsTheme> RemoteWeatherServerList::getFilterThemesEligible()
{
	return _filterThemesEligible;
}

std::vector<struct wsPCDTheme> RemoteWeatherServerList::getPCDThemes()
{
	return _pcdThemes;
}

bool RemoteWeatherServerList::loadIntersectionThemeData()
{
	// Efetua chamada remota
#ifdef OFFLINE_TEST
	std::vector<wsTheme> result(baseRMT, baseRMT + sizeof(baseRMT)/sizeof(wsTheme));
#else
	std::vector<struct wsTheme> result;
	if (_service->collection__getEligibleThemesForIntersection(result) != SOAP_OK)
		return _manager->showModColetaError(QObject::tr("Não foi possível carregar a lista de temas disponíveis \npara serem usados como máscara pelos filtros."));
#endif

	_intersectionThemesEligible = result;
	return true;
}

std::vector<struct wsTheme> RemoteWeatherServerList::getIntersectionThemesEligible()
{
	return _intersectionThemesEligible;
}
