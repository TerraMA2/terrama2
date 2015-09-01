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
  \file terrama2/gui/config/MainDialogWeatherTab.hpp

  \brief Definition of Class MainDialogWeatherTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/


#ifndef _MAINDLG_WEATHER_TAB_H_
#define _MAINDLG_WEATHER_TAB_H_

// TerraMA2
#include "MainDialogTab.hpp"
#include "soapModAnaliseProxy.h"
#include "configData.h"

// QT  
#include <QtNetwork/QFtp>
#include <QTimer>
#include <QProgressDialog>


class RemoteWeatherServer;
class RemoteWeatherServerList;
class RemoteWeatherServerData;

class QNetworkReply;
class QNetworkAccessManager;


typedef struct
{
  int idSrv;
  int idDat;
  QTreeWidgetItem*	twiItem;
  QItemSelection slMod;
} STR_CTRLCHG;

/*!
\brief Classe responsável pelo tratamento da interface da ficha de
       dados climáticos da janela principal
*/      
class MainDialogWeatherTab : public MainDlgTab
{
Q_OBJECT

public:
  MainDialogWeatherTab(MainDialog* main_dialog, Services* manager);

  virtual ~MainDialogWeatherTab();

  virtual void load();
  virtual bool dataChanged();
  virtual bool validate(QString& err);
  virtual bool save();
  virtual void discardChanges(bool restore_data);

private slots:
  //void treeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
  void pcdInfPlaneClicked();
  void pcdWFSConfigurationClicked();
  void surfaceConfigDialogRequested();
  
  void cancelServerCheckConnection();
  void cancelOGCServerCheckConnection();
  void serverCheckConnectionClicked();
  void ftp_finished_command_slot(int, bool);
  void ftp_done_slot(bool);
  void timeoutSlot();
  void ogcTimeoutSlot();

  void filterClicked();
  void filterPointClicked();
  void intersectionClicked();
  void projectionDialogRequested();

  void setServerChanged();
  void setGridChanged();
  void setPointChanged();
  void setPointDiffChanged();

  void setGridStackDataFormat();
  void setPointStackDataFormat();
  void numGradsBandsChanged(int num);

  void pointInsertItemsFileNameLocation();
  void pointDeleteItemsFileNameLocation();
  void pointUpdateItemsFileNameLocation(QTableWidgetItem* item);
  void pointUpdateCollectionRuleLuaScript();

  void insertServerRequested();
  void insertPointRequested();
  void insertGridRequested();
  void removeServerRequested();
  void listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection);
  void removeFormatDataRequested();

  void insertPointDiffRequested();

  void menuMaskClick(QAction* actMenu);
  
  void pointThemeChanged(int index);
  void pointInfluenceChanged(int index);

  void activatePCDs();

  void replyFinished(QNetworkReply*);

  void importRemoteWeatherServerRequested();
  void exportRemoteWeatherServer();
  void updateRemoteWeatherServerFieldsRequested();


  void importDataFieldsRequested();

  void exportDataGridFields();
  void updateDataGridFieldsRequested();
  void updateDataPointFieldsRequested();
  void updateDataPointDiffFieldsRequested();

  void exportDataPointFields();
  void exportDataPointDiffFields();


