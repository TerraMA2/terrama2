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
//  * \file ConfigData.hpp
//  *
//  * \brief Definicao da classe ConfigData
//  *
//  * \author Carlos Augusto Teixeira Mendes
//  * \date march, 2009 
//  * \version $Id$
//  *
//  */
 
// #ifndef _CONFIG_DATA_H_
// #define _CONFIG_DATA_H_


// #include <QString>
// #include <QStringList>
// #include <QVector>
// #include <QFileInfo>
// #include <QtXml>

// class QDomDocument;
// class QDomElement;

// /*! \brief Classe que armazena os dados de configuração do sistema
// */      
// class ConfigData
// {
// public:

// 	//! Os elementos desta enumeração descrevem os possíveis drivers de conexão a uma base de dados.
// 	enum DatabaseDriver
// 	{
// 		DRIVER_PostGIS    = 0,  //!< Conexão a uma base PostgreSQL com a extensão PostGIS instalada.
// 		DRIVER_PostgreSQL = 1,  //!< Conexão a uma base PostgreSQL sem a extensão PostGIS instalada. 
// 		DRIVER_MySQL      = 2   //!< Conexão a uma base MySQL
// 	};

//   //! Dados relacionados com o servidor de base de dados
//   struct DataBaseServer
//   {
// 	  QString        _hostName;
// 	  int            _port;
// 	  QString        _user;
// 	  QString        _password;
// 	  QString        _dbName;
// 	  DatabaseDriver _driver;
// 	  bool           _study;
//   };
  
//   //! Dados globais comuns a todos os serviços
//   struct CommonData
//   {
//     int     _servicePort; //!< Porta utilizada para publicar serviço
//     QString _address;     //!< endereço de onde o serviço está rodando para o módulo de configuração
//     QString _logFile;     //!< Arquivo de log utilizado
//     QString _cmd;         //!< Comando utilizado para iniciar o módulo
//     QString _params;      //!< Parâmetros a serem passados para o comando
//   };
  
//   //! Dados relacionados com o módulo de análise
//   struct Analysis : public CommonData
//   {
//     int _interval;     //!< Intervalo em segundos entre cada execução das análises
//     int _numInstances; //!< Numero de instâncias desejadas do módulo de análises
//     QVector<CommonData> _instanceData;  //!< Informaçoes das instâncias adicionais
    
//     int _instanceId;   //!< Identificador da instância atual.  Usado pelo módulo de análise.  Não é lido ou salvo do arq. de config.
// 	QString _imageFilePath; //!< Diretorio onde as imagens geradas pelas analises serao guardadas.
//   };
  
//   //! Dados relacionados com o módulo de coleta
//   struct Collection : public CommonData
//   {
//     int _timeout;  //!< Intervalo máximo de espera em segundos por resposta de um servidor
// 	QString _dirPath; //!< Diretorio para salvar os dados brutos coletados (opcional)
//   };

//   //! Dados relacionados com o módulo de planos
//   struct Plans  : public CommonData
//   {
//   };

//   //! Dados relacionados com o módulo de animacao
//   struct Animation : public CommonData
//   {
// 	  QString _imageDirectory; //diretorio onde sao salvas as imagens
//   };

//   //! Dados relacionados com o módulo de notificações
//   struct Notification : public CommonData
//   {
//     int      _interval;    //!< Intervalo em segundos entre cada busca por novos alertas
//     QString  _smtpServer;  //!< Endereço do servidor SMTP
//     int      _smtpPort;    //!< Porta do servidor SMTP
// 	QString  _smtpMail;	   //!< Email do servidor SMTP
// 	QString  _smtpPsswrd;  //!< Senha do email do servidor de SMTP
// 	int		 _smtpTimeout; //!< Timeout para os comandos enviados ao servidor SMTP
// 	QString  _smtpAlias;   //!< Alias usado no envio de emails, para servidores internos
// 	bool     _smtpSSL;	   //!< Define o uso do protocolo SSL no envio de emails
// 	QString  _smtpPriority;//!< Define o envio de emails com prioridade alta
// 	QString  _terraPHPURL; //!< URL do endereço do site do TerraPHP
// 	QString  _proxyUser;   //!< Nome do usuário para acesso via proxy
// 	QString  _proxyPsswrd; //!< Senha do usuário para acesso via proxy
// 	bool     _sendUpEmail; //!< Define o envio de email, caso verdadeiro apenas quando subir o nível do alerta que o email será enviado
// 	int      _intervalBulletin;		//!< Intervalo em segundos entre cada busca por novos boletins
// 	bool	 _removeLevelZero;		//!< Caso verdadeiro remove da tabela a análise que possui todos os alertas em nível 0
// 	bool	 _bulletinPDF;			//!< Caso verdadeiro o boletim é enviado via PDF
// 	QString  _bulletinPath;			//!< Copia o boletim para um diretório específio, para que a aplicacao web possa consultar
// 	bool	 _readConfirmation;		//!< Caso verdadeiro pede confirmação de leitura
// 	bool	 _imagePDF;				//!< Caso verdadeiro a imagem é enviada via PDF

