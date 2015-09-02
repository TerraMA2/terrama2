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
  \file terrama2/gui/config/Services.hpp

  \brief Definition of methods in class Services.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _SERVICES_H_
#define _SERVICES_H_

// QT
#include <QString>

#include "soapModAnaliseProxy.h"
#include "soapModColetaProxy.h"
#include "soapModNotificacaoProxy.h"

#include "configData.h"

//#define OFFLINE_TEST

class AnalysisList;
class UserAnalysisList;
class BulletinList;
class RiskMapList;
class AdditionalMapList;
class CellularSpaceList;
class ViewList;
class AlertIconList;

class WeatherGridList;
class RemoteWeatherServerList;

class QTextStream;

class ModAnalise;
class ServPlanos;

//! Classe respons�vel por gerenciar as conex�es com os servi�os remotos
class Services
{
public:
  Services();
  ~Services();

  bool loadConfiguration(QString filename, QString& err);
  bool saveConfiguration(QString filename, QString& err);

  bool connect(bool initOnly);

  bool showModAnaliseError(QString serviceDescription);
  bool showModColetaError(QString serviceDescription);
  bool showModServPlanosError(QString serviceDescription);
  bool showModNotificacaoError(QString serviceDescription);

  //! Retorna a lista de dados de regras de análise
  AnalysisList* analysisList() const { return _analysisList; }

  //! Retorna a lista de usuarios 
  UserAnalysisList* userAnalysisList() const { return _userAnalysisList; }

  //! Retorna a lista de boletins
  BulletinList* bulletinList() const { return _bulletinList; }

  //! Retorna a lista de objetos monitorados
  RiskMapList* riskMapList() const { return _riskMapList; }

  //! Retorna a lista de mapas adicionais
  AdditionalMapList* additionalMapList() const { return _additionalMapList; }

  //! Retorna a lista de espaços celulares
  CellularSpaceList* cellularSpaceList() const { return _cellularSpaceList; }

  //! Retorna a lista de grids com informações climáticas
  WeatherGridList* weatherGridList() const { return _weatherGridList; }

  //! Retorna a lista de grids do tipo PCD
  WeatherGridList* pcdList() const { return _pcdList; }

	//! Retorna a lista de vistas
  ViewList* viewList() const { return _viewList; }

  //! Retorna a lista de servidores com informações climáticas
  RemoteWeatherServerList* remoteWeatherServerList() const { return _remoteWeatherServerList; }

  //! Retorna o conjunto de icones disponiveis para mostrar alertas
  AlertIconList* alertIconList() const { return _alertIconList; }

  //! Retorna o endereço do serviço de dados meteorologicos
  QString weatherDataServiceAddress() const { return _cfg.collection()._address; }

  //! Retorna a porta do serviço de dados meteorologicos
  int weatherDataServicePort() const { return _cfg.collection()._servicePort; }

  //! Altera o endereço completo do serviço de dados meteorologicos
  void setWeatherDataServiceAddress(QString addr, int port) { _cfg.collection()._address = addr; _cfg.collection()._servicePort = port; }

  //! Retorna o endereço do serviço de análise
  QString analysisServiceAddress() const { return _cfg.analysis()._address; }

  //! Retorna a porta do serviço de análise
  int analysisServicePort() const { return _cfg.analysis()._servicePort; }

  //! Altera o endereço completo do serviço de análise
  void setAnalysisServiceAddress(QString addr, int port) { _cfg.analysis()._address = addr; _cfg.analysis()._servicePort = port; }

  //! Retorna o endereço do serviço de notificação
  QString notificationServiceAddress() const { return _cfg.notification()._address; }

  //! Retorna a porta do serviço de notificação
  int notificationServicePort() const { return _cfg.notification()._servicePort; }

  //! Altera o endereço completo do serviço de notificação
  void setNotificationServiceAddress(QString addr, int port) { _cfg.notification()._address = addr; _cfg.notification()._servicePort = port; }

  //! Retorna o endereço do serviço de planos
  QString planServiceAddress() const { return _cfg.plans()._address; }

  //! Retorna a porta do serviço de planos
  int planServicePort() const { return _cfg.plans()._servicePort; }

  //! Altera o endereço completo do serviço de planos
  void setPlanServiceAddress(QString addr, int port) { _cfg.plans()._address = addr; _cfg.plans()._servicePort = port; }

  //! Carrega a lista de grids
  bool loadGridList();

  //! Carrega a lista de grids PCD
  bool loadPCDList();

	//! Carrega a lista de vistas
  bool loadViewList();

	//! Carrega a lista de análises
  bool loadAnalysisList();

  //! Carrega a lista de temas para os mapas adicionais
  bool loadAdditionalMapThemeData();

  //! Carrega a lista de temas para os objetos monitorados
  bool loadRiskMapThemeData();

  //! Carrega a lista de mapas adicionais
  bool loadAdditionalMapData();

  //! Carrega a lista de temas para os espaços celulares
  bool loadCellThemeData();

