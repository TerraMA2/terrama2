// /************************************************************************************
// TerraMA² is a development platform of Monitoring, Analysis and Alert for
// geoenvironmental extremes. Applied to natural disasters, quality of water / air / atmosphere,
// tailings basins, epidemics and others.Copyright © 2012 INPE.

// This code is part of the TerraMA² framework.
// This framework is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// You should have received a copy of the GNU General Public
// License along with this library.

// The authors reassure the license terms regarding the warranties.
// They specifically disclaim any warranties, including, but not limited to,
// the implied warranties of merchantability and fitness for a particular purpose.
// The library provided hereunder is on an "as is" basis, and the authors have no
// obligation to provide maintenance, support, updates, enhancements,
// or modifications.

// In no event shall INPE or K2 SISTEMAS be held liable to any party for direct, indirect,
// special, incidental, or consequential damages arising out of the use of this
// library and its documentation.
// *************************************************************************************/

// /*!
//  * \file configData.cpp
//  *
//  * \brief Definicao dos metodos declarados na classe ConfigData
//  *
//  * \author Carlos Augusto Teixeira Mendes
//  * \date march, 2008 
//  * \version $Id
//  *
//  */

// /*!
// \class ConfigData

// Clase responsável por armazenar todos os parâmetros de configuração do
// sistema necessários para a execução de cada um dos módulos.
// */

// #include <assert.h>
// #include <QFile>
// #include <QtXml>

// #include "ConfigData.hpp"

// #if defined WIN32
// static const char* ConfigDataAnalysisCmd     = "analise.exe";
// static const char* ConfigDataAnalysisPar     = "%c %i";
// static const char* ConfigDataCollectionCmd   = "coleta.exe";
// static const char* ConfigDataCollectionPar   = "%c";
// static const char* ConfigDataPlansCmd        = "planos.exe";
// static const char* ConfigDataPlansPar        = "%c";
// static const char* ConfigDataAnimationCmd        = "animacao.exe";
// static const char* ConfigDataAnimationPar        = "%c";
// static const char* ConfigDataNotificationCmd = "notificacao.exe";
// static const char* ConfigDataNotificationPar = "%c";
// #else
// static const char* ConfigDataAnalysisCmd     = "/bin/bash";
// static const char* ConfigDataAnalysisPar     = "start.sh %p %m %c %i";
// static const char* ConfigDataCollectionCmd   = "/bin/bash";
// static const char* ConfigDataCollectionPar   = "start.sh %p %m %c";
// static const char* ConfigDataPlansCmd        = "/bin/bash";
// static const char* ConfigDataPlansPar        = "start.sh %p %m %c";
// static const char* ConfigDataAnimationCmd        = "/bin/bash";
// static const char* ConfigDataAnimationPar        = "start.sh %p %m %c";
// static const char* ConfigDataNotificationCmd = "/bin/bash";
// static const char* ConfigDataNotificationPar = "start.sh %p %m %c";
// #endif


// //! Construtor.  
// ConfigData::ConfigData()
// {
// }

// //! Destrutor
// ConfigData::~ConfigData()
// {
// }

// //! Retorna os comandos e parâmetros default para serviço de análise
// void ConfigData::getDefaultAnalysisCommands(QString& cmd, QString& param)
// {
//   cmd   = ConfigDataAnalysisCmd;
//   param = ConfigDataAnalysisPar;
// }

// //! Preenche estruturas com valores default
// void ConfigData::loadDefaults(int ninstances)
// {
//   _name = "";
  
//   _dbData._hostName = "localhost";
//   _dbData._port     = 0;
//   _dbData._user     = "";
//   _dbData._password = "";
//   _dbData._dbName   = "";
//   _dbData._driver   = DRIVER_PostGIS;
//   _dbData._study    = false;
  
//   _analysisData._logFile      = "log_analise.txt";
//   _analysisData._servicePort  = 8080;
//   _analysisData._address      = "localhost";
//   _analysisData._cmd          = ConfigDataAnalysisCmd;
//   _analysisData._params       = ConfigDataAnalysisPar;
//   _analysisData._interval     = 5*60;
//   _analysisData._numInstances = 1;
//   _analysisData._instanceId   = 1;
//   _analysisData._imageFilePath = "";
  
//   if(ninstances > 1)
//   {
//     int ninst = ninstances-1;
//     _analysisData._instanceData.resize(ninst);
//     for(int i=0; i<ninst; i++)
//     {
//       _analysisData._instanceData[i]._address     = "localhost";
//       _analysisData._instanceData[i]._cmd         = ConfigDataAnalysisCmd;
//       _analysisData._instanceData[i]._params      = ConfigDataAnalysisPar;
//       _analysisData._instanceData[i]._servicePort = 8080;
//       _analysisData._instanceData[i]._logFile     = QString("log_analise_%1.txt").arg(i+2); // Primeira inst. adicional é a de índice 2
//     }  
//   }

//   _collectionData._logFile      = "log_coleta.txt";
//   _collectionData._servicePort  = 8080;
//   _collectionData._address      = "localhost";
//   _collectionData._cmd          = ConfigDataCollectionCmd;
//   _collectionData._params       = ConfigDataCollectionPar;
//   _collectionData._timeout      = 180;
//   _collectionData._dirPath		= "";
  
//   _plansData._logFile      = "log_planos.txt";
//   _plansData._servicePort  = 8080;
//   _plansData._address      = "localhost";
//   _plansData._cmd          = ConfigDataPlansCmd;
//   _plansData._params       = ConfigDataPlansPar;

//   _animationData._logFile = "log_animacao.txt";
//   _animationData._servicePort = 8080;
//   _animationData._address = "localhost";
//   _animationData._cmd = ConfigDataAnimationCmd;
//   _animationData._params = ConfigDataAnimationPar;
//   _animationData._imageDirectory = "";

//   _notificationData._logFile     = "log_notificacao.txt";
//   _notificationData._servicePort = 8080;
//   _notificationData._address     = "localhost";
//   _notificationData._cmd         = ConfigDataNotificationCmd;
//   _notificationData._params      = ConfigDataNotificationPar;
//   _notificationData._interval    = 5*60;
//   _notificationData._smtpServer  = "smtp.k2sistemas.com.br";
//   _notificationData._smtpPort    = 587;
//   _notificationData._smtpMail    = "ysperanza@k2sistemas.com.br";
//   _notificationData._smtpPsswrd  = "Sp3r4nz4";
//   _notificationData._terraPHPURL = "www.terraweb.com.br";
//   _notificationData._proxyUser   = "";
//   _notificationData._proxyPsswrd = "";
//   _notificationData._smtpTimeout = 30000;
//   _notificationData._smtpAlias   = "";
//   _notificationData._smtpPriority= "normal";
//   _notificationData._smtpSSL	 = false;
//   _notificationData._sendUpEmail = false;
//   _notificationData._intervalBulletin = 5*60;
//   _notificationData._removeLevelZero = true;
//   _notificationData._imagePDF = false;

//   _notificationData._bulletinPDF = false;
//   _notificationData._readConfirmation = false;
//   _notificationData._bulletinPath = "";

//   _notificationData._legendPath = "";
//   _notificationData._xPositionLegend = 600;
//   _notificationData._yPositionLegend = 500;

//   _notificationData._levelTexts << QObject::tr("Nenhum") << QObject::tr("Observação") << QObject::tr("Atenção") << QObject::tr("Alerta") << QObject::tr("Alerta Máximo");

//   _previewData._title		= "TerraMA2";     
//   _previewData._logo		= "";		
//   _previewData._longitude	= "-52.00928";
//   _previewData._latitude	= "-24.053";	
//   _previewData._zoomLevel	= "6";
//   _previewData._googleKey	= "";
// #ifdef WIN32
//   _previewData._terraOGC	= "http://localhost/teogc/terraogcwmscgi.exe";	
// #else
//   _previewData._terraOGC	= "http://localhost/teogc/terraogcwmscgi";	
// #endif
//   _previewData._opacidade	= "0.7";
//   _previewData._intervalo	= "600000";
//   _previewData._outputPath	= "";
// }