// 	QString  _legendPath;	//!< Diretorio que armazena as legendas
// 	int		 _xPositionLegend; //!< Posição X da legenda
// 	int		 _yPositionLegend; //!< Posição Y da legenda

// 	QStringList _levelTexts;
//   };

//   //! Dados relacionados ao módulo WEB
//   struct Preview
//   {
// 	  QString _title;       //!< Título da aplicação WEB
// 	  QString _logo;		//!< Endereço do logo que substituirá o TerraMA2
// 	  QString _longitude;	//!< Longitude do Centro inicial
// 	  QString _latitude;	//!< Latitude do Centro inicial
// 	  QString _zoomLevel;	//!< Nivel de zoom inicial
// 	  QString _googleKey;	//!< Chave do Google
// 	  QString _terraOGC;	//!< Endereço do servidor OGC
// 	  QString _opacidade;	//!< Valor da opacidade inicial
// 	  QString _intervalo;	//!< Intervalo de atualização
// 	  QString _outputPath;	//!< Diretório de saída
//   };

//   struct dataRemoteWeatherServer
//     {
//         QString     serverName;
//         QString     description;
//         QString     adress;
//         QString     port;
//         int         protocol;
//         QString     user;
//         QString     password;
//         //bool        active;
//         int         intervalMinutes;
//         QString     basePath;
//     };

//   struct dataGrid
//   {
//       QString   gridFormatDataName;
//       int       gridFormatDataType;
//       QString   gridFormatDataResolution;
//       QString   gridFormatDataFrequency;
//       QString   gridFormatDataDescription;
//       QString   gridFormatDataPath;
//       QString   gridFormatDataMask;
//       int       gridFormatDataFormat;
//       int       gridFormatDataTimeZoneCmb;
//       QString   gridFormatDataPrefix;
//       QString   gridFormatDataUnit;
//       QString   ledGridTIFFArqNavegacao;
//       bool      rbGridAscUnidGrausDec;
//       bool      rbGridAscUnidGrausMil;
//       QString   ledGridGrADSArqControle;
//       bool      rbGridGrADSTipoDadosInt;
//       bool      rbGridGrADSTipoDadosFloat;
//       QString   ledGridGrADSMultiplicador;
//       int       cmbGridGrADSByteOrder;
//       int       spbGridGrADSNumBands;
//       int       spbGridGrADSTimeOffset;
//       int       spbGridGrADSHeaderSize;
//       int       spbGridGrADSTraillerSize;
//       QString   ledGridWCSDummy;

//       int       projection_name;
//       int       projection_datum;
//       int       projection_hemisphere;
//       double    projection_lat0;
//       double    projection_lon0;
//       double    projection_offx;
//       double    projection_offy;
//       double    projection_scale;
//       double    projection_stlat1;
//       double    projection_stlat2;
//       QString   projection_units;
// /*
//       bool      filter_dateBeforeEnabled;
//       int       filter_dateBeforeYear;
//       int       filter_dateBeforeMonth;
//       int       filter_dateBeforeDay;
//       bool      filter_dateAfterEnabled;
//       int       filter_dateAfterYear;
//       int       filter_dateAfterMonth;
//       int       filter_dateAfterDay;
//       QString   filter_areaFilterType;
//       int       filter_xMin;
//       int       filter_xMax;
//       int       filter_yMin;
//       int       filter_yMax;
//       QString   filter_preAnalysisType;
//       int       filter_preAnalysisValue;
//       bool      filter_preAnalysisThemeEnabled;
//       QString   filter_bandFilterString;
//       bool      filter_useNewDummy;

//       QString   intersection_rasterIntersection;
//       QString   intersection_vectorIntersection;
//       QString   intersection_dataSourceIntersection;
// */
//   };

//   struct pointParams{
//       QString fileName;
//       QString latitude;
//       QString longitude;
//       bool    isActive;
//   };

//   struct dataPoint{

//     QString     pointFormatDataName;
//     int         pointFormatDataType;
//     QString     pointFormatDataRadius;
//     QString     pointFormatDataFrequency;
//     QString     pointFormatDataDescription;
//     QString     pointFormatDataPath;
//     QString     pointFormatDataMask;
//     QString     pointFormatDataPrefix;
//     QString     pointFormatDataUnit;
//     int         pointFormatDataTimeZoneCmb;
//     int         pointFormatDataFormat;
// /*
//     int         pointFormatDataInfluenceCmb;
//     int         pointFormatDataThemeCmb;
//     int         pointFormatDataAttributeCmb;
// */

//     std::vector<pointParams> pt;

//     int         projection_name;
//     int         projection_datum;
//     int         projection_hemisphere;
//     double      projection_lat0;
//     double      projection_lon0;
//     double      projection_offx;
//     double      projection_offy;
//     double      projection_scale;
//     double      projection_stlat1;
//     double      projection_stlat2;
//     QString     projection_units;

//   };

//   struct dataPointDiff{

