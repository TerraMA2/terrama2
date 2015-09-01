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
  \file terrama2/gui/config/RemoteWeatherServerDataList.hpp

  \brief Definition of methods in class RemoteWeatherServerDataList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/

#ifndef _REMOTESERVERDATA_LIST_H_
#define _REMOTESERVERDATA_LIST_H_

// QT
#include <QList>
#include <QObject>

// TerraMA2
#include "RemoteWeatherServerData.hpp"

class Services;
class ModColeta;

/*! \brief Classe responsável por manter e gerenciar a lista de formatos
           de dados disponíveis
*/           
class RemoteWeatherServerDataList : public QObject, public QList<RemoteWeatherServerData*>
{
	Q_OBJECT

public:
  RemoteWeatherServerDataList(Services* manager, ModColeta* service, int idServer);
  ~RemoteWeatherServerDataList();
  
  bool loadData(int idServ);
  bool addNewRemoteWeatherServerData(int idServ, const RemoteWeatherServerData* rwsd, int &newId);
  bool updateRemoteWeatherServerData(int idServ, int id, RemoteWeatherServerData* rwsd); 
  bool deleteRemoteWeatherServerData(int id, bool deleteLayers);
  
  int findIndexServerDataById(int id );
  RemoteWeatherServerData* findServerDataById( int id );

signals:
  void afterUpdateRWSData(int id, QString newName);

private:
  RemoteWeatherServerDataList(const RemoteWeatherServerDataList&);             //!< Remove construtor de cópia default
  RemoteWeatherServerDataList& operator=(const RemoteWeatherServerDataList&);  //!< Remove operador de atrib. default

  Services*   _manager; //! Gerenciador de serviços
  ModColeta* _service; //! Referência para acesso ao serviço remoto de coleta
  int		_idServer;	// Identificador do servidor associado aos dados

};


#endif