// //! Carrega dados de configuração do arquivo recebido como parâmetro
// bool ConfigData::load(QString fileName, QString& err)
// {
// 	// Tentar abrir o arquivo de configurações:
// 	QFile configFile(fileName);
// 	if (!configFile.open(QIODevice::ReadOnly))
// 	{
// #if !defined QT_NO_DEBUG 	
//     // Auxilio ao processo de debug:
//     // Procura no diretório do executável (ou no anterior se o path incluir debug no final)
//     QString dir = QCoreApplication::applicationDirPath();
//     if(dir.endsWith("/debug")) // Estamos rodando de dentro do debugger...
//       dir.chop(6); // remove diretório 
//     configFile.setFileName(dir + "/" + fileName);
//   	if(!configFile.open(QIODevice::ReadOnly))
//   	{
//   	  err = QObject::tr("Erro abrindo arquivo %1.").arg(fileName);
// 		  return false;
//     }
// #else
// 		err = QObject::tr("Erro abrindo arquivo %1.").arg(fileName);
// 		return false;
// #endif
// 	}
	
// 	_configFile = QFileInfo(configFile);
// 	QString conteudo = configFile.readAll();
// 	// Criar o DOM para interpretar o conteúdo do XML:
// 	QDomDocument doc("sismaden");
// 	QString domErrorMsg;
// 	int domErrorLine, domErrorColumn;
// 	if(!doc.setContent(conteudo, &domErrorMsg, &domErrorLine, &domErrorColumn)) 
// 	{
// 		err = QObject::tr("Erro interpretando arquivo %1 (linha %2, coluna %3):\n%4")
// 		                 .arg(fileName).arg(domErrorLine).arg(domErrorColumn).arg(domErrorMsg);
// 		return false;
// 	}
// 	QDomElement docElem = doc.documentElement();

//   // Valida versão do documento
//  	if(docElem.attribute("configVersion", "") != "1.0")
//  	{
// 		err = QObject::tr("Documento %1 possui versão inválida de formato de configuração.").arg(fileName);
// 		return false;
//  	}

//   // Preenche dados atuais com defaults que serão usados caso alguma config. esteja ausente
//   loadDefaults();

//   // Carrega nome da configuração
//   _name = docElem.attribute("name", fileName);

//   // Iterar sobre os nós
// 	QDomNode n = docElem.firstChild();
	
// 	while(!n.isNull()) 
// 	{
// 		bool ret = true;
// 		QDomElement section = n.toElement(); // Tentar converter para um elememto
// 		if(!section.isNull()) 
// 		{
// 		  if(section.tagName() == "database")
// 			ret = loadSection(section, _dbData, err);
// 		  else if(section.tagName() == "analysis")
// 			ret = loadSection(section, _analysisData, err);
// 		  else if(section.tagName() == "collection")
// 			ret = loadSection(section, _collectionData, err);
// 		  else if(section.tagName() == "plans")
// 			ret = loadSection(section, _plansData, err);
// 		  else if(section.tagName() == "animation")
// 			ret = loadSection(section, _animationData, err);
// 		  else if(section.tagName() == "notification")
// 			ret = loadSection(section, _notificationData, err);
// 		  else if(section.tagName() == "preview")
// 			  ret = loadSection(section, _previewData, err);
// 		}

// 		if(!ret)
// 		  return false;

// 		n = n.nextSibling();
// 	}
//   return true;
// }

// //! Carrega configuracoes da base de dados
// bool ConfigData::loadSection(QDomElement& section, DataBaseServer& data, QString& err)
// {
//   QDomElement dbServer = section.firstChildElement("dbServer");
//   if(!dbServer.isNull())
//   {
//     data._hostName = dbServer.attribute("hostName", data._hostName);
//   	data._user     = dbServer.attribute("user",     data._user);
//   	data._password = dbServer.attribute("password", data._password);
//   	data._dbName   = dbServer.attribute("dbName",   data._dbName);
	
// 		bool ok;
// 	  QString portStr   = dbServer.attribute("port", QString::number(data._port));
// 		data._port = portStr.toInt(&ok);
// 		if(!ok)
//     {
// 			err = QObject::tr("Parâmetro port da seção dbServer deve ser um número inteiro.");
// 			return false;
// 		}
 
//     if(dbServer.hasAttribute("driver"))
//     {	  
// 	    QString driverStr = dbServer.attribute("driver");
//       if(!parseDatabaseDriver(driverStr, &data._driver)) 
//       {
// 	  		err = QObject::tr("Valor desconhecido para parâmetro driver da seção dbServer.");
// 		  	return false;
// 		  }
// 		} 
//   }

//   QDomElement params = section.firstChildElement("params");
//   if(!params.isNull())
//   {
//     if(params.hasAttribute("isStudy"))
//     {
//       QString study = params.attribute("isStudy");
// 	    data._study = (study.trimmed().toLower() == "true");
// 	  }  
//   }

//   return true;
// }

// //! Carrega configuracoes comuns aos diversos servicos
// bool ConfigData::loadSection(QDomElement& section, CommonData& data, QString& err)
// {
//   QDomElement log = section.firstChildElement("log");
//   if(!log.isNull())
//   {
//     data._logFile = log.attribute("outputFile", data._logFile);
//   }
  
//   QDomElement ws = section.firstChildElement("webservice");
//   if(!ws.isNull())
//   {
//     data._address = ws.attribute("address", data._address);
//     data._cmd     = ws.attribute("command", data._cmd);
//     data._params  = ws.attribute("parameters", data._params);
    
// 	  QString portStr = ws.attribute("portNumber", QString::number(data._servicePort));
//  		bool ok;
// 		data._servicePort = portStr.toInt(&ok);
// 		if(!ok)
//     {
// 			err = QObject::tr("Parâmetro portNumber da seção %1 deve ser um número inteiro.").arg(section.tagName());
// 			return false;
// 		}
//   }
//   return true;
// }

// //! Carrega configuracoes do modulo de analise
// bool ConfigData::loadSection(QDomElement& section, Analysis& data, QString& err)
// {
//   if(!loadSection(section, (CommonData&)data, err))
//     return false;
  
//   // Carrega numero de instancias do modulo 
//   QString numInstStr = section.attribute("numInstances", QString::number(data._numInstances));
// 	bool ok;
// 	data._numInstances = numInstStr.toInt(&ok);
// 	if(!ok || data._numInstances<1)
//   {
// 		err = QObject::tr("Parâmetro numInstances da seção analysis deve ser um número inteiro maior ou igual a 1.");
// 		return false;
// 	}
  
//   // Carrega intervalo entre rodadas das análises
//   QDomElement interval = section.firstChildElement("interval");
//   if(!interval.isNull())
//   {
// 		int h = 0, m = 0, s = 0;
// 		bool hOK, mOK, sOK;
		
// 		h = interval.attribute("h", "0").toInt(&hOK);
// 		m = interval.attribute("m", "0").toInt(&mOK);
// 		s = interval.attribute("s", "0").toInt(&sOK);
		
// 		if (!hOK || !mOK || !sOK || h < 0 || m < 0 || s < 0)
// 		{
// 			err = QObject::tr("Valor inválido para o intervalo da seção analysis");
// 			return false;
// 		}
// 		data._interval = s + 60 * m + 3600 * h;
//   }

//   //Carrega nome do diretorio onde as imagens de um estudo serao guardadas
//   QDomElement imageFilePath = section.firstChildElement("imageFilePath");
//   if(!imageFilePath.isNull())
//   {
// 	  data._imageFilePath = imageFilePath.attribute("path", "");
//   }
   
//   // Carrega dados das instâncias auxiliares
//   if(data._numInstances <= 1)
//     return true;
  
//   int ninst = data._numInstances-1;
//   data._instanceData.resize(ninst);
//   for(int i=0; i<ninst; i++)
//   {
//     // Carrega estrutura com defaults
//     data._instanceData[i]._address     = "localhost";
//     data._instanceData[i]._cmd         = ConfigDataAnalysisCmd;
//     data._instanceData[i]._params      = ConfigDataAnalysisPar;
//     data._instanceData[i]._servicePort = 8080;
//     data._instanceData[i]._logFile     = QString("log_analise_%1.txt").arg(i+2); // Primeira inst. adicional é a de índice 2