//     QString pointDiffFormatDataName;
//     int     pointDiffFormatDataType;
//     QString pointDiffFormatDataFrequency;
//     QString pointDiffFormatDataDescription;
//     QString pointDiffFormatDataPath;
//     QString pointDiffFormatDataMask;
//     int     pointDiffFormatDataFormat;
//     int     pointDiffFormatDataTimeZoneCmb;
//     QString pointDiffFormatDataPrefix;
//     QString pointDiffFormatDataUnit;

//     int       projection_name;
//     int       projection_datum;
//     int       projection_hemisphere;
//     double    projection_lat0;
//     double    projection_lon0;
//     double    projection_offx;
//     double    projection_offy;
//     double    projection_scale;
//     double    projection_stlat1;
//     double    projection_stlat2;
//     QString   projection_units;

//   };


  
//   ConfigData();
//   ~ConfigData();
  
//   bool load(QString fileName, QString& err);

//   //! Retorna o arquivo XML e o tipo de arquivo
//   QDomNode loadConfig(QString fileName, QString& err, QString& configType);

//   //! Preenche structure com os dados do arquivo XML
//   dataRemoteWeatherServer loadRemoteWeatherServerConfig(QDomNode node);
//   dataGrid loadDataGridConfig(QDomNode node);
//   dataPoint loadDataPointConfig(QDomNode node);
//   dataPointDiff loadDataPointDiffConfig(QDomNode node);

//   bool save(QString fileName, QString& err) const;

//   //! Exporta os dados de configura��o armazenados em uma structure para XML
//   bool exportConfig(dataRemoteWeatherServer dados ,QString fileName);
//   bool exportConfig(dataGrid dados ,QString fileName);
//   bool exportConfig(dataPoint dados ,QString fileName);
//   bool exportConfig(dataPointDiff dados ,QString fileName);


//   //! Return the configuration file
//   QFileInfo configFile() { return _configFile; }
  
//   //! Retorna o nome da configuração
//   QString name() const { return _name; }
  
//   //! Seta o nome da configuração
//   void setName(QString name) { _name = name; }
  
//   //! Retorna referência para parâmetros de configuração da base de dados
//   DataBaseServer&       db()       { return _dbData; }
//   const DataBaseServer& db() const { return _dbData; }
  
//   //! Retorna referência para configuração do módulo de análise
//   Analysis&       analysis()       { return _analysisData; }
//   const Analysis& analysis() const { return _analysisData; }

//   //! Retorna referência para configuração do módulo de coleta
//   Collection&       collection()       { return _collectionData; }  
//   const Collection& collection() const { return _collectionData; }  

//   //! Retorna referência para configuração do módulo de planos
//   Plans&       plans()       { return _plansData; }  
//   const Plans& plans() const { return _plansData; } 

//   //! Retorna referencia para configuracao do modulo de animacao
//   Animation&       animation()       { return _animationData; }  
//   const Animation& animation() const { return _animationData; } 

//   //! Retorna referência para configuração do módulo de visualizacao
//   Preview&       preview()       { return _previewData; }  
//   const Preview& preview() const { return _previewData; }  

//   //! Retorna referência para configuração do módulo de notificação
//   Notification&       notification()       { return _notificationData; }  
//   const Notification& notification() const { return _notificationData; }  

//   void loadDefaults(int ninstances = 1);
 
//   static void getDefaultAnalysisCommands(QString& cmd, QString& param);
  
// private:
//   bool loadSection(QDomElement& section, DataBaseServer& data, QString& err);
//   bool loadSection(QDomElement& section, CommonData& data, QString& err);
//   bool loadSection(QDomElement& section, Analysis& data, QString& err);
//   bool loadSection(QDomElement& section, Collection& data, QString& err);
//   bool loadSection(QDomElement& section, Plans& data, QString& err);
//   bool loadSection(QDomElement& section, Animation& data, QString& err);
//   bool loadSection(QDomElement& section, Notification& data, QString& err);
//   bool loadSection(QDomElement& section, Preview& data, QString& err);

//   void saveSection(QDomDocument& doc, QDomElement& root, const DataBaseServer& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const CommonData& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const Analysis& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const Collection& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const Plans& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const Animation& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const Notification& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const Preview& data) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const dataRemoteWeatherServer& configValues ) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const dataGrid& configValues ) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const dataPoint &configValues ) const;
//   void saveSection(QDomDocument& doc, QDomElement& root, const dataPointDiff& configValues ) const;

//   bool parseDatabaseDriver(const QString& inStr, DatabaseDriver* outID);
//   QString getDatabaseDriver(DatabaseDriver id) const;

//   QFileInfo		 _configFile;		//!< Configuration file

//   QString        _name;             //!< Nome da configuração
//   DataBaseServer _dbData;           //!< Configuração da base de dados
//   Analysis       _analysisData;     //!< Configuração do módulo de análises 
//   Collection     _collectionData;   //!< Configuração do módulo de coleta
//   Plans          _plansData;        //!< Configuração do módulo de planos
//   Notification   _notificationData; //!< Configuração do módulo de notificações 
//   Animation		 _animationData;	//!< Configuração do módulo de animacao 
//   Preview		 _previewData;		//!< Configuração do módulo de visualização
// };

// #endif