private:
  //Verificacao de conexao com servidor FTP
  QFtp* _ftp;
  bool _checkConnection; //armazena valor "false" quando usuario cancela um teste de conexao
  QTimer _ftpTimeoutTimer;
  int _progressBarValue; 
  QProgressDialog* _progressDlg;

  //Verificacao de conexao com servidor OGC
  QNetworkAccessManager* _networkAccessManager;
  QTimer _ogcTimer;
  int _ogcProgressBarValue; 
  QProgressDialog* _ogcProgressDlg;

  bool _ignoreChangeEvents;
  bool _serverChanged;
  bool _newServer;
  bool _gridChanged;
  bool _newGrid;
  bool _pointChanged;
  bool _newPoint;
  bool _pointDiffChanged;
  bool _newPointDiff;
  bool back;

  STR_CTRLCHG	ctrlChg;
  STR_CTRLCHG	ctrlOld;

  wsProjectionParams _projection;
  wsWeatherDataSourceFilter _filter;
  wsWeatherDataSourceIntersection _intersection;
  wsGridOutputConfig _surfacesConfig;
  QVector<struct wsPCD> _collectionPoints;
  QString _pcdCollectionRuleLuaScript;

  QString _wfsPCDIdAttr;
  QString _wfsPCDTimestampAttr;
  QString _wfsPCDTimestampMask;

  QVector<std::string>_collectionAtributes;
  QVector<struct wsWeatherDataPcdSurface> _collectionSurfaces;

  struct wsAlertIcons _alertIcons;

  std::vector<struct wsRiskMapTheme> _pcdTheme;	// Lista de temas disponiveis para associar com uma PCD

  QDomNode openConfigurationFile(QString &configType);

  void updateRemoteWeatherServer(QDomNode node);
  void updateDataGridFields(QDomNode node);
  void updateDataPointFields(QDomNode node);
  void updateDataPointDiffFields(QDomNode node);

  bool saveServer();
  bool saveDataFormat(wsWeatherDataSourceGeometry geometryType, bool newData);
  bool saveGrid();
  bool savePoint();
  bool savePointDiff();  

  void clearChangeStruct();

  void clearServerChanged();
  void clearGridChanged();
  void clearPointChanged();
  void clearPointDiffChanged();

  void AuxInsertDataRequested(int type);
  void changeServerInterface();
  void changeGridInterface(bool isWCS);
  void changePointInterface(bool isWFS);

  void setServerFields(const RemoteWeatherServer* rws);
  void getServerFields(RemoteWeatherServer* rws);  

  void setDataFields(RemoteWeatherServerData* rwsd);
  void setDataGridFields(RemoteWeatherServerData* rwsd);
  void setDataPointFields(RemoteWeatherServerData* rwsd);
  void setDataPointDiffFields(RemoteWeatherServerData* rwsd);

  void getDataGridFields(RemoteWeatherServerData* rwsd, bool isWCS);
  void getDataPointFields(RemoteWeatherServerData* rwsd, bool isWFS);
  void getDataPointDiffFields(RemoteWeatherServerData* rwsd);


  void fillTableCollectionPoints(QTableWidget* tblColPoints, QPushButton* btnDel);

  void clearAllFields(bool clearlist);
  void clearServerFields(void);
  void clearFormatDataGridFields(void);
  void clearFormatDataPointFields(void);
  void clearFormatDataPointDiffFields(void);

  void showButtons(int index);

  void discardChangesNew(bool restore_data);
  void discardChangesUpdate(bool restore_data);

  bool auxDeleteItemsFileNameLocation(QTableWidget* tbl);
  bool auxInsUpdInsertItemsFileNameLocation(wsPCD& colPoint);
  bool auxUpdPcdCollectionRuleLuaScript(QString &luaScript);

  template <class T> void CopyVectorToQVector(const std::vector<T>& vect, QVector<T>& qvect);

  void fillComboFormat(QComboBox* combo, int type);
  wsWeatherDataSourceFileFormat indexToFileFormat(int index, int type);
  int fileFormatToIndex(wsWeatherDataSourceFileFormat value);
  
  wsWeatherDataPcdInfluenceType indexToPcdInfluenceType(int index);
  int pcdInfluenceTypeToIndex(wsWeatherDataPcdInfluenceType value);

  //QString indexToTimeZone(int index);
  int timeZoneToIndex(QString timezone);
  
  wsGrADSSwapType indexToGradsSwapType(int index);
  int gradsSwapTypeToIndex(wsGrADSSwapType value);
  
  void fillPointThemeCmb();
  void setThemeData(int themeId, QString attribute);

  QString verifyAndEnableChangeMsg();

  void finishOGCCheckConnection();

  RemoteWeatherServerList*    _remoteWeatherServerList;          //!< Referência para lista com as informações dos servidores remottos

  ConfigData::dataRemoteWeatherServer getRemoteWeatherServerExportFields();
  ConfigData::dataGrid getDataGridExportFields();
  ConfigData::dataPoint getDataPointExportFields();
  ConfigData::dataPointDiff getDataPointDiffExportFields();

  void setImportedRemoteWeatherServerFields(ConfigData::dataRemoteWeatherServer configValues);
  void setImportedDataGridFields(ConfigData::dataGrid configValues);
  void setImportedDataPointFields(ConfigData::dataPoint configValues);
  void setImportedDataPointDiffFields(ConfigData::dataPointDiff configValues);

  
signals:
  //! Chamado quando o usuário inicia a criação/edição de um servidor.
  void editServerStarted();
  
  //! Chamado quando o usuário termina a criação/edição de um servidor
  //! (confirmando ou cancelando).
  void editServerFinished();
  
  //! Chamado quando o usuário inicia a criação/edição de uma fonte de dados
  //! matriciais.
  void editGridStarted();
  
  //! Chamado quando o usuário termina a criação/edição de uma fonte de dados
  //! matriciais (confirmando ou cancelando).
  void editGridFinished();
  
  //! Chamado quando o usuário inicia a criação/edição de uma fonte de dados
  //! pontuais.
  void editPointStarted();
  
  //! Chamado quando o usuário termina a criação/edição de uma fonte de dados
  //! pontuais (confirmando ou cancelando).
  void editPointFinished();

  //! Chamado quando o usuário inicia a criação/edição de uma fonte de dados
  //! pontuais de diferentes localizacoes.
  void editPointDiffStarted();

  //! Chamado quando o usuário termina a criação/edição de uma fonte de dados
  //! pontuais de diferentes localizacoes (confirmando ou cancelando).
  void editPointDiffFinished();
};

#endif