//     // Tenta ler do arquivo
//     QDomElement idata = section.firstChildElement(QString("instance_%1").arg(i+2));
//     if(!idata.isNull())
//     {
//       if(!loadSection(idata, data._instanceData[i], err))
//         return false;
//     }
//   }
    
//   return true;
// }

// //! Carrega configuracoes do modulo de coleta
// bool ConfigData::loadSection(QDomElement& section, Collection& data, QString& err)
// {
//   if(!loadSection(section, (CommonData&)data, err))
//     return false;
    
//   // Carrega timeout para conexões
//   QDomElement con = section.firstChildElement("connection");
//   if(!con.isNull())
//   {
//     bool ok;
//     QString timeoutStr = con.attribute("timeout", QString::number(data._timeout));
//     data._timeout = timeoutStr.toInt(&ok);
//     if(!ok)
//     {
//  	 	  err = QObject::tr("Parâmetro timeout da seção collection deve ser um número inteiro.");
//       return false;
//     }
//   }

//   //Carrega nome do diretorio onde os dados coletados serao guardados
//   QDomElement dirPathElement = section.firstChildElement("collectionDirPath");
//   if(!dirPathElement.isNull())
//   {
// 	  data._dirPath = dirPathElement.attribute("path", "");
//   }

//   return true;
// }

// //! Carrega configuracoes do modulo de planos
// bool ConfigData::loadSection(QDomElement& section, Plans& data, QString& err)
// {
//   return loadSection(section, (CommonData&)data, err);
// }

// //! Carrega configuracoes do modulo de animacao
// bool ConfigData::loadSection(QDomElement& section, Animation& data, QString& err)
// {

// 	if(!loadSection(section, (CommonData&)data, err))
// 	{
// 		return false;
// 	}   

// 	//Carrega nome do diretorio onde as imagens serao gravadas
// 	QDomElement imageDirectory = section.firstChildElement("imageDirectory");
// 	if(!imageDirectory.isNull())
// 	{
// 		data._imageDirectory = imageDirectory.attribute("directory", "");
// 	}

// 	return true;	
// }


// //! Carrega configuracoes do modulo de notificação
// bool ConfigData::loadSection(QDomElement& section, Notification& data, QString& err)
// {
//   if(!loadSection(section, (CommonData&)data, err))
//     return false;
  
//   // Carrega dados SMTP 
//   bool ok;
//   QDomElement smtp = section.firstChildElement("smtp");
//   if(!smtp.isNull())
//   {
//     data._smtpServer	= smtp.attribute("server", data._smtpServer);
// 	data._smtpMail		= smtp.attribute("mail", data._smtpMail);
// 	data._smtpPsswrd	= smtp.attribute("password", data._smtpPsswrd);
// 	data._smtpAlias		= smtp.attribute("alias", data._smtpAlias);
// 	data._smtpPriority  = smtp.attribute("priority", data._smtpPriority);
	
// 	if(smtp.hasAttribute("ssl"))
// 	{
// 		QString SSLstr = smtp.attribute("ssl");
// 		data._smtpSSL = (SSLstr.trimmed().toLower() == "true");
// 	}

// 	QString portStr  = smtp.attribute("port", QString::number(data._smtpPort));
// 	data._smtpPort = portStr.toInt(&ok);
// 	if(!ok)
// 	{
// 		err = QObject::tr("Parâmetro port da seção notification deve ser um número inteiro.");
// 		return false;
// 	}

// 	QString timeoutStr  = smtp.attribute("timeout", QString::number(data._smtpTimeout));
// 	data._smtpTimeout = timeoutStr.toInt(&ok);
// 	if(!ok)
// 	{
// 		err = QObject::tr("Parâmetro timeout da seção notification deve ser um número inteiro.");
// 		return false;
// 	}

// 	if(smtp.hasAttribute("readConfirmation"))
// 	{
// 		QString readStr = smtp.attribute("readConfirmation");
// 		data._readConfirmation = (readStr.trimmed().toLower() == "true");
// 	}
// 	else
// 		data._readConfirmation = false;
//   }

//   // Carrega dados de proxy
//   QDomElement proxy = section.firstChildElement("proxy");
//   if(!proxy.isNull())
//   {
//     data._proxyUser = proxy.attribute("user", data._proxyUser);
// 	data._proxyPsswrd = proxy.attribute("password", data._proxyPsswrd);
//   }

//   // Carrega o link para o serviço de visualização
//   QDomElement url = section.firstChildElement("terraphpurl");
//   if(!url.isNull())
//   {
//     data._terraPHPURL = url.attribute("url", data._terraPHPURL);
//   }
  
//   // Carrega intervalo entre rodadas das análises
//   QDomElement interval = section.firstChildElement("interval");
//   if(!interval.isNull())
//   {
// 		int h = 0, m = 0, s = 0;
// 		bool hOK, mOK, sOK;
		
// 		h = interval.attribute("h", "0").toInt(&hOK);
// 		m = interval.attribute("m", "0").toInt(&mOK);
// 		s = interval.attribute("s", "0").toInt(&sOK);
		
// 		if (!hOK || !mOK || !sOK || h < 0 || m < 0 || s < 0)
// 		{
// 			err = QObject::tr("Valor inválido para o intervalo da seção notification");
// 			return false;
// 		}
// 		data._interval = s + 60 * m + 3600 * h;
//   }

//   // Carrega o tipo de envio de email
//   QDomElement sendmail = section.firstChildElement("sendemail");
//   if(!sendmail.isNull())
//   {
// 	  if(sendmail.hasAttribute("sendupemail"))
// 	  {
// 		  QString UPstr = sendmail.attribute("sendupemail");
// 		  data._sendUpEmail = (UPstr.trimmed().toLower() == "true");
// 	  }
//   }

//   // Carrega o tipo de envio de email
//   QDomElement imagepdf = section.firstChildElement("imagepdf");
//   if(!imagepdf.isNull())
//   {
// 	  QString PDFstr = imagepdf.attribute("sendimagepdf");
// 	  data._imagePDF = (PDFstr.trimmed().toLower() == "true");
//   }

//   // Carrega intervalo entre rodadas das análises
//   QDomElement intervalBulletin = section.firstChildElement("bulletin");
//   if(!intervalBulletin.isNull())
//   {
// 	  int h = 0, m = 0, s = 0;
// 	  bool hOK, mOK, sOK;

// 	  h = intervalBulletin.attribute("h", "0").toInt(&hOK);
// 	  m = intervalBulletin.attribute("m", "0").toInt(&mOK);
// 	  s = intervalBulletin.attribute("s", "0").toInt(&sOK);

// 	  if (!hOK || !mOK || !sOK || h < 0 || m < 0 || s < 0)
// 	  {
// 		  err = QObject::tr("Valor inválido para o intervalo da seção notification");
// 		  return false;
// 	  }
// 	  data._intervalBulletin = s + 60 * m + 3600 * h;

// 	  QString remove = intervalBulletin.attribute("removelevel","false");
// 	  data._removeLevelZero = (remove.trimmed().toLower() == "true");

// 	  QString pdf = intervalBulletin.attribute("pdf","false");
// 	  data._bulletinPDF = (pdf.trimmed().toLower() == "true");

// 	  QString pdfPath = intervalBulletin.attribute("pdfPath","");
// 	  data._bulletinPath = pdfPath;
//   }

//   QDomElement legend = section.firstChildElement("legend");
//   if(!legend.isNull())
//   {
// 	  data._legendPath = legend.attribute("path",QString());

// 	  bool lOk;
// 	  data._xPositionLegend = legend.attribute("xPos", "600").toInt(&lOk);
// 	  data._yPositionLegend = legend.attribute("yPos", "500").toInt(&lOk);
//   }
//   else
//   {
// 	  data._legendPath = QString();
// 	  data._xPositionLegend = 600;
// 	  data._yPositionLegend = 500;
//   }

