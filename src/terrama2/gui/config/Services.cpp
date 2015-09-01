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
  \file terrama2/gui/config/Services.cpp

  \brief Definition of methods in class Services.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class Services

Armazena os endereços dos serviços consultados, bem como referências
para os mesmos.

É responsável ainda por armazenar as listas de dados lidas dos servidores
remotos
*/

#include <assert.h>
#include <sstream>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "Services.hpp"
#include "AnalysisList.hpp"
#include "RiskMapList.hpp"
#include "AdditionalMapList.hpp"
#include "CellularSpaceList.hpp"
#include "UserAnalysisList.hpp"
#include "BulletinList.hpp"
#include "ViewList.hpp"
#include "WeatherGridList.hpp"
#include "RemoteWeatherServerList.hpp"
#include "AlertIconList.hpp"

#include "ModAnalise.nsmap" // Apenas um dos nsmap's deve ser incluido...

/*! \brief Construtor 

Inicializa todos os endereços com http://localhost:8080
*/
Services::Services()
{
  _analysisService   = new ModAnalise;
  _collectionService = new ModColeta;
  _notificationService = new ModNotificacao;
  _planService       = new ServPlanos;

  _analysisList    = NULL;
  _riskMapList     = NULL;
  _additionalMapList = NULL;
  _cellularSpaceList = NULL;
  _userAnalysisList    = NULL;
  _userAnalysisList = NULL;
  _bulletinList = NULL;
  _alertIconList = NULL;

  _weatherGridList = NULL;
  _remoteWeatherServerList = NULL;
  _pcdList = NULL;
  _viewList = NULL;
}

//! Destrutor
Services::~Services()
{
  delete _analysisList;
  delete _riskMapList;
  delete _additionalMapList;
  delete _cellularSpaceList;
  delete _userAnalysisList;
  delete _bulletinList;
  delete _alertIconList;

  delete _weatherGridList;
  delete _remoteWeatherServerList;
  delete _pcdList;
  delete _viewList;

  delete _analysisService;
  delete _collectionService;
  delete _notificationService;
  delete _planService;
}

/*! \brief Carrega a configuração dos servidores a partir do arquivo recebido

\param file Path para arquivo contendo a configuração
\param err  Retorna mensagem de erro caso o arquivo não possa ser lido
\return Retorna true se a configuração foi lida com sucesso, false caso contrário
*/
bool Services::loadConfiguration(QString filename, QString& err)
{
  return _cfg.load(filename, err); 
}

/*! \brief Salva a configuração atual para o arquivo especificado

\param file Arquivo a ser salvo
\param err  Retorna mensagem de erro caso o arquivo não possa ser escrito
\return Retorna true se a configuração foi salva com sucesso, false caso contrário
*/
bool Services::saveConfiguration(QString filename, QString& err)
{
  return _cfg.save(filename, err);
}

/*! \brief  Configura endereços e carrega dados dos serviços remotos

\param initOnly Se true, indica que as estruturas devem ser inicializadas
                mas nenhuma conexão deve ser tentada.  Neste caso a função
                irá retornar false.

Retorna true se todos os dados foram carregados com sucesso, false se 
houve falha em alguma das cargas (Obs: no processo de carga são mostradas
mensagens de erro).
*/
bool Services::connect(bool initOnly)
{
  bool ok = true;

  // Apaga dados antigos, se houver
  delete _analysisList;
  delete _riskMapList;
  delete _additionalMapList;
  delete _cellularSpaceList;
  delete _userAnalysisList;
  delete _bulletinList;

  delete _weatherGridList;
  delete _remoteWeatherServerList;
  delete _pcdList;
  delete _viewList;
  delete _alertIconList;

  // Seta endereço do serviço de análise
  QString addr = analysisServiceAddress() + ":" + QString::number(analysisServicePort());
  _analysisService->endpoint = strdup(addr.toAscii());

  addr = weatherDataServiceAddress() + ":" + QString::number(weatherDataServicePort());
  _collectionService->endpoint = strdup(addr.toAscii());

  addr = notificationServiceAddress() + ":" + QString::number(notificationServicePort());
  _notificationService->endpoint = strdup(addr.toAscii());

  addr = planServiceAddress() + ":" + QString::number(planServicePort());
  _planService->endpoint = strdup(addr.toAscii());
  
  // Cria listas de dados
  _analysisList     = new AnalysisList     (this, _analysisService);
  _userAnalysisList = new UserAnalysisList (this, _notificationService);
  _bulletinList		= new BulletinList (this, _notificationService);
  _riskMapList      = new RiskMapList      (this, _planService);
  _additionalMapList= new AdditionalMapList(this, _planService);
  _cellularSpaceList= new CellularSpaceList(this, _planService);

  _weatherGridList = new WeatherGridList(this, _collectionService);
  _remoteWeatherServerList = new RemoteWeatherServerList(this, _collectionService);

  _pcdList = new WeatherGridList(this, _collectionService, WS_WDSGEOM_POINTS);

  _viewList = new ViewList(this, _planService);

  _alertIconList = new AlertIconList(this, _planService);

  if(initOnly)
    return false;

  // Le dados.  Se a primeira leitura falhou para um serviço, não é necessário ler a segunda...
  // Dados do serviço de análise
  ok  = _analysisList->loadData();
  // Dados do serviço de notificacao
  ok  = _userAnalysisList->loadData();
  ok  &= _bulletinList->loadData();
  // Dados do serviço de planos
  ok &= _riskMapList->loadData() && _riskMapList->loadRiskMapThemeData() &&
        _additionalMapList->loadData() && _additionalMapList->loadAdditionalMapThemeData() &&
        _cellularSpaceList->loadData() && _cellularSpaceList->loadCellThemeData() && _viewList->loadData() && _alertIconList->loadData();
  // Dados do serviço de coleta     
  ok &= _weatherGridList->loadData() && _remoteWeatherServerList->loadData() && _remoteWeatherServerList->loadFilterThemeData() &&
			  _remoteWeatherServerList->loadPCDThemeData() && _remoteWeatherServerList->loadIntersectionThemeData() && _pcdList->loadData();

  return ok;
}