  //! Carrega o conjunto de imagens disponiveis para servir como icones de alerta na analises
  bool loadIconList();

  //! Carrega lista remota de regras de arquivamento
  bool getArchivingRules(std::vector<struct wsArchivingRule> &archivingRules);
  
  //! Grava no banco de dados as regras de arquivamento informadas pelo usuario
  bool saveArchivingRules(std::vector<struct wsArchivingRule> &archivingRules);

  //! Obtem o intervalo de tempo valido para um estudo (valor inicial: menor data entre as coletas realizadas, valor final: maior data entre as coletas realizadas)
  bool getValidStudyInterval(struct wsTimestampInterval& interval);

  //! Carrega os eventos de um estudo
  bool getStudyEvents(int initialYear,
					int initialMonth,
					int initialDay,
					int initialHour,
					int initialMinutes,
					int initialSeconds,
					int finalYear,
					int finalMonth,
					int finalDay,
					int finalHour,
					int finalMinutes,
					int finalSeconds,
					int tolerance,
					std::vector<struct wsStudyEvent>& result);

  //! Executa todas as analises ativas em um determinado timestamp que é passado como parametro. Só sao consideradas as coletas realizadas ate este timestamp
  bool runAllAnalyses(bool ignoreTimeStamps,
					  int year,
					  int month,
					  int day,
					  int hour,
					  int minutes,
					  int seconds);

  bool runAllNotifications(bool ignoreTimeStamps);

  //! Executa todas as analises ativas em um determinado timestamp que é passado como parametro. Só sao consideradas as coletas realizadas ate este timestamp
  bool runAllBulletin(bool ignoreTimeStamps);

  //! Atribui 'true' �  variavel 'status' se existir alguma sequencia de análises em execucao
  bool getAnalysisRunning(bool & status);

  //! Atribui 'true' �  variavel 'isStudyDB' se o banco de dados estiver configurado para estudos.
  bool getDBType(bool & isStudyDB);

  //! Recupera o maior alerta retornado pelo ultimo evento de analise (ou seja, pela ultima sequencia de analises) de um estudo.
  bool getLastStudyAnalysesMaxWarning(int & warning);

  //! Prepara o banco de dados para iniciar um novo estudo, removendo logs de analise e layers gerados por analises baseadas em modelo
  bool clearStudyDatabase();

  //! Recupera o caminho do diretorio em que estao as imagens geradas pela analise
  bool getImageDirectory(std::string& dir);

  //! Pega a atual configuração do sismaden
  ConfigData* const getConfigData() { return &_cfg; }

  //! Carrega as colunas do espaço celular
  bool getColumnsCelularSpace(int celularSpaceID,
                              std::vector<std::string>& result);

  //! Carrega os valores de todas as linhas a partir de uma coluna.
  bool getValuesByColumnName(int celularSpaceID,
                              std::string columnName, 
                              std::vector<std::string>& result);

  //! Carrega os nomes dos atributos de um layer WFS
  bool getWFSLayerAttributes(std::string url, 
							 std::string weatherServerDataMask, 
							 std::vector<wsDBColumn>& result);

  //! Carrega todos os logs gerados pela execucao de todas analises TerraME
  bool getAllTerraMEAnalysisLogs(std::vector<wsAnalysisLog>& terraMEAnalysisLogs);

  //! Shutdown service type defines the service 0: Collection, 1: Plans, 2: Notification, 3: Animation, 4: Analysis
  bool shutdown(int type);

  //! Ping service type defines the service 0: Collection, 1: Plans, 2: Notification, 3: Animation, 4: Analysis
  bool ping(int type);

private:
  bool showError(QString modname, QString serviceDescription, struct soap* soap);

  ConfigData _cfg; //!< Configuração atual para localização dos serviços

  ModAnalise* _analysisService;    //!< Referência para o serviço de analise
  ModColeta*  _collectionService;  //!< Referência para o serviço de coleta de dados
  ModNotificacao* _notificationService; //!< Referência para o serviço de notificação
  ServPlanos* _planService;  //!< Referência para o serviço de coleta de dados


  AnalysisList*      _analysisList;    //!< Lista de regras de análises
  UserAnalysisList*  _userAnalysisList;//!< Lista de usuários
  BulletinList*		 _bulletinList;	   //!< Lista de boletins
  RiskMapList*       _riskMapList;     //!< Lista de objetos monitorados
  AdditionalMapList* _additionalMapList; //!< Lista de mapas adicionais
  CellularSpaceList* _cellularSpaceList; //!< Lista de espaços celulares
  WeatherGridList*   _weatherGridList; //!< Lista de grids com informações climáticas
  WeatherGridList*   _pcdList; //!< Lista de grids do tipo PCD
  ViewList*          _viewList; //!< Lista de vistas
  RemoteWeatherServerList* _remoteWeatherServerList; //!< Lista de servidores com informações climáticas
  AlertIconList*           _alertIconList;
};


#endif