//   QDomElement levels = section.firstChildElement("levels");
//   if(!levels.isNull())
//   {
// 	  QString level0 = levels.attribute("level0",QObject::tr("Nenhum"));
// 	  QString level1 = levels.attribute("level1",QObject::tr("Observação"));
// 	  QString level2 = levels.attribute("level2",QObject::tr("Atenção"));
// 	  QString level3 = levels.attribute("level3",QObject::tr("Alerta"));
// 	  QString level4 = levels.attribute("level4",QObject::tr("Alerta Máximo"));

// 	  data._levelTexts.clear();
// 	  data._levelTexts << level0 << level1 << level2 << level3 << level4;					   
//   }
//   else
//   {
// 	  data._levelTexts.clear();
// 	  data._levelTexts << QObject::tr("Nenhum") << QObject::tr("Observação") << QObject::tr("Atenção") << QObject::tr("Alerta") << QObject::tr("Alerta Máximo");
//   }
    
//   return true;
// }

// bool ConfigData::loadSection( QDomElement& section, Preview& data, QString& err )
// {
// 	QDomElement config = section.firstChildElement("configuration");
// 	if(!config.isNull())
// 	{
// 		data._title		= config.attribute("title", "TerraMA2");
// 		data._logo		= config.attribute("logo", "");
// 		data._longitude = config.attribute("longitude", "0.000");
// 		data._latitude	= config.attribute("latitude", "0.000");
// 		data._zoomLevel = config.attribute("zoom", "6");
// 		data._googleKey = config.attribute("googlekey", "");
// #ifdef WIN32
// 		data._terraOGC  = config.attribute("terraogc", "http://localhost/teogc/terraogcwmscgi.exe");
// #else
// 		data._terraOGC  = config.attribute("terraogc", "http://localhost/teogc/terraogcwmscgi");
// #endif
// 		data._opacidade = config.attribute("opacity", "0.7");
// 		data._intervalo = config.attribute("interval", "600000");
// 	}

// 	QDomElement output = section.firstChildElement("output");
// 	if(!output.isNull())
// 	{
// 		data._outputPath = output.attribute("path", "");
// 	}

// 	return true;
// }

// /**
//  * Converte uma string com o nome do driver para o identificador correspondente.
//  * @param inStr A string com o nome do driver
//  * @param outID Ponteiro onde o resultado da conversão será armazenado
//  * @return true  Se a string foi convertida,
//  *         false Se a string não foi reconhecida como um nome de driver.
//  */
// bool ConfigData::parseDatabaseDriver(const QString& inStr, DatabaseDriver* outID)
// {
// 	if(inStr == "PostGIS")
// 		*outID = DRIVER_PostGIS;
// 	else if(inStr == "PostgreSQL")
// 		*outID = DRIVER_PostgreSQL;
// 	else if(inStr == "MySQL")
// 	  *outID = DRIVER_MySQL;
// 	else  
// 		return false;
// 	return true;
// }

// //! Salva dados de configuração no arquivo recebido como parâmetro
// bool ConfigData::save(QString fileName, QString& err) const
// {
//   // Cria a raiz da árvore e apenda atributos de versão e nome
//   QDomDocument doc("sismaden");
//   QDomElement root = doc.createElement("sismaden");
//   root.setAttribute("configVersion", "1.0");
//   root.setAttribute("name", _name);
//   doc.appendChild(root);

//   // Cria seçoes
//   saveSection(doc, root, _dbData);
//   saveSection(doc, root, _analysisData);
//   saveSection(doc, root, _collectionData);
//   saveSection(doc, root, _plansData);
//   saveSection(doc, root, _animationData);
//   saveSection(doc, root, _notificationData);
//   saveSection(doc, root, _previewData);

//   // Salva resultado no arquivo
//   QString xml = doc.toString(); 
 
// 	QFile configFile(fileName);
// 	if(!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
// 	{
// 		err = QObject::tr("Erro criando arquivo %1.").arg(fileName);
// 		return false;
// 	}
  
//   if(configFile.write(xml.toLatin1().data(), xml.size()) != xml.size())
//   {
// 		err = QObject::tr("Erro escrevendo no arquivo %1.").arg(fileName);
// 		return false;
//   }
//   return true;
// }

// //! Exporta dados de configuração de Servidor
//  bool ConfigData::exportConfig(dataRemoteWeatherServer dados ,QString fileName){

//     QString err("");

//     // Cria a raiz da árvore e apenda atributos de versão e nome
//     QDomDocument doc("TERRAMA2");
//     QDomElement root = doc.createElement("TERRAMA2");
//     root.setAttribute("configVersion", "1.0");
//     root.setAttribute("name", "Remote Weather Server Configuration");
//     doc.appendChild(root);

//     // Cria seçoes
//     saveSection(doc, root, dados);


//     // Salva resultado no arquivo
//     QString xml = doc.toString();

//       QFile configFile(fileName);
//       if(!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//       {
//           err = QObject::tr("Erro criando arquivo %1.").arg(fileName);
//           return false;
//       }

//     if(configFile.write(xml.toLatin1().data(), xml.size()) != xml.size())
//     {
//           err = QObject::tr("Erro escrevendo no arquivo %1.").arg(fileName);
//           return false;
//     }
//     return true;
// }

//  bool ConfigData::exportConfig(dataGrid dados ,QString fileName){

//     QString err("");

//     // Cria a raiz da árvore e apenda atributos de versão e nome
//     QDomDocument doc("TERRAMA2");
//     QDomElement root = doc.createElement("TERRAMA2");
//     root.setAttribute("configVersion", "1.0");
//     root.setAttribute("name", "DataGridConfiguration");
//     doc.appendChild(root);

//     // Cria seçoes
//     saveSection(doc, root, dados);


//     // Salva resultado no arquivo
//     QString xml = doc.toString();

//       QFile configFile(fileName);
//       if(!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//       {
//           err = QObject::tr("Erro criando arquivo %1.").arg(fileName);
//           return false;
//       }

//     if(configFile.write(xml.toLatin1().data(), xml.size()) != xml.size())
//     {
//           err = QObject::tr("Erro escrevendo no arquivo %1.").arg(fileName);
//           return false;
//     }
//     return true;
// }

//  bool ConfigData::exportConfig(dataPoint dados ,QString fileName){

//     QString err("");

//     // Cria a raiz da árvore e apenda atributos de versão e nome
//     QDomDocument doc("TERRAMA2");
//     QDomElement root = doc.createElement("TERRAMA2");
//     root.setAttribute("configVersion", "1.0");
//     root.setAttribute("name", "DataPointConfiguration");
//     doc.appendChild(root);

//     // Cria seçoes
//     saveSection(doc, root, dados);


//     // Salva resultado no arquivo
//     QString xml = doc.toString();

//       QFile configFile(fileName);
//       if(!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//       {
//           err = QObject::tr("Erro criando arquivo %1.").arg(fileName);
//           return false;
//       }

//     if(configFile.write(xml.toLatin1().data(), xml.size()) != xml.size())
//     {
//           err = QObject::tr("Erro escrevendo no arquivo %1.").arg(fileName);
//           return false;
//     }
//     return true;
// }

//  bool ConfigData::exportConfig(dataPointDiff dados ,QString fileName){

//     QString err("");

//     // Cria a raiz da árvore e apenda atributos de versão e nome
//     QDomDocument doc("TERRAMA2");
//     QDomElement root = doc.createElement("TERRAMA2");
//     root.setAttribute("configVersion", "1.0");
//     root.setAttribute("name", "DataPointDiffConfiguration");
//     doc.appendChild(root);

//     // Cria seçoes
//     saveSection(doc, root, dados);


//     // Salva resultado no arquivo
//     QString xml = doc.toString();

//       QFile configFile(fileName);
//       if(!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//       {
//           err = QObject::tr("Erro criando arquivo %1.").arg(fileName);
//           return false;
//       }

//     if(configFile.write(xml.toLatin1().data(), xml.size()) != xml.size())
//     {
//           err = QObject::tr("Erro escrevendo no arquivo %1.").arg(fileName);
//           return false;
//     }
//     return true;
// }


// // ! Retorna o nome do driver associado a seu Id
// QString ConfigData::getDatabaseDriver(DatabaseDriver id) const
// {
//   switch(id)
//   {
//     case DRIVER_PostGIS:    return "PostGIS";
//     case DRIVER_PostgreSQL: return "PostgreSQL";
// 	case DRIVER_MySQL:      return "MySQL";
//   }
//   return ""; // Nunca deve chegara aqui...
// }