bool Services::loadGridList()
{
  return _weatherGridList->loadData();
}

bool Services::loadPCDList()
{
  return _pcdList->loadData();
}

bool Services::loadViewList()
{
  return _viewList->loadData();
}

bool Services::loadAnalysisList()
{
	return _analysisList->loadData();
}

bool Services::loadAdditionalMapThemeData()
{
  return _additionalMapList->loadAdditionalMapThemeData();
}

bool Services::loadRiskMapThemeData()
{
  return _riskMapList->loadRiskMapThemeData();
}

bool Services::loadCellThemeData()
{
	return _cellularSpaceList->loadCellThemeData();
}

bool Services::loadIconList()
{
  return _alertIconList->loadData();
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           última operação remota efetuada com o módulo de análise.
           
Por conveniência, retorna sempre false
*/
bool Services::showModAnaliseError(QString serviceDescription)
{
	return showError(QObject::tr("Módulo de Análise"), serviceDescription, _analysisService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
última operação remota efetuada com o módulo de notificação.

Por conveniência, retorna sempre false
*/
bool Services::showModNotificacaoError(QString serviceDescription)
{
	return showError(QObject::tr("Módulo de Notificação"), serviceDescription, _notificationService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           última operação remota efetuada com o módulo de coleta.
           
Por conveniência, retorna sempre false
*/
bool Services::showModColetaError(QString serviceDescription)
{
	return showError(QObject::tr("Módulo de coleta de dados"), serviceDescription, _collectionService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           última operação remota efetuada com o módulo de coleta.
           
Por conveniência, retorna sempre false
*/
bool Services::showModServPlanosError(QString serviceDescription)
{
	return showError(QObject::tr("Módulo de servico de planos"), serviceDescription, _planService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           última operação remota efetuada com o módulo recebido
           como parâmetro.
           
Por conveniência, retorna sempre false
*/
bool Services::showError(QString modname, QString serviceDescription, struct soap* soap)
{
  std::stringstream err;
  soap_stream_fault(soap, err);

  QMessageBox box(QMessageBox::Critical, QObject::tr("Erro..."),
	  QObject::tr("Erro de comunicação com o %1.\n%2").arg(modname, serviceDescription), 
                  QMessageBox::Ok);
  
  box.setDetailedText(QString::fromStdString(err.str()));
  box.exec();
  return false;
}

bool Services::getArchivingRules(std::vector<struct wsArchivingRule> &archivingRules)
{
	if (_collectionService->collection__getArchivingRules(archivingRules) != SOAP_OK)
		return showModColetaError(QObject::tr("Não foi possível carregar a lista de regras de arquivamento."));

	return true;
}

bool Services::saveArchivingRules(std::vector<struct wsArchivingRule> &archivingRules)
{
	int dummy = 0;
	if (_collectionService->collection__saveArchivingRules(archivingRules, dummy) != SOAP_OK)
		return showModColetaError(QObject::tr("Não foi possível salvar a lista de regras de arquivamento."));

	return true;
}

bool Services::getValidStudyInterval(struct wsTimestampInterval& interval)
{
	if(_collectionService->collection__getValidStudyInterval(interval) != SOAP_OK)
		return showModColetaError(QObject::tr("Não foi possível definir um intervalo de tempo válido para o estudo."));

	return true;
}

bool Services::getStudyEvents(int initialYear,
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
							  std::vector<struct wsStudyEvent>& result)
{
	if(_collectionService->collection__getStudyEvents(initialYear,
														initialMonth,
														initialDay,
														initialHour,
														initialMinutes,
														initialSeconds,
														finalYear,
														finalMonth,
														finalDay,
														finalHour,
														finalMinutes,
														finalSeconds,
														tolerance,
														result) != SOAP_OK)
		return showModColetaError(QObject::tr("Não foi possível construir a tabela de eventos deste estudo."));

	return true;
}

bool Services::runAllAnalyses(bool ignoreTimeStamps,
							  int year,
							  int month,
							  int day,
							  int hour,
							  int minutes,
							  int seconds)
{
	if(_analysisService->analysis__runAllAnalyses(ignoreTimeStamps,
												  year,
												  month,
												  day,
												  hour,
												  minutes,
												  seconds) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível executar as análises."));

	return true;
}

bool Services::runAllBulletin(bool ignoreTimeStamps)
{
	if(_notificationService->notification__runAllBulletin(ignoreTimeStamps) != SOAP_OK)
		return showModNotificacaoError(QObject::tr("Não foi possível executar os boletins."));

	return true;
}

bool Services::runAllNotifications(bool ignoreTimeStamps)
{
	if(_notificationService->notification__runAllWarningQueries(ignoreTimeStamps) != SOAP_OK)
		return showModNotificacaoError(QObject::tr("Não foi possível executar os boletins."));

	return true;
}

bool Services::getAnalysisRunning(bool & status)
{
	if(_analysisService->analysis__getAnalysisRunning(status) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível verificar se exite análise em execução."));

	return true;
}

bool Services::getDBType(bool & isStudyDB)
{
	if(_analysisService->analysis__getDBType(isStudyDB) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível verificar se o banco de dados está configurado para estudos."));

	return true;
}

bool Services::getLastStudyAnalysesMaxWarning(int & warning)
{
	if(_analysisService->analysis__getLastStudyAnalysesMaxWarning(warning) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível recuperar o maior alerta gerado \npelo último evento de análise do estudo."));

	return true;
}

bool Services::clearStudyDatabase()
{
	int dummy;

	if(_analysisService->analysis__clearStudyDatabase(dummy) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível preparar o banco de dados para um novo estudo."));
	
	return true;
}

bool Services::getImageDirectory(std::string& dir)
{
	if(_analysisService->analysis__getImageDirectory(dir) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível recuperar a localização do diretório \nem que estão as imagens geradas pela análise."));

	return true;
}

bool Services::loadAdditionalMapData()
{
	return _additionalMapList->loadData();
}

bool Services::getColumnsCelularSpace(int celularSpaceID,std::vector<std::string>& result)
{
    if(_planService->layer__getColumnsCellularSpace(celularSpaceID,result) != SOAP_OK)
		return showModServPlanosError(QObject::tr("Não foi possível recuperar as colunas do espaço celular."));

	return true;
}

bool Services::getValuesByColumnName(int celularSpaceID,  std::string columnName, 
                                         std::vector<std::string>& result)
{
    if(_planService->layer__getValuesByColumnName(celularSpaceID, columnName, result) != SOAP_OK)
		return showModServPlanosError(QObject::tr("Não foi possivel recuperar os valores da coluna  no espaço celular."));

	return true;
}

bool Services::getWFSLayerAttributes(std::string url, std::string weatherServerDataMask, std::vector<wsDBColumn>& result)
{
	if(_collectionService->collection__getWFSLayerAttributes(url, weatherServerDataMask, result) != SOAP_OK)
		return showModColetaError(QObject::tr("Não foi possível carregar lista de atributos do layer WFS."));

	return true;
}

bool Services::getAllTerraMEAnalysisLogs(std::vector<wsAnalysisLog>& terraMEAnalysisLogs)
{
	if(_analysisService->analysis__getAllTerraMEAnalysisLogs(terraMEAnalysisLogs) != SOAP_OK)
		return showModAnaliseError(QObject::tr("Não foi possível recuperar os resultados das análises TerraME."));

	return true;
}

bool Services::shutdown(int type)
{
	if(type == 0) // Coleta
	{
		if(_collectionService->collection__shutdownCollectionModule() != SOAP_OK)
			return false;
	}
	else if(type == 1) // Planos
	{
		if(_planService->layer__shutdownLayerService() != SOAP_OK)
			return false;
	}
	else if(type == 2) // Notificacao
	{
		if(_notificationService->notification__shutdownNotificationModule() != SOAP_OK)
			return false;
	}
	else if(type == 3) // Animcacao
	{
		/*
		if(mod->animacao__shutdownAnimacaoService() != SOAP_OK)
			return false;
		*/
	}
	else if(type == 4) //Analise (principal)
	{
		if(_analysisService->analysis__shutdownAnalysisModule() != SOAP_OK)
			return false;
	}
	
	return true;
}

bool Services::ping(int type)
{
	std::string answer;
	if(type == 0) // Coleta
	{
		if(_collectionService->collection__ping(answer) != SOAP_OK)
			return false;
	}
	else if(type == 1) // Planos
	{
		if(_planService->layer__ping(answer) != SOAP_OK)
			return false;
	}
	else if(type == 2) // Notificacao
	{
		if(_notificationService->notification__ping(answer) != SOAP_OK)
			return false;
	}
	else if(type == 3) // Animcacao
	{
		/*
		if(mod->animacao__ping(answer) != SOAP_OK)
		return false;
		*/
	}
	else if(type == 4) //Analise (principal)
	{
		if(_analysisService->analysis__ping(answer) != SOAP_OK)
			return false;
	}

	if(answer != "Sismaden pong")
		return false;  
	return true;
}