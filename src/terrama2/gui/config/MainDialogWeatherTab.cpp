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
  \file terrama2/gui/config/MainDialogWeatherTab.cpp

  \brief Definition of methods in class MainDialogWeatherTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>

// QT
#include <QMessageBox>
#include <QHeaderView>
#include <QDoubleValidator>
#include <QUrl>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileDialog>

// TerraMA2
#include "MainDialogWeatherTab.hpp"
#include "RemoteWeatherServerList.hpp"
#include "RemoteWeatherServerData.hpp"
#include "RemoteWeatherServerDataList.hpp"
#include "RiskMapList.hpp"
#include "WeatherGridList.hpp"
#include "CrossDialog.hpp"
#include "FilterDialog.hpp"
#include "ProjectionDialog.hpp"
#include "ColPointDialog.hpp"
#include "ColRuleDialog.hpp"
#include "Services.hpp"
#include "Utils.hpp"
#include "PcdInfPlaneDialog.hpp"
#include "PcdWFSAttrsDialog.hpp"
#include "DeleteWeatherDataSourceDialog.hpp"
#include "IntersectionDialog.hpp"
#include "SurfacesConfigDialog.hpp"

//! Construtor.  Prepara interface e estabelece conexões
MainDialogWeatherTab::MainDialogWeatherTab(MainDialog* main_dialog, Services* manager)
  : MainDialogTab(main_dialog, manager)
{
  _remoteWeatherServerList = NULL;
//  _currentTreeIndex   = -1;
  _serverChanged = false;
  _newServer     = false;
  _gridChanged	 = false;
  _newGrid		 = false;
  _pointChanged	 = false;
  _newPoint		 = false;
  _pointDiffChanged	 = false;
  _newPointDiff		 = false;

  clearChangeStruct();

  back = false;

  _ignoreChangeEvents = false;

  _networkAccessManager = NULL; // Sera criado apenas quando necessario (eh usado para verificar conexao com servidor OGC)

  // Prepara listas de tipos de formatos de arquivos
  fillComboFormat(_ui->gridFormatDataFormat, 0);
  fillComboFormat(_ui->pointFormatDataFormat, 1);

  setGridStackDataFormat();
  setPointStackDataFormat();
  numGradsBandsChanged(1);

  // Prepara árvore
  _ui->weatherDataTree->header()->hide();
  
  _ui->weatherDataTree->setCurrentItem(_ui->weatherDataTree->topLevelItem(0));
  _ui->weatherDataTree->setExpanded(_ui->weatherDataTree->model()->index(0, 0), true);
  
  connect(_ui->projectionGridBtn,  SIGNAL(clicked()),   SLOT(projectionDialogRequested()));
  connect(_ui->projectionPointBtn, SIGNAL(clicked()),   SLOT(projectionDialogRequested()));
  connect(_ui->projectionPointDiffBtn, SIGNAL(clicked()),   SLOT(projectionDialogRequested()));
  connect(_ui->filterGridBtn,      SIGNAL(clicked()),   SLOT(filterClicked()));
  connect(_ui->filterPointDiffBtn,      SIGNAL(clicked()),   SLOT(filterPointClicked()));
  connect(_ui->intersectionBtn,      SIGNAL(clicked()),   SLOT(intersectionClicked()));
  connect(_ui->btnPCDInformationPlane, SIGNAL(clicked()),   SLOT(pcdInfPlaneClicked()));
  connect(_ui->btnPCDWFSConfiguration, SIGNAL(clicked()),   SLOT(pcdWFSConfigurationClicked()));
  connect(_ui->pointFormatSurfaceConfigBtn, SIGNAL(clicked()), SLOT(surfaceConfigDialogRequested()));

  // Conecta sinais tratados pela classe
  // Para monitorar a mudança de regra na lista de regras de análise, estamos
  // usando um sinal do modelo de seleção e não o tradicional currentRowChanged()
  // Isso é feito pois em currentRowChanged() não conseguimos voltar para a seleção
  // anterior caso o usuário deseje cancelar a troca.
  connect(_ui->weatherDataTree->selectionModel(), 
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));

  // Conecta sinais para detectar dados alterados
  // Servidor Remoto
  connect(_ui->serverName,				  SIGNAL(textEdited(const QString&)), SLOT(setServerChanged()));
  connect(_ui->serverDescription,		SIGNAL(textChanged()), SLOT(setServerChanged()));
  connect(_ui->connectionAddress,		SIGNAL(textEdited(const QString&)), SLOT(setServerChanged()));
  connect(_ui->connectionPort,			SIGNAL(textEdited(const QString&)), SLOT(setServerChanged()));
  connect(_ui->connectionProtocol,		SIGNAL(currentIndexChanged(const QString&)), SLOT(setServerChanged()));
  connect(_ui->connectionUserName,		SIGNAL(textEdited(const QString&)), SLOT(setServerChanged()));
  connect(_ui->connectionPassword,		SIGNAL(textEdited(const QString&)), SLOT(setServerChanged()));
  connect(_ui->serverActiveServer,		SIGNAL(stateChanged(int)), SLOT(setServerChanged()));
  connect(_ui->serverIntervalData,		SIGNAL(valueChanged(const QString&)), SLOT(setServerChanged()));
  connect(_ui->serverDataBasePath,		SIGNAL(textEdited(const QString&)), SLOT(setServerChanged()));

  // Conecta sinais tratados de maneira generica por MainDlgTab
  // Servidor Remoto
  connect(_ui->saveBtn,			SIGNAL(clicked()), SLOT(saveRequested()));
  connect(_ui->cancelBtn,			SIGNAL(clicked()), SLOT(cancelRequested()));  

  // Conecta sinais para adicionar e remover Servidor remoto e Formatos de dado
  // MainTab
  connect(_ui->insertServerBtn,			SIGNAL(clicked()), SLOT(insertServerRequested()));


  // Servidor Remoto
  connect(_ui->serverInsertPointBtn,	SIGNAL(clicked()), SLOT(insertPointRequested()));
  connect(_ui->serverInsertGridBtn,		SIGNAL(clicked()), SLOT(insertGridRequested()));
  connect(_ui->serverDeleteBtn,			SIGNAL(clicked()), SLOT(removeServerRequested()));
  // Formato de Dados (grid e point)
  connect(_ui->gridFormatDataDeleteBtn,	SIGNAL(clicked()), SLOT(removeFormatDataRequested()));
  connect(_ui->pointFormatDataDeleteBtn,SIGNAL(clicked()), SLOT(removeFormatDataRequested()));

  // Formato de pontos de diferentes localizacoes
  connect(_ui->serverInsertPointDiffBtn,		SIGNAL(clicked()), SLOT(insertPointDiffRequested()));
  connect(_ui->serverRemovePointDiffBtn,		SIGNAL(clicked()), SLOT(removeFormatDataRequested()));

  // Conecta sinais para detectar dados alterados
  // Formato de Dados (tipo Grid)
  connect(_ui->gridFormatDataName,			SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataType,			SIGNAL(currentIndexChanged(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataResolution,	SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataFrequency,		SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataDescription,	SIGNAL(textChanged()), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataPath,			SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataMask,			SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataFormat,		SIGNAL(currentIndexChanged(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataPrefix,		SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataUnit,          SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->gridFormatDataTimeZoneCmb,	SIGNAL(currentIndexChanged(int)), SLOT(setGridChanged()));

  connect(_ui->rbGridAscUnidGrausDec,       SIGNAL(toggled(bool)), SLOT(setGridChanged()));
  connect(_ui->rbGridAscUnidGrausMil,       SIGNAL(toggled(bool)), SLOT(setGridChanged()));
  connect(_ui->rbGridGrADSTipoDadosInt,     SIGNAL(toggled(bool)), SLOT(setGridChanged()));
  connect(_ui->rbGridGrADSTipoDadosFloat,   SIGNAL(toggled(bool)), SLOT(setGridChanged()));
  connect(_ui->ledGridGrADSArqControle,	    SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->ledGridGrADSMultiplicador,   SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));
  connect(_ui->cmbGridGrADSByteOrder,       SIGNAL(currentIndexChanged(const QString&)), SLOT(setGridChanged()));
  connect(_ui->spbGridGrADSNumBands,        SIGNAL(valueChanged(int)), SLOT(setGridChanged()));
  connect(_ui->spbGridGrADSTimeOffset,      SIGNAL(valueChanged(int)), SLOT(setGridChanged()));
  connect(_ui->spbGridGrADSHeaderSize,      SIGNAL(valueChanged(int)), SLOT(setGridChanged()));
  connect(_ui->spbGridGrADSTraillerSize,    SIGNAL(valueChanged(int)), SLOT(setGridChanged()));
  
  connect(_ui->ledGridTIFFArqNavegacao,	    SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));

  // Formato de Dados (tipo OGC WCS)
  connect(_ui->ledGridWCSDummy,				SIGNAL(textEdited(const QString&)), SLOT(setGridChanged()));

  // Trata mudanca do tipo de formato de dados (muda para stack correspondente - Grid e Point)
  connect(_ui->gridFormatDataFormat,		SIGNAL(currentIndexChanged(const QString&)), SLOT(setGridStackDataFormat()));
  connect(_ui->pointFormatDataFormat,		SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointStackDataFormat()));

  // Trata mudança no número de bandas
  connect(_ui->spbGridGrADSNumBands, SIGNAL(valueChanged(int)), SLOT(numGradsBandsChanged(int)));

  // Conecta botoes de inclusao/exclusao do itemsFileNameLocation (Point)
  connect(_ui->btnPointPCDInsertFileNameLocation,  SIGNAL(clicked(bool)), SLOT(pointInsertItemsFileNameLocation()));
  connect(_ui->btnPointPCDDeleteFileNameLocation,  SIGNAL(clicked(bool)), SLOT(pointDeleteItemsFileNameLocation()));

  // Conecta botoes de edição do collection rule Lua script (Point)
  connect(_ui->btnUpdatePcdCollectionRule,  SIGNAL(clicked(bool)), SLOT(pointUpdateCollectionRuleLuaScript()));
  
  // Conecta sinais para detectar dados alterados
  // Formato de Dados (tipo Point)
  connect(_ui->pointFormatDataName,			SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataType,			SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataRadius,		SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataFrequency,	SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataDescription,	SIGNAL(textChanged()), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataPath,			SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataMask,			SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataFormat,		SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataInfluenceCmb, SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataThemeCmb,     SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataAttributeCmb, SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataPrefix,		SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataUnit,		SIGNAL(textEdited(const QString&)), SLOT(setPointChanged()));
  connect(_ui->pointFormatDataTimeZoneCmb,	SIGNAL(currentIndexChanged(int)), SLOT(setPointChanged()));

  connect(_ui->pointFormatDataInfluenceCmb,	SIGNAL(currentIndexChanged(int)), SLOT(pointInfluenceChanged(int)));
  connect(_ui->pointFormatDataThemeCmb,     SIGNAL(currentIndexChanged(int)), SLOT(pointThemeChanged(int)));

  // Conecta sinais para detectar dados alterados
  // Formato de Dados (tipo point diff)
  connect(_ui->pointDiffFormatDataName,			SIGNAL(textEdited(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataType,			SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataFrequency,		SIGNAL(textEdited(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataDescription,	SIGNAL(textChanged()), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataPath,			SIGNAL(textEdited(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataMask,			SIGNAL(textEdited(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataFormat,		SIGNAL(currentIndexChanged(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataPrefix,		SIGNAL(textEdited(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataUnit,		SIGNAL(textEdited(const QString&)), SLOT(setPointDiffChanged()));
  connect(_ui->pointDiffFormatDataTimeZoneCmb,	SIGNAL(currentIndexChanged(int)), SLOT(setPointDiffChanged()));

  // Define os tipos para validação (grid)
  _ui->gridFormatDataFrequency->setValidator(new QDoubleValidator(_ui->gridFormatDataFrequency));
  _ui->gridFormatDataResolution->setValidator(new QDoubleValidator(_ui->gridFormatDataResolution));
  _ui->ledGridGrADSMultiplicador->setValidator(new QDoubleValidator(_ui->ledGridGrADSMultiplicador));

  // Define os tipos para validação (OGC WCS)
  _ui->ledGridWCSDummy->setValidator(new QDoubleValidator(_ui->ledGridWCSDummy));

  // Define os tipos para validação (point)
  _ui->pointFormatDataFrequency->setValidator(new QDoubleValidator(_ui->pointFormatDataFrequency));
  _ui->pointFormatDataRadius->setValidator(new QDoubleValidator(_ui->pointFormatDataRadius));

  // Trata duplo clique na tabela de collectionPoint (point)
  connect(_ui->tblPointPCDFileNameLocation,	SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(pointUpdateItemsFileNameLocation(QTableWidgetItem*)));

  // Trata a ativacao de PCDs pelo clique com botao direito
  connect(_ui->tblPointPCDFileNameLocation,	SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(activatePCDs()));

  connect(_ui->serverCheckConnectionBtn, SIGNAL(clicked()), SLOT(serverCheckConnectionClicked()));
  _ftp = new QFtp();
  connect(_ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftp_finished_command_slot(int, bool)));
  connect(_ftp, SIGNAL(done(bool)), this, SLOT(ftp_done_slot(bool)));
  connect(&_ftpTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
  connect(&_ogcTimer, SIGNAL(timeout()), this, SLOT(ogcTimeoutSlot()));

  // Menus associados a botões
  QMenu* menuMask = new QMenu(tr("Máscaras"), _parent);
  menuMask->addAction(tr("%a - ano com dois digitos"));
  menuMask->addAction(tr("%A - ano com quatro digitos"));
  menuMask->addAction(tr("%d - dia com dois digitos"));
  menuMask->addAction(tr("%M - mes com dois digitos"));
  menuMask->addAction(tr("%h - hora com dois digitos"));
  menuMask->addAction(tr("%m - minuto com dois digitos"));
  menuMask->addAction(tr("%s - segundo com dois digitos"));
  menuMask->addAction(tr("%. - um caracter qualquer"));

  _ui->fileGridMaskBtn->setMenu(menuMask);
  _ui->fileGridMaskBtn->setPopupMode(QToolButton::InstantPopup);
  _ui->filePointMaskBtn->setMenu(menuMask);
  _ui->filePointMaskBtn->setPopupMode(QToolButton::InstantPopup);
  _ui->filePointDiffMaskBtn->setMenu(menuMask);
  _ui->filePointDiffMaskBtn->setPopupMode(QToolButton::InstantPopup);

  // Conecta o clique do menu com a ação de incluir a mascara no edit
  connect(menuMask,		SIGNAL(triggered(QAction*)), SLOT(menuMaskClick(QAction*)));

  _ui->weatherPageStack->setCurrentIndex(0);

  // Sinais de botões importar/exportar/atualizar dados
  connect(_ui->exportServerBtn, SIGNAL(clicked()), SLOT(exportRemoteWeatherServer()));
  connect(_ui->updateServerBtn, SIGNAL(clicked()), SLOT(updateRemoteWeatherServerFieldsRequested()));
  connect(_ui->importServerBtn,			SIGNAL(clicked()), SLOT(importRemoteWeatherServerRequested()));

  connect(_ui->importDataBtn, SIGNAL(clicked()), SLOT(importDataFieldsRequested()));

  connect(_ui->exportDataGridBtn, SIGNAL(clicked()), SLOT(exportDataGridFields()));
  connect(_ui->updateDataGridBtn, SIGNAL(clicked()), SLOT(updateDataGridFieldsRequested()));

  connect(_ui->exportDataPointBtn, SIGNAL(clicked()), SLOT(exportDataPointFields()));
  connect(_ui->updateDataPointBtn, SIGNAL(clicked()), SLOT(updateDataPointFieldsRequested()));

  connect(_ui->exportDataPointDiffBtn, SIGNAL(clicked()), SLOT(exportDataPointDiffFields()));
  connect(_ui->updateDataPointDiffBtn, SIGNAL(clicked()), SLOT(updateDataPointDiffFieldsRequested()));

}

//! Destrutor
MainDialogWeatherTab::~MainDialogWeatherTab()
{
}

/*! Retorna o tipo de arquivo relacionado a um index da combobox

\param index Indice do elemento da lista
\param type Tipo de dado: 0 = Grade, 1 = Pontos, 2 = Pontos Diferente Localizacao
*/
wsWeatherDataSourceFileFormat MainDialogWeatherTab::indexToFileFormat(int index, int type)
{
  if(type == 0)  // Grid. Lista = Ascii, TIFF, Grads
  {
    switch(index)
    {
      case 0: return WS_WDSFFMT_ASCII_Grid;
      case 1: return WS_WDSFFMT_TIFF;
      case 2: return WS_WDSFFMT_GrADS;
   }
  }
  else if(type == 1)  // Ponto. Lista = PCD
  {
    switch(index)
	{
	  case 0: return WS_WDSFFMT_PCD;
	  case 1: return WS_WDSFFMT_Instrument;
	}
  }
  else if(type == 2)  // Ponto. Diferente localizacao
  {
	return WS_WDSFFMT_PointPROARCO;
  }
  
  assert(0); // Nunca deve chegar aqui pois deve sempre ser um dos casos acima  
  return WS_WDSFFMT_ASCII_Grid;
}

//! Retorna o indice na combobox de um tipo de arquivo
int MainDialogWeatherTab::fileFormatToIndex(wsWeatherDataSourceFileFormat value)
{
  switch(value)
  {
    // Grids
    case WS_WDSFFMT_ASCII_Grid: return 0;
    case WS_WDSFFMT_TIFF:       return 1;
    case WS_WDSFFMT_GrADS:      return 2;
	// Grid OGC WCS
	case WS_WDSFFMT_OGC_WCS: return 0;
	case WS_WDSFFMT_PCD_OGC_WFS: return 0;
    // Pontos
    case WS_WDSFFMT_PCD: return 0;
	case WS_WDSFFMT_Instrument: return 1;
	// Pontos Diferente localizacao
	case WS_WDSFFMT_PointPROARCO: return 0;
  }
  assert(0);  // Todos os casos cobertos acima
  return -1;
}

/*! Retorna o tipo de influência relacionado a um index da combobox

\param index Indice do elemento da lista
*/
wsWeatherDataPcdInfluenceType MainDialogWeatherTab::indexToPcdInfluenceType(int index)
{
  switch(index)
  {
    case 0: return WS_WDPIT_RADIUS_TOUCH;
    case 1: return WS_WDPIT_RADIUS_CENTER;
    case 2: return WS_WDPIT_THEME;
 }
  
  assert(0); // Nunca deve chegar aqui pois deve sempre ser um dos casos acima  
  return WS_WDPIT_RADIUS_TOUCH;
}

//! Retorna o indice na combobox de um tipo de arquivo
int MainDialogWeatherTab::pcdInfluenceTypeToIndex(wsWeatherDataPcdInfluenceType value)
{
  switch(value)
  {
    // Grids
    case WS_WDPIT_RADIUS_TOUCH:  return 0;
    case WS_WDPIT_RADIUS_CENTER: return 1;
    case WS_WDPIT_THEME:         return 2;
  }
  assert(0);  // Todos os casos cobertos acima
  return -1;
}

/*! Retorna o tipo de ordem de byte relacionado a um index da combobox

\param index Indice do elemento da lista
*/
wsGrADSSwapType MainDialogWeatherTab::indexToGradsSwapType(int index)
{
  switch(index)
  {
    case 0: return WS_GRADSSWAPTYPE_AUTO;
    case 1: return WS_GRADSSWAPTYPE_LITTLE_ENDIAN;
    case 2: return WS_GRADSSWAPTYPE_BIG_ENDIAN;
 }
  
  assert(0); // Nunca deve chegar aqui pois deve sempre ser um dos casos acima  
  return WS_GRADSSWAPTYPE_AUTO;
}

//! Retorna o indice na combobox de um tipo de ordenação de bytes
int MainDialogWeatherTab::gradsSwapTypeToIndex(wsGrADSSwapType value)
{
  switch(value)
  {
    // Grids
    case WS_GRADSSWAPTYPE_AUTO:          return 0;
    case WS_GRADSSWAPTYPE_LITTLE_ENDIAN: return 1;
    case WS_GRADSSWAPTYPE_BIG_ENDIAN:    return 2;
  }
  assert(0);  // Todos os casos cobertos acima
  return -1;
}

//! Essa função retorna o índice da timezone dentro das combos.
int MainDialogWeatherTab::timeZoneToIndex(QString timezone)
{
	//  Como dentro da combo os tz estão organizados de +12:00 até
	//  -12:00, a conta abaixo pega uma timezone nesse formato e pega
	//  o índice
	return 12 - timezone.left(3).toInt();
}

/*! \brief Preenche combo com descricao dos formatos (Grid/Point)

\param combo Combobox a ser preenchida
\param type Tipo de dado: 0 = Grade, 1 = Pontos, 2 = Pontos Diferente Localizacao
*/
void MainDialogWeatherTab::fillComboFormat(QComboBox* combo, int type)
{
  combo->clear();
  if(type == 0) // Grids
  {
    QStringList items;
    items << "Asc-GRID" << "TIFF" << "GrADS";
    combo->addItems(items);
  }
  else if (type == 1)// Pontos
  {
    combo->addItem("PCD");
	//combo->addItem(tr("Instrumento"));
  }
  else if (type == 2)// Pontos Diferentes Localizacao
	combo->addItem("Points");
}


/*! Preenche combobox com os temas disponíveis para associação com PCDs
*/
void MainDialogWeatherTab::fillPointThemeCmb()
{
  _ignoreChangeEvents = true;
  _pcdTheme = _manager->riskMapList()->getRiskMapThemes();
  _ui->pointFormatDataThemeCmb->clear();
  _ui->pointFormatDataThemeCmb->addItem(tr("Selecione o tema c/ a área de infl."));
  for(unsigned i=0; i<_pcdTheme.size(); i++)
	  _ui->pointFormatDataThemeCmb->addItem(QString::fromStdString(_pcdTheme.at(i).name));
	_ui->pointFormatDataThemeCmb->setCurrentIndex(0);  
	_ignoreChangeEvents = false;
}

//! Slot chamado quando o tema selecionado para uma PCD mudou. Popula lista de atributos do tema
void MainDialogWeatherTab::pointThemeChanged(int index)
{
  _ui->pointFormatDataAttributeCmb->clear();
  _ui->pointFormatDataAttributeCmb->addItem(tr("Selecione a coluna de ident. das PCDs"));

  if(index > 0) // usuario selecionou um tema
  {
    struct wsRiskMapTheme& themeData = _pcdTheme.at(index -1);
    for(unsigned i=0; i<themeData.attributes.size(); i++)
    {
      QString name = QString::fromStdString(themeData.attributes[i].name + "  (" + 
                                            Utils::columnTypeToString(themeData.attributes[i].type).toStdString() + ")");
	    _ui->pointFormatDataAttributeCmb->addItem(name);
	  }  
  }  
  
	_ui->pointFormatDataAttributeCmb->setCurrentIndex(0);  
}

//! Slota chamado quando o tipo de area de influencia para uma PCD mudou.
void MainDialogWeatherTab::pointInfluenceChanged(int index)
{
  if(indexToPcdInfluenceType(index) == WS_WDPIT_THEME)
    _ui->pointFormatInfluenceStack->setCurrentIndex(1);
  else
    _ui->pointFormatInfluenceStack->setCurrentIndex(0);
}

//! Rotina auxiliar para setar o elemento corrente das combos de tema e atributo para uma PCD
void MainDialogWeatherTab::setThemeData(int themeId, QString attribute)
{
  // Procura pelo tema
  for(unsigned i=0; i<_pcdTheme.size(); i++)
  {
    if(_pcdTheme[i].id == themeId)
    {
      // Achou.  Seta entrada do combo de tema
      _ui->pointFormatDataThemeCmb->setCurrentIndex(i+1);  // + 1 por causa da entrada inicial de nenhum tema selecionado
      
      // Agora procura qual o atributo
      std::string attr = attribute.toStdString();
      for(unsigned j=0; j<_pcdTheme[i].attributes.size(); j++)
      {
        if(_pcdTheme[i].attributes[j].name == attr)
        {
          // Achou
          _ui->pointFormatDataAttributeCmb->setCurrentIndex(j+1); // + 1 por causa da entrada inicial de nenhum atributo selecionado
          return;
        }  
      }
      // Não achou o atributo. Combo já foi inicializada com indice 0 em pointThemeChanged
      return;
    }  
  }
  // Tema não encontrado.  Seta ambas as combos para indice 0 ( a de atributos via pointThemeChanged)
  _ui->pointFormatDataThemeCmb->setCurrentIndex(0);
}

//Slot chamado para ativar/desativar PCDs
void MainDialogWeatherTab::activatePCDs()
{
	QList<QTableWidgetItem*> selectedItems = _ui->tblPointPCDFileNameLocation->selectedItems();
	QTableWidgetItem* item;
	wsPCD colPoint;

	QMessageBox::StandardButton answer;
	answer = QMessageBox::question(_parent, tr("PCDs"), 
									tr("As PCDs selecionadas devem estar ativas?"),
									QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
									QMessageBox::No);

	if(answer != QMessageBox::Cancel && selectedItems.size() > 0)
	{
		for(int i=0; i<selectedItems.size(); i++)
		{
			item = selectedItems.at(i);
			if(item->column() != 0)
				continue;

			colPoint = _collectionPoints.at(item->row());
			colPoint.isActive = (answer == QMessageBox::Yes ? true : false);
			_collectionPoints.replace(item->row(), colPoint);
		}

		fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
		setPointChanged();
	}
}


//! Rotina auxiliar para tratar exclusao do itemsFileNameLocation (Grid/Point)
bool MainDialogWeatherTab::auxDeleteItemsFileNameLocation(QTableWidget* tbl)
{
	bool ret = false;

	QList<QTableWidgetItem*> selectedItems = tbl->selectedItems();

	QList<int> itemsToRemove;
	QTableWidgetItem* item;

	if (selectedItems.size() > 0)
	{
		ret = true;
		itemsToRemove.clear();

		for(int i=0; i<selectedItems.size(); i++)
		{
			item = selectedItems.at(i);
			if(item->column() != 0)
				continue;

			itemsToRemove.push_front(item->row()); //guarda o numero da linha a ser removida
		}

		qSort(itemsToRemove.begin(), itemsToRemove.end()); //armazenar numeros das linhas a serem removidas em ordem crescente
														   //para nao ter problemas (acessar indice invalido) na hora de deletar

		for(int i = 0; i < itemsToRemove.size();) //nao precisa incrementar variavel i (está sendo usado takeLast())
			_collectionPoints.remove(itemsToRemove.takeLast());
	}

	return ret;
}

//! Rotina auxiliar para tratar inclusao/alteracao do itemsFileNameLocation (Grid/Point)
bool MainDialogWeatherTab::auxInsUpdInsertItemsFileNameLocation(wsPCD& colPoint)
{
  ColPointDlg dlg;
  bool changed = false;

  dlg.setFields(colPoint);

  if (dlg.exec()==QDialog::Accepted)		// else QDialog::Rejected
    dlg.getFields(&colPoint, changed);

  return changed;
}

//! Rotina auxiliar para tratar alteração da regra de coleta (Point)
bool MainDialogWeatherTab::auxUpdPcdCollectionRuleLuaScript(QString & luaScript)
{
	ColRuleDlg dlg;
	bool changed = false;
	
	dlg.setFields(luaScript);
	
	if (dlg.exec()==QDialog::Accepted)		// else QDialog::Rejected
		dlg.getFields(&luaScript, changed);
	
	return changed;
}

//! Trata inclusao do itemsFileNameLocation (Point)
void MainDialogWeatherTab::pointInsertItemsFileNameLocation()
{
  wsPCD newColPoint;

  newColPoint.fileName  = "";
  newColPoint.latitude  = 0;
  newColPoint.longitude = 0;

  if (auxInsUpdInsertItemsFileNameLocation(newColPoint))
  {
    _collectionPoints.push_back(newColPoint);
    fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
    setPointChanged();
  }
}

//! Trata edição do Lua script da regra de coleta (Point)
void MainDialogWeatherTab::pointUpdateCollectionRuleLuaScript()
{
	QString luaScript = _pcdCollectionRuleLuaScript;
	
	if (auxUpdPcdCollectionRuleLuaScript(luaScript))
	{
		_pcdCollectionRuleLuaScript = luaScript;
		setPointChanged();
	}
}

//! Trata atualização do itemsFileNameLocation (Point)
void MainDialogWeatherTab::pointUpdateItemsFileNameLocation(QTableWidgetItem* item)
{
  if (item != NULL)
  {
	  wsPCD colPoint = _collectionPoints.at(item->row());

    if (auxInsUpdInsertItemsFileNameLocation(colPoint))
    {
	    _collectionPoints.replace(item->row(), colPoint);
      fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
      setPointChanged();
	  }
  }
}

//! Trata exclusao do itemsFileNameLocation (Point)
void MainDialogWeatherTab::pointDeleteItemsFileNameLocation()
{
  if (auxDeleteItemsFileNameLocation(_ui->tblPointPCDFileNameLocation))
  {
    fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
    setPointChanged();
  }
}

//! Trata mudanca do tipo de formato de dados (muda para stack correspondente - Grid)
void MainDialogWeatherTab::setGridStackDataFormat()
{
  QWidget* pgActivate = NULL;
  switch(indexToFileFormat(_ui->gridFormatDataFormat->currentIndex(), 0))
  {
    case WS_WDSFFMT_ASCII_Grid: pgActivate = _ui->pgGridAscGRID; break;
    case WS_WDSFFMT_TIFF:   	  pgActivate = _ui->pgGridTIFF; 	 break;
    case WS_WDSFFMT_GrADS:  	  pgActivate = _ui->pgGridGrADS;   break;
  }

  if(pgActivate == NULL)
    _ui->stkwGridTipoFormato->setVisible(false);
  else
  {
    _ui->stkwGridTipoFormato->setVisible(true);
	  _ui->stkwGridTipoFormato->setCurrentWidget(pgActivate);
  }
}

//! Trata mudanca do tipo de formato de dados (muda para stack correspondente - Point)
void MainDialogWeatherTab::setPointStackDataFormat()
{
  // Preparado para inclusao de nvos tipos...
  switch(indexToFileFormat(_ui->pointFormatDataFormat->currentIndex(), 1))
  {
    case WS_WDSFFMT_PCD:  
      _ui->pointFormatDataMask->setEnabled(false);
      _ui->filePointMaskBtn->setEnabled(false);
      _ui->lblPointMask->setEnabled(false);
	  _ui->btnUpdatePcdCollectionRule->setEnabled(true);
      break;
	case WS_WDSFFMT_Instrument:
	  _ui->btnUpdatePcdCollectionRule->setEnabled(false);
		break;
  }
}

// Função chamada quando o número de bandas de um arquivo Gradas é alterado na interface 
void MainDialogWeatherTab::numGradsBandsChanged(int num)
{
  bool enabled = (num > 1);

  _ui->spbGridGrADSTimeOffset->setEnabled(enabled);
/*  
  _ui->spbGridGrADSHeaderSize->setEnabled(enabled);
  _ui->spbGridGrADSTraillerSize->setEnabled(enabled);
*/  
}


// Funcao comentada na classe base
void MainDialogWeatherTab::load()
{
  // Carrega novas informações
  _remoteWeatherServerList	= _manager->remoteWeatherServerList();

  assert(_remoteWeatherServerList);

  fillPointThemeCmb();

  // Limpa dados
  _ui->weatherDataTree->clear();

  QTreeWidgetItem* mainChild = new QTreeWidgetItem;

  mainChild->setText(0, tr("Servidores remotos"));
  mainChild->setIcon(0, QIcon(":/data/icons/servers.png"));
  _ui->weatherDataTree->addTopLevelItem(mainChild);
  _ui->weatherDataTree->setCurrentItem(mainChild);
  _ui->weatherDataTree->currentItem()->setExpanded(true);

  clearAllFields(true);

  // Preenche a lista de servidores e forato de dados.
  // já foi incluida e selecionda por clearFields()
  if(_remoteWeatherServerList->count()) 
  {
    for(int i=0, count=(int)_remoteWeatherServerList->count(); i<count; i++)
    {
		RemoteWeatherServer* map = _remoteWeatherServerList->at(i);
		QTreeWidgetItem* item = new QTreeWidgetItem;

		item->setText(0, map->serverName());
		item->setIcon(0, QIcon(":/data/icons/server.png"));
		item->setData(0, Qt::UserRole, map->id()+1);

		RemoteWeatherServerDataList* remoteWeatherServerDataList = map->getRemoteWeatherServerDataList();

		if (remoteWeatherServerDataList != NULL)
		{
		    for(int j=0; j < (int) remoteWeatherServerDataList->count(); j++)
			{
				RemoteWeatherServerData* data = remoteWeatherServerDataList->at(j);
				QTreeWidgetItem* child = new QTreeWidgetItem;

				child->setText(0, data->name());


				if (data->geometry()==WS_WDSGEOM_RASTER)   //  WS_WDSGEOM_POINTS, WS_WDSGEOM_RASTER, WS_WDSGEOM_POINTSDIFF
					child->setIcon(0, QIcon(":/data/icons/dado_grid.png"));
				else if (data->geometry()==WS_WDSGEOM_POINTS)
					child->setIcon(0, QIcon(":/data/icons/dado_pontual.png"));
				else if (data->geometry()==WS_WDSGEOM_POINTSDIFF)
					child->setIcon(0, QIcon(":/data/icons/dado_pontual_diferente.png"));

				child->setData(0, Qt::UserRole, -(data->id()+1) );

				item->addChild(child);
			}
		}

		_ui->weatherDataTree->currentItem()->addChild(item );
    }
  }
}

// Funcao comentada na classe base
bool MainDialogWeatherTab::dataChanged()
{
  return (_serverChanged || _gridChanged || _pointChanged || _pointDiffChanged);
}

// Funcao comentada na classe base
bool MainDialogWeatherTab::validate(QString& err)
{
  // Nome do mapa é obrigatório
  if(_serverChanged)
  {
	  if(_ui->serverName->text().trimmed().isEmpty())
	  {
	  	err = tr("Nome do servidor remoto não foi preenchido!");
		  return false;
	  }
  }
  else if(_gridChanged)
  {
	  if(_ui->gridFormatDataName->text().trimmed().isEmpty())
	  {
		  err = tr("Nome do formato de dados não foi preenchido!");
		  return false;
	  }
	
	  if(_projection.name == WS_PROJNAME_NoProjection)
	  {
		  err = tr("É necessário definir uma projeção!");
		  return false;
	  }

	  if(_filter.bandFilterString != "")
	  {
		  QString bandFilter(_filter.bandFilterString.c_str());
		  //Divide o filtro pelos intervalos
		  QStringList bandFilters = bandFilter.split(",", QString::SkipEmptyParts);

		  QString filter;
		  int number1, number2;

		  for(int i=0; i<bandFilters.size(); ++i)
		  {
			  filter = bandFilters.at(i);
			  QStringList numbers = filter.split("-", QString::SkipEmptyParts);
			  if(numbers.size() > 1)
			  {
				  //Verificar se o intervalo informado é valido. Ex.: intervalo 15-10 é invalido.
				  number1 = numbers.at(0).toInt();
				  number2 = numbers.at(1).toInt();
				  if(number1 >= number2)
				  {
					  err = tr("O intervalo %1 informado para filtro por bandas é inválido!").arg(filter);
					  return false;
				  }
			  }
		  }

	  }
  }
  else if(_pointChanged)
  {
	  if(_ui->pointFormatDataName->text().trimmed().isEmpty())
	  {
	  	err = tr("Nome do formato de dados não foi preenchido!");
		  return false;
	  }
	
	  if(_projection.name == WS_PROJNAME_NoProjection)
	  {
	  	err = tr("É necessário definir uma projeção!");
	  	return false;
	  }
	  
	  if(indexToPcdInfluenceType(_ui->pointFormatDataInfluenceCmb->currentIndex()) == WS_WDPIT_THEME) // Região
	  {
	    if(_ui->pointFormatDataThemeCmb->currentIndex() <= 0 ||
	       _ui->pointFormatDataAttributeCmb->currentIndex() <= 0) 
	    {
	      err = tr("Para influência por região, é necessário informar o tema associado\n"
	               "e o atributo de ligação entre polígonos e PCDs!");
	      return false;
	    }   
	  }

	  // Se a fonte de dados do tipo PCD pertencer a um servidor OGC WFS,
	  // alguns parametros adicionais precisam estar definidos
	  if(_ui->btnPCDWFSConfiguration->isVisible())
	  {
		  if(_wfsPCDIdAttr.isEmpty())
		  {
			  err = tr("É necessário definir um atributo do layer WFS\n"
					   "que represente o identificador da PCD.");
			  return false;
		  }

		  if(_wfsPCDTimestampAttr.isEmpty())
		  {
			  err = tr("É necessário definir um atributo do layer WFS\n"
					   "que represente a data/hora de cada coleta.");
			  return false;
		  }

		  if(_wfsPCDTimestampMask.isEmpty())
		  {
			  err = tr("É necessário definir uma máscara para interpretação\n"
					   "do atributo temporal do layer WFS.");
			  return false;
		  }
	  }
  }
  else if(_pointDiffChanged)
  {
	  if(_ui->pointDiffFormatDataName->text().trimmed().isEmpty())
	  {
	    err = tr("Nome do formato de dados não foi preenchido!");
	    return false;
	  }

	  if(_projection.name == WS_PROJNAME_NoProjection)
	  {
		  err = tr("É necessário definir uma projeção!");
		  return false;
	  }
  }
  
  return true;
}

// Funcao comentada na classe base
bool MainDialogWeatherTab::save()
{
  bool lRet;  

  if(_serverChanged)
	  lRet = saveServer();

  else if(_gridChanged)
	  lRet = saveGrid();

  else if(_pointChanged)
	  lRet = savePoint();

  else if(_pointDiffChanged)
  {
	  lRet = savePointDiff();
  }

  return lRet;
}

// Grava informacoes do Servidor Remoto
bool MainDialogWeatherTab::saveServer()
{
  RemoteWeatherServer* rws;
  bool lRet = false;

  if (!_newServer && ctrlChg.idSrv<0)
	  return false;

  if (_newServer)
    rws = new (RemoteWeatherServer);
  else
	rws = (_remoteWeatherServerList->findServerById(ctrlChg.idSrv));

  // Carrega dados da interface
  getServerFields(rws);
  
  bool ok;
  
  // Salva
  if(_newServer)
  {
	  int newId;
	  ok = _remoteWeatherServerList->addNewRemoteWeatherServer(rws, newId);  
	  rws->setId(newId);
  }
  else
    ok = _remoteWeatherServerList->updateRemoteWeatherServer(rws->id(), rws);
  
  // Se a operação de salvar não deu certo, retorna false e mantém o estado atual
  if(ok)
  {
	// Atualiza nome da entrada atual da lista que pode ter sido alterado
	  if (ctrlChg.twiItem !=NULL)
	  {
		ctrlChg.twiItem->setText(0, rws->serverName());  
		ctrlChg.twiItem->setData(0, Qt::UserRole, rws->id()+1);
	  }

	// Atualiza estado para dados não alterados*/

	clearServerChanged();
	_newServer = false;
	lRet = true;
  }

  return lRet;
}

// Grava informacoes de formatos de dados tipo grid
bool MainDialogWeatherTab::saveGrid()
{
	return saveDataFormat(WS_WDSGEOM_RASTER, _newGrid);
}

// Grava informacoes de formatos de dados tipo point
bool MainDialogWeatherTab::savePoint()
{
	return saveDataFormat(WS_WDSGEOM_POINTS, _newPoint);
}

// Grava informacoes de formatos de dados (point/grid)
bool MainDialogWeatherTab::saveDataFormat(wsWeatherDataSourceGeometry geometryType, bool newData)
{
  RemoteWeatherServer* rws;
  RemoteWeatherServerData rwsd;
  bool lRet = false;

  // FILTROS: Preencher informacao de filtros em rwsd com dados da interface


  rws =  (_remoteWeatherServerList->findServerById(ctrlChg.idSrv));
  if (!_newGrid && !_newPoint && !_newPointDiff)
	rwsd = *(rws->findServerDataById(ctrlChg.idDat));

  // Carrega dados da interface
  if (geometryType == WS_WDSGEOM_RASTER)
  {
	  bool isWCS = (rws->protocol() == WS_PROTOCOL_OGC);
	  getDataGridFields(&rwsd, isWCS);
  }
  else if (geometryType == WS_WDSGEOM_POINTS)
  {
	  bool isWFS = (rws->protocol() == WS_PROTOCOL_OGC);
	  getDataPointFields(&rwsd, isWFS);
  }
  else if (geometryType == WS_WDSGEOM_POINTSDIFF)
	  getDataPointDiffFields(&rwsd);
  
  bool ok;
  
  int idUpPCD = ctrlChg.idDat;
  // Salva
  if(newData)
  {
    int newId;
    ok = rws->getRemoteWeatherServerDataList()->addNewRemoteWeatherServerData(ctrlChg.idSrv, &rwsd, newId);  

    rwsd.setId(newId);
	idUpPCD = newId;
  }
  else
	  ok = rws->getRemoteWeatherServerDataList()->updateRemoteWeatherServerData(ctrlChg.idSrv, rwsd.id(), &rwsd);
  
  // Se a operação de salvar não deu certo, retorna false e mantém o estado atual
  if(ok)
  {
  	// Atualiza nome da entrada atual da lista que pode ter sido alterado
	  //_ui->weatherDataTree->currentItem()->setText(0, rwsd.name());  
	  if (ctrlChg.twiItem!=NULL)
	  {
	  	 ctrlChg.twiItem->setText(0, rwsd.name());  
		 ctrlChg.twiItem->setData(0, Qt::UserRole, -(rwsd.id()+1));
	  }
  
	  lRet = true;
		// Atualiza estado para dados não alterados*/
	  if (geometryType == WS_WDSGEOM_RASTER)
	  {
		  _newGrid = false;
		  clearGridChanged();
	  }
	  else if (geometryType == WS_WDSGEOM_POINTS)
	  {
		  RemoteWeatherServerData* rwsdi = rws->findServerDataById(idUpPCD);
		  CopyVectorToQVector<struct wsWeatherDataPcdSurface>(rwsdi->pcd_collectionSurfaces(), _collectionSurfaces);

		  if(_manager->pcdList())
		  {
	  	  	  _manager->pcdList()->setListChanged();
		  }
		  _newPoint = false;
		  clearPointChanged();
  	  }
	  else if (geometryType == WS_WDSGEOM_POINTSDIFF)
	  {
		  _newPointDiff = false;
		  clearPointDiffChanged();
	  }
  }

  return lRet;
}

// Funcao comentada na classe base
void MainDialogWeatherTab::discardChanges(bool restore_data)
{
  _ignoreChangeEvents = true;

  if(_newServer || _newGrid || _newPoint || _newPointDiff)
	  discardChangesNew(restore_data);
  else
	  discardChangesUpdate(restore_data);

  clearChangeStruct();

  _ignoreChangeEvents = false;
}

/*! \brief Trata desconsiderar dados quando foi uma inclusão

restore_data indica se os dados devem ser recarregados
*/
void MainDialogWeatherTab::discardChangesNew(bool restore_data)
{
  int		index;

  // Estamos descartando um servidor ou fonte de dados recém criado que não foi salva na base
  // 1) Remove entrada da lista (interface).  Deve ser a última linha

  _ignoreChangeEvents = true;
  if (restore_data)
	  _ui->weatherDataTree->setCurrentItem(ctrlChg.twiItem->parent());

  _ui->weatherDataTree->removeItemWidget(ctrlChg.twiItem, 0);
  delete ctrlChg.twiItem;

  // 2) Desmarca indicador de nova regra e de dados modificados
  if (_newServer)
  {
    index = 0;
	  _newServer = false;
	  clearServerChanged();

	  if (restore_data)
	    clearAllFields(false);  
  }

  if (_newGrid)
  {
    index = 1;
	  _newGrid = false;
	  clearGridChanged();
  }

  if (_newPoint)
  {
    index = 1;
	  _newPoint = false;
	  clearPointChanged();
  }

  if (_newPointDiff)
  {
	  index = 1;
	  _newPointDiff = false;
	  clearPointDiffChanged();
  }

  showButtons(index);
    
  // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
  if(restore_data)
  {
    _ui->weatherPageStack->setCurrentIndex(index);
  }

  _ignoreChangeEvents = false;
}

/*! \brief Trata desconsiderar dados quando foi uma alteração

restore_data indica se os dados devem ser recarregados
*/
void MainDialogWeatherTab::discardChangesUpdate(bool restore_data)
{
  // Estamos descartando as edições feitas em uma item antigo
  if(restore_data)
  {
	// Estamos tratando uma seleção normal feita pelo usuário

	  if (ctrlChg.idDat == -1)
	  {
		  RemoteWeatherServer* srv = _remoteWeatherServerList->findServerById(ctrlChg.idSrv);
		  setServerFields(srv);
	  }
	  else
	  {
		  RemoteWeatherServerData* srvData = _remoteWeatherServerList->findServerDataById(ctrlChg.idSrv, ctrlChg.idDat );
		  setDataFields( srvData );
	  }
  }

  clearServerChanged(); 
  clearGridChanged();
  clearPointChanged();
  clearPointDiffChanged();
}


//====================================================


/*! \brief Limpa a interface (todas as telas).  

Flag indica se a lista dos servidores e a lista dos formatos de dados risco também devem ser limpas
*/
void MainDialogWeatherTab::clearAllFields(bool clearlist)
{
	QTreeWidgetItem *parentQTree = _ui->weatherDataTree->currentItem();

	_ignoreChangeEvents = true; 

  if(clearlist)
  {
    while (parentQTree->childCount()>0)
    {
      QTreeWidgetItem *child;
      child = parentQTree->child ( 0 );
		
      parentQTree->removeChild ( child );
      delete child;
    }
  }
		
  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearServerChanged();
  clearGridChanged();
  clearPointChanged();
  clearPointDiffChanged();
}


/*! \brief Limpa a interface (somente os campos da tela do servidor).  
*/
void MainDialogWeatherTab::clearServerFields(void)
{
	// Tela do servidor
  _ui->serverName->setText("");
  _ui->serverDescription->setText("");
  _ui->connectionAddress->setText("");
  _ui->connectionPort->setText("");
  _ui->connectionProtocol->setCurrentIndex(0);;
  _ui->connectionUserName->setText("");
  _ui->connectionPassword->setText("");
  _ui->serverActiveServer->setCheckState( Qt::Checked);
  _ui->serverIntervalData->setValue(1);
  _ui->serverDataBasePath->setText("");
}

/*! \brief Limpa a interface (somente os campos da tela do formato de dados tipo grid).  
*/
void MainDialogWeatherTab::clearFormatDataGridFields(void)
{
    _ui->gridFormatDataName->setText("");
    _ui->gridFormatDataType->setCurrentIndex(0);
    _ui->gridFormatDataResolution->setText("");
    _ui->gridFormatDataFrequency->setText("");
    _ui->gridFormatDataDescription->setText("");
    _ui->gridFormatDataPath->setText("");
    _ui->gridFormatDataMask->setText("");
    _ui->gridFormatDataFormat->setCurrentIndex(0);
	_ui->gridFormatDataPrefix->setText("");
	_ui->gridFormatDataUnit->setText("");
	_ui->gridFormatDataTimeZoneCmb->setCurrentIndex(12);
}

/*! \brief Limpa a interface (somente os campos da tela do formato de dados tipo point).  
*/
void MainDialogWeatherTab::clearFormatDataPointFields(void)
{
  _ui->pointFormatDataName->setText("");
  _ui->pointFormatDataType->setCurrentIndex(0);
  _ui->pointFormatDataRadius->setText("");
  _ui->pointFormatDataFrequency->setText("");
  _ui->pointFormatDataDescription->setText("");
  _ui->pointFormatDataPath->setText("");
  _ui->pointFormatDataMask->setText("");
  _ui->pointFormatDataFormat->setCurrentIndex(0);
  _ui->pointFormatDataInfluenceCmb->setCurrentIndex(0);
  _ui->pointFormatDataThemeCmb->setCurrentIndex(0);   
  _ui->pointFormatDataAttributeCmb->setCurrentIndex(0);
  _ui->pointFormatDataPrefix->setText("");
  _ui->pointFormatDataUnit->setText("");
  _ui->pointFormatDataTimeZoneCmb->setCurrentIndex(12);

  // Limpar tambem variaveis usadas no preenchimento do dialogo de configuracao de fontes do tipo PCD WFS
  _wfsPCDIdAttr = "";
  _wfsPCDTimestampAttr = "";
  _wfsPCDTimestampMask = "";
}



//! Preenche a interface com os dados de um servidor
void MainDialogWeatherTab::setServerFields(const RemoteWeatherServer* rws)
{
  _ignoreChangeEvents = true; 

  // Preenche dados comuns
  // Tela do servidor
  _ui->serverName->setText(rws->serverName());
  _ui->serverDescription->setText(rws->details());
  _ui->connectionAddress->setText(rws->address());

  _ui->connectionPort->setText(QString::number(rws->port()));

  _ui->connectionProtocol->setCurrentIndex(rws->protocol());
  _ui->connectionProtocol->setEnabled(false);

  _ui->connectionUserName->setText(rws->userName());
  _ui->connectionPassword->setText(rws->password());
  _ui->serverActiveServer->setCheckState( rws->active()? Qt::Checked : Qt::Unchecked );
  _ui->serverIntervalData->setValue(rws->intervalMinutes());
  _ui->serverDataBasePath->setText(rws->basePath());

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearServerChanged();
}

//! Preenche a interface com os dados do formato de dados
void MainDialogWeatherTab::setDataFields(RemoteWeatherServerData* rwsd)
{
  _ignoreChangeEvents = true; 

  if (rwsd->geometry() == WS_WDSGEOM_RASTER)  //  WS_WDSGEOM_POINTS, WS_WDSGEOM_RASTER, WS_WDSGEOM_POINTSDIFF
	  setDataGridFields(rwsd);
  else if (rwsd->geometry() == WS_WDSGEOM_POINTS)
	  setDataPointFields(rwsd);
  else if (rwsd->geometry() == WS_WDSGEOM_POINTSDIFF)
	  setDataPointDiffFields(rwsd);

  _ignoreChangeEvents = false;
}

//! Preenche a interface com os dados do formato de dados (tipo grid)
void MainDialogWeatherTab::setDataGridFields(RemoteWeatherServerData* rwsd)
{
  _ui->gridFormatDataName->setText(rwsd->name());
  _ui->gridFormatDataType->setCurrentIndex(rwsd->type());

  _ui->gridFormatDataResolution->setText(QString::number(rwsd->spatialResolution()));
  _ui->gridFormatDataFrequency->setText(QString::number(rwsd->updateFrequencyMins()));

  _ui->gridFormatDataTimeZoneCmb->setCurrentIndex(timeZoneToIndex(rwsd->timeZone()));
  _ui->gridFormatDataPrefix->setText(rwsd->prefix());
  _ui->gridFormatDataUnit->setText(rwsd->unit());

  _ui->gridFormatDataDescription->setText(rwsd->details());
  _ui->gridFormatDataPath->setText(rwsd->path());
  _ui->gridFormatDataMask->setText(rwsd->mask());
  _ui->gridFormatDataFormat->setCurrentIndex(fileFormatToIndex(rwsd->format()));

  _ui->ledGridTIFFArqNavegacao->setText(rwsd->tiff_navigationFile());

  _ui->rbGridAscUnidGrausDec->setChecked(rwsd->asciiGrid_coordUnit() == WS_ASCIIGRIDCOORD_DECIMAL_DEGREES);
  _ui->rbGridAscUnidGrausMil->setChecked(rwsd->asciiGrid_coordUnit() == WS_ASCIIGRIDCOORD_MILLIDEGREES);

  _ui->ledGridGrADSArqControle->setText(rwsd->grads_ctlFile());
  _ui->rbGridGrADSTipoDadosInt->setChecked(rwsd->grads_dataType() == WS_GRADSDATATYPE_INTEGER16);
  _ui->rbGridGrADSTipoDadosFloat->setChecked(rwsd->grads_dataType() == WS_GRADSDATATYPE_FLOAT32);
  _ui->ledGridGrADSMultiplicador->setText(QString::number(rwsd->grads_multiplier()));
  
  _ui->cmbGridGrADSByteOrder->setCurrentIndex(gradsSwapTypeToIndex(rwsd->grads_swap()));
  _ui->spbGridGrADSNumBands->setValue(rwsd->grads_numBands());
  _ui->spbGridGrADSTimeOffset->setValue(rwsd->grads_bandTimeOffset());  
  _ui->spbGridGrADSHeaderSize->setValue(rwsd->grads_bandHeader());  
  _ui->spbGridGrADSTraillerSize->setValue(rwsd->grads_bandTrailler());

  _ui->ledGridWCSDummy->setText(QString::number(rwsd->wcs_dummy()));

  _collectionPoints.clear();
  _collectionAtributes.clear();
  _collectionSurfaces.clear();
  _pcdCollectionRuleLuaScript = "";

  _wfsPCDIdAttr = "";
  _wfsPCDTimestampAttr = "";
  _wfsPCDTimestampMask = "";

  _projection = rwsd->projection();
  _filter = rwsd->filter();
  _intersection = rwsd->intersection();

  if(_filter.dateBeforeEnabled || _filter.dateAfterEnabled)
	  _ui->dateFilterLabel->setText(QString("Sim"));
  else
	  _ui->dateFilterLabel->setText(QString("Não"));

  if(_filter.areaFilterType != WS_NoFilter)
	  _ui->areaFilterLabel->setText(QString("Sim"));
  else
	  _ui->areaFilterLabel->setText(QString("Não"));

  if(_filter.preAnalysisType != WS_PATYPE_NoPreAnalysis)
	  _ui->preAnalysisLabel->setText(QString("Sim"));
  else
	  _ui->preAnalysisLabel->setText(QString("Não"));

  if(_filter.bandFilterString != "")
	  _ui->bandFilterLabel->setText(QString("Sim"));
  else
	  _ui->bandFilterLabel->setText(QString("Não"));

  clearGridChanged();
}

//! Preenche a interface com os dados do formato de dados (tipo point)
void MainDialogWeatherTab::setDataPointFields(RemoteWeatherServerData* rwsd)
{
  _ui->pointFormatDataName->setText(rwsd->name());
  _ui->pointFormatDataType->setCurrentIndex(rwsd->type());

  _ui->pointFormatDataRadius->setText(QString::number(rwsd->spatialResolution()));
  _ui->pointFormatDataFrequency->setText(QString::number(rwsd->updateFrequencyMins()));

  _ui->pointFormatDataDescription->setText(rwsd->details());
  _ui->pointFormatDataPath->setText(rwsd->path());
  _ui->pointFormatDataMask->setText(rwsd->mask());
  _ui->pointFormatDataFormat->setCurrentIndex(fileFormatToIndex(rwsd->format()));

  _ui->pointFormatDataPrefix->setText(rwsd->prefix());
  _ui->pointFormatDataUnit->setText(rwsd->unit());
  _ui->pointFormatDataTimeZoneCmb->setCurrentIndex(timeZoneToIndex(rwsd->timeZone()));

  _ui->pointFormatDataInfluenceCmb->setCurrentIndex(pcdInfluenceTypeToIndex(rwsd->pcd_influenceType()));
  if(rwsd->pcd_influenceType() == WS_WDPIT_THEME)
  {
    setThemeData(rwsd->pcd_influenceThemeId(), rwsd->pcd_influenceAttribute());
  }
  else
  {
    _ui->pointFormatDataThemeCmb->setCurrentIndex(0);
    _ui->pointFormatDataAttributeCmb->setCurrentIndex(0);
  }

  CopyVectorToQVector<struct wsPCD>(rwsd->pcd_collectionPoints(), _collectionPoints);
  CopyVectorToQVector<std::string>(rwsd->pcd_collectionAtributes(), _collectionAtributes);
  CopyVectorToQVector<struct wsWeatherDataPcdSurface>(rwsd->pcd_collectionSurfaces(), _collectionSurfaces);

  _pcdCollectionRuleLuaScript = rwsd->pcd_collectionRuleLuaScript();

  if(rwsd->format() == WS_WDSFFMT_PCD_OGC_WFS)
  {
	  _wfsPCDIdAttr = rwsd->wfs_pcd_id_attr();
	  _wfsPCDTimestampAttr = rwsd->wfs_pcd_timestamp_attr();
	  _wfsPCDTimestampMask = rwsd->wfs_pcd_timestamp_mask();
  }
  else
  {
	  _wfsPCDIdAttr = "";
	  _wfsPCDTimestampAttr = "";
	  _wfsPCDTimestampMask = "";
  }

  _projection = rwsd->projection();
  _filter.dateBeforeEnabled = false;
  _filter.dateAfterEnabled  = false;
  _filter.areaFilterType    = WS_NoFilter;
  _filter.preAnalysisType   = WS_PATYPE_NoPreAnalysis;
  _filter.preAnalysisThemeEnabled = false;
  _filter.bandFilterString  = "";
  _intersection.rasterIntersection.clear();
  _intersection.vectorIntersection.clear();
  _intersection.dataSourceIntersection.clear();

  fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);

  clearPointChanged();
}

//! Preenche os dados sobre o servidor com os dados da interface, que já devem ter sido validados
void MainDialogWeatherTab::getServerFields(RemoteWeatherServer* rws)
{
  // Preenche dados comuns
  // Tela do servidor
  rws->setServerName(_ui->serverName->text().trimmed());
  rws->setDescription(_ui->serverDescription->toPlainText().trimmed());
  rws->setAddress(_ui->connectionAddress->text().trimmed());
  rws->setPort(_ui->connectionPort->text().trimmed().toInt());
  rws->setProtocol((enum wsProtocol)_ui->connectionProtocol->currentIndex());
  rws->setUserName(_ui->connectionUserName->text().trimmed());
  rws->setPassword(_ui->connectionPassword->text().trimmed());
  rws->setActive(_ui->serverActiveServer->checkState() == Qt::Checked);
  rws->setIntervalMinutes(_ui->serverIntervalData->value());
  rws->setBasePath(_ui->serverDataBasePath->text().trimmed());
}

//! Preenche os dados sobre o Formato de dados (grid) com os dados da interface, que já devem ter sido validados
void MainDialogWeatherTab::getDataGridFields(RemoteWeatherServerData* rwsd, bool isWCS)
{
  rwsd->setGeometry(WS_WDSGEOM_RASTER);		//	WS_WDSGEOM_POINTS,	WS_WDSGEOM_RASTER,

  rwsd->setName(_ui->gridFormatDataName->text().trimmed());
  rwsd->setType((enum wsWeatherDataSourceType)_ui->gridFormatDataType->currentIndex());
  rwsd->setSpatialResolution(_ui->gridFormatDataResolution->text().trimmed().toDouble());
  rwsd->setUpdateFrequencyMins(_ui->gridFormatDataFrequency->text().trimmed().toDouble());
  rwsd->setDetails(_ui->gridFormatDataDescription->toPlainText().trimmed());
  rwsd->setPath(_ui->gridFormatDataPath->text().trimmed());
  rwsd->setMask(_ui->gridFormatDataMask->text().trimmed());

  if(isWCS)
	  rwsd->setFormat(WS_WDSFFMT_OGC_WCS);
  else
	  rwsd->setFormat(indexToFileFormat(_ui->gridFormatDataFormat->currentIndex(), 0));
  
  rwsd->setProjection(_projection);
  rwsd->setFilter(_filter);
  rwsd->setIntersection(_intersection);
  rwsd->setTimeZone(_ui->gridFormatDataTimeZoneCmb->currentText());
  rwsd->setPrefix(_ui->gridFormatDataPrefix->text().trimmed().replace(" ", "_"));
  rwsd->setUnit(_ui->gridFormatDataUnit->text().trimmed());

  rwsd->setTiff_navigationFile(_ui->ledGridTIFFArqNavegacao->text().trimmed());

  if (_ui->rbGridAscUnidGrausDec->isChecked())
    rwsd->setAsciiGrid_coordUnit(WS_ASCIIGRIDCOORD_DECIMAL_DEGREES);
  else if (_ui->rbGridAscUnidGrausMil->isChecked())
    rwsd->setAsciiGrid_coordUnit(WS_ASCIIGRIDCOORD_MILLIDEGREES);

  rwsd->setGrads_ctlFile(_ui->ledGridGrADSArqControle->text().trimmed());

  if (_ui->rbGridGrADSTipoDadosInt->isChecked())
	  rwsd->setGrads_dataType(WS_GRADSDATATYPE_INTEGER16);
  else if (_ui->rbGridGrADSTipoDadosFloat->isChecked())
    rwsd->setGrads_dataType(WS_GRADSDATATYPE_FLOAT32);

  rwsd->setGrads_multiplier(_ui->ledGridGrADSMultiplicador->text().toDouble());

  rwsd->setGrads_swap(indexToGradsSwapType(_ui->cmbGridGrADSByteOrder->currentIndex()));
  rwsd->setGrads_numBands(_ui->spbGridGrADSNumBands->value());
  rwsd->setGrads_bandTimeOffset(_ui->spbGridGrADSTimeOffset->value());  
  rwsd->setGrads_bandHeader(_ui->spbGridGrADSHeaderSize->value());
  rwsd->setGrads_bandTrailler(_ui->spbGridGrADSTraillerSize->value());

  rwsd->setWCS_dummy(_ui->ledGridWCSDummy->text().toDouble());

  // "Zera" atributos usados apenas pelas PCDs
  std::vector<struct wsPCD> dummy;
  std::vector<struct wsWeatherDataPcdSurface> dummySurfaces;
  rwsd->setPcd_collectionPoints(dummy);
  rwsd->setPcd_collectionRuleLuaScript("");
  rwsd->setPcd_influenceType(WS_WDPIT_RADIUS_TOUCH);
  rwsd->setPcd_influenceThemeId(-1);
  rwsd->setPcd_influenceAttribute("");
  rwsd->setPcd_collectionSurfaces(dummySurfaces);
}

//! Preenche os dados sobre o Formato de dados (point) com os dados da interface, que já devem ter sido validados
void MainDialogWeatherTab::getDataPointFields(RemoteWeatherServerData* rwsd, bool isWFS)
{
  rwsd->setGeometry(WS_WDSGEOM_POINTS);		//	WS_WDSGEOM_POINTS,	WS_WDSGEOM_RASTER,

  rwsd->setName(_ui->pointFormatDataName->text().trimmed());
  rwsd->setType((enum wsWeatherDataSourceType)_ui->pointFormatDataType->currentIndex());
  rwsd->setSpatialResolution(_ui->pointFormatDataRadius->text().trimmed().toDouble());
  rwsd->setUpdateFrequencyMins(_ui->pointFormatDataFrequency->text().trimmed().toDouble());
  rwsd->setDetails(_ui->pointFormatDataDescription->toPlainText().trimmed());
  rwsd->setPath(_ui->pointFormatDataPath->text().trimmed());
  rwsd->setMask(_ui->pointFormatDataMask->text().trimmed());
  rwsd->setPrefix(_ui->pointFormatDataPrefix->text().trimmed().replace(" ", "_"));
  rwsd->setUnit(_ui->pointFormatDataUnit->text().trimmed());
  rwsd->setTimeZone(_ui->pointFormatDataTimeZoneCmb->currentText());

  if(isWFS)
  {
	  rwsd->setFormat(WS_WDSFFMT_PCD_OGC_WFS);

	  rwsd->setWFS_pcd_id_attr(_wfsPCDIdAttr);
	  rwsd->setWFS_pcd_timestamp_attr(_wfsPCDTimestampAttr);
	  rwsd->setWFS_pcd_timestamp_mask(_wfsPCDTimestampMask);
  }
  else
  {
	  rwsd->setFormat(indexToFileFormat(_ui->pointFormatDataFormat->currentIndex(), 1));

	  rwsd->setWFS_pcd_id_attr("");
	  rwsd->setWFS_pcd_timestamp_attr("");
	  rwsd->setWFS_pcd_timestamp_mask("");
  }

  rwsd->setPcd_influenceType(indexToPcdInfluenceType(_ui->pointFormatDataInfluenceCmb->currentIndex()));
  if(rwsd->pcd_influenceType() != WS_WDPIT_THEME) // area de influencia por raio
  {
    rwsd->setPcd_influenceThemeId(-1);
    rwsd->setPcd_influenceAttribute("");
  }
  else
  {
    int themeIndex = _ui->pointFormatDataThemeCmb->currentIndex();
    assert(themeIndex > 0); // Validado anteriormente
    struct wsRiskMapTheme& themeData = _pcdTheme.at(themeIndex - 1); // -1 por causa da opção de não selecionado
    rwsd->setPcd_influenceThemeId(themeData.id);
    
    int attributeIndex = _ui->pointFormatDataAttributeCmb->currentIndex();
    assert(attributeIndex > 0); // Validado anteriormente
    rwsd->setPcd_influenceAttribute(QString::fromStdString(themeData.attributes.at(attributeIndex - 1).name)); // -1 por causa da opção de não selecionado
  }

  rwsd->setProjection(_projection);
  rwsd->setFilter(_filter);
  rwsd->setIntersection(_intersection);

  rwsd->setPcd_collectionPoints(_collectionPoints.toStdVector());
  rwsd->setPcd_collectionRuleLuaScript(_pcdCollectionRuleLuaScript);
  rwsd->setPcd_collectionAtributes(_collectionAtributes.toStdVector());
  rwsd->setPcd_collectionSurfaces(_collectionSurfaces.toStdVector());

  // "Zera" atributos usados apenas pelo grid
  rwsd->setTiff_navigationFile("");
  rwsd->setAsciiGrid_coordUnit(WS_ASCIIGRIDCOORD_DECIMAL_DEGREES);
  rwsd->setGrads_ctlFile("");
  rwsd->setGrads_dataType(WS_GRADSDATATYPE_INTEGER16);
  rwsd->setGrads_multiplier(0.0);
  rwsd->setGrads_swap(WS_GRADSSWAPTYPE_AUTO);
  rwsd->setGrads_numBands(1);
  rwsd->setGrads_bandTimeOffset(0);  
  rwsd->setGrads_bandHeader(0);
  rwsd->setGrads_bandTrailler(0);
}

//! Preenche a tabela de colection points.
void MainDialogWeatherTab::fillTableCollectionPoints(QTableWidget* tblColPoints, QPushButton* btnDel)
{
  QTableWidgetItem *itemFileName;
  QTableWidgetItem *itemLatitude;
  QTableWidgetItem *itemLongitude;
  QTableWidgetItem *itemActive;
  wsPCD            strPCD;

  tblColPoints->setRowCount(_collectionPoints.size());

  for (int i = 0; i < _collectionPoints.size(); i++)
  {
	  strPCD = _collectionPoints.at(i);

	  itemFileName  = new QTableWidgetItem(QString::fromStdString(strPCD.fileName));

	  //dependendo da projecao escolhida, exibir latitude e longitude com precisao diferente
	  if(_projection.name == WS_PROJNAME_LatLong)
	  {
		  itemLatitude  = new QTableWidgetItem(QString::number(strPCD.latitude, 'f', 10));
		  itemLongitude = new QTableWidgetItem(QString::number(strPCD.longitude, 'f', 10));
	  }
	  else
	  {
		  itemLatitude  = new QTableWidgetItem(QString::number(strPCD.latitude, 'f', 3));
		  itemLongitude = new QTableWidgetItem(QString::number(strPCD.longitude, 'f', 3));
	  }

	  itemActive    = new QTableWidgetItem(strPCD.isActive ? tr("Sim") : tr("Não"));

	  tblColPoints->setItem(i, 0, itemFileName);
	  tblColPoints->setItem(i, 1, itemLatitude);
	  tblColPoints->setItem(i, 2, itemLongitude);
	  tblColPoints->setItem(i, 3, itemActive);
  }

  btnDel->setEnabled(_collectionPoints.size()>0);
  tblColPoints->setEnabled(_collectionPoints.size()>0);
}

//! Slot chamado quando o usuário pressiona o botão para informar um plano de informaçao onde estão as PCDs
void MainDialogWeatherTab::pcdInfPlaneClicked()
{
	PcdInfPlaneDlg dlg(_remoteWeatherServerList->getPCDThemes());
	
	wsPCDInfPlaneAttributes pcdInfPlaneAttributes;

	if (dlg.exec()==QDialog::Accepted)	
	{
		if(_projection.name != WS_PROJNAME_NoProjection)
		{
			dlg.getFields(&pcdInfPlaneAttributes);

			if(pcdInfPlaneAttributes.themeID >= 0)
			{
				_collectionPoints.clear();

				std::vector<struct wsPCD> pcdColPts;
				_remoteWeatherServerList->getPCDColPtsByTheme(_projection, pcdInfPlaneAttributes, pcdColPts);

				CopyVectorToQVector<struct wsPCD>(pcdColPts, _collectionPoints);

				fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
				setPointChanged();	
			}
		}
		else
		{
			//exibir mensagem informando que projecao da serie de dados deve ser definida
			QMessageBox::warning(_parent, tr("Importação cancelada"), tr("A série de dados não possui projeção definida!\nA importação foi cancelada."));
		}
	}
}

//! Slot chamado quando o usuário pressiona o botao para definir parametros necessarios para coletar dados de PCD de um servidor OGC WFS
void MainDialogWeatherTab::pcdWFSConfigurationClicked()
{
	// Verificar se o endereco do servidor e a mascara de coleta ja foram definidos,
	// para que seja possivel obter o layer WFS
	QString weatherServerDataMask = _ui->pointFormatDataMask->text().trimmed();
	
	QString url = "";
	QTreeWidgetItem* treeItem = _ui->weatherDataTree->currentItem()->parent();

	if(_remoteWeatherServerList && treeItem)
	{
		RemoteWeatherServer* rws = _remoteWeatherServerList->findServerById(treeItem->data(0,Qt::UserRole).toInt() -1);
		url = (rws ? rws->address() : "");
	}
	
	if(weatherServerDataMask.isEmpty() || url.isEmpty())
	{
		QMessageBox::warning(_parent, tr("Impossível prosseguir"), tr("É necessário que o endereço do servidor e a\nmáscara de coleta da fonte de dados estejam definidos."));
		return;
	}

	// Exibir mensagem informando que resposta do servidor WFS pode demorar um pouco
	QMessageBox msgBox(QMessageBox::NoIcon, "Por favor, aguarde", "Carregando dados do servidor WFS...", QMessageBox::NoButton, _parent);
	msgBox.setStandardButtons(QMessageBox::NoButton);
	msgBox.show();
	QApplication::processEvents();

	// Chamar o serviço que fara a requisicao ao servidor WFS
	std::vector<wsDBColumn> attributes;
	_manager->getWFSLayerAttributes(url.toStdString(), weatherServerDataMask.toStdString(), attributes);

	// Verificar se a lista de atributos foi carregada
	if(attributes.size() == 0)
	{
		msgBox.hide();
		QMessageBox::warning(_parent, tr("Erro..."), tr("Não foi possível carregar a lista de atributos do layer WFS.\nVerifique os parâmetros de conexão e a máscara da fonte de dados."));
		return;
	}

	// Exibir interface para que usuario altere os parametros
	msgBox.hide();
	PcdWFSAttrsDlg dlg(attributes, _parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
	
	dlg.setFields(_wfsPCDIdAttr, _wfsPCDTimestampAttr, _wfsPCDTimestampMask);

	if (dlg.exec()==QDialog::Accepted)	
	{
		QString idAttr, timeAttr, mask;
		dlg.getFields(idAttr, timeAttr, mask);

		// Verificar se houve alguma alteracao
		if(idAttr != _wfsPCDIdAttr || timeAttr != _wfsPCDTimestampAttr || mask != _wfsPCDTimestampMask)
		{
			_wfsPCDIdAttr = idAttr;
			_wfsPCDTimestampAttr = timeAttr;
			_wfsPCDTimestampMask = mask;
			setPointChanged();
		}
	}
}

//! Slot chamado quando o usuário pressiona o botão para criar superfícies a partir de um PCD
void MainDialogWeatherTab::surfaceConfigDialogRequested()
{
	SurfacesConfigDlg dlg(_collectionPoints, _collectionAtributes, _collectionSurfaces, _projection);

	if (dlg.exec()== QDialog::Accepted)
	{
		bool changed = false;
		dlg.getFields(_collectionSurfaces, changed);
		if (changed)
			setPointChanged();
	}
}

//! Slot chamado quando o usuário pressiona o botão de teste de conexão com um servidor
void MainDialogWeatherTab::serverCheckConnectionClicked()
{
	QString strAddress = _ui->connectionAddress->text().trimmed();
	QString strBasePath = _ui->serverDataBasePath->text().trimmed();
	
	if(((enum wsProtocol)_ui->connectionProtocol->currentIndex()) == WS_PROTOCOL_FTP)
	{
		if (!strAddress.startsWith("ftp://", Qt::CaseInsensitive))
			strAddress = "ftp://" + strAddress;
		
		QUrl url ( strAddress );
		if (!url.isValid())
		{
			QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("URL inválida!"));
			return;
		}

		_progressDlg = new QProgressDialog("Verificando conexão com servidor...", "Cancelar", 0, 10, _parent);
		_progressDlg->setWindowModality(Qt::WindowModal);
		_checkConnection = true;

		connect(_progressDlg, SIGNAL(canceled()), this, SLOT(cancelServerCheckConnection()));

		_progressBarValue = 1;
		_progressDlg->setValue(_progressBarValue);
		_progressDlg->show();
		
		_ftpTimeoutTimer.stop();
		_ftpTimeoutTimer.setSingleShot(false);
		_ftpTimeoutTimer.start(1000);

		if(_checkConnection)
			if(_ftp->state() != QFtp::Connected)
				_ftp->connectToHost(url.host(), _ui->connectionPort->text().toInt());

		if(_checkConnection)
			_ftp->login(_ui->connectionUserName->text(), _ui->connectionPassword->text());

		if(!strBasePath.isEmpty() && _checkConnection)
			_ftp->cd(strBasePath);

		if(_ftp->state() != QFtp::Unconnected)
			_ftp->close();
	}
	else if(((enum wsProtocol)_ui->connectionProtocol->currentIndex()) == WS_PROTOCOL_FILE)
	{
		QString path = strAddress;
		path.append("/");
		path.append(strBasePath);

		QDir dir(path);

		if(dir.exists())
			QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Os parâmetros de conexão estão corretos!"));
		else
			QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Diretório informado não existente!"));
	}
	else if(((enum wsProtocol)_ui->connectionProtocol->currentIndex()) == WS_PROTOCOL_OGC)
	{
		if (!strAddress.startsWith("http://", Qt::CaseInsensitive))
			strAddress = "http://" + strAddress;

		QUrl url ( strAddress );
		if (!url.isValid())
		{
			QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("URL inválida!"));
			return;
		}

		if(!_networkAccessManager)
		{
			_networkAccessManager = new QNetworkAccessManager(this);
			connect(_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
		}

		_ogcProgressDlg = new QProgressDialog("Verificando conexão com servidor...", "Cancelar", 0, 10, _parent);
		_ogcProgressDlg->setWindowModality(Qt::WindowModal);

		connect(_ogcProgressDlg, SIGNAL(canceled()), this, SLOT(cancelOGCServerCheckConnection()));

		_ogcProgressBarValue = 1;
		_ogcProgressDlg->setValue(_ogcProgressBarValue);
		_ogcProgressDlg->show();

		_ogcTimer.stop();
		_ogcTimer.setSingleShot(false);
		_ogcTimer.start(1000);

		_networkAccessManager->head(QNetworkRequest(QUrl(strAddress)));
	}
}

void MainDialogWeatherTab::replyFinished(QNetworkReply* networkReply)
{
	_ogcTimer.stop();
	_ogcProgressDlg->hide();
	_ogcProgressDlg->deleteLater();

	if(networkReply->error() == QNetworkReply::NoError)
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Os parâmetros de conexão estão corretos!"));
	else
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Erro na conexão com servidor!"));

	networkReply->deleteLater();
}

void MainDialogWeatherTab::ftp_done_slot(bool)
{
	_ftpTimeoutTimer.stop();

	_ftp->disconnect();
	_ftp->deleteLater();
	_ftp = new QFtp();
	connect(_ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftp_finished_command_slot(int, bool)));
	connect(_ftp, SIGNAL(done(bool)), this, SLOT(ftp_done_slot(bool)));

	_progressDlg->reset();
	_progressDlg->deleteLater();
}

void MainDialogWeatherTab::timeoutSlot()
{
	if(_progressBarValue == 10) //se o teste durar 10 segundos, ele é finalizado e um erro é exibido
	{
		ftp_done_slot(true);
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Tempo esgotado. Servidor não respondeu.\nVerifique os parâmetros de conexão."));
	}
	else //senao o tempo para teste ainda nao foi esgotado. A variavel que armazena quantos segundos ja se passaram e tambem a barra de progresso em _progressDlg sao atualizadas
		_progressDlg->setValue(++_progressBarValue);
}

//! Slot chamado quando o usuário cancela um teste de conexão com servidor
void MainDialogWeatherTab::cancelServerCheckConnection()
{
	ftp_done_slot(true);
	QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Verificação cancelada!"));
	_checkConnection = false;
}

void MainDialogWeatherTab::ogcTimeoutSlot()
{
	if(_ogcProgressBarValue == 10) //se o teste durar 10 segundos, ele é finalizado e um erro é exibido
	{
		finishOGCCheckConnection();
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Tempo esgotado. Servidor não respondeu.\nVerifique os parâmetros de conexão."));
	}
	else //senao o tempo para teste ainda nao foi esgotado. A variavel que armazena quantos segundos ja se passaram e tambem a barra de progresso em _ogcProgressBarValue sao atualizadas
		_ogcProgressDlg->setValue(++_ogcProgressBarValue);
}

//! Slot chamado quando o usuário cancela um teste de conexão com servidor OGC
void MainDialogWeatherTab::cancelOGCServerCheckConnection()
{
	finishOGCCheckConnection();
	QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Verificação cancelada!"));
}

void MainDialogWeatherTab::finishOGCCheckConnection()
{
	_networkAccessManager->blockSignals(true);
	_networkAccessManager->deleteLater();
	_networkAccessManager = NULL;

	_ogcTimer.stop();
	_ogcProgressDlg->hide();
	_ogcProgressDlg->deleteLater();
}

void MainDialogWeatherTab::ftp_finished_command_slot(int seq_command, bool error)
{
  (void) seq_command; // Evitar warning
  
	if(!error && ((_ftp->currentCommand() == QFtp::Login && _ui->serverDataBasePath->text().trimmed().isEmpty()) || _ftp->currentCommand() == QFtp::Cd))
	{	//Para um servidor do tipo FTP, os parametros de conexao estao corretos se o login foi realizado com sucesso e nao existe um caminho base
		//ou se existe um caminho base e o comando "cd" foi realizado com sucesso
		_ftpTimeoutTimer.stop();
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Os parâmetros de conexão estão corretos!"));
	}
	else if(error && _ftp->currentCommand() == QFtp::Login)
	{
		_ftpTimeoutTimer.stop();
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Login inválido!"));
	}
	else if(error && _ftp->currentCommand() == QFtp::ConnectToHost)
	{
		_ftpTimeoutTimer.stop();
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Erro na conexão com servidor!"));
	}
	else if(error && _ftp->currentCommand() == QFtp::Cd)
	{
		_ftpTimeoutTimer.stop();
		QMessageBox::warning(_parent, tr("Resultado da verificação:"), tr("Caminho base não existe no servidor!"));
	}
}

//! Slot chamado quando o usuário pressiona o botão de filtro de dados
void MainDialogWeatherTab::filterClicked()
{
  /*FilterDlg dlg;
  dlg.exec();*/

  // O filtro por bandas nao esta implementado para serie de dados WCS. Entao vamos desabilitar a interface, se necessario
  FilterDlgType filterType;
  RemoteWeatherServer* rws = _remoteWeatherServerList->findServerById(_ui->weatherDataTree->currentItem()->parent()->data(0,Qt::UserRole).toInt() -1);
  if(rws && rws->protocol() == WS_PROTOCOL_OGC)
	  filterType = NO_BANDFILTER;
  else
	  filterType = FULLFILTER;
  
  FilterDlg dlg(filterType);
  bool changed = false;

  dlg.setFields(_filter, _remoteWeatherServerList->getFilterThemesEligible());
  
  if (dlg.exec()==QDialog::Accepted)	
  {
    dlg.getFields(&_filter, changed);

	if (changed)
	{
	  if (_ui->weatherPageStack->currentIndex()==2)
        setGridChanged();
	}

	if(_filter.dateBeforeEnabled || _filter.dateAfterEnabled)
		_ui->dateFilterLabel->setText(QString("Sim"));
	else
		_ui->dateFilterLabel->setText(QString("Não"));

	if(_filter.areaFilterType != WS_NoFilter)
		_ui->areaFilterLabel->setText(QString("Sim"));
	else
		_ui->areaFilterLabel->setText(QString("Não"));

	if(_filter.preAnalysisType != WS_PATYPE_NoPreAnalysis)
		_ui->preAnalysisLabel->setText(QString("Sim"));
	else
		_ui->preAnalysisLabel->setText(QString("Não"));

	if(_filter.bandFilterString != "")
		_ui->bandFilterLabel->setText(QString("Sim"));
	else
		_ui->bandFilterLabel->setText(QString("Não"));
  }
}

//! Slot chamado quando o botão de alterar projeções é pressionado
void MainDialogWeatherTab::projectionDialogRequested()
{
  ProjectionDlg dlg;
  bool changed = false;

/*  wsProjectionParams projectionParams;*/

  dlg.setEnabledFields(true);
  dlg.setFields(_projection);
  
  if (dlg.exec()==QDialog::Accepted)		// else QDialog::Rejected
  {
    dlg.getFields(&_projection, changed);

    if (changed)
	  {
      if (_ui->weatherPageStack->currentIndex()==2)
        setGridChanged();
      else if (_ui->weatherPageStack->currentIndex()==3)
        setPointChanged();
	  else if (_ui->weatherPageStack->currentIndex()==4)
	   	setPointDiffChanged();
	  }
  }
}

/*! \brief Indica que algum dos dados do apresentados de Servidor foi alterado.  

Habilita botões de salvar e cancelar (Servidor)
*/
void MainDialogWeatherTab::setServerChanged()
{
  if(_ignoreChangeEvents)
    return;

  _serverChanged = true;

  if (_ui->weatherDataTree->currentItem()->parent() == NULL)
	ctrlChg.idSrv = -1;
  else
	ctrlChg.idSrv = _ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt() -1;

  ctrlChg.idDat = -1;
  ctrlChg.twiItem = _ui->weatherDataTree->currentItem();
  ctrlChg.slMod = _ui->weatherDataTree->selectionModel()->selection();

  ctrlOld.idSrv = -1;
  ctrlOld.idDat = -1;
  ctrlOld.twiItem = NULL;

  back = true;

  _ui->saveBtn->setEnabled(true);
  _ui->cancelBtn->setEnabled(true);
  if(_newServer)
    _parent->statusBar()->showMessage(tr("Novo servidor remoto."));
  else
    _parent->statusBar()->showMessage(tr("Servidor remoto alterado."));

  _ui->serverInsertGridBtn->setEnabled(false);
  _ui->serverInsertPointBtn->setEnabled(false);
  _ui->serverInsertPointDiffBtn->setEnabled(false);
  
  emit editServerStarted();
}

void MainDialogWeatherTab::clearChangeStruct()
{
  ctrlChg.idSrv = -1;
  ctrlChg.idDat = -1;
  ctrlChg.twiItem = NULL;

  ctrlOld.idSrv = -1;
  ctrlOld.idDat = -1;
  ctrlOld.twiItem = NULL;
}

/*! \brief Indica que os dados mostrados de servidor estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar (servidor)
*/
void MainDialogWeatherTab::clearServerChanged()
{
  clearChangeStruct();

  back = false;

  _serverChanged = false;
  _ui->saveBtn->setEnabled(false);
  _ui->cancelBtn->setEnabled(false);
  _ui->serverInsertGridBtn->setEnabled(true);
  _ui->serverInsertPointBtn->setEnabled(true);
  _ui->serverInsertPointDiffBtn->setEnabled(true); 

  changeServerInterface();

  _parent->statusBar()->clearMessage();
  
  emit editServerFinished();
}

/*! \brief Indica que algum dos dados do apresentados de Formato de Dados (grid) foi alterado.  

Habilita botões de salvar e cancelar (Grid)
*/
void MainDialogWeatherTab::setGridChanged()
{
  if(_ignoreChangeEvents)
    return;

  _gridChanged = true;

  ctrlChg.idSrv = _ui->weatherDataTree->currentItem()->parent()->data(0,Qt::UserRole).toInt() -1;
  ctrlChg.idDat = (-_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt()) -1;
  ctrlChg.twiItem = _ui->weatherDataTree->currentItem();
  ctrlChg.slMod = _ui->weatherDataTree->selectionModel()->selection();

  ctrlOld.idSrv = -1;
  ctrlOld.idDat = -1;
  ctrlOld.twiItem = NULL;

  back = true;

  _ui->saveBtn->setEnabled(true);
  _ui->cancelBtn->setEnabled(true);
  if(_newGrid)
    _parent->statusBar()->showMessage(tr("Novo dado tipo grid remoto."));
  else
    _parent->statusBar()->showMessage(tr("Dado tipo grid alterado."));
  
  emit editGridStarted();
}

/*! \brief Indica que os dados mostrados de Formato de Dados (grid) estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar (grid)
*/
void MainDialogWeatherTab::clearGridChanged()
{
  _gridChanged = false;

  clearChangeStruct();
  back = false;

  _ui->saveBtn->setEnabled(false);
  _ui->cancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();
  
  emit editGridFinished();
}

/*! \brief Indica que algum dos dados do apresentados de Formato de Dados (point) foi alterado.  

Habilita botões de salvar e cancelar (Point)
*/
void MainDialogWeatherTab::setPointChanged()
{
  if(_ignoreChangeEvents)
    return;

  _pointChanged = true;
  
  //emit 

  ctrlChg.idSrv = _ui->weatherDataTree->currentItem()->parent()->data(0,Qt::UserRole).toInt() -1;
  ctrlChg.idDat = (-_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt()) -1;
  ctrlChg.twiItem = _ui->weatherDataTree->currentItem();
  ctrlChg.slMod = _ui->weatherDataTree->selectionModel()->selection();

  ctrlOld.idSrv = -1;
  ctrlOld.idDat = -1;
  ctrlOld.twiItem = NULL;

  back = true;

  _ui->saveBtn->setEnabled(true);
  _ui->cancelBtn->setEnabled(true);
  if(_newPoint)
    _parent->statusBar()->showMessage(tr("Novo dado tipo point remoto."));
  else
    _parent->statusBar()->showMessage(tr("Dado tipo point alterado."));
  
  emit editPointStarted();
}

/*! \brief Indica que os dados mostrados de Formato de Dados (point) estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar (point)
*/
void MainDialogWeatherTab::clearPointChanged()
{
  _pointChanged = false;

  clearChangeStruct();
  back = false;

  _ui->saveBtn->setEnabled(false);
  _ui->cancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();
  
  emit editPointFinished();
}

//! Slot chamado quando o usuário clica no botao para inserir um novo servidor
void MainDialogWeatherTab::insertServerRequested()
{
  QTreeWidgetItem* item = new QTreeWidgetItem;

  _ui->connectionProtocol->setEnabled(true);

  setServerChanged();

  ctrlOld.idSrv = ctrlChg.idSrv;
  ctrlOld.idDat = ctrlChg.idDat;
  ctrlOld.twiItem = ctrlChg.twiItem;
  ctrlOld.slMod = ctrlChg.slMod;

  item->setText(0, "New Server");
  item->setIcon(0, QIcon(":/data/icons/server.png"));
  _newServer = true;
  _serverChanged = true;
  clearServerFields();
  _ui->serverName->setText("New Server");

  _ignoreChangeEvents = true;
  _ui->weatherDataTree->currentItem()->addChild(item);
  _ui->weatherDataTree->setCurrentItem(item);
  _ui->weatherPageStack->setCurrentIndex(1);

  ctrlChg.idSrv = _ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt() -1;
  ctrlChg.idDat = -1;
  ctrlChg.twiItem = _ui->weatherDataTree->currentItem();
  ctrlChg.slMod = _ui->weatherDataTree->selectionModel()->selection();

  _ignoreChangeEvents = false;
  showButtons(1);
}

//! Altera a interface para mostrar/ocultar elementos da configuracao de um Servidor OGC
void MainDialogWeatherTab::changeServerInterface()
{
	QTreeWidgetItem* currentItem = _ui->weatherDataTree->currentItem();
	if(_remoteWeatherServerList && currentItem)
	{
		RemoteWeatherServer* rws = NULL;

		// Verificar se o elemento selecionado na arvore de servidores eh realmente um servidor
		// (e nao uma fonte de dados ou o elemento raiz da arvore)
		if(currentItem->parent() && !currentItem->parent()->parent())
			rws = _remoteWeatherServerList->findServerById(_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt() -1);

		// Se o servidor for OGC, ocultar botao para insercao de fonte de dados pontuais de ocorrencias
		if(rws && rws->protocol() == WS_PROTOCOL_OGC)
			_ui->serverInsertPointDiffBtn->setVisible(false);
	}
}

//! Altera a interface para mostrar/ocultar elementos da configuracao de uma serie de dados Grid WCS
void MainDialogWeatherTab::changeGridInterface(bool isWCS)
{
	_ui->gridFormatDataPath->setHidden(isWCS);
	_ui->gridFormatDataPathLabel->setHidden(isWCS);
	_ui->gridFormatDataFormat->setHidden(isWCS);
	_ui->gridFormatDataFormatLabel->setHidden(isWCS);

	// Os campos spbGridGrADSNumBands e spbGridGrADSTimeOffset tambem estao compartilhados entre os formatos GrADS e WCS
	if(isWCS)
	{
		_ui->stkwGridTipoFormato->setVisible(true);
		_ui->stkwGridTipoFormato->setCurrentWidget(_ui->pgGridGrADS);
	}

	_ui->ledGridGrADSArqControle->setHidden(isWCS);
	_ui->ledGridGrADSArqControleLabel->setHidden(isWCS);
	_ui->ledGridGrADSMultiplicador->setHidden(isWCS);
	_ui->ledGridGrADSMultiplicadorLabel->setHidden(isWCS);
	_ui->rbGridGrADSTipoDadosInt->setHidden(isWCS);
	_ui->rbGridGrADSTipoDadosFloat->setHidden(isWCS);
	_ui->rbGridGrADSTipoDadosLabel->setHidden(isWCS);
	_ui->cmbGridGrADSByteOrder->setHidden(isWCS);
	_ui->cmbGridGrADSByteOrderLabel->setHidden(isWCS);
	_ui->spbGridGrADSHeaderSize->setHidden(isWCS);
	_ui->spbGridGrADSHeaderSizeLabel->setHidden(isWCS);
	_ui->spbGridGrADSTraillerSize->setHidden(isWCS);
	_ui->spbGridGrADSTraillerSizeLabel->setHidden(isWCS);

	// O campo para preenchimento de valor dummy so vai aparecer se a fonte for WCS.
	// Se a fonte for de arquivos, esse valor esta no arquivo de controle.
	_ui->ledGridWCSDummy->setVisible(isWCS);
	_ui->ledGridWCSDummyLabel->setVisible(isWCS);
}

//! Altera a interface para mostrar/ocultar elementos da configuracao de uma serie de dados PCD WFS
void MainDialogWeatherTab::changePointInterface(bool isWFS)
{
	_ui->lblPointMask->setEnabled(isWFS);
	_ui->pointFormatDataMask->setEnabled(isWFS);
	_ui->filePointMaskBtn->setEnabled(isWFS);

	_ui->pointFormatDataPath->setHidden(isWFS);
	_ui->pointFormatDataPathLabel->setHidden(isWFS);

	// Exibir botao para definicao do atributo identificador e do atributo temporal
	// se a serie de dados pertencer a um servidor OGC
	_ui->btnPCDWFSConfiguration->setVisible(isWFS);
}

//! Rotina auxiliar de inclusao com as tarefas executadas para os 3 tipos de formato de dados (Grid, Point e Point Diff)
void MainDialogWeatherTab::AuxInsertDataRequested(int type)
{
  int index;
  QTreeWidgetItem* child = new QTreeWidgetItem;

  _collectionPoints.clear();
  _collectionSurfaces.clear();

  ctrlOld.idSrv = ctrlChg.idSrv;
  ctrlOld.idDat = ctrlChg.idDat;
  ctrlOld.twiItem = ctrlChg.twiItem;
  ctrlOld.slMod = ctrlChg.slMod;

  _projection.datum      = WS_DATUM_WGS84;
  _projection.hemisphere = WS_HEMISPHERE_SOUTH;
  _projection.lat0       = 0;
  _projection.lon0       = 0;
  _projection.name       = WS_PROJNAME_NoProjection;
  _projection.offx       = 0;
  _projection.offy       = 0;
  _projection.scale      = 0;
  _projection.stlat1     = 0;
  _projection.stlat2     = 0;

  _filter.dateBeforeEnabled = false;
  _filter.dateBeforeYear = 2000;
  _filter.dateBeforeMonth = 1;
  _filter.dateBeforeDay = 1;
  _filter.dateAfterEnabled = false;
  _filter.dateAfterYear = 2000;
  _filter.dateAfterMonth = 1;
  _filter.dateAfterDay = 1;
  _filter.areaFilterType = WS_NoFilter;
  _filter.xMin = _filter.xMax = _filter.yMin = _filter.yMax = 0;
  _filter.preAnalysisType = WS_PATYPE_NoPreAnalysis;
  _filter.preAnalysisValue = 0;
  _filter.preAnalysisThemeEnabled = false;
  _filter.bandFilterString = "";

  _filter.useNewDummy = false;

  _intersection.rasterIntersection.clear();
  _intersection.vectorIntersection.clear();
  _intersection.dataSourceIntersection.clear();

  if ( type==0 )		// Tipo Grid
  {
	  RemoteWeatherServer* rws = _remoteWeatherServerList->findServerById(_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt() -1);
	  bool isWCS = (rws && rws->protocol() == WS_PROTOCOL_OGC);

	  QString newGridName;
	  if(isWCS)
	  {
		  // Este grid faz parte de um servidor WCS
		  newGridName = "New WCS Grid";
	  }
	  else
	  {
		  // Este grid faz parte de um servidor de arquivos
		  newGridName = "New Grid";
	  }

	  child->setText(0, newGridName);
	  child->setIcon(0, QIcon(":/data/icons/dado_grid.png"));
	  _newGrid = true;
	  _gridChanged = true;
	  clearFormatDataGridFields();
	  _ui->gridFormatDataName->setText(newGridName);
	  _ui->gridFormatDataTimeZoneCmb->setCurrentIndex(12);
	  _ui->gridFormatDataPrefix->setText("");
	  _ui->gridFormatDataUnit->setText("");
	  
	  index = 2;

	  _ui->dateFilterLabel->setText(QString("Não"));
	  _ui->areaFilterLabel->setText(QString("Não"));
	  _ui->preAnalysisLabel->setText(QString("Não"));
	  _ui->bandFilterLabel->setText(QString("Não"));

	  changeGridInterface(isWCS);
  }
  else if ( type==1 )		//	Tipo Point
  {
	  RemoteWeatherServer* rws = _remoteWeatherServerList->findServerById(_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt() -1);
	  bool isWFS = (rws && rws->protocol() == WS_PROTOCOL_OGC);

	  QString newPCDName;
	  if(isWFS)
	  {
		  // Este grid faz parte de um servidor WCS
		  newPCDName = "New WFS PCD Point";
	  }
	  else
	  {
		  // Este grid faz parte de um servidor de arquivos
		  newPCDName = "New PCD Point";
	  }

	  fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
	  child->setText(0, newPCDName);
	  child->setIcon(0, QIcon(":/data/icons/dado_pontual.png"));
	  _newPoint = true;
	  _pointChanged = true;
	  clearFormatDataPointFields();
	  _ui->pointFormatDataName->setText(newPCDName);
	  _ui->pointFormatDataPrefix->setText("");
	  _ui->pointFormatDataUnit->setText("");
	  _ui->pointFormatDataTimeZoneCmb->setCurrentIndex(12);
	  index = 3;

	  changePointInterface(isWFS);
  }
  else if (type == 2) //	Tipo Point Diff
  {
	  child->setText(0, "New Occurrence Point");
	  child->setIcon(0, QIcon(":/data/icons/dado_pontual_diferente.png"));
	  _newPointDiff = true;
	  _pointDiffChanged = true;
	  clearFormatDataPointDiffFields();
	  _ui->pointDiffFormatDataName->setText("New Occurrence Point");
	  _ui->pointDiffFormatDataPrefix->setText("");
	  _ui->pointDiffFormatDataUnit->setText("");
	  _ui->pointDiffFormatDataTimeZoneCmb->setCurrentIndex(12);
	  _ui->dateFilterPointDiffLabel->setText(QString("Não"));
	  index = 4;
  }

  _ignoreChangeEvents = true;
  _ui->weatherDataTree->currentItem()->addChild(child);
  _ui->weatherDataTree->setCurrentItem(child);
  _ui->weatherPageStack->setCurrentIndex(index);

  ctrlChg.idSrv = _ui->weatherDataTree->currentItem()->parent()->data(0,Qt::UserRole).toInt() -1;
  ctrlChg.idDat = (-_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt()) -1;
  ctrlChg.twiItem = _ui->weatherDataTree->currentItem();
  ctrlChg.slMod = _ui->weatherDataTree->selectionModel()->selection();

  _ignoreChangeEvents = false;
}

//! Slot chamado quando o usuário clica no botao para inserir um novo formato de dados tipo Point
void MainDialogWeatherTab::insertPointRequested()
{
	AuxInsertDataRequested(1);
	setPointChanged();
  showButtons(3);
}

//! Slot chamado quando o usuário clica no botao para inserir um novo formato de dados tipo Grid
void MainDialogWeatherTab::insertGridRequested()
{
	AuxInsertDataRequested(0);
	setGridChanged();
  showButtons(2);
}

//! Slot chamado quando o usuário clica no botao para excluir o servidor atual
void MainDialogWeatherTab::removeServerRequested()
{
  // Se o servidor for novo, basta ajustar a interface.  Mesmo procedimento de quando o
  // botão de cancelar é apertado.
  if(_newServer)
  {
    discardChangesNew(true);
    return;
  }  

	QTreeWidgetItem* oldItem = _ui->weatherDataTree->currentItem();
	QTreeWidgetItem* oldParentItem = _ui->weatherDataTree->currentItem()->parent();

	if (oldItem==NULL)
		return;

  // Remove objeto do servidor.  Se não deu certo, retorna e mantém o estado atual
  // Antes, verifca com o usuário se ele deseja realmente remover o servidor remoto
	DeleteWeatherDataSourceDlg dlg;
	dlg.setWindowTitle("Remover servidor remoto...");
	dlg.setQuestionLabel("Deseja realmente remover este servidor remoto ?");

	if(dlg.exec()==QDialog::Rejected)
		return;

	bool deleteLayers = dlg.getDeleteLayersCbxValue();

	int idSrv = oldItem->data(0,Qt::UserRole).toInt()-1;
	bool ok = _remoteWeatherServerList->deleteRemoteWeatherServer(idSrv, deleteLayers);

  if (ok)
  {
    // Remove objeto da lista (interface) e desmarca seleção
    _ui->weatherDataTree->setCurrentItem(oldParentItem);

    _ignoreChangeEvents = true;
    _ui->weatherDataTree->removeItemWidget(oldItem, 0);
    delete oldItem;
    _ignoreChangeEvents = false;

    // Desmarca indicadores de modificação e nova regra
    clearServerChanged();
    _newServer = false;
  }  
}

//! Slot chamado quando o usuário clica no botao para excluir o formato de dados atual (grid ou point)
void MainDialogWeatherTab::removeFormatDataRequested()
{
  QTreeWidgetItem* oldItem = _ui->weatherDataTree->currentItem();
  QTreeWidgetItem* oldParentItem = _ui->weatherDataTree->currentItem()->parent();

  bool ok = true;

  if (oldItem==NULL)
	return;

  if(!_newGrid && !_newPoint && !_newPointDiff)
  {
	// Verifca com o usuário se ele deseja realmente remover o formato de dados
	DeleteWeatherDataSourceDlg dlg;
	
	if(dlg.exec()==QDialog::Rejected)
		return;
	
	bool deleteLayers = dlg.getDeleteLayersCbxValue();

  // Remove objeto do servidor.  Se não deu certo, retorna e mantém o estado atual
    int idSrv, idDat;
	RemoteWeatherServer* rws = NULL;

	idSrv = oldParentItem->data(0,Qt::UserRole).toInt()-1;
	idDat = (-oldItem->data(0,Qt::UserRole).toInt())-1;

	rws =  (_remoteWeatherServerList->findServerById(idSrv));

	ok = rws->getRemoteWeatherServerDataList()->deleteRemoteWeatherServerData(idDat, deleteLayers);
  }

  if (ok)
  {
	// Remove objeto da lista (interface) e desmarca seleção
	_ignoreChangeEvents = true;
	_ui->weatherDataTree->removeItemWidget(oldItem, 0);
	delete oldItem;
	_ignoreChangeEvents = false;

	// Desmarca indicadores de modificação e nova regra
	clearPointChanged();
	clearGridChanged();
	clearPointDiffChanged();

	_newPoint = false;
	_newGrid = false;
	_newPointDiff = false;
	
	if (oldParentItem->parent()!=NULL)
      _ui->weatherDataTree->setCurrentItem(oldParentItem->parent());

	_ui->weatherDataTree->setCurrentItem(oldParentItem);
  }  
}

// Função comentada na classe base
QString MainDialogWeatherTab::verifyAndEnableChangeMsg()
{
  return tr("As alterações efetuadas na tela de dados climáticos\n"
            "ainda não foram salvas.  Deseja salvar as alterações?");
}


//! Slot chamado quando a linha corrente é alterada na árvore de dados climáticos
void MainDialogWeatherTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
  if(_ignoreChangeEvents)
    return;

  QModelIndexList selected_indexes = selected.indexes();
  QModelIndexList oldSelection_indexes = oldSelection.indexes();
  
  // Se usuário clicou na lista fora de qq. item, remarca item anterior
  if(!selected_indexes.count())
  {
    if(oldSelection.indexes().count()) // Evita loop infinito se não existir seleção anterior...
      _ui->weatherDataTree->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    return;
  }
  
  // Obtem a linha selecionada    
  QTreeWidgetItem* currentItem = _ui->weatherDataTree->currentItem();
  int id = currentItem->data(0,Qt::UserRole).toInt();
  int idSrv, idDat;

  if (id<0)
  {
	idSrv = (currentItem->parent()->data(0,Qt::UserRole).toInt())-1;
	idDat = (-id)-1;
  }
  else if (id == 0)
  {
	  ;	// Ver que situacao isto ocorre...
  }
  else
  {
	idSrv = id-1;
	idDat = -1;
  }

  // Verifica se estamos apenas voltando à mesma opção atual.  Ocorre 
  // quando uma troca de regra foi cancelada
  
  if (id<0)
  {
	  if(ctrlChg.idDat == (-id)-1)
		return;
  }
  else
  {
      if(ctrlChg.idSrv == id)
		return;
  }

  if(verifyAndEnableChange(false))
  {
	currentItem = _ui->weatherDataTree->currentItem();
	id = currentItem->data(0,Qt::UserRole).toInt();
    // Operação permitida.  Troca dados na tela
	int		index;
	QString	text = currentItem->text(0);

	if (currentItem->parent()==NULL)
		index = 0;
	else
	{
	  if(id >0)
	  {
		RemoteWeatherServer* rwServer;
		int idServ = id-1;

		rwServer = _remoteWeatherServerList->findServerById( idServ );

		if (rwServer !=NULL)
		{
			index = 1;

			setServerFields(rwServer);
		}
	  }
	  else
	  {
		RemoteWeatherServerData* rwServerData;
		int idServ = currentItem->parent()->data(0,Qt::UserRole).toInt() -1;
		int idData = (-id) -1;

		rwServerData = _remoteWeatherServerList->findServerDataById(idServ, idData );

		if (rwServerData != NULL)
		{
			setDataFields(rwServerData);

			if (rwServerData->geometry() == WS_WDSGEOM_RASTER)  //  WS_WDSGEOM_POINTS, WS_WDSGEOM_RASTER,
			{
				bool isWCS = (rwServerData->format() == WS_WDSFFMT_OGC_WCS);
				changeGridInterface(isWCS);
				index = 2;
			}
			else if (rwServerData->geometry() == WS_WDSGEOM_POINTS)
			{
				bool isWFS = (rwServerData->format() == WS_WDSFFMT_PCD_OGC_WFS);
				changePointInterface(isWFS);
				index = 3;
			}
			else if (rwServerData->geometry() == WS_WDSGEOM_POINTSDIFF)
				index = 4;
		}
	  }
	}

    _ui->weatherPageStack->setCurrentIndex(index);
    showButtons(index); 
  }
  else
  {
    if (ctrlChg.twiItem != NULL)
	{
      // Operação foi cancelada.  Devemos reverter à regra original
	  _ignoreChangeEvents = true;

	  _ui->weatherDataTree->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
	  _ui->weatherDataTree->setCurrentItem(ctrlChg.twiItem);

	  _ignoreChangeEvents = false;
	}
  }    
}

/*! \brief Habilita ou desabilita botões para inserir e remover 
           elementos na árvore de acordo com o tipo de dado 
           selecionado, conforme indicado por index

\param index Indica qual o tipo de dado selecionado na árvore           
             0 = raiz
             1 = servidor
             2 = fonte de dados tipo grid
             3 = fonte de dados pontuais
			 4 = fonte de dados pontuais de diferentes localizacoes
*/
void MainDialogWeatherTab::showButtons(int index) 
{
  _ui->groupBox_25->setHidden(index == 0);

  if(index == 1)
      changeServerInterface();

  _ui->serverDeleteBtn->setVisible(index == 1);
  _ui->serverInsertGridBtn->setVisible(index == 1);
  _ui->serverInsertPointBtn->setVisible(index == 1);
  _ui->serverInsertPointDiffBtn->setVisible(index == 1);
  _ui->exportServerBtn->setVisible(index == 1);
  _ui->updateServerBtn->setVisible(index == 1);
  _ui->importDataBtn->setVisible(index == 1);

  _ui->exportDataGridBtn->setVisible(index == 2);
  _ui->updateDataGridBtn->setVisible(index == 2);
  _ui->gridFormatDataDeleteBtn->setVisible(index == 2);

  _ui->exportDataPointBtn->setVisible(index == 3);
  _ui->updateDataPointBtn->setVisible(index == 3);
  _ui->pointFormatDataDeleteBtn->setVisible(index == 3);

  _ui->exportDataPointDiffBtn->setVisible(index == 4);
  _ui->updateDataPointDiffBtn->setVisible(index == 4);
  _ui->serverRemovePointDiffBtn->setVisible(index == 4);
}


//! Slot chamado quando o usuário clica no botao de mascara
void MainDialogWeatherTab::menuMaskClick(QAction* actMenu)
{
  if (_ui->weatherPageStack->currentIndex()==2)
  {
    _ui->gridFormatDataMask->setText(_ui->gridFormatDataMask->text() + actMenu->text().left(2));
	setGridChanged();
  }
  else if (_ui->weatherPageStack->currentIndex()==3)
  {
    _ui->pointFormatDataMask->setText(_ui->pointFormatDataMask->text() + actMenu->text().left(2));
	  setPointChanged();
  }
  else if (_ui->weatherPageStack->currentIndex()==4)
  {
	  _ui->pointDiffFormatDataMask->setText(_ui->pointDiffFormatDataMask->text() + actMenu->text().left(2));
	  setPointDiffChanged();
  }
}

//! Copia um std::vector<T> para um QVector<T> (QVector::fromStdVector() nao funcionou (?))
template <class T>
void MainDialogWeatherTab::CopyVectorToQVector(const std::vector<T>& vect, QVector<T>& qvect)
{
	qvect.clear();
	for (unsigned i = 0; i< vect.size(); i++)
	{
		T data;
		data = vect.at(i);
		qvect.push_back(data);
	}
}

void MainDialogWeatherTab::insertPointDiffRequested()
{
	AuxInsertDataRequested(2);
	setPointDiffChanged();
	showButtons(4);
}

/*! \brief Indica que algum dos dados do apresentados de Formato de Dados (point diff) foi alterado.  
Habilita botões de salvar e cancelar (Point diff)
*/
void MainDialogWeatherTab::setPointDiffChanged()
{
	if(_ignoreChangeEvents)
		return;

	_pointDiffChanged = true;

	ctrlChg.idSrv = _ui->weatherDataTree->currentItem()->parent()->data(0,Qt::UserRole).toInt() -1;
	ctrlChg.idDat = (-_ui->weatherDataTree->currentItem()->data(0,Qt::UserRole).toInt()) -1;
	ctrlChg.twiItem = _ui->weatherDataTree->currentItem();
	ctrlChg.slMod = _ui->weatherDataTree->selectionModel()->selection();

	ctrlOld.idSrv = -1;
	ctrlOld.idDat = -1;
	ctrlOld.twiItem = NULL;

	back = true;

    _ui->saveBtn->setEnabled(true);
	_ui->cancelBtn->setEnabled(true);
	if(_newPointDiff)
		_parent->statusBar()->showMessage(tr("Novo dado tipo point diferente."));
	else
		_parent->statusBar()->showMessage(tr("Dado tipo point diferente alterado."));

	emit editPointDiffStarted();
}

/*! \brief Indica que os dados mostrados de Formato de Dados (point diff) estão atualizados com o servidor. 
Desabilita os botões de salvar e cancelar (point diff)
*/
void MainDialogWeatherTab::clearPointDiffChanged()
{
	_pointDiffChanged = false;

	clearChangeStruct();
	back = false;

	_ui->saveBtn->setEnabled(false);
	_ui->cancelBtn->setEnabled(false);
	_parent->statusBar()->clearMessage();

	emit editPointDiffFinished();
}

// Grava informacoes de formatos de dados tipo point diff
bool MainDialogWeatherTab::savePointDiff()
{
	return saveDataFormat(WS_WDSGEOM_POINTSDIFF, _newPointDiff);
}

//! Preenche os dados sobre o Formato de dados (point diff) com os dados da interface, que já devem ter sido validados
void MainDialogWeatherTab::getDataPointDiffFields( RemoteWeatherServerData* rwsd )
{
	rwsd->setGeometry(WS_WDSGEOM_POINTSDIFF);		//	WS_WDSGEOM_POINTS,	WS_WDSGEOM_RASTER,

	rwsd->setName(_ui->pointDiffFormatDataName->text().trimmed());
	rwsd->setType((enum wsWeatherDataSourceType)_ui->pointDiffFormatDataType->currentIndex());
	rwsd->setSpatialResolution(1.);
	rwsd->setUpdateFrequencyMins(_ui->pointDiffFormatDataFrequency->text().trimmed().toDouble());
	rwsd->setDetails(_ui->pointDiffFormatDataDescription->toPlainText().trimmed());
	rwsd->setPath(_ui->pointDiffFormatDataPath->text().trimmed());
	rwsd->setMask(_ui->pointDiffFormatDataMask->text().trimmed());
	rwsd->setFormat(indexToFileFormat(_ui->pointDiffFormatDataFormat->currentIndex(), 2));
	rwsd->setProjection(_projection);
	rwsd->setFilter(_filter);
	rwsd->setIntersection(_intersection);
	rwsd->setTimeZone(_ui->pointDiffFormatDataTimeZoneCmb->currentText());
	rwsd->setPrefix(_ui->pointDiffFormatDataPrefix->text().trimmed().replace(" ", "_"));
	rwsd->setUnit(_ui->pointDiffFormatDataUnit->text().trimmed());

	// "Zera" atributos usados apenas pelo grid
	rwsd->setTiff_navigationFile("");
	rwsd->setAsciiGrid_coordUnit(WS_ASCIIGRIDCOORD_DECIMAL_DEGREES);
	rwsd->setGrads_ctlFile("");
	rwsd->setGrads_dataType(WS_GRADSDATATYPE_INTEGER16);
	rwsd->setGrads_multiplier(0.0);
	rwsd->setGrads_swap(WS_GRADSSWAPTYPE_AUTO);
	rwsd->setGrads_numBands(1);
	rwsd->setGrads_bandTimeOffset(0);  
	rwsd->setGrads_bandHeader(0);
	rwsd->setGrads_bandTrailler(0);

	// "Zera" atributos usados apenas pelas PCDs
	std::vector<struct wsPCD> dummy;
	std::vector<struct wsWeatherDataPcdSurface> dummySurfaces;
	rwsd->setPcd_collectionPoints(dummy);
	rwsd->setPcd_collectionRuleLuaScript("");
	rwsd->setPcd_influenceType(WS_WDPIT_RADIUS_TOUCH);
	rwsd->setPcd_influenceThemeId(-1);
	rwsd->setPcd_influenceAttribute("");
	rwsd->setPcd_collectionSurfaces(dummySurfaces);
}

//! Preenche a interface com os dados do formato de dados (tipo point diff)
void MainDialogWeatherTab::setDataPointDiffFields( RemoteWeatherServerData* rwsd )
{
	_ui->pointDiffFormatDataName->setText(rwsd->name());
	_ui->pointDiffFormatDataType->setCurrentIndex(rwsd->type());

	_ui->pointDiffFormatDataFrequency->setText(QString::number(rwsd->updateFrequencyMins()));

	_ui->pointDiffFormatDataTimeZoneCmb->setCurrentIndex(timeZoneToIndex(rwsd->timeZone()));
	_ui->pointDiffFormatDataPrefix->setText(rwsd->prefix());
	_ui->pointDiffFormatDataUnit->setText(rwsd->unit());

	_ui->pointDiffFormatDataDescription->setText(rwsd->details());
	_ui->pointDiffFormatDataPath->setText(rwsd->path());
	_ui->pointDiffFormatDataMask->setText(rwsd->mask());
	_ui->pointDiffFormatDataFormat->setCurrentIndex(fileFormatToIndex(rwsd->format()));

	_collectionPoints.clear();
	_collectionSurfaces.clear();
	_pcdCollectionRuleLuaScript = "";

	_wfsPCDIdAttr = "";
	_wfsPCDTimestampAttr = "";
	_wfsPCDTimestampMask = "";

	_projection = rwsd->projection();
	_filter = rwsd->filter();
	_intersection = rwsd->intersection();

	if(_filter.dateBeforeEnabled || _filter.dateAfterEnabled)
		_ui->dateFilterPointDiffLabel->setText(QString("Sim"));
	else
		_ui->dateFilterPointDiffLabel->setText(QString("Não"));
	
	clearPointDiffChanged();
}

/*! \brief Limpa a interface (somente os campos da tela do formato de dados tipo point diff).  
*/
void MainDialogWeatherTab::clearFormatDataPointDiffFields( void )
{
	_ui->pointDiffFormatDataName->setText("");
	_ui->pointDiffFormatDataType->setCurrentIndex(0);
	_ui->pointDiffFormatDataFrequency->setText("");
	_ui->pointDiffFormatDataDescription->setText("");
	_ui->pointDiffFormatDataPath->setText("");
	_ui->pointDiffFormatDataMask->setText("");
	_ui->pointDiffFormatDataFormat->setCurrentIndex(0);
	_ui->pointDiffFormatDataPrefix->setText("");
	_ui->pointDiffFormatDataUnit->setText("");
	_ui->pointDiffFormatDataTimeZoneCmb->setCurrentIndex(12);
}

void MainDialogWeatherTab::filterPointClicked()
{
	FilterDlg dlg(DATEFILTER);
	bool changed = false;

	dlg.setFields(_filter, _remoteWeatherServerList->getFilterThemesEligible());

	if (dlg.exec()==QDialog::Accepted)	
	{
		dlg.getFields(&_filter, changed);

		if (changed)
		{
			if (_ui->weatherPageStack->currentIndex()==4)
				setPointDiffChanged();
		}

		if(_filter.dateBeforeEnabled || _filter.dateAfterEnabled)
			_ui->dateFilterPointDiffLabel->setText(QString("Sim"));
		else
			_ui->dateFilterPointDiffLabel->setText(QString("Não"));
	}
}

void MainDialogWeatherTab::intersectionClicked()
{
	IntersectionDlg dlg;
	bool changed = false;

	// Recarregamos a lista de grids
	if(_manager->weatherGridList())
	{
		_manager->weatherGridList()->clear();
		_manager->loadGridList();
	}
	
	dlg.setFields(_intersection, _remoteWeatherServerList->getIntersectionThemesEligible(), _manager->weatherGridList());

	if (dlg.exec()==QDialog::Accepted)	
	{
		dlg.getFields(&_intersection, changed);

		if (changed)
		{
			if (_ui->weatherPageStack->currentIndex()==4)
				setPointDiffChanged();
		}		
	}
}

QDomNode MainDialogWeatherTab::openConfigurationFile(QString& configType){

    QString filename = QFileDialog::getOpenFileName(NULL, tr("Escolha uma configuração"),".", tr("Configurações (*.xml)"));
    QDomNode node;

    if(filename.isEmpty()){
        QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), "Selecione um arquivo de configuração.");
        node.clear();
        return node;
    }else{
        QString err;
        ConfigData cfg;
        node = cfg.loadConfig(filename,err,configType);
        if(err!=NULL){
            QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), err);
        }
        return node;
    }
}


void MainDialogWeatherTab::importRemoteWeatherServerRequested(){

    //Abrir arquivo XML e verificar se é do tipo Remote Weather Server Configuration
    QString configType;//Variável para receber o tipo de configuração do arquivo XML
    QDomNode node = openConfigurationFile(configType);

    if (node.isNull()) return;

    if(configType == "Remote Weather Server Configuration"){
        insertServerRequested();
        updateRemoteWeatherServer(node);
    }else{
        QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Esse arquivo é do tipo %1, você deve informar um arquivo do tipo Remote Weather Server Configuration!").arg(configType));
        return;
    }

}


void MainDialogWeatherTab::importDataFieldsRequested(){

    //Abrir arquivo XML e verifica o tipo de dados contidos
    QString configType;//Variável para receber o tipo de configuração do arquivo XML
    QDomNode node = openConfigurationFile(configType);

    if (node.isNull()) return;


    if(configType == "DataGridConfiguration"){

        insertGridRequested();
        updateDataGridFields(node);
        //add filtro para o dia
    }else{
        if(configType == "DataPointConfiguration"){
            insertPointRequested();
            updateDataPointFields(node);
            //add filtro para o dia
        }else{
            if(configType == "DataPointDiffConfiguration"){
                insertPointDiffRequested();
                updateDataPointDiffFields(node);
            }else{
                if(configType == "Weather Server Configuration"){
                    QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Este é um arquivo de configurações de Servidor Remoto, informe um arquivo de configuração de série de dados!"));
                }else{
                    QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Este não é um aquivo de configuração válido!"));
                }
            }
        }
    }
}


void MainDialogWeatherTab::updateRemoteWeatherServerFieldsRequested(){

    //Abrir arquivo XML e verificar se é do tipo Remote Weather Server Configuration
    QString configType;//Variável para receber o tipo de configuração do arquivo XML
    QDomNode node = openConfigurationFile(configType);

    if (node.isNull()) return;

    if(configType == "Remote Weather Server Configuration"){
        updateRemoteWeatherServer(node);
    }else{
        QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Esse arquivo é do tipo %1, você deve informar um arquivo do tipo Remote Weather Server Configuration!").arg(configType));
        return;
    }
}


void MainDialogWeatherTab::updateDataGridFieldsRequested(){

    //Abrir arquivo XML e verificar se é do tipo DATA GRID
    QString configType;//Variável para receber o tipo de configuração do arquivo XML
    QDomNode node = openConfigurationFile(configType);

    if (node.isNull()) return;

    if(configType == "DataGridConfiguration"){
        updateDataGridFields(node);
    }else{
        QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Esse arquivo é do tipo %1, você deve informar um arquivo de dados Grid!").arg(configType));
        return;
    }
}


void MainDialogWeatherTab::updateDataPointFieldsRequested(){

    //Abrir arquivo XML e verificar se é do tipo DATA POINT
    QString configType;//Variável para receber o tipo de configuração do arquivo XML
    QDomNode node = openConfigurationFile(configType);

    if (node.isNull()) return;

    if(configType == "DataPointConfiguration"){
        updateDataPointFields(node);
    }else{
        QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Esse arquivo é do tipo %1, você deve informar um arquivo de Dados Pontuais do tipo PCD!").arg(configType));
        return;
    }
}


void MainDialogWeatherTab::updateDataPointDiffFieldsRequested(){

    //Abrir arquivo XML e verificar se é do tipo DATA POINT
    QString configType;//Variável para receber o tipo de configuração do arquivo XML
    QDomNode node = openConfigurationFile(configType);

    if (node.isNull()) return;

    if(configType == "DataPointDiffConfiguration"){
        updateDataPointDiffFields(node);
    }else{
        QMessageBox::warning(NULL, tr("Erro abrindo configuração..."), tr("Esse arquivo é do tipo %1, você deve informar um arquivo de Dados Pontuais de Ocorrências!").arg(configType));
        return;
    }
}


void MainDialogWeatherTab::exportRemoteWeatherServer(){

    ConfigData::dataRemoteWeatherServer configValues = getRemoteWeatherServerExportFields();

    QString path = QFileDialog::getSaveFileName(NULL , tr("Informe o nome para a configuração"),configValues.serverName + ".xml", tr("Configurações (*.xml)"));

    if(path.isEmpty())
        return;

    if(!path.endsWith(".xml"))
        path.append(".xml");

    ConfigData cfg;
    cfg.exportConfig(configValues,path);

}


void MainDialogWeatherTab::exportDataGridFields(){

    ConfigData::dataGrid configValues = getDataGridExportFields();

    QString path = QFileDialog::getSaveFileName(NULL , tr("Informe o nome para a configuração"),configValues.gridFormatDataName + ".xml", tr("Configurações (*.xml)"));

    if(path.isEmpty())
        return;

    if(!path.endsWith(".xml"))
        path.append(".xml");

    ConfigData cfg;
    cfg.exportConfig(configValues,path);

}


void MainDialogWeatherTab::exportDataPointFields(){

    ConfigData::dataPoint configValues = getDataPointExportFields();

    QString path = QFileDialog::getSaveFileName(NULL , tr("Informe o nome para a configuração"),configValues.pointFormatDataName + ".xml", tr("Configurações (*.xml)"));

    if(path.isEmpty())
        return;

    if(!path.endsWith(".xml"))
        path.append(".xml");

    ConfigData cfg;
    cfg.exportConfig(configValues,path);

}


void MainDialogWeatherTab::exportDataPointDiffFields(){

    ConfigData::dataPointDiff configValues = getDataPointDiffExportFields();

    QString path = QFileDialog::getSaveFileName(NULL , tr("Informe o nome para a configuração"),configValues.pointDiffFormatDataName + ".xml", tr("Configurações (*.xml)"));

    if(path.isEmpty())
        return;

    if(!path.endsWith(".xml"))
        path.append(".xml");

    ConfigData cfg;
    cfg.exportConfig(configValues,path);

}


void MainDialogWeatherTab::updateRemoteWeatherServer(QDomNode node){

    //Carregar dados do arquivo XML na structure
    ConfigData cfg;
    ConfigData::dataRemoteWeatherServer configValues = cfg.loadRemoteWeatherServerConfig(node);

    //Preencher campos da interface com os dados da structure
    setImportedRemoteWeatherServerFields(configValues);

}


void MainDialogWeatherTab::updateDataGridFields(QDomNode node){

    //Carregar dados do arquivo XML na structure dataGrid
    ConfigData cfg;
    ConfigData::dataGrid configValues = cfg.loadDataGridConfig(node);

    //Preencher campos da interface com os dados da structure dataGrid
    setImportedDataGridFields(configValues);

}


void MainDialogWeatherTab::updateDataPointFields(QDomNode node){

    //Carregar dados do arquivo XML na structure dataGrid
    ConfigData cfg;
    ConfigData::dataPoint configValues = cfg.loadDataPointConfig(node);

    //Preencher campos da interface com os dados da structure dataGrid
    setImportedDataPointFields(configValues);

}


void MainDialogWeatherTab::updateDataPointDiffFields(QDomNode node){

    //Carregar dados do arquivo XML na structure dataGrid
    ConfigData cfg;
    ConfigData::dataPointDiff configValues = cfg.loadDataPointDiffConfig(node);

    //Preencher campos da interface com os dados da structure dataGrid
    setImportedDataPointDiffFields(configValues);

}


ConfigData::dataRemoteWeatherServer MainDialogWeatherTab::getRemoteWeatherServerExportFields(){

    ConfigData::dataRemoteWeatherServer configValues;
    configValues.serverName =       _ui->serverName->text().trimmed();
    configValues.description =      _ui->serverDescription->toPlainText().trimmed();
    configValues.adress =           _ui->connectionAddress->text().trimmed();
    configValues.port =             _ui->connectionPort->text().trimmed();
    configValues.protocol =         _ui->connectionProtocol->currentIndex();
    configValues.user =             _ui->connectionUserName->text().trimmed();
    configValues.password =         _ui->connectionPassword->text().trimmed();
    //configValues.active =         (_ui->serverActiveServer->checkState() == Qt::Checked);
    configValues.intervalMinutes =  _ui->serverIntervalData->value();
    configValues.basePath =         _ui->serverDataBasePath->text().trimmed();

    return configValues;

}


ConfigData::dataGrid MainDialogWeatherTab::getDataGridExportFields(){

    ConfigData::dataGrid configValues;

    configValues.gridFormatDataName             = _ui->gridFormatDataName->text().trimmed();
    configValues.gridFormatDataType             = _ui->gridFormatDataType->currentIndex();
    configValues.gridFormatDataResolution       = _ui->gridFormatDataResolution->text().trimmed();
    configValues.gridFormatDataFrequency        = _ui->gridFormatDataFrequency->text().trimmed();
    configValues.gridFormatDataDescription      = _ui->gridFormatDataDescription->toPlainText().trimmed();
    configValues.gridFormatDataPath             = _ui->gridFormatDataPath->text().trimmed();
    configValues.gridFormatDataMask             = _ui->gridFormatDataMask->text().trimmed();
    configValues.gridFormatDataFormat           = _ui->gridFormatDataFormat->currentIndex();
    configValues.gridFormatDataTimeZoneCmb      = _ui->gridFormatDataTimeZoneCmb->currentIndex();
    configValues.gridFormatDataPrefix           = _ui->gridFormatDataPrefix->text().trimmed();//.replace(" ", "_");
    configValues.gridFormatDataUnit             = _ui->gridFormatDataUnit->text().trimmed();
    configValues.ledGridTIFFArqNavegacao        = _ui->ledGridTIFFArqNavegacao->text().trimmed();
    configValues.rbGridAscUnidGrausDec          = _ui->rbGridAscUnidGrausDec->isChecked();
    configValues.rbGridAscUnidGrausMil          = _ui->rbGridAscUnidGrausMil->isChecked();
    configValues.ledGridGrADSArqControle        = _ui->ledGridGrADSArqControle->text().trimmed();
    configValues.rbGridGrADSTipoDadosInt        = _ui->rbGridGrADSTipoDadosInt->isChecked();
    configValues.rbGridGrADSTipoDadosFloat      = _ui->rbGridGrADSTipoDadosFloat->isChecked();
    configValues.ledGridGrADSMultiplicador      = _ui->ledGridGrADSMultiplicador->text();
    configValues.cmbGridGrADSByteOrder          = _ui->cmbGridGrADSByteOrder->currentIndex();
    configValues.spbGridGrADSNumBands           = _ui->spbGridGrADSNumBands->value();
    configValues.spbGridGrADSTimeOffset         = _ui->spbGridGrADSTimeOffset->value();
    configValues.spbGridGrADSHeaderSize         = _ui->spbGridGrADSHeaderSize->value();
    configValues.spbGridGrADSTraillerSize       = _ui->spbGridGrADSTraillerSize->value();
    configValues.ledGridWCSDummy                = _ui->ledGridWCSDummy->text();

    configValues.projection_name                = _projection.name;
    configValues.projection_datum               = _projection.datum;
    configValues.projection_hemisphere          = _projection.hemisphere;
    configValues.projection_lat0                = _projection.lat0;
    configValues.projection_lon0                = _projection.lon0;
    configValues.projection_offx                = _projection.offx;
    configValues.projection_offy                = _projection.offy;
    configValues.projection_scale               = _projection.scale;
    configValues.projection_stlat1              = _projection.stlat1;
    configValues.projection_stlat2              = _projection.stlat2;
    configValues.projection_units               = QString::fromStdString(_projection.units);
/*
    configValues.intersection_rasterIntersection        = _intersection.rasterIntersection;
    configValues.intersection_vectorIntersection        = _intersection.vectorIntersection;
    configValues.intersection_dataSourceIntersection    = _intersection.dataSourceIntersection;
*/
    return configValues;

}


ConfigData::dataPoint MainDialogWeatherTab::getDataPointExportFields(){

    ConfigData::dataPoint configValues;

    configValues.pointFormatDataName            = _ui->pointFormatDataName->text().trimmed();
    configValues.pointFormatDataType            = _ui->pointFormatDataType->currentIndex();
    configValues.pointFormatDataRadius          = _ui->pointFormatDataRadius->text().trimmed();
    configValues.pointFormatDataFrequency       = _ui->pointFormatDataFrequency->text().trimmed();
    configValues.pointFormatDataDescription     = _ui->pointFormatDataDescription->toPlainText().trimmed();
    configValues.pointFormatDataPath            = _ui->pointFormatDataPath->text().trimmed();
    configValues.pointFormatDataMask            = _ui->pointFormatDataMask->text().trimmed();
    configValues.pointFormatDataPrefix          = _ui->pointFormatDataPrefix->text().trimmed().replace(" ", "_");
    configValues.pointFormatDataUnit            = _ui->pointFormatDataUnit->text().trimmed();
    configValues.pointFormatDataTimeZoneCmb     = _ui->pointFormatDataTimeZoneCmb->currentIndex();
    configValues.pointFormatDataFormat          = _ui->pointFormatDataFormat->currentIndex();

/*    configValues.pointFormatDataInfluenceCmb    = _ui->pointFormatDataInfluenceCmb->currentIndex();
    configValues.pointFormatDataThemeCmb        = _ui->pointFormatDataThemeCmb->currentIndex();
    configValues.pointFormatDataAttributeCmb    = _ui->pointFormatDataAttributeCmb->currentIndex();
*/
    configValues.projection_name                = _projection.name;
    configValues.projection_datum               = _projection.datum;
    configValues.projection_hemisphere          = _projection.hemisphere;
    configValues.projection_lat0                = _projection.lat0;
    configValues.projection_lon0                = _projection.lon0;
    configValues.projection_offx                = _projection.offx;
    configValues.projection_offy                = _projection.offy;
    configValues.projection_scale               = _projection.scale;
    configValues.projection_stlat1              = _projection.stlat1;
    configValues.projection_stlat2              = _projection.stlat2;
    configValues.projection_units               = QString::fromStdString(_projection.units);

    ConfigData::pointParams pcd;
    wsPCD strPCD;
    for (int i = 0; i < _collectionPoints.size(); i++)
    {
        strPCD          = _collectionPoints.at(i);
        pcd.fileName    = QString::fromStdString(strPCD.fileName);
        pcd.latitude    = QString::number(strPCD.latitude,'g',15);
        pcd.longitude   = QString::number(strPCD.longitude,'g',15);
        pcd.isActive    = strPCD.isActive;
        //itemActive    = new QTableWidgetItem(strPCD.isActive ? tr("Sim") : tr("Não"));
        configValues.pt.push_back(pcd);
    }

    return configValues;

}


ConfigData::dataPointDiff MainDialogWeatherTab::getDataPointDiffExportFields(){

    ConfigData::dataPointDiff configValues;

    configValues.pointDiffFormatDataName        =_ui->pointDiffFormatDataName->text().trimmed();
    configValues.pointDiffFormatDataType        =_ui->pointDiffFormatDataType->currentIndex();
    configValues.pointDiffFormatDataFrequency   =_ui->pointDiffFormatDataFrequency->text().trimmed();
    configValues.pointDiffFormatDataDescription =_ui->pointDiffFormatDataDescription->toPlainText().trimmed();
    configValues.pointDiffFormatDataPath        =_ui->pointDiffFormatDataPath->text().trimmed();
    configValues.pointDiffFormatDataMask        =_ui->pointDiffFormatDataMask->text().trimmed();
    configValues.pointDiffFormatDataFormat      =_ui->pointDiffFormatDataFormat->currentIndex();
    configValues.pointDiffFormatDataTimeZoneCmb =_ui->pointDiffFormatDataTimeZoneCmb->currentIndex();
    configValues.pointDiffFormatDataPrefix      =_ui->pointDiffFormatDataPrefix->text().trimmed().replace(" ", "_");
    configValues.pointDiffFormatDataUnit        =_ui->pointDiffFormatDataUnit->text().trimmed();

    configValues.projection_name                = _projection.name;
    configValues.projection_datum               = _projection.datum;
    configValues.projection_hemisphere          = _projection.hemisphere;
    configValues.projection_lat0                = _projection.lat0;
    configValues.projection_lon0                = _projection.lon0;
    configValues.projection_offx                = _projection.offx;
    configValues.projection_offy                = _projection.offy;
    configValues.projection_scale               = _projection.scale;
    configValues.projection_stlat1              = _projection.stlat1;
    configValues.projection_stlat2              = _projection.stlat2;
    configValues.projection_units               = QString::fromStdString(_projection.units);

    return configValues;

}


void MainDialogWeatherTab::setImportedRemoteWeatherServerFields(ConfigData::dataRemoteWeatherServer configValues){

    _ui->serverName->setText(configValues.serverName);
    _ui->serverDescription->setText(configValues.description);
    _ui->connectionAddress->setText(configValues.adress);
    _ui->connectionPort->setText(configValues.port);
    _ui->connectionProtocol->setCurrentIndex(configValues.protocol);
    _ui->connectionUserName->setText(configValues.user);
    _ui->connectionPassword->setText(configValues.password);
    _ui->serverActiveServer->setCheckState(Qt::Unchecked);
    _ui->serverIntervalData->setValue(configValues.intervalMinutes);
    _ui->serverDataBasePath->setText(configValues.basePath);

}


void MainDialogWeatherTab::setImportedDataGridFields(ConfigData::dataGrid configValues){

    _ui->gridFormatDataName->setText(configValues.gridFormatDataName);
    _ui->gridFormatDataType->setCurrentIndex(configValues.gridFormatDataType);
    _ui->gridFormatDataResolution->setText(configValues.gridFormatDataResolution);
    _ui->gridFormatDataFrequency->setText(configValues.gridFormatDataFrequency);
    _ui->gridFormatDataDescription->setText(configValues.gridFormatDataDescription);
    _ui->gridFormatDataPath->setText(configValues.gridFormatDataPath);
    _ui->gridFormatDataMask->setText(configValues.gridFormatDataMask);
    _ui->gridFormatDataFormat->setCurrentIndex(configValues.gridFormatDataFormat);
    _ui->gridFormatDataTimeZoneCmb->setCurrentIndex(configValues.gridFormatDataTimeZoneCmb);
    _ui->gridFormatDataPrefix->setText(configValues.gridFormatDataPrefix);
    _ui->gridFormatDataUnit->setText(configValues.gridFormatDataUnit);
    _ui->ledGridTIFFArqNavegacao->setText(configValues.ledGridTIFFArqNavegacao);
    _ui->rbGridAscUnidGrausDec->setChecked(configValues.rbGridAscUnidGrausDec);
    _ui->rbGridAscUnidGrausMil->setChecked(configValues.rbGridAscUnidGrausMil);
    _ui->ledGridGrADSArqControle->setText(configValues.ledGridGrADSArqControle);
    _ui->rbGridGrADSTipoDadosInt->setChecked(configValues.rbGridGrADSTipoDadosInt);
    _ui->rbGridGrADSTipoDadosFloat->setChecked(configValues.rbGridGrADSTipoDadosFloat);
    _ui->ledGridGrADSMultiplicador->setText(configValues.ledGridGrADSMultiplicador);
    _ui->cmbGridGrADSByteOrder->setCurrentIndex(configValues.cmbGridGrADSByteOrder);
    _ui->spbGridGrADSNumBands->setValue(configValues.spbGridGrADSNumBands);
    _ui->spbGridGrADSTimeOffset->setValue(configValues.spbGridGrADSTimeOffset);
    _ui->spbGridGrADSHeaderSize->setValue(configValues.spbGridGrADSHeaderSize);
    _ui->spbGridGrADSTraillerSize->setValue(configValues.spbGridGrADSTraillerSize);
    _ui->ledGridWCSDummy->setText(configValues.ledGridWCSDummy);

    _projection.datum      = (wsDatum) configValues.projection_datum;
    _projection.hemisphere = configValues.projection_hemisphere ? WS_HEMISPHERE_SOUTH : WS_HEMISPHERE_NORTH;
    _projection.lat0       = configValues.projection_lat0;
    _projection.lon0       = configValues.projection_lon0;
    _projection.name       = (wsProjectionName) configValues.projection_name;
    _projection.offx       = configValues.projection_offx;
    _projection.offy       = configValues.projection_offy;
    _projection.scale      = configValues.projection_scale;
    _projection.stlat1     = configValues.projection_stlat1;
    _projection.stlat2     = configValues.projection_stlat2;
    _projection.units      = configValues.projection_units.toStdString();

}


void MainDialogWeatherTab::setImportedDataPointFields(ConfigData::dataPoint configValues){

    _ui->pointFormatDataName->setText(configValues.pointFormatDataName);
    _ui->pointFormatDataType->setCurrentIndex(configValues.pointFormatDataType);
    _ui->pointFormatDataRadius->setText(configValues.pointFormatDataRadius);
    _ui->pointFormatDataFrequency->setText(configValues.pointFormatDataFrequency);
    _ui->pointFormatDataDescription->setText(configValues.pointFormatDataDescription);
    _ui->pointFormatDataPath->setText(configValues.pointFormatDataPath);
    _ui->pointFormatDataMask->setText(configValues.pointFormatDataMask);
    _ui->pointFormatDataFormat->setCurrentIndex(configValues.pointFormatDataFormat);
    _ui->pointFormatDataPrefix->setText(configValues.pointFormatDataPrefix);
    _ui->pointFormatDataUnit->setText(configValues.pointFormatDataUnit);
    _ui->pointFormatDataTimeZoneCmb->setCurrentIndex(configValues.pointFormatDataTimeZoneCmb);

/*
    _ui->pointFormatDataInfluenceCmb->setCurrentIndex(configValues.pointFormatDataInfluenceCmb);
    _ui->pointFormatDataThemeCmb->setCurrentIndex(configValues.pointFormatDataThemeCmb);
    _ui->pointFormatDataAttributeCmb->setCurrentIndex(configValues.pointFormatDataAttributeCmb);
*/

    _projection.datum      = (wsDatum) configValues.projection_datum;
    _projection.hemisphere = configValues.projection_hemisphere ? WS_HEMISPHERE_SOUTH : WS_HEMISPHERE_NORTH;
    _projection.lat0       = configValues.projection_lat0;
    _projection.lon0       = configValues.projection_lon0;
    _projection.name       = (wsProjectionName) configValues.projection_name;
    _projection.offx       = configValues.projection_offx;
    _projection.offy       = configValues.projection_offy;
    _projection.scale      = configValues.projection_scale;
    _projection.stlat1     = configValues.projection_stlat1;
    _projection.stlat2     = configValues.projection_stlat2;
    _projection.units      = configValues.projection_units.toStdString();

    wsPCD newColPoint;
    ConfigData::pointParams pcd;
    bool duplicate;
    wsPCD strPCD;


    for (int i = 0; i < configValues.pt.size(); i++)
    {
        duplicate = false;
        pcd                   = configValues.pt.at(i);
        newColPoint.fileName  = pcd.fileName.toStdString() ;
        newColPoint.latitude  = pcd.latitude.toDouble();
        newColPoint.longitude = pcd.longitude.toDouble();
        newColPoint.isActive  = pcd.isActive;


        for (int i = 0; i < _collectionPoints.size(); i++)
        {
            strPCD          = _collectionPoints.at(i);
            if(newColPoint.fileName == strPCD.fileName)
                duplicate = true;
        }

        if(!duplicate)
        _collectionPoints.push_back(newColPoint);
    }
    fillTableCollectionPoints(_ui->tblPointPCDFileNameLocation, _ui->btnPointPCDDeleteFileNameLocation);
}


void MainDialogWeatherTab::setImportedDataPointDiffFields(ConfigData::dataPointDiff configValues){

    _ui->pointDiffFormatDataName->setText(configValues.pointDiffFormatDataName);
    _ui->pointDiffFormatDataType->setCurrentIndex(configValues.pointDiffFormatDataType);
    _ui->pointDiffFormatDataFrequency->setText(configValues.pointDiffFormatDataFrequency);
    _ui->pointDiffFormatDataTimeZoneCmb->setCurrentIndex(configValues.pointDiffFormatDataTimeZoneCmb);
    _ui->pointDiffFormatDataPrefix->setText(configValues.pointDiffFormatDataPrefix);
    _ui->pointDiffFormatDataUnit->setText(configValues.pointDiffFormatDataUnit);
    _ui->pointDiffFormatDataDescription->setText(configValues.pointDiffFormatDataDescription);
    _ui->pointDiffFormatDataPath->setText(configValues.pointDiffFormatDataPath);
    _ui->pointDiffFormatDataMask->setText(configValues.pointDiffFormatDataMask);
    _ui->pointDiffFormatDataFormat->setCurrentIndex(configValues.pointDiffFormatDataFormat);

    _projection.datum      = (wsDatum) configValues.projection_datum;
    _projection.hemisphere = configValues.projection_hemisphere ? WS_HEMISPHERE_SOUTH : WS_HEMISPHERE_NORTH;
    _projection.lat0       = configValues.projection_lat0;
    _projection.lon0       = configValues.projection_lon0;
    _projection.name       = (wsProjectionName) configValues.projection_name;
    _projection.offx       = configValues.projection_offx;
    _projection.offy       = configValues.projection_offy;
    _projection.scale      = configValues.projection_scale;
    _projection.stlat1     = configValues.projection_stlat1;
    _projection.stlat2     = configValues.projection_stlat2;
    _projection.units      = configValues.projection_units.toStdString();

}