// //! Salva no documento XML dados referentes à seção de base de dados
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const DataBaseServer& data) const
//  {
//    QDomElement databaseTag = doc.createElement("database");
//    root.appendChild(databaseTag);

//    QDomElement dbServerTag = doc.createElement("dbServer");
//    dbServerTag.setAttribute("hostName", data._hostName);
//    dbServerTag.setAttribute("port",     data._port);
//    dbServerTag.setAttribute("user",     data._user);
//    dbServerTag.setAttribute("password", data._password);
//    dbServerTag.setAttribute("dbName",   data._dbName);
//    dbServerTag.setAttribute("driver",   getDatabaseDriver(data._driver));
//    databaseTag.appendChild(dbServerTag);

//    QDomElement paramsTag = doc.createElement("params");
//    paramsTag.setAttribute("isStudy", data._study ? "true" : "false");
//    databaseTag.appendChild(paramsTag);
//  }
 
// //! Salva no documento XML dados referentes à dados comuns de um serviço
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const CommonData& data) const
//  {
//    QDomElement logTag = doc.createElement("log");
//    logTag.setAttribute("outputFile", data._logFile);
//    root.appendChild(logTag);
   
//    QDomElement webserviceTag = doc.createElement("webservice");
//    webserviceTag.setAttribute("portNumber", data._servicePort);
//    webserviceTag.setAttribute("address",    data._address);
//    webserviceTag.setAttribute("command",    data._cmd);
//    webserviceTag.setAttribute("parameters", data._params);
//    root.appendChild(webserviceTag);
//  }
 
// //! Salva no documento XML dados referentes ao serviço de análise
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const Analysis& data) const
//  {
//    QDomElement analysisTag = doc.createElement("analysis");
//    analysisTag.setAttribute("numInstances", data._numInstances);
//    root.appendChild(analysisTag);

//    saveSection(doc, analysisTag, (CommonData&)data);

//    QDomElement intervalTag = doc.createElement("interval");
//    int mins = data._interval / 60;
//    intervalTag.setAttribute("h", mins / 60);
//    intervalTag.setAttribute("m", mins % 60);
//    intervalTag.setAttribute("s", data._interval % 60);
//    analysisTag.appendChild(intervalTag);
   
//   int ninst = data._numInstances-1;
//   assert(data._instanceData.size() == ninst);
//   for(int i=0; i<ninst; i++)
//   {
//     QDomElement instanceTag = doc.createElement(QString("instance_%1").arg(i+2));
//     saveSection(doc, instanceTag, data._instanceData[i]); 
//     analysisTag.appendChild(instanceTag);
//   }

//   QDomElement imageFilePath = doc.createElement("imageFilePath");
//   imageFilePath.setAttribute("path", data._imageFilePath);
//   analysisTag.appendChild(imageFilePath);
// }
 
// //! Salva no documento XML dados referentes ao serviço de aquisição
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const Collection& data) const
//  {
//    QDomElement collectionTag = doc.createElement("collection");
//    root.appendChild(collectionTag);

//    saveSection(doc, collectionTag, (CommonData&)data);

//    QDomElement connectionTag = doc.createElement("connection");
//    connectionTag.setAttribute("timeout", data._timeout);
//    collectionTag.appendChild(connectionTag);

//    QDomElement dirPathElement = doc.createElement("collectionDirPath");
//    dirPathElement.setAttribute("path", data._dirPath);
//    collectionTag.appendChild(dirPathElement);
//  }
 
// //! Salva no documento XML dados referentes ao serviço de planos
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const Plans& data) const
//  {
//    QDomElement plansTag = doc.createElement("plans");
//    root.appendChild(plansTag);
   
//    saveSection(doc, plansTag, (CommonData&)data);   
//  }

//  //! Salva no documento XML dados referentes ao serviço de animacao
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const Animation& data) const
//  {
//    QDomElement animationTag = doc.createElement("animation");
//    root.appendChild(animationTag);

//    saveSection(doc, animationTag, (CommonData&)data);

//    QDomElement imageDirectory = doc.createElement("imageDirectory");
//    imageDirectory.setAttribute("directory", data._imageDirectory);
//    animationTag.appendChild(imageDirectory);
//  }
 
// //! Salva no documento XML dados referentes ao serviço de notificações
//  void ConfigData::saveSection(QDomDocument& doc, QDomElement& root, const Notification& data) const
//  {
//    QDomElement notificationTag = doc.createElement("notification");
//    root.appendChild(notificationTag);

//    saveSection(doc, notificationTag, (CommonData&)data);

//    QDomElement intervalTag = doc.createElement("interval");
//    int mins = data._interval / 60;
//    intervalTag.setAttribute("h", mins / 60);
//    intervalTag.setAttribute("m", mins % 60);
//    intervalTag.setAttribute("s", data._interval % 60);
//    notificationTag.appendChild(intervalTag);

//    QDomElement smtpTag = doc.createElement("smtp");
//    smtpTag.setAttribute("server", data._smtpServer);
//    smtpTag.setAttribute("port",   data._smtpPort);
//    smtpTag.setAttribute("mail",   data._smtpMail);
//    smtpTag.setAttribute("password", data._smtpPsswrd);
//    smtpTag.setAttribute("timeout", data._smtpTimeout);
//    smtpTag.setAttribute("alias", data._smtpAlias);
//    smtpTag.setAttribute("priority", data._smtpPriority);
//    smtpTag.setAttribute("ssl", data._smtpSSL? "true" : "false");
//    smtpTag.setAttribute("readConfirmation", data._readConfirmation ? "true" : "false");
//    notificationTag.appendChild(smtpTag);

//    QDomElement proxyTag = doc.createElement("proxy");
//    proxyTag.setAttribute("user",  data._proxyUser);
//    proxyTag.setAttribute("port",  data._proxyUser);
//    notificationTag.appendChild(proxyTag);

//    QDomElement urlTag = doc.createElement("terraphpurl");
//    urlTag.setAttribute("url", data._terraPHPURL);
//    notificationTag.appendChild(urlTag);

//    QDomElement emailTag = doc.createElement("sendemail");
//    emailTag.setAttribute("sendupemail", data._sendUpEmail? "true" : "false");
//    notificationTag.appendChild(emailTag);

//    QDomElement imgpdfTag = doc.createElement("imagepdf");
//    imgpdfTag.setAttribute("sendimagepdf", data._imagePDF? "true" : "false");
//    notificationTag.appendChild(imgpdfTag);

//    QDomElement intervalBulletinTag = doc.createElement("bulletin");
//    int minsB = data._intervalBulletin / 60;
//    intervalBulletinTag.setAttribute("h", minsB / 60);
//    intervalBulletinTag.setAttribute("m", minsB % 60);
//    intervalBulletinTag.setAttribute("s", data._intervalBulletin % 60);
   
//    intervalBulletinTag.setAttribute("removelevel", data._removeLevelZero ? "true" : "false");
//    intervalBulletinTag.setAttribute("pdf", data._bulletinPDF ? "true" : "false");
//    intervalBulletinTag.setAttribute("pdfPath", data._bulletinPath);
//    notificationTag.appendChild(intervalBulletinTag);

//    QDomElement legendTag = doc.createElement("legend");
//    legendTag.setAttribute("path", data._legendPath);
//    legendTag.setAttribute("xPos", data._xPositionLegend);
//    legendTag.setAttribute("yPos", data._yPositionLegend);
//    notificationTag.appendChild(legendTag);

//    QDomElement levelsTag = doc.createElement("levels");
//    levelsTag.setAttribute("level0", data._levelTexts[0]);
//    levelsTag.setAttribute("level1", data._levelTexts[1]);
//    levelsTag.setAttribute("level2", data._levelTexts[2]);
//    levelsTag.setAttribute("level3", data._levelTexts[3]);
//    levelsTag.setAttribute("level4", data._levelTexts[4]);
//    notificationTag.appendChild(levelsTag);
//  }


//  void ConfigData::saveSection( QDomDocument& doc, QDomElement& root, const Preview& data ) const
//  {
// 	 QDomElement previewTag = doc.createElement("preview");
// 	 root.appendChild(previewTag);

// 	 QDomElement configTag = doc.createElement("configuration");
// 	 configTag.setAttribute("title", data._title);
// 	 configTag.setAttribute("logo", data._logo);
// 	 configTag.setAttribute("longitude", data._longitude);
// 	 configTag.setAttribute("latitude", data._latitude);
// 	 configTag.setAttribute("zoom", data._zoomLevel);
// 	 configTag.setAttribute("googlekey", data._googleKey);
// 	 configTag.setAttribute("terraogc", data._terraOGC);
// 	 configTag.setAttribute("opacity", data._opacidade);
// 	 configTag.setAttribute("interval", data._intervalo);
// 	 previewTag.appendChild(configTag);

// 	 QDomElement outputTag = doc.createElement("output");
// 	 outputTag.setAttribute("path", data._outputPath);
// 	 previewTag.appendChild(outputTag);
//  }


// void ConfigData::saveSection( QDomDocument& doc, QDomElement& root, const dataRemoteWeatherServer& configValues ) const
//  {
//      QDomElement previewTag = doc.createElement("ServerConfiguration");
//      root.appendChild(previewTag);

//      QDomElement configTag = doc.createElement("configuration");
//      configTag.setAttribute("connvc_name", configValues.serverName );
//      configTag.setAttribute("connte_details", configValues.description);
//      configTag.setAttribute("connvc_url", configValues.adress);
//      configTag.setAttribute("connin_port", configValues.port);
//      configTag.setAttribute("connvc_protocol", configValues.protocol);
//      configTag.setAttribute("connvc_username", configValues.user);
//      configTag.setAttribute("connvc_password", configValues.password);
//      //configTag.setAttribute("connbo_active", data.active);
//      configTag.setAttribute("connin_interval_minutes", configValues.intervalMinutes);
//      configTag.setAttribute("connte_base_path", configValues.basePath);
//      previewTag.appendChild(configTag);

//  }


// void ConfigData::saveSection( QDomDocument& doc, QDomElement& root, const dataGrid& configValues ) const
//  {
//      QDomElement previewTag = doc.createElement("DataGridConfiguration");
//      root.appendChild(previewTag);

//      QDomElement configTag = doc.createElement("configuration");
//      configTag.setAttribute("dconvc_name", configValues.gridFormatDataName );
//      configTag.setAttribute("dconte_type", configValues.gridFormatDataType);
//      configTag.setAttribute("dconre_spatial_resolution", configValues.gridFormatDataResolution);
//      configTag.setAttribute("dconre_update_freq_minutes", configValues.gridFormatDataFrequency);
//      configTag.setAttribute("dconte_details", configValues.gridFormatDataDescription);
//      configTag.setAttribute("dconte_path", configValues.gridFormatDataPath);
//      configTag.setAttribute("dconte_mask", configValues.gridFormatDataMask);
//      configTag.setAttribute("dconte_format", configValues.gridFormatDataFormat);
//      configTag.setAttribute("dconte_timezone", configValues.gridFormatDataTimeZoneCmb);
//      configTag.setAttribute("dconte_prefix", configValues.gridFormatDataPrefix);
//      configTag.setAttribute("dconte_unit", configValues.gridFormatDataUnit);
//      configTag.setAttribute("dconte_tiff_navigation_file", configValues.ledGridTIFFArqNavegacao);
//      configTag.setAttribute("dconte_ascii_grid_coord_unitDec", configValues.rbGridAscUnidGrausDec);
//      configTag.setAttribute("dconte_ascii_grid_coord_unitMil", configValues.rbGridAscUnidGrausMil);
//      configTag.setAttribute("dconte_grads_ctl_file", configValues.ledGridGrADSArqControle);
//      configTag.setAttribute("dconte_grads_data_typeInt", configValues.rbGridGrADSTipoDadosInt);
//      configTag.setAttribute("dconte_grads_data_typeFloat", configValues.rbGridGrADSTipoDadosFloat);
//      configTag.setAttribute("dconre_grads_multiplier", configValues.ledGridGrADSMultiplicador);
//      configTag.setAttribute("dconte_grads_swap", configValues.cmbGridGrADSByteOrder);
//      configTag.setAttribute("dconin_grads_numbands", configValues.spbGridGrADSNumBands);
//      configTag.setAttribute("dconin_grads_bandtimeoffset", configValues.spbGridGrADSTimeOffset);
//      configTag.setAttribute("dconin_grads_bandheader", configValues.spbGridGrADSHeaderSize);
//      configTag.setAttribute("dconin_grads_bandtrailler", configValues.spbGridGrADSTraillerSize);
//      configTag.setAttribute("dconre_wcs_dummy", configValues.ledGridWCSDummy);

//      configTag.setAttribute("projection_name", configValues.projection_name);
//      configTag.setAttribute("projection_datum", configValues.projection_datum);
//      configTag.setAttribute("projection_hemis", configValues.projection_hemisphere);
//      configTag.setAttribute("projection_lat0", configValues.projection_lat0);
//      configTag.setAttribute("projection_lon0", configValues.projection_lon0);
//      configTag.setAttribute("projection_offx", configValues.projection_offx);
//      configTag.setAttribute("projection_offy", configValues.projection_offy);
//      configTag.setAttribute("projection_scale", configValues.projection_scale);
//      configTag.setAttribute("projection_stlat1", configValues.projection_stlat1);
//      configTag.setAttribute("projection_stlat2", configValues.projection_stlat2);
//      configTag.setAttribute("projection_units", configValues.projection_units);

//      previewTag.appendChild(configTag);

//  }


// void ConfigData::saveSection( QDomDocument& doc, QDomElement& root, const dataPoint& configValues ) const
//  {
//      QDomElement previewTag = doc.createElement("DataPointConfiguration");
//      root.appendChild(previewTag);

//      QDomElement configTag = doc.createElement("configuration");
//      configTag.setAttribute("dconvc_name", configValues.pointFormatDataName );
//      configTag.setAttribute("dconte_type", configValues.pointFormatDataType);
//      configTag.setAttribute("pointFormatDataRadius", configValues.pointFormatDataRadius);
//      configTag.setAttribute("dconre_update_freq_minutes", configValues.pointFormatDataFrequency);
//      configTag.setAttribute("dconte_details", configValues.pointFormatDataDescription);
//      configTag.setAttribute("dconte_path", configValues.pointFormatDataPath);
//      configTag.setAttribute("dconte_mask", configValues.pointFormatDataMask);
//      configTag.setAttribute("dconte_prefix", configValues.pointFormatDataPrefix);
//      configTag.setAttribute("dconte_unit", configValues.pointFormatDataUnit);
//      configTag.setAttribute("dconte_timezone", configValues.pointFormatDataTimeZoneCmb);
//      configTag.setAttribute("dconte_format", configValues.pointFormatDataFormat);

// /*   configTag.setAttribute("dconvc_pcd_infl_type", configValues.pointFormatDataInfluenceCmb);
//      configTag.setAttribute("pcd_theme_id", configValues.pointFormatDataThemeCmb);
//      configTag.setAttribute("dconv_pcd_attribute", configValues.pointFormatDataAttributeCmb);
// */

//      configTag.setAttribute("projection_name", configValues.projection_name);
//      configTag.setAttribute("projection_datum", configValues.projection_datum);
//      configTag.setAttribute("projection_hemis", configValues.projection_hemisphere);
//      configTag.setAttribute("projection_lat0", configValues.projection_lat0);
//      configTag.setAttribute("projection_lon0", configValues.projection_lon0);
//      configTag.setAttribute("projection_offx", configValues.projection_offx);
//      configTag.setAttribute("projection_offy", configValues.projection_offy);
//      configTag.setAttribute("projection_scale", configValues.projection_scale);
//      configTag.setAttribute("projection_stlat1", configValues.projection_stlat1);
//      configTag.setAttribute("projection_stlat2", configValues.projection_stlat2);
//      configTag.setAttribute("projection_units", configValues.projection_units);

//      previewTag.appendChild(configTag);

