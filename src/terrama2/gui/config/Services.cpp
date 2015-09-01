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

Armazena os endere�os dos servi�os consultados, bem como refer�ncias
para os mesmos.

� respons�vel ainda por armazenar as listas de dados lidas dos servidores
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

Inicializa todos os endere�os com http://localhost:8080
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

/*! \brief Carrega a configura��o dos servidores a partir do arquivo recebido

\param file Path para arquivo contendo a configura��o
\param err  Retorna mensagem de erro caso o arquivo n�o possa ser lido
\return Retorna true se a configura��o foi lida com sucesso, false caso contr�rio
*/
bool Services::loadConfiguration(QString filename, QString& err)
{
  return _cfg.load(filename, err); 
}

/*! \brief Salva a configura��o atual para o arquivo especificado

\param file Arquivo a ser salvo
\param err  Retorna mensagem de erro caso o arquivo n�o possa ser escrito
\return Retorna true se a configura��o foi salva com sucesso, false caso contr�rio
*/
bool Services::saveConfiguration(QString filename, QString& err)
{
  return _cfg.save(filename, err);
}

/*! \brief  Configura endere�os e carrega dados dos servi�os remotos

\param initOnly Se true, indica que as estruturas devem ser inicializadas
                mas nenhuma conex�o deve ser tentada.  Neste caso a fun��o
                ir� retornar false.

Retorna true se todos os dados foram carregados com sucesso, false se 
houve falha em alguma das cargas (Obs: no processo de carga s�o mostradas
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

  // Seta endere�o do servi�o de an�lise
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

  // Le dados.  Se a primeira leitura falhou para um servi�o, n�o � necess�rio ler a segunda...
  // Dados do servi�o de an�lise
  ok  = _analysisList->loadData();
  // Dados do servi�o de notificacao
  ok  = _userAnalysisList->loadData();
  ok  &= _bulletinList->loadData();
  // Dados do servi�o de planos
  ok &= _riskMapList->loadData() && _riskMapList->loadRiskMapThemeData() &&
        _additionalMapList->loadData() && _additionalMapList->loadAdditionalMapThemeData() &&
        _cellularSpaceList->loadData() && _cellularSpaceList->loadCellThemeData() && _viewList->loadData() && _alertIconList->loadData();
  // Dados do servi�o de coleta     
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
           �ltima opera��o remota efetuada com o m�dulo de an�lise.
           
Por conveni�ncia, retorna sempre false
*/
bool Services::showModAnaliseError(QString serviceDescription)
{
	return showError(QObject::tr("M�dulo de An�lise"), serviceDescription, _analysisService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
�ltima opera��o remota efetuada com o m�dulo de notifica��o.

Por conveni�ncia, retorna sempre false
*/
bool Services::showModNotificacaoError(QString serviceDescription)
{
	return showError(QObject::tr("M�dulo de Notifica��o"), serviceDescription, _notificationService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           �ltima opera��o remota efetuada com o m�dulo de coleta.
           
Por conveni�ncia, retorna sempre false
*/
bool Services::showModColetaError(QString serviceDescription)
{
	return showError(QObject::tr("M�dulo de coleta de dados"), serviceDescription, _collectionService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           �ltima opera��o remota efetuada com o m�dulo de coleta.
           
Por conveni�ncia, retorna sempre false
*/
bool Services::showModServPlanosError(QString serviceDescription)
{
	return showError(QObject::tr("M�dulo de servico de planos"), serviceDescription, _planService->soap);
}

/*! \brief Mostra tela contendo mensagem de erro associada com a 
           �ltima opera��o remota efetuada com o m�dulo recebido
           como par�metro.
           
Por conveni�ncia, retorna sempre false
*/
bool Services::showError(QString modname, QString serviceDescription, struct soap* soap)
{
  std::stringstream err;
  soap_stream_fault(soap, err);

  QMessageBox box(QMessageBox::Critical, QObject::tr("Erro..."),
	  QObject::tr("Erro de comunica��o com o %1.\n%2").arg(modname, serviceDescription), 
                  QMessageBox::Ok);
  
  box.setDetailedText(QString::fromStdString(err.str()));
  box.exec();
  return false;
}

bool Services::getArchivingRules(std::vector<struct wsArchivingRule> &archivingRules)
{
	if (_collectionService->collection__getArchivingRules(archivingRules) != SOAP_OK)
		return showModColetaError(QObject::tr("N�o foi poss�vel carregar a lista de regras de arquivamento."));

	return true;
}

bool Services::saveArchivingRules(std::vector<struct wsArchivingRule> &archivingRules)
{
	int dummy = 0;
	if (_collectionService->collection__saveArchivingRules(archivingRules, dummy) != SOAP_OK)
		return showModColetaError(QObject::tr("N�o foi poss�vel salvar a lista de regras de arquivamento."));

	return true;
}

bool Services::getValidStudyInterval(struct wsTimestampInterval& interval)
{
	if(_collectionService->collection__getValidStudyInterval(interval) != SOAP_OK)
		return showModColetaError(QObject::tr("N�o foi poss�vel definir um intervalo de tempo v�lido para o estudo."));

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
		return showModColetaError(QObject::tr("N�o foi poss�vel construir a tabela de eventos deste estudo."));

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
		return showModAnaliseError(QObject::tr("N�o foi poss�vel executar as an�lises."));

	return true;
}

bool Services::runAllBulletin(bool ignoreTimeStamps)
{
	if(_notificationService->notification__runAllBulletin(ignoreTimeStamps) != SOAP_OK)
		return showModNotificacaoError(QObject::tr("N�o foi poss�vel executar os boletins."));

	return true;
}

bool Services::runAllNotifications(bool ignoreTimeStamps)
{
	if(_notificationService->notification__runAllWarningQueries(ignoreTimeStamps) != SOAP_OK)
		return showModNotificacaoError(QObject::tr("N�o foi poss�vel executar os boletins."));

	return true;
}

bool Services::getAnalysisRunning(bool & status)
{
	if(_analysisService->analysis__getAnalysisRunning(status) != SOAP_OK)
		return showModAnaliseError(QObject::tr("N�o foi poss�vel verificar se exite an�lise em execu��o."));

	return true;
}

bool Services::getDBType(bool & isStudyDB)
{
	if(_analysisService->analysis__getDBType(isStudyDB) != SOAP_OK)
		return showModAnaliseError(QObject::tr("N�o foi poss�vel verificar se o banco de dados est� configurado para estudos."));

	return true;
}

bool Services::getLastStudyAnalysesMaxWarning(int & warning)
{
	if(_analysisService->analysis__getLastStudyAnalysesMaxWarning(warning) != SOAP_OK)
		return showModAnaliseError(QObject::tr("N�o foi poss�vel recuperar o maior alerta gerado \npelo �ltimo evento de an�lise do estudo."));

	return true;
}

bool Services::clearStudyDatabase()
{
	int dummy;

	if(_analysisService->analysis__clearStudyDatabase(dummy) != SOAP_OK)
		return showModAnaliseError(QObject::tr("N�o foi poss�vel preparar o banco de dados para um novo estudo."));
	
	return true;
}

bool Services::getImageDirectory(std::string& dir)
{
	if(_analysisService->analysis__getImageDirectory(dir) != SOAP_OK)
		return showModAnaliseError(QObject::tr("N�o foi poss�vel recuperar a localiza��o do diret�rio \nem que est�o as imagens geradas pela an�lise."));

	return true;
}

bool Services::loadAdditionalMapData()
{
	return _additionalMapList->loadData();
}

bool Services::getColumnsCelularSpace(int celularSpaceID,std::vector<std::string>& result)
{
    if(_planService->layer__getColumnsCellularSpace(celularSpaceID,result) != SOAP_OK)
		return showModServPlanosError(QObject::tr("N�o foi poss�vel recuperar as colunas do espa�o celular."));

	return true;
}

bool Services::getValuesByColumnName(int celularSpaceID,  std::string columnName, 
                                         std::vector<std::string>& result)
{
    if(_planService->layer__getValuesByColumnName(celularSpaceID, columnName, result) != SOAP_OK)
		return showModServPlanosError(QObject::tr("N�o foi possivel recuperar os valores da coluna  no espa�o celular."));

	return true;
}

bool Services::getWFSLayerAttributes(std::string url, std::string weatherServerDataMask, std::vector<wsDBColumn>& result)
{
	if(_collectionService->collection__getWFSLayerAttributes(url, weatherServerDataMask, result) != SOAP_OK)
		return showModColetaError(QObject::tr("N�o foi poss�vel carregar lista de atributos do layer WFS."));

	return true;
}

bool Services::getAllTerraMEAnalysisLogs(std::vector<wsAnalysisLog>& terraMEAnalysisLogs)
{
	if(_analysisService->analysis__getAllTerraMEAnalysisLogs(terraMEAnalysisLogs) != SOAP_OK)
		return showModAnaliseError(QObject::tr("N�o foi poss�vel recuperar os resultados das an�lises TerraME."));

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