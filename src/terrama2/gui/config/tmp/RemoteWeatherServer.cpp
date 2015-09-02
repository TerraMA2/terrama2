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
  \file terrama2/gui/config/RemoteWeatherServer.cpp

  \brief Definition of methods in class RemoteWeatherServer.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/

/*!
\class RemoteWeatherServer

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados 
retornada pelo modulo de coleta com os dados dos servidores

*/

// STL
#include <assert.h>

// TerraMA2
#include "RemoteWeatherServer.hpp"
#include "RemoteWeatherServerDataList.hpp"

//! Construtor para novos servidores, ainda não salvas na base de dados
RemoteWeatherServer::RemoteWeatherServer()
{
  _data.id = -1;  
  _dataServerDataList = NULL;
}

//! Constroi um novo servidor inicializado com dados recebidos do módulo de coleta
RemoteWeatherServer::RemoteWeatherServer(const wsRemoteWeatherServer& data)
{
  assert(data.id >= 0);
  _data = data;
  _dataServerDataList = NULL;
}

//! Destrutor
RemoteWeatherServer::~RemoteWeatherServer()
{
	if (_dataServerDataList !=NULL)
	{
		delete _dataServerDataList;
		_dataServerDataList = NULL;
	}
}

/*! \brief Altera o identificador de um servidor.

Deve ser utilizado apenas em servidores novos que ainda não tenham
sido salvos, para setar seu novo Id.
*/
void RemoteWeatherServer::setId(int id)  
{
  assert(id >= 0);
  assert(isNew());
  _data.id = id;
}

bool RemoteWeatherServer::loadServerDataList(Services* manager, ModColeta* service)
{
	if (_dataServerDataList == NULL)
		_dataServerDataList = new RemoteWeatherServerDataList(manager, service, id());

	return _dataServerDataList->loadData(id());
}

void RemoteWeatherServer::initDatalist(Services* manager, ModColeta* service)
{
	if (_dataServerDataList == NULL)
		delete _dataServerDataList;

	_dataServerDataList = new RemoteWeatherServerDataList(manager, service, id());
}

RemoteWeatherServerData* RemoteWeatherServer::findServerDataById(int id )
{
	return getRemoteWeatherServerDataList()->findServerDataById( id );
}