//      pointParams pcd;
//      for (int i = 0; i < configValues.pt.size(); i++)
//      {
//          configTag                = doc.createElement("point");
//          pcd       = configValues.pt.at(i);

//          configTag.setAttribute("fileName", pcd.fileName);
//          configTag.setAttribute("latitude", pcd.latitude);
//          configTag.setAttribute("longitude", pcd.longitude);
//          configTag.setAttribute("isActive", pcd.isActive);

//          previewTag.appendChild(configTag);
//      }
//  }


// void ConfigData::saveSection( QDomDocument& doc, QDomElement& root, const dataPointDiff& configValues ) const
//  {
//      QDomElement previewTag = doc.createElement("DataPointDiffConfiguration");
//      root.appendChild(previewTag);

//      QDomElement configTag = doc.createElement("configuration");
//      configTag.setAttribute("dconvc_name", configValues.pointDiffFormatDataName);
//      configTag.setAttribute("dconte_type", configValues.pointDiffFormatDataType);
//      configTag.setAttribute("dconre_update_freq_minutes", configValues.pointDiffFormatDataFrequency);
//      configTag.setAttribute("dconte_timezone", configValues.pointDiffFormatDataTimeZoneCmb);
//      configTag.setAttribute("dconte_prefix", configValues.pointDiffFormatDataPrefix);
//      configTag.setAttribute("dconte_unit", configValues.pointDiffFormatDataUnit);
//      configTag.setAttribute("dconte_details", configValues.pointDiffFormatDataDescription);
//      configTag.setAttribute("dconte_path", configValues.pointDiffFormatDataPath);
//      configTag.setAttribute("dconte_mask", configValues.pointDiffFormatDataMask);
//      configTag.setAttribute("dconte_format", configValues.pointDiffFormatDataFormat);

//      configTag.setAttribute("projection_name", configValues.projection_name);
//      configTag.setAttribute("projection_datum", configValues.projection_datum);
//      configTag.setAttribute("projection_hemis", configValues.projection_hemisphere);
//      configTag.setAttribute("projection_lat0", configValues.projection_lat0);
//      configTag.setAttribute("projection_lon0", configValues.projection_lon0);
//      configTag.setAttribute("projection_offx", configValues.projection_offx);
//      configTag.setAttribute("projection_offy", configValues.projection_offy);
//      configTag.setAttribute("projection_scale", configValues.projection_scale);
//      configTag.setAttribute("projection_stlat1", configValues.projection_stlat1);
//      configTag.setAttribute("projection_stlat2", configValues.projection_stlat2);
//      configTag.setAttribute("projection_units", configValues.projection_units);

//      previewTag.appendChild(configTag);

//  }


// QDomNode ConfigData::loadConfig(QString fileName, QString& err, QString& configType)
// {

//     // Tentar abrir o arquivo de configurações:
//     QFile configFile(fileName);

//     if (!configFile.open(QIODevice::ReadOnly))
//     {
//         err = QObject::tr("Erro abrindo arquivo %1.").arg(fileName);
//     }

//     QString conteudo = configFile.readAll();
//     // Criar o DOM para interpretar o conteúdo do XML:
//     QDomDocument doc("TERRAMA2");
//     QString domErrorMsg;
//     int domErrorLine, domErrorColumn;

//     if(!doc.setContent(conteudo, &domErrorMsg, &domErrorLine, &domErrorColumn))
//     {
//         err = QObject::tr("Erro interpretando arquivo %1 (linha %2, coluna %3):\n%4")
//                          .arg(fileName).arg(domErrorLine).arg(domErrorColumn).arg(domErrorMsg);
//     }
//     QDomElement docElem = doc.documentElement();

//   // Valida versão do documento
//     if(docElem.attribute("configVersion", "") != "1.0")
//     {
//         err = QObject::tr("Documento %1 possui versão inválida de formato de configuração.").arg(fileName);
//     }

//     configType = docElem.attribute("name", "");

//     //QDomNode n = docElem.firstChild();
//     return docElem.firstChild();
// }


// ConfigData::dataRemoteWeatherServer ConfigData::loadRemoteWeatherServerConfig(QDomNode node){

//     ConfigData::dataRemoteWeatherServer configValues;
//     // Iterar sobre os nós
//     while(!node.isNull())
//     {
//         QDomElement section = node.toElement(); // Tentar converter para um elememto
//         if(!section.isNull())
//         {
//           if(section.tagName() == "ServerConfiguration"){

//               QDomElement server = section.firstChildElement("configuration");
//               if(!server.isNull())
//               {                  
//                   //configValues.active               = (server.attribute("connbo_active", false) == "1"? true:false);
//                   configValues.adress               = server.attribute("connvc_url", "");
//                   configValues.basePath             = server.attribute("connte_base_path", "");
//                   configValues.description          = server.attribute("connte_details", "");
//                   configValues.intervalMinutes      = server.attribute("connin_interval_minutes", "1" ).toInt();
//                   configValues.password             = server.attribute("connvc_password", "");
//                   configValues.port                 = server.attribute("connin_port", "");
//                   configValues.protocol             = server.attribute("connvc_protocol", 0).toInt();
//                   configValues.serverName           = server.attribute("connvc_name", "");
//                   configValues.user                 = server.attribute("connvc_username", "");
//               }
//           }
//         }

//         node = node.nextSibling();
//     }

//     return configValues;
// }


// ConfigData::dataGrid ConfigData::loadDataGridConfig(QDomNode node){

//     ConfigData::dataGrid configValues;
//     // Iterar sobre os nós
//     while(!node.isNull())
//     {
//         QDomElement section = node.toElement(); // Tentar converter para um elememto
//         if(!section.isNull())
//         {
//           if(section.tagName() == "DataGridConfiguration"){

//               QDomElement server = section.firstChildElement("configuration");
//               if(!server.isNull())
//               {
//                   configValues.gridFormatDataName             = server.attribute("dconvc_name", configValues.gridFormatDataName );
//                   configValues.gridFormatDataType             = server.attribute("dconte_type", 0).toInt();
//                   configValues.gridFormatDataResolution       = server.attribute("dconre_spatial_resolution", configValues.gridFormatDataResolution);
//                   configValues.gridFormatDataFrequency        = server.attribute("dconre_update_freq_minutes", configValues.gridFormatDataFrequency);
//                   configValues.gridFormatDataDescription      = server.attribute("dconte_details", configValues.gridFormatDataDescription);
//                   configValues.gridFormatDataPath             = server.attribute("dconte_path", configValues.gridFormatDataPath);
//                   configValues.gridFormatDataMask             = server.attribute("dconte_mask", configValues.gridFormatDataMask);
//                   configValues.gridFormatDataFormat           = server.attribute("dconte_format", 0).toInt();
//                   configValues.gridFormatDataTimeZoneCmb      = server.attribute("dconte_timezone", 0).toInt();
//                   configValues.gridFormatDataPrefix           = server.attribute("dconte_prefix", configValues.gridFormatDataPrefix);
//                   configValues.gridFormatDataUnit             = server.attribute("dconte_unit", configValues.gridFormatDataUnit);
//                   configValues.ledGridTIFFArqNavegacao        = server.attribute("dconte_tiff_navigation_file", configValues.ledGridTIFFArqNavegacao);
//                   configValues.rbGridAscUnidGrausDec          = (server.attribute("dconte_ascii_grid_coord_unitDec", false) == "1"? true:false);
//                   configValues.rbGridAscUnidGrausMil          = (server.attribute("dconte_ascii_grid_coord_unitMil", false) == "1"? true:false);
//                   configValues.ledGridGrADSArqControle        = server.attribute("dconte_grads_ctl_file", configValues.ledGridGrADSArqControle);
//                   configValues.rbGridGrADSTipoDadosInt        = (server.attribute("dconte_grads_data_typeInt", false) == "1"? true:false);
//                   configValues.rbGridGrADSTipoDadosFloat      = (server.attribute("dconte_grads_data_typeFloat", false) == "1"? true:false);
//                   configValues.ledGridGrADSMultiplicador      = server.attribute("dconre_grads_multiplier", configValues.ledGridGrADSMultiplicador);
//                   configValues.cmbGridGrADSByteOrder          = server.attribute("dconte_grads_swap", 0).toInt();
//                   configValues.spbGridGrADSNumBands           = server.attribute("dconin_grads_numbands", 0).toInt();
//                   configValues.spbGridGrADSTimeOffset         = server.attribute("dconin_grads_bandtimeoffset", 0).toInt();
//                   configValues.spbGridGrADSHeaderSize         = server.attribute("dconin_grads_bandheader", 0).toInt();
//                   configValues.spbGridGrADSTraillerSize       = server.attribute("dconin_grads_bandtrailler", 0).toInt();
//                   configValues.ledGridWCSDummy                = server.attribute("dconre_wcs_dummy", configValues.ledGridWCSDummy);

