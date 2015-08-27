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
  \file terrama2/gui/config/RemoteWeatherServer.hpp

  \brief Definition of methods in class RemoteWeatherServer.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/

#ifndef _REMOTESERVERS_H_
#define _REMOTESERVERS_H_

// QT
#include <QString>
#include <QList>

#include "soapModAnaliseProxy.h"

class RemoteWeatherServerDataList;
class RemoteWeatherServerData;

class Services;
class ModColeta;

//! Classe responsável por representar um servidor
class RemoteWeatherServer
{
public:
  RemoteWeatherServer();
  RemoteWeatherServer(const wsRemoteWeatherServer& data);
  ~RemoteWeatherServer();

  //! Retorna o identificador do servidor
  int id() const { return _data.id; }

  void setId(int id);  

  //! Retorna true se for um servidor novo ainda não salvo na base de dados
  bool isNew() const { return _data.id == -1; }
  
  //! Retorna o nome do servidor
  QString serverName() const { return QString::fromStdString(_data.name); }

  //! Altera o nome do servidor
  void setServerName(QString name) { _data.name = name.toStdString(); }

  //! Retorna uma descricao do servidor
  QString details() const { return QString::fromStdString(_data.details); }

  //! Altera a descrição do servidor
  void setDescription(QString details) { _data.details = details.toStdString(); }

  //! Retorna o intervalo de busca das informações
  int intervalMinutes() const { return _data.nIntervalMinutes; }

  //! Altera o intervalo de busca das informações
  void setIntervalMinutes(int intervalMinutes) { _data.nIntervalMinutes = intervalMinutes; }

  //! Retorna o caminho base para a busca das informações
  QString basePath() const { return QString::fromStdString(_data.basePath); }

  //! Altera o caminho base para a busca das informações
  void setBasePath(QString basePath) { _data.basePath = basePath.toStdString(); }

  //! Retorna se o servidor está ativo
  bool active() const { return _data.lActive; }

  //! Altera se o servidor está ativo
  void setActive(bool active) { _data.lActive = active; }

  //! Retorna o endereco de acesso ao servidor
  QString address() const { return QString::fromStdString(_data.address); }

  //! Altera o endereco de acesso ao servidor
  void setAddress(QString address) { _data.address = address.toStdString(); }

  //! Retorna a porta de acesso ao servidor
  int port() const { return _data.nPort; }

  //! Altera a porta de acesso ao servidor
  void setPort(int nPort) { _data.nPort = nPort; }

  //! Retorna o protocolo de comunicacao com o servidor
  enum wsProtocol protocol() const { return _data.protocol; }

  //! Altera o protocolo de comunicacao com o servidor
  void setProtocol(enum wsProtocol protocol) { _data.protocol = protocol; }

  //! Retorna o nome do usuario para acesso ao servidor
  QString userName() const { return QString::fromStdString(_data.userName); }

  //! Altera o nome do usuario para acesso ao servidor
  void setUserName(QString userName) { _data.userName = userName.toStdString(); }

  //! Retorna a senha do usuario para acesso ao servidor
  QString password() const { return QString::fromStdString(_data.password); }

  //! Altera a senha do usuario para acesso ao servidor
  void setPassword(QString password) { _data.password = password.toStdString(); }

  //! Carrega lista formato de dados disponiveis no servidor
  bool loadServerDataList(Services* manager, ModColeta* service);

  //! Carrega inicialisa DataList (lista do formato de dados)
  void initDatalist(Services* manager, ModColeta* service);

  //! Carrega lista de dados disponiveis no servidor
  RemoteWeatherServerDataList* getRemoteWeatherServerDataList(void) { return _dataServerDataList; };

  //! Retorna a estrutura que empacota os dados de análise
  const wsRemoteWeatherServer& data() const { return _data; }

  RemoteWeatherServerData* findServerDataById(int id );

private:
  wsRemoteWeatherServer _data;  //! Dados do servidor empacotados na estrutura lida
  RemoteWeatherServerDataList* _dataServerDataList;
};


#endif

