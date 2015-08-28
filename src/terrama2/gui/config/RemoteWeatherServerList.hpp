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
  \file terrama2/gui/config/RemoteWeatherServerList.hpp

  \brief Definition of methods in class RemoteWeatherServerList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


#ifndef _REMOTESERVER_LIST_H_
#define _REMOTESERVER_LIST_H_

// QT
#include <QList>

// TerraMA2  
#include "RemoteWeatherServer.hpp"

class RemoteWeatherServerData;

class Services;
class ModColeta;

/*! \brief Classe responsável por manter e gerenciar a lista de servidores 
           de dados disponíveis
*/           
class RemoteWeatherServerList : public QList<RemoteWeatherServer*>
{
public:
  RemoteWeatherServerList(Services* manager, ModColeta* service);
  ~RemoteWeatherServerList();
  
  bool loadData();
  bool addNewRemoteWeatherServer(const RemoteWeatherServer* rws, int &newId);
  bool updateRemoteWeatherServer(int id, const RemoteWeatherServer* rws); 
  bool deleteRemoteWeatherServer(int id, bool deleteLayers);

  int findIndexServerById(int id );
  RemoteWeatherServer* findServerById(int id );
  RemoteWeatherServerData* findServerDataById(int srvId, int id );
  bool loadFilterThemeData();
  bool loadIntersectionThemeData();
  std::vector<struct wsTheme> getFilterThemesEligible();
  std::vector<struct wsTheme> getIntersectionThemesEligible();
  bool loadPCDThemeData();
  std::vector<struct wsPCDTheme> getPCDThemes();
  bool getPCDColPtsByTheme(wsProjectionParams proj, wsPCDInfPlaneAttributes pcdInfPlaneAttributes, std::vector<struct wsPCD> &pcdColPts);

private:
  RemoteWeatherServerList(const RemoteWeatherServerList&);             //!< Remove construtor de cópia default
  RemoteWeatherServerList& operator=(const RemoteWeatherServerList&);  //!< Remove operador de atrib. default
 
  Services*   _manager; //! Gerenciador de serviços
  ModColeta* _service; //! Referência para acesso ao serviço remoto de coleta

  std::vector<struct wsTheme> _filterThemesEligible;
  std::vector<struct wsTheme> _intersectionThemesEligible;
  std::vector<struct wsPCDTheme> _pcdThemes; //Temas para informar a localizacao das PCDs 
};


#endif