//                   configValues.projection_name                = server.attribute("projection_name", 0).toInt();
//                   configValues.projection_datum               = server.attribute("projection_datum", 0).toInt();
//                   configValues.projection_hemisphere          = server.attribute("projection_hemis", 0).toInt();
//                   configValues.projection_lat0                = server.attribute("projection_lat0", 0).toDouble();
//                   configValues.projection_lon0                = server.attribute("projection_lon0", 0).toDouble();
//                   configValues.projection_offx                = server.attribute("projection_offx", 0).toDouble();
//                   configValues.projection_offy                = server.attribute("projection_offy", 0).toDouble();
//                   configValues.projection_scale               = server.attribute("projection_scale", 0).toDouble();
//                   configValues.projection_stlat1              = server.attribute("projection_stlat1", 0).toDouble();
//                   configValues.projection_stlat2              = server.attribute("projection_stlat2", 0).toDouble();
//                   configValues.projection_units               = server.attribute("projection_units", configValues.projection_units);

//               }
//           }
//         }

//         node = node.nextSibling();
//     }

//     return configValues;
// }


// ConfigData::dataPoint ConfigData::loadDataPointConfig(QDomNode node){

//     ConfigData::dataPoint configValues;
//     // Iterar sobre os nós
//     while(!node.isNull())
//     {
//         QDomElement section = node.toElement(); // Tentar converter para um elememto
//         if(!section.isNull())
//         {
//           if(section.tagName() == "DataPointConfiguration")
//           {
//               QDomElement server = section.firstChildElement("configuration");
//               if(!server.isNull())
//               {

//                   configValues.pointFormatDataName             = server.attribute("dconvc_name", configValues.pointFormatDataName );
//                   configValues.pointFormatDataType             = server.attribute("dconte_type", 0).toInt();
//                   configValues.pointFormatDataRadius           = server.attribute("pointFormatDataRadius", configValues.pointFormatDataRadius);
//                   configValues.pointFormatDataFrequency        = server.attribute("dconre_update_freq_minutes", configValues.pointFormatDataFrequency);
//                   configValues.pointFormatDataDescription      = server.attribute("dconte_details", configValues.pointFormatDataDescription);
//                   configValues.pointFormatDataPath             = server.attribute("dconte_path", configValues.pointFormatDataPath);
//                   configValues.pointFormatDataMask             = server.attribute("dconte_mask", configValues.pointFormatDataMask);
//                   configValues.pointFormatDataFormat           = server.attribute("dconte_format", 0).toInt();
//                   configValues.pointFormatDataTimeZoneCmb      = server.attribute("dconte_timezone", 0).toInt();
//                   configValues.pointFormatDataPrefix           = server.attribute("dconte_prefix", configValues.pointFormatDataPrefix);
//                   configValues.pointFormatDataUnit             = server.attribute("dconte_unit", configValues.pointFormatDataUnit);

//                   configValues.projection_name                = server.attribute("projection_name", 0).toInt();
//                   configValues.projection_datum               = server.attribute("projection_datum", 0).toInt();
//                   configValues.projection_hemisphere          = server.attribute("projection_hemis", 0).toInt();
//                   configValues.projection_lat0                = server.attribute("projection_lat0", 0).toDouble();
//                   configValues.projection_lon0                = server.attribute("projection_lon0", 0).toDouble();
//                   configValues.projection_offx                = server.attribute("projection_offx", 0).toDouble();
//                   configValues.projection_offy                = server.attribute("projection_offy", 0).toDouble();
//                   configValues.projection_scale               = server.attribute("projection_scale", 0).toDouble();
//                   configValues.projection_stlat1              = server.attribute("projection_stlat1", 0).toDouble();
//                   configValues.projection_stlat2              = server.attribute("projection_stlat2", 0).toDouble();
//                   configValues.projection_units               = server.attribute("projection_units", configValues.projection_units);
//               }

//               ConfigData::pointParams pcd;
//               QDomElement params = section.firstChildElement("point");

//               while (!params.isNull())
//               {
//                   if(params.tagName() == "point")
//                   {
//                         pcd.fileName    = params.attribute("fileName", pcd.fileName);
//                         pcd.latitude    = params.attribute("latitude", pcd.latitude);
//                         pcd.longitude   = params.attribute("longitude", pcd.longitude);
//                         pcd.isActive    = (params.attribute("isActive",   false) == "1"? true:false);
//                         configValues.pt.push_back(pcd);
//                   }

//                   params = params.nextSibling().toElement();

//               }
//           }
//         }

//         node = node.nextSibling();
//     }

//     return configValues;
// }


// ConfigData::dataPointDiff ConfigData::loadDataPointDiffConfig(QDomNode node){

//     ConfigData::dataPointDiff configValues;
//     // Iterar sobre os nós
//     while(!node.isNull())
//     {
//         QDomElement section = node.toElement(); // Tentar converter para um elememto
//         if(!section.isNull())
//         {
//           if(section.tagName() == "DataPointDiffConfiguration"){

//               QDomElement server = section.firstChildElement("configuration");
//               if(!server.isNull())
//               {
//                   configValues.pointDiffFormatDataName        = server.attribute("dconvc_name", configValues.pointDiffFormatDataName );
//                   configValues.pointDiffFormatDataType        = server.attribute("dconte_type", 0).toInt();
//                   configValues.pointDiffFormatDataFrequency   = server.attribute("dconre_update_freq_minutes", configValues.pointDiffFormatDataFrequency);
//                   configValues.pointDiffFormatDataTimeZoneCmb = server.attribute("dconte_timezone", 0).toInt();
//                   configValues.pointDiffFormatDataPrefix      = server.attribute("dconte_prefix", configValues.pointDiffFormatDataPrefix);
//                   configValues.pointDiffFormatDataUnit        = server.attribute("dconte_unit", configValues.pointDiffFormatDataUnit);
//                   configValues.pointDiffFormatDataDescription = server.attribute("dconte_details", configValues.pointDiffFormatDataDescription);
//                   configValues.pointDiffFormatDataPath        = server.attribute("dconte_path", configValues.pointDiffFormatDataPath);
//                   configValues.pointDiffFormatDataMask        = server.attribute("dconte_mask", configValues.pointDiffFormatDataMask);
//                   configValues.pointDiffFormatDataFormat      = server.attribute("dconte_format", 0).toInt();

//                   configValues.projection_name                = server.attribute("projection_name", 0).toInt();
//                   configValues.projection_datum               = server.attribute("projection_datum", 0).toInt();
//                   configValues.projection_hemisphere          = server.attribute("projection_hemis", 0).toInt();
//                   configValues.projection_lat0                = server.attribute("projection_lat0", 0).toDouble();
//                   configValues.projection_lon0                = server.attribute("projection_lon0", 0).toDouble();
//                   configValues.projection_offx                = server.attribute("projection_offx", 0).toDouble();
//                   configValues.projection_offy                = server.attribute("projection_offy", 0).toDouble();
//                   configValues.projection_scale               = server.attribute("projection_scale", 0).toDouble();
//                   configValues.projection_stlat1              = server.attribute("projection_stlat1", 0).toDouble();
//                   configValues.projection_stlat2              = server.attribute("projection_stlat2", 0).toDouble();
//                   configValues.projection_units               = server.attribute("projection_units", configValues.projection_units);

//               }
//           }
//         }

//         node = node.nextSibling();
//     }

//     return configValues;
// }

