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
  \file terrama2/gui/config/MainDialogAnalysisTab.cpp

  \brief Definition of Class MainDialogAnalysisTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>

// QT  
#include <QMessageBox>

// TerraMA2
#include "MainDialogAnalysisTab.hpp"
#include "WizardDialog.hpp"
#include "CrossDialog.hpp"
#include "AlertIconDialog.hpp"
#include "GridListDialog.hpp"
#include "AnalysisConditionDialog.hpp"
#include "AnalysisFillCellOpDialog.hpp"
#include "Services.hpp"
#include "AnalysisList.hpp"
#include "RiskMapList.hpp"
#include "AnalysisCreateLuaDialog.hpp"
#include "CellularSpaceList.hpp"
#include "WeatherGridList.hpp"
#include "ViewList.hpp"
#include "RemoteWeatherServerDataList.hpp"
#include "RemoteWeatherServerList.hpp"
#include "RemoteWeatherServerData.hpp"
#include "AdditionalMapList.hpp"
#include "GridConfigDialog.hpp"
#include "AlertIconList.hpp"
#include "LuaSyntaxHighlighter.hpp"
#include "LuaUtils.hpp"
#include "QtUtils.hpp"
#include "Utils.hpp"

//! Construtor.  Prepara interface e estabelece conexões
MainDialogAnalysisTab::MainDialogAnalysisTab(MainDlg* main_dialog, Services* manager)
  : MainDlgTab(main_dialog, manager)
{
  _anaList         = NULL;
  _riskMapList     = NULL;
  _cellularSpaceList = NULL;
  _weatherGridList = NULL;
  _pcdList         = NULL;
  _ruleChanged = false;
  _newRule     = false;
  _ignoreChangeEvents = false;
  _currentRuleIndex    = -1;
  _numAnalysisModules  = 0;
  _conditionAnalysisId = -1;
  
  resetGridOutputConfig();

  resetAlertIcons();

  // Conecta sinais tratados pela classe
  // Para monitorar a mudança de regra na lista de regras de análise, estamos
  // usando um sinal do modelo de seleção e não o tradicional currentRowChanged()
  // Isso é feito pois em currentRowChanged() não conseguimos voltar para a seleção
  // anterior caso o usuário deseje cancelar a troca.
  connect(_ui->analysisListWidget->selectionModel(), 
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));

#ifdef _DEBUG
  connect(_ui->analysisListWidget, SIGNAL(itemDoubleClicked ( QListWidgetItem * ) ), SLOT( analysisDoubleClicked(QListWidgetItem *)));
#endif // _DEBUG
  
  connect(_ui->analysisCreateLuaBtn,       SIGNAL(clicked()), SLOT(wizardCreateLua()));
  connect(_ui->analysisWizardBtn,       SIGNAL(clicked()), SLOT(wizardRequested()));
  connect(_ui->analysisSyntaxBtn,       SIGNAL(clicked()), SLOT(syntaxCheckRequested()));
  connect(_ui->analysisCrossBtn,        SIGNAL(clicked()), SLOT(crossRequested()));
  connect(_ui->analysisPcdCrossBtn,     SIGNAL(clicked()), SLOT(crossRequested()));
  connect(_ui->analysisAlertIconsBtn,   SIGNAL(clicked()), SLOT(alertIconRequested()));
  connect(_ui->analysisRiskAddGridBtn,  SIGNAL(clicked()), SLOT(addGridRequested()));
  connect(_ui->analysisModelAddGridBtn, SIGNAL(clicked()), SLOT(addGridRequested()));
  connect(_ui->analysisTerraMEAddGridBtn,	SIGNAL(clicked()), SLOT(addGridRequested()));
  connect(_ui->analysisRiskRemoveGridBtn,	SIGNAL(clicked()), SLOT(removeGridRequested()));
  connect(_ui->analysisModelRemoveGridBtn,	SIGNAL(clicked()), SLOT(removeGridRequested()));
  connect(_ui->analysisTerraMERemoveGridBtn,SIGNAL(clicked()), SLOT(removeGridRequested()));
  connect(_ui->analysisModelGridOutputConfigBtn, SIGNAL(clicked()), SLOT(configOutputGridRequested()));
  connect(_ui->analysisRiskCmb,         SIGNAL(currentIndexChanged(int)), SLOT(riskMapChanged(int)));
  connect(_ui->analysisPCDCmb,          SIGNAL(currentIndexChanged(int)), SLOT(PCDChanged(int)));
	connect(_ui->analysisViewCmb,          SIGNAL(currentIndexChanged(int)), SLOT(ViewChanged(int)));
  connect(_ui->analysisCellularSpaceCmb,SIGNAL(currentIndexChanged(int)), SLOT(cellularSpaceChanged(int)));
  connect(_ui->analysisFillCellOpBtn,   SIGNAL(clicked()), SLOT(fCellOpRequested()));

  // Conecta sinais para adicionar e remover análises
  connect(_ui->analysisAddRiskBtn,		SIGNAL(clicked()), SLOT(addNewAnalysisRequested()));
  connect(_ui->analysisAddModelBtn,		SIGNAL(clicked()), SLOT(addNewAnalysisRequested()));
  connect(_ui->analysisAddTerraMEBtn,	SIGNAL(clicked()), SLOT(addNewAnalysisRequested()));
  connect(_ui->analysisAddPCDBtn,   	SIGNAL(clicked()), SLOT(addNewAnalysisRequested()));
  connect(_ui->analysisRemoveBtn,		SIGNAL(clicked()), SLOT(removeAnalysisRequested()));
  
  // Conecta sinais tratados de maneira generica por MainDlgTab
  connect(_ui->analysisSaveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  connect(_ui->analysisCancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

  // Conecta sinais para detectar dados alterados
  connect(_ui->analysisNameLed,        SIGNAL(textEdited(const QString&)), SLOT(setRuleChanged()));
  connect(_ui->analysisAuthorLed,      SIGNAL(textEdited(const QString&)), SLOT(setRuleChanged()));
  connect(_ui->analysisInstitutionLed, SIGNAL(textEdited(const QString&)), SLOT(setRuleChanged()));
  connect(_ui->analysisDescriptionTed, SIGNAL(textChanged()), SLOT(setRuleChanged()));
  connect(_ui->analysisScriptTed,      SIGNAL(textChanged()), SLOT(setRuleChanged()));
  connect(_ui->analysisOutputLed,      SIGNAL(textEdited(const QString&)), SLOT(setRuleChanged()));
  connect(_ui->analysisCreateImageCbx, SIGNAL(stateChanged(int)), SLOT(setRuleChanged()));
  
  connect(_ui->analysisActiveRdb,      SIGNAL(toggled(bool)), SLOT(setRuleChanged()));
  connect(_ui->analysisInactiveRdb,    SIGNAL(toggled(bool)), SLOT(setRuleChanged()));
  connect(_ui->analysisConditionalRdb, SIGNAL(toggled(bool)), SLOT(setRuleChanged()));
  connect(_ui->analysisLoadCmb,        SIGNAL(currentIndexChanged(int)), SLOT(setRuleChanged()));

  connect(_ui->analysisConditionBtn, SIGNAL(clicked()), SLOT(conditionChangeRequested()));
  connect(_ui->analysisConditionalRdb, SIGNAL(toggled(bool)), _ui->analysisConditionBtn, SLOT(setEnabled(bool)));


  // Menus associados a botões
  QMenu* menuMask = new QMenu(tr("Máscaras"), _parent);
  menuMask->addAction(tr("%a - ano com dois digitos"));
  menuMask->addAction(tr("%A - ano com quatro digitos"));
  menuMask->addAction(tr("%d - dia com dois digitos"));
  menuMask->addAction(tr("%M - mes com dois digitos"));
  menuMask->addAction(tr("%h - hora com dois digitos"));
  menuMask->addAction(tr("%m - minuto com dois digitos"));
  menuMask->addAction(tr("%s - segundo com dois digitos"));

  _ui->fileAnaGridMaskBtn->setMenu(menuMask);
  _ui->fileAnaGridMaskBtn->setPopupMode(QToolButton::InstantPopup);

  connect(menuMask,		SIGNAL(triggered(QAction*)), SLOT(menuMaskClick(QAction*)));

  // Preenche menu de operadores zonais
  fillZoneOperatorMenu();
  fillPCDZoneOperatorMenu();
  fillSampleOperatorMenu();
  fillAlertLevelsMenu();
  fillFunctionsMenu();
  fillCommandsMenu();
  fillTerraMEFunctionsMenu();
  
  // Adiciona syntax highlighter ao texto do script
  new LuaSyntaxHighlighter(_ui->analysisScriptTed);
}

void MainDialogAnalysisTab::deleteMapListInCombo(const int& index, const wsRiskMap& data)
{
  _ui->analysisRiskCmb->removeItem(index+1);
}

void MainDialogAnalysisTab::insertMapListInCombo(const int& index, const wsRiskMap& data)
{
    _ui->analysisRiskCmb->insertItem(index+1, QIcon(":/data/icons/plano de risco.png"), QString::fromStdString(data.name));
}

void MainDialogAnalysisTab::updateMapListInCombo(const int& index, const wsRiskMap& data)
{
  _ui->analysisRiskCmb->setItemText(index+1, QString::fromStdString(data.name));
}

void MainDialogAnalysisTab::deleteCellularSpaceListInCombo(const int& index)
{
	_ui->analysisCellularSpaceCmb->removeItem(index+1);
}

void MainDialogAnalysisTab::insertCellularSpaceListInCombo(const int& index, const wsCellularSpace& data)
{
	_ui->analysisCellularSpaceCmb->insertItem(index+1, QIcon(":/data/icons/espaco_celular.png"), QString::fromStdString(data.name));
}

void MainDialogAnalysisTab::updateCellularSpaceListInCombo(const int& index, const wsCellularSpace& data)
{
	_ui->analysisCellularSpaceCmb->setItemText(index+1, QString::fromStdString(data.name));
}

void MainDialogAnalysisTab::updateWeatherDataSourceInList(int id, QString newName)
{
  for (int i = 0; i < _ui->analysisGridListWidget->count(); i++)
  {
	if(_ui->analysisGridListWidget->item(i)->data(Qt::UserRole).toInt() == id)
	{
	  _ui->analysisGridListWidget->item(i)->setData(Qt::DisplayRole, newName);
	}
  }
  for (int i = 0; i < _ui->analysisModelGridListWidget->count(); i++)
  {
	  if(_ui->analysisModelGridListWidget->item(i)->data(Qt::UserRole).toInt() == id)
	  {
		  _ui->analysisModelGridListWidget->item(i)->setData(Qt::DisplayRole, newName);
	  }
  }
  for (int i = 0; i < _ui->analysisTerraMEListWidget->count(); i++)
  {
	  if(_ui->analysisTerraMEListWidget->item(i)->data(Qt::UserRole).toInt() == id)
	  {
		  _ui->analysisTerraMEListWidget->item(i)->setData(Qt::DisplayRole, newName);
	  }
  }
  for(int i = 0; i < _anaList->count(); i++)
  {
	  for(int j = 0; j < _anaList->at(i)->getWeatherGrids().count(); j++)
	  {
		  if(_anaList->at(i)->getWeatherGrids().at(j)->id == id)
		  {
			  _anaList->at(i)->getWeatherGrids().at(j)->name = newName.toStdString();
		  }
	  }
  }
}

void MainDialogAnalysisTab::updatePCDList()
{
  _pcdList->reloadGridList();

  _ui->analysisPCDCmb->clear();
  _ui->analysisPCDCmb->addItem(tr("Selecione o PCD..."));

  // Preenche a lista de PCDs.
  for(int i=0, count=(int)_pcdList->count(); i<count; i++)
  {
    WeatherGrid* pcd = _pcdList->at(i);
    _ui->analysisPCDCmb->addItem(QIcon(":/data/icons/dado_pontual.png"), pcd->name());
  }
}

//! Destrutor
MainDialogAnalysisTab::~MainDialogAnalysisTab()
{
}

// Funcao comentada na classe base
void MainDialogAnalysisTab::load()
{
  // Carrega novas informações
  _anaList         = _manager->analysisList();
  _riskMapList     = _manager->riskMapList();
  _cellularSpaceList = _manager->cellularSpaceList();
  _weatherGridList = _manager->weatherGridList();
  _pcdList         = _manager->pcdList();
  _viewList        = _manager->viewList();
  _alertIconList   = _manager->alertIconList();

  assert(_anaList && _riskMapList && _weatherGridList && _cellularSpaceList && _pcdList);
  
  // Obtem número de instâncias do módulo de análise ativas
  _numAnalysisModules = _anaList->numInstances();
  fillInstanceList();

  connect(_riskMapList, SIGNAL(beforeDeleteRiskMapList(const int&, const wsRiskMap&)), SLOT(deleteMapListInCombo(const int&, const wsRiskMap&)));
  connect(_riskMapList, SIGNAL(afterUpdateRiskMapList(const int&, const wsRiskMap&)), SLOT(updateMapListInCombo(const int&, const wsRiskMap&)));
  connect(_riskMapList, SIGNAL(afterInsertRiskMapList(const int&, const wsRiskMap&)), SLOT(insertMapListInCombo(const int&, const wsRiskMap&)));

  connect(_cellularSpaceList, SIGNAL(beforeDeleteCellularSpaceList(const int&)), SLOT(deleteCellularSpaceListInCombo(const int&)));
  connect(_cellularSpaceList, SIGNAL(afterUpdateCellularSpaceList (const int&, const wsCellularSpace&)), SLOT(updateCellularSpaceListInCombo(const int&, const wsCellularSpace&)));
  connect(_cellularSpaceList, SIGNAL(afterInsertCellularSpaceList (const int&, const wsCellularSpace&)), SLOT(insertCellularSpaceListInCombo(const int&, const wsCellularSpace&)));

  connect(_pcdList, SIGNAL(listChanged()), SLOT(updatePCDList()));
  
  for(int i = 0; i < _manager->remoteWeatherServerList()->count(); i++)
  {
	  for (int j = 0; j < _manager->remoteWeatherServerList()->at(i)->getRemoteWeatherServerDataList()->count(); j++)
	  {
		connect(_manager->remoteWeatherServerList()->at(i)->getRemoteWeatherServerDataList(),
					   SIGNAL(afterUpdateRWSData(int, QString)), SLOT(updateWeatherDataSourceInList(int, QString)));
	  }
  }

  connect(_manager->additionalMapList(), 
	  SIGNAL(afterUpdateAdditionalMap(int, QString)), 
	  SLOT(updateWeatherDataSourceInList(int, QString)));

  // Limpa dados
  clearFields(true);
  _currentRuleIndex   = -1;

  // Preenche a lista de objetos monitorados. Entrada para "selecionar objeto monitorado"
  // já foi incluida e selecionda por clearFields()
  if(_riskMapList->count()) 
  {
    for(int i=0, count=(int)_riskMapList->count(); i<count; i++)
    {
      RiskMap* map = _riskMapList->at(i);
      _ui->analysisRiskCmb->addItem(QIcon(":/data/icons/plano de risco.png"), map->name());
    }
  }

  // Preenche a lista de PCDs. Entrada para "selecionar PCD"
  // já foi incluida e selecionda por clearFields()
  for(int i=0, count=(int)_pcdList->count(); i<count; i++)
  {
    WeatherGrid* pcd = _pcdList->at(i);
    _ui->analysisPCDCmb->addItem(QIcon(":/data/icons/dado_pontual.png"), pcd->name());
  }

	// Preenche a lista de PCDs. Entrada para "selecionar PCD"
  // já foi incluida e selecionda por clearFields()
  for(int i=0, count=(int)_viewList->count(); i<count; i++)
  {
    View* view = _viewList->at(i);
    _ui->analysisViewCmb->addItem(/* QIcon(":/data/icons/dado_pontual.png"), */ view->name());
  }

  // Preenche a lista de espaços celulares. Entrada "Selecione o espaço celular..."
  // já foi incluida e selecionda por clearFields()
  if(_cellularSpaceList->count())
  {
	  for(int i=0, count=(int)_cellularSpaceList->count(); i<count; i++)
	  {
		  CellularSpace* cs = _cellularSpaceList->at(i);
		  _ui->analysisCellularSpaceCmb->addItem(QIcon(":/data/icons/espaco_celular.png"), cs->name());
	  }
  }

  // Preenche lista de regras e mostra dados da primeira regra
  if(_anaList->count()) 
  {
    enableFields(true);
	wsAnalysisType anaType;

    for(int i=0, count=(int)_anaList->count(); i<count; i++)
    {
      Analysis* ana = _anaList->at(i);
      QString ico;
	  anaType = ana->getAnalysisType();
	  
	  if(anaType == WS_ANALYSISTYPE_RISK)
          ico = ":/data/icons/analise_plano de risco.png";
	  else if(anaType == WS_ANALYSISTYPE_MODEL)
		  ico = ":/data/icons/analizemodel.png";
	  else if(anaType == WS_ANALYSISTYPE_PCD)
		  ico = ":/data/icons/dado_pontual.png";
	  else if(anaType == WS_ANALYSISTYPE_TERRAME)
		  ico = ":/data/icons/terrame.png";

      _ui->analysisListWidget->addItem(new QListWidgetItem(QIcon(ico), ana->name()));
    }
    _ui->analysisListWidget->setCurrentRow(0);
  }  
  else
    enableFields(false); // Se não há entradas na lista, desabilita campos
}

// Funcao comentada na classe base
bool MainDialogAnalysisTab::dataChanged()
{
  return _ruleChanged;
}

// Funcao comentada na classe base
bool MainDialogAnalysisTab::validate(QString& err)
{
  // Nome da regra é obrigatório
  if(_ui->analysisNameLed->text().trimmed().isEmpty())
  {
    err = tr("Nome da regra de análise não foi preenchido!");
    return false;
  }
  
  if(_ui->analysisWidgetStack->currentIndex() == 0) // Analise com objeto monitorado
  {
    // objeto monitorado deve estar selecionado
    if(_ui->analysisRiskCmb->currentIndex() == 0)
    {
      err = tr("objeto monitorado não foi selecionado!");
      return false;
    }
  }
  else if(_ui->analysisWidgetStack->currentIndex() == 1)// Análise baseada em modelo
  {
    // Nome de saida deve estar preenchido
    if(_ui->analysisOutputLed->text().trimmed().isEmpty())
    {
      err = tr("Nome do plano de saída não foi preenchido!");
      return false;
    }
    else if(_ui->analysisOutputLed->text().trimmed().at(0) == '%')
    {
      err = tr("Nome do plano de saída não pode começar pelo caractere ´%´!");
      return false;
    }
  }
  else if(_ui->analysisWidgetStack->currentIndex() == 2)// Análise com PCD
  {
    // PCD deve estar selecionado
    if(_ui->analysisPCDCmb->currentIndex() == 0)
    {
      err = tr("Fonte PCD não foi selecionada!");
      return false;
    }

		// Vista deve estar selecionada
    if(_ui->analysisViewCmb->currentIndex() == 0)
    {
      err = tr("Vista não foi selecionada!");
      return false;
    }
  }
  else // Análise TerraME
  {
	  if(_ui->analysisCellularSpaceCmb->currentIndex() == 0)
	  {
		  err = tr("Espaço celular não foi selecionado!");
		  return false;
	  }
  }
  
  // Script é obrigatório.
  QString script = _ui->analysisScriptTed->toPlainText().trimmed();
  if(script.isEmpty())
  {
    err = tr("Modelo de análise não foi preenchido!");
    return false;
  }
  
  // Se a análise for condicionada, o script decondição é obrigatório
  if(_ui->analysisConditionalRdb->isChecked())
  {
    if(_conditionRule.isEmpty())
    {
      err = tr("Regra de condição não foi preenchida!");
      return false;
    }
    if(_conditionAnalysisId == -1)
    {
      err = tr("Análise condicionante não foi preenchida!");
      return false;
    }
  }
  
  return true;
}

// Funcao comentada na classe base
bool MainDialogAnalysisTab::save()
{
  Analysis ana;
  if(!_newRule)
    ana = *(_anaList->at(_currentRuleIndex));
  
  // Carrega dados da interface
  getFields(&ana);
  
  bool ok;
  
  // Salva
  if(!_newRule)
    ok = _anaList->updateAnalysis(_currentRuleIndex, &ana);
  else
    ok = _anaList->addNewAnalysis(&ana);  
  
  // Se a operação de salvar não deu certo, retorna false e mantém o estado atual
  if(!ok)
    return false;
  
  // Atualiza nome da entrada atual da lista que pode ter sido alterado
  _ui->analysisListWidget->item(_currentRuleIndex)->setText(ana.name());  
  
  // Atualiza estado para dados não alterados
  clearRuleChanged();
  _newRule = false;
  _ui->analysisRiskCmb->setEnabled(false);
  _ui->analysisCellularSpaceCmb->setEnabled(false);
  _ui->analysisPCDCmb->setEnabled(false);
  _ui->analysisViewCmb->setEnabled(false);

  return true;
}

// Funcao comentada na classe base
void MainDialogAnalysisTab::discardChanges(bool restore_data)
{
  if(_newRule)
  {
    // Estamos descartando uma análise recém criada que não foi salva na base
    // 1) Remove entrada da lista (interface).  Deve ser a última linha
    assert(_currentRuleIndex == _ui->analysisListWidget->count()-1);
    _ignoreChangeEvents = true;
    delete _ui->analysisListWidget->takeItem(_currentRuleIndex);    
    _ui->analysisListWidget->setCurrentRow(-1);
    _currentRuleIndex = -1;
    _ignoreChangeEvents = false;

    // 2) Desmarca indicador de nova regra e de dados modificados
    _newRule = false;
    clearRuleChanged();  
    
    // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
    if(restore_data)
    {
      if(_ui->analysisListWidget->count())
        _ui->analysisListWidget->setCurrentRow(0);
      else
      {
        clearFields(false);  
        enableFields(false);
      }  
    }  
  }
  else
  {
    // Estamos descartando as edições feitas em uma análise antiga
    if(restore_data)
      setFields(_anaList->at(_currentRuleIndex));
    else  
      clearRuleChanged(); 
  }  
}

/*! \brief Limpa a interface.  

Flag indica se a lista de regras e a lista de objetos monitorados também devem ser limpas
*/
void MainDialogAnalysisTab::clearFields(bool clearlist)
{
  _ignoreChangeEvents = true;

  // Lista de regras
  if(clearlist)
  {
    _ui->analysisListWidget->clear();
    _ui->analysisRiskCmb->clear();
    _ui->analysisRiskCmb->addItem(tr("Selecione o objeto monitorado..."));

    _ui->analysisPCDCmb->clear();
    _ui->analysisPCDCmb->addItem(tr("Selecione o PCD..."));

		_ui->analysisViewCmb->clear();
    _ui->analysisViewCmb->addItem(tr("Selecione a vista..."));

    _ui->analysisCellularSpaceCmb->clear();
    _ui->analysisCellularSpaceCmb->addItem(tr("Selecione o espaço celular..."));
  }
  // Campos globais
  _ui->analysisNameLed->clear();
  _ui->analysisAuthorLed->clear();
  _ui->analysisInstitutionLed->clear();
  _ui->analysisDescriptionTed->clear();
  _ui->analysisScriptTed->clear();
  _ui->analysisActiveRdb->setChecked(true);
  _ui->analysisLoadCmb->setCurrentIndex(0);
  _ui->analysisCreateImageCbx->setChecked(false);
  _conditionAnalysisId = -1;
  _conditionRule = "";

  // Campos da página de análise com objeto monitorado
  _ui->analysisRiskCmb->setCurrentIndex(0);
  _ui->analysisGridListWidget->clear();
  // Campos da página de análise baseada em modelo
  _ui->analysisOutputLed->clear();
  _ui->analysisModelGridListWidget->clear();
  // Campos da página de análise com PCD
  _ui->analysisPCDCmb->setCurrentIndex(0);
  _ui->analysisViewCmb->setCurrentIndex(0);
  // Campos da página de análise TerraME
  _ui->analysisCellularSpaceCmb->setCurrentIndex(0);
  _ui->analysisTerraMEListWidget->clear();

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearRuleChanged();
}

//! Habilita ou desabilita campos da interface
void MainDialogAnalysisTab::enableFields(bool mode)
{
  // Campos
  _ui->analysisRiskCmb->setEnabled(mode);
  _ui->analysisNameLed->setEnabled(mode);
  _ui->analysisAuthorLed->setEnabled(mode);
  _ui->analysisInstitutionLed->setEnabled(mode);
  _ui->analysisDescriptionTed->setEnabled(mode);
  _ui->analysisScriptTed->setEnabled(mode);
  _ui->analysisPCDCmb->setEnabled(mode);
  _ui->analysisViewCmb->setEnabled(mode);
  _ui->analysisGridListWidget->setEnabled(mode);
  _ui->analysisOutputLed->setEnabled(mode);
  _ui->analysisModelGridListWidget->setEnabled(mode);
  _ui->analysisCellularSpaceCmb->setEnabled(mode);
  _ui->analysisTerraMEListWidget->setEnabled(mode);
  _ui->analysisActiveRdb->setEnabled(mode);
  _ui->analysisInactiveRdb->setEnabled(mode);
  _ui->analysisConditionalRdb->setEnabled(mode);
  _ui->analysisLoadCmb->setEnabled(mode);
  _ui->analysisCreateImageCbx->setEnabled(mode);
  _ui->analysisCreateImageLabel->setEnabled(mode);

  // Botões
  _ui->analysisCreateLuaBtn->setVisible(mode);
  _ui->analysisCrossBtn->setEnabled(mode);
	_ui->analysisPcdCrossBtn->setEnabled(mode);
  _ui->analysisFillCellOpBtn->setEnabled(mode);

  _ui->analysisRiskAddGridBtn->setEnabled(mode);
  _ui->analysisModelAddGridBtn->setEnabled(mode);
  _ui->analysisTerraMEAddGridBtn->setEnabled(mode);
  _ui->analysisRiskRemoveGridBtn->setEnabled(mode);
  _ui->analysisModelRemoveGridBtn->setEnabled(mode);
  _ui->analysisTerraMERemoveGridBtn->setEnabled(mode);

  _ui->analysisZonaisBtn->setEnabled(mode);
  _ui->analysisPCDZonaisBtn->setEnabled(mode);
  _ui->analysisSampleBtn->setVisible(mode);
  _ui->analysisLevelsBtn->setVisible(mode);
  _ui->analysisFunctionsBtn->setVisible(mode);
  _ui->analysisTerraMEFunctionsBtn->setVisible(mode);
  _ui->analysisCommandsBtn->setVisible(mode);
  _ui->analysisSyntaxBtn->setVisible(mode);

  _ui->fileAnaGridMaskBtn->setEnabled(mode);
  _ui->analysisRemoveBtn->setEnabled(mode);
  _ui->analysisModelGridOutputConfigBtn->setEnabled(mode);
  _ui->analysisAlertIconsBtn->setEnabled(mode);

  if(_ui->analysisConditionalRdb->isChecked())
    _ui->analysisConditionBtn->setEnabled(mode);
  else
    _ui->analysisConditionBtn->setEnabled(false);
}

void MainDialogAnalysisTab::showScriptButtons(wsAnalysisType anaType)
{
	bool risk = (anaType == WS_ANALYSISTYPE_RISK);
	bool pcd = (anaType == WS_ANALYSISTYPE_PCD);
	bool model = (anaType == WS_ANALYSISTYPE_MODEL);
	bool terraME = (anaType == WS_ANALYSISTYPE_TERRAME);

    // Botões visíveis apenas para objetos monitorados
	_ui->analysisAtributesBtn->setVisible(risk);
	_ui->analysisZonaisBtn->setVisible(risk);
	_ui->analysisWizardBtn->setVisible(risk);

	// Botões visíveis apenas para análises de PCD
	_ui->analysisPCDAttributesBtn->setVisible(pcd);
	_ui->analysisPCDZonaisBtn->setVisible(pcd);

    // Botões visíveis para objetos monitorados e análises de PCD
	_ui->analysisLevelsBtn->setVisible(risk||pcd);

	// Botões visíveis apenas em análises de modelos
	_ui->analysisSampleBtn->setVisible(model);

	// Botões visíveis apenas em análises TerraME
	_ui->analysisCellularSpaceAtributesBtn->setVisible(terraME);
	_ui->analysisTerraMEFunctionsBtn->setVisible(terraME);
    _ui->analysisCreateLuaBtn->setVisible(terraME);

}

//! Função chamada quando o botão de condição for pressionado
void MainDialogAnalysisTab::conditionChangeRequested()
{
  AnalysisConditionDlg dlg(_anaList, _riskMapList,
                           _newRule ? -1 : _currentRuleIndex, 
                           _ui->analysisNameLed->text().trimmed(),
                           _conditionAnalysisId, _conditionRule);

  if(dlg.exec() == QDialog::Accepted)
  {
    _conditionAnalysisId = dlg.conditionAnalysisId();
    _conditionRule       = dlg.conditionRule();
    setRuleChanged();    
  }
}

//! Preenche a interface com os dados de uma regra de análise
void MainDialogAnalysisTab::setFields(const Analysis* ana)
{
  _ignoreChangeEvents = true; 

  // Preenche dados comuns
  _ui->analysisNameLed->setText(ana->name());
  _ui->analysisAuthorLed->setText(ana->author());
  _ui->analysisInstitutionLed->setText(ana->institution());
  _ui->analysisDescriptionTed->setPlainText(ana->description());
  _ui->analysisCreateImageCbx->setChecked(ana->generateImage());

  switch(ana->conditionType())
  {
    case WS_ANALYSISCONDITION_ACTIVE:      _ui->analysisActiveRdb->setChecked(true);      break;
    case WS_ANALYSISCONDITION_INACTIVE:    _ui->analysisInactiveRdb->setChecked(true);    break;
    case WS_ANALYSISCONDITION_CONDITIONED: _ui->analysisConditionalRdb->setChecked(true); break;
    default:
      assert(0);
  }
  _conditionAnalysisId = ana->conditionAnalysisId();
  _conditionRule       = ana->conditionScript();

  _ui->analysisLoadCmb->setCurrentIndex(ana->instance() - 1);

  _listAddMapId = ana->listAdditionalMapId();
  _listFillOperations = ana->getFillOperations();

  _gridOutputConfig = ana->gridOutputConfig();
  _alertIcons = ana->alertIcons();

  _ui->analysisScriptTed->setPlainText(ana->script());
  
  // Define qual o modelo de análise visível (com ou sem objeto monitorado)
  wsAnalysisType anaType = ana->getAnalysisType();
  QListWidget* gridlist;
  
  if(anaType == WS_ANALYSISTYPE_RISK)
  {  
    // Campos da página de análise com objeto monitorado
    int index = _riskMapList->findMapIndex(ana->riskMapId());
    _ui->analysisRiskCmb->setCurrentIndex(index + 1); // Se id não foi encontrado, index == -1, logo seleciona entrada "escolher objeto monitorado" na posição zero
    gridlist = _ui->analysisGridListWidget;

    // Como não queremos que o usuário altere o objeto monitorado
	// de uma análise já criada:
	 _ui->analysisRiskCmb->setEnabled(_newRule);
  }
  else if(anaType == WS_ANALYSISTYPE_PCD)
  {
    // Campos da página de análise com objeto monitorado
    int index = _pcdList->findGridIndex(ana->pcdId());
    _ui->analysisPCDCmb->setCurrentIndex(index + 1); // Se id não foi encontrado, index == -1, logo seleciona entrada "escolher objeto monitorado" na posição zero

    index = _viewList->findViewIndex(ana->viewId());
    _ui->analysisViewCmb->setCurrentIndex(index + 1);

    // Como não queremos que o usuário altere o PCD ou a vista
    // de uma análise já criada:
    _ui->analysisPCDCmb->setEnabled(_newRule);
    _ui->analysisViewCmb->setEnabled(_newRule);
  }
  else if(anaType == WS_ANALYSISTYPE_MODEL)
  { 
    // Campos da página de análise baseada em modelo
    _ui->analysisOutputLed->setText(ana->outputName());
    gridlist = _ui->analysisModelGridListWidget;
  }
  else
  {
	  // Campos da página de análise TerraME
	  int index = _cellularSpaceList->findCellularSpaceIndex(ana->cellularSpaceId());
	  _ui->analysisCellularSpaceCmb->setCurrentIndex(index + 1); // Se id não foi encontrado, index == -1, logo seleciona entrada "escolher espaço celular" na posição zero
	  gridlist = _ui->analysisTerraMEListWidget;

	  // Como não queremos que o usuário altere o espaço celular
	  // de uma análise já criada:
	  _ui->analysisCellularSpaceCmb->setEnabled(_newRule);
  }
  
  // Preenche lista de grids
	if(anaType != WS_ANALYSISTYPE_PCD)
	{
		gridlist->clear();
		for(int i=0, count=(int)ana->numWeatherGrids(); i<count; i++)
		{
			int id       = ana->weatherGridId(i);
			QString name = ana->weatherGridName(i);
			const char*   icon_name;
			if (ana->weatherGeometry(i) == WS_WDSGEOM_RASTER)
			{
				if (ana->weatherFormat(i) == WS_WDSFFMT_Additional_Map)
					icon_name = ":/data/icons/novo_grid.png";
				else if (ana->weatherFormat(i) == WS_WDSFFMT_Model)
					icon_name = ":/data/icons/analizemodel.png";
				else if (ana->weatherFormat(i) == WS_WDSFFMT_Surface)
					icon_name = ":/data/icons/surface.png";
				else
					icon_name = ":/data/icons/dado_grid.png";
			}
			else if (ana->weatherGeometry(i) == WS_WDSGEOM_POINTS)
				icon_name = ":/data/icons/dado_pontual.png";
			else if (ana->weatherGeometry(i) == WS_WDSGEOM_POINTSDIFF)
				icon_name = ":/data/icons/dado_pontual_diferente.png";

			QListWidgetItem* item = new QListWidgetItem(QIcon(icon_name), name);
			item->setData(Qt::UserRole, id);
			gridlist->addItem(item);
		}
	}

  // Seta tipo de análise
  _ui->analysisWidgetStack->setCurrentIndex(anaType);
  showScriptButtons(anaType);
  //So é possivel selecionar a opcao de criar imagens para analises com objeto monitorado ou PCD
  _ui->analysisCreateImageCbx->setEnabled(anaType==WS_ANALYSISTYPE_RISK || anaType==WS_ANALYSISTYPE_PCD);
  _ui->analysisCreateImageLabel->setEnabled(anaType==WS_ANALYSISTYPE_RISK || anaType==WS_ANALYSISTYPE_PCD);

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearRuleChanged();
}

//! Preenche a análise com os dados da interface, que já devem ter sido validados
void MainDialogAnalysisTab::getFields(Analysis* ana)
{
  // Preenche dados comuns
  ana->setName(_ui->analysisNameLed->text().trimmed());
  ana->setAuthor(_ui->analysisAuthorLed->text().trimmed());
  ana->setInstitution(_ui->analysisInstitutionLed->text().trimmed());
  ana->setDescription(_ui->analysisDescriptionTed->toPlainText().trimmed());
  ana->setGenerateImage(_ui->analysisCreateImageCbx->isChecked());

  if(_ui->analysisActiveRdb->isChecked())
    ana->setConditionType(WS_ANALYSISCONDITION_ACTIVE);
  else if(_ui->analysisInactiveRdb->isChecked())
    ana->setConditionType(WS_ANALYSISCONDITION_INACTIVE);
  else if(_ui->analysisConditionalRdb->isChecked())
    ana->setConditionType(WS_ANALYSISCONDITION_CONDITIONED);
  else
  {
    assert(0);
  }

  ana->setConditionAnalysisId(_conditionAnalysisId);
  ana->setConditionScript(_conditionRule);
  ana->setInstance(_ui->analysisLoadCmb->currentIndex() + 1);

  ana->setListAdditionalMapId(_listAddMapId);
  ana->setFillOperations(_listFillOperations);

  ana->setScript(_ui->analysisScriptTed->toPlainText().trimmed());

  // Preenche tipo da regra (pois a análise pode ser nova)
  wsAnalysisType anaType = (wsAnalysisType) _ui->analysisWidgetStack->currentIndex();
  ana->setAnalysisType(anaType);

  QListWidget* gridlist;
  if(anaType == WS_ANALYSISTYPE_RISK)
  {
    // Preenche campos relativos a análise com objeto monitorado
    int index = _ui->analysisRiskCmb->currentIndex() - 1;
    ana->setRiskMap(*_riskMapList->at(index));
    gridlist = _ui->analysisGridListWidget;
  }
  else if(anaType == WS_ANALYSISTYPE_MODEL)
  {
    // Preenche campos relativos a análise baseada em modelo
    ana->setOutputName(_ui->analysisOutputLed->text().trimmed());
    ana->setGridOutputConfig(_gridOutputConfig);
    gridlist = _ui->analysisModelGridListWidget;
  }
  else if(anaType == WS_ANALYSISTYPE_PCD)
  {
    // Preenche campos relativos a análise com PCD analysisPCDCmb
    int index = _ui->analysisPCDCmb->currentIndex() - 1;
    ana->setPCD(*_pcdList->at(index));

    index = _ui->analysisViewCmb->currentIndex() - 1;
    ana->setView(*_viewList->at(index));

    ana->setAlertIcons(_alertIcons);
  }
  else
  {
	  // Preenche campos relativos a análise TerraME
	  int index = _ui->analysisCellularSpaceCmb->currentIndex() - 1;
	  ana->setCellularSpace(*_cellularSpaceList->at(index));
	  gridlist = _ui->analysisTerraMEListWidget;
  }

  if(anaType != WS_ANALYSISTYPE_PCD)
  {
    // Obtem dados dos grids selecionados
    QList<WeatherGrid*> selgrids;
    fillSelectedGrids(selgrids, gridlist);
    ana->setWeatherGrids(selgrids);
  }
}

/*! \brief Preenche lista com grids selecionados pelo usuário

\param sellist Lista a ser preenchida
\param grdlist Widget contendo a seleção do usuário
*/
void MainDialogAnalysisTab::fillSelectedGrids(QList<WeatherGrid*>& sellist, QListWidget* gridlist)
{
  for(int i=0, num=(int)gridlist->count(); i<num; i++)
  {
    int id = gridlist->item(i)->data(Qt::UserRole).toInt();
    WeatherGrid* grid = _weatherGridList->findGrid(id);
    if(grid) // Nao deveria acontecer do grid não existir, porém a recuperação entre a regra de análise e a lista de grids pode ter sido defasada...
      sellist.append(grid);
  }
}

/*! \brief Indica que algum dos dados apresentados foi alterado.  

Habilita botões de salvar e cancelar
*/
void MainDialogAnalysisTab::setRuleChanged()
{
  if(_ignoreChangeEvents)
    return;

  _ruleChanged = true;
  _ui->analysisSaveBtn->setEnabled(true);
  _ui->analysisCancelBtn->setEnabled(true);
  if(_newRule)
    _parent->statusBar()->showMessage(tr("Nova regra de análise."));
  else
    _parent->statusBar()->showMessage(tr("Regra de análise alterada."));

  _ui->analysisAddRiskBtn->setEnabled(false);
  _ui->analysisAddModelBtn->setEnabled(false);
  _ui->analysisAddTerraMEBtn->setEnabled(false);
  _ui->analysisAddPCDBtn->setEnabled(false);

  emit editAnalysisRuleStarted();
}

/*! \brief Indica que os dados mostrados estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar
*/
void MainDialogAnalysisTab::clearRuleChanged()
{
  _ruleChanged = false;
  _ui->analysisSaveBtn->setEnabled(false);
  _ui->analysisCancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();

  _ui->analysisAddRiskBtn->setEnabled(true);
  _ui->analysisAddModelBtn->setEnabled(true);
  _ui->analysisAddTerraMEBtn->setEnabled(true);
  _ui->analysisAddPCDBtn->setEnabled(true);

  emit editAnalysisRuleFinished();
}

//! Preenche lista de instâncias de acordo com o número de instâncias cadastradas
void MainDialogAnalysisTab::fillInstanceList()
{
  _ui->analysisLoadCmb->clear();
  for(int i=0; i<_numAnalysisModules; i++)
  {
    _ui->analysisLoadCmb->addItem(tr("Instância %1").arg(i+1));
  }
}

//! Preenche o menu de operadores zonais
void MainDialogAnalysisTab::fillZoneOperatorMenu()
{
  QMenu* zoneOpMenu = new QMenu(tr("Operadores Zonais"), _parent);

  QMenu* gridMenu = new QMenu(tr("Op. zonal p/ grades"), _parent);
  gridMenu->addAction(tr("Mínimo"            ))->setProperty("added_text", "minimo('_Nome_da_grade_')");
  gridMenu->addAction(tr("Máximo"            ))->setProperty("added_text", "maximo('_Nome_da_grade_')");
  gridMenu->addAction(tr("Média"             ))->setProperty("added_text", "media('_Nome_da_grade_')");  
  gridMenu->addAction(tr("Número de amostras"))->setProperty("added_text", "conta_amostras('_Nome_da_grade_')"); 

  QMenu* gridBandMenu = new QMenu(tr("Op. zonal p/ previsões numéricas"), _parent);
  gridBandMenu->addAction(tr("Mínimo PN"))->setProperty("added_text", "minimo_pn('_Nome_da_grade_', _Numero_de_horas_)");
  gridBandMenu->addAction(tr("Máximo PN"))->setProperty("added_text", "maximo_pn('_Nome_da_grade_', _Numero_de_horas_)");
  gridBandMenu->addAction(tr("Média PN" ))->setProperty("added_text", "media_pn('_Nome_da_grade_', _Numero_de_horas_)");  
  gridBandMenu->addAction(tr("Soma PN"))->setProperty("added_text", "soma_pn('_Nome_da_grade_', _Numero_de_horas_inicial_, _Numero_de_horas_final_ )");

  QMenu* gridHistMenu = new QMenu(tr("Op. histórico p/ grades"), _parent);

  QMenu* avgHistMenu = new QMenu(tr("Taxa"), gridHistMenu);
  avgHistMenu->addAction(tr("Mínimo"))->setProperty("added_text", "taxa_min_historico_grid('_Nome_da_grade_', _Numero_de_horas_)");
  avgHistMenu->addAction(tr("Máximo"))->setProperty("added_text", "taxa_max_historico_grid('_Nome_da_grade_', _Numero_de_horas_)");
  avgHistMenu->addAction(tr("Média" ))->setProperty("added_text", "taxa_media_historico_grid('_Nome_da_grade_', _Numero_de_horas_)");

  QMenu* precHistMenu = new QMenu(tr("Precipitação total (mm)"), gridHistMenu);
  precHistMenu->addAction(tr("Mínimo"))->setProperty("added_text", "prec_min_historico_grid('_Nome_da_grade_', _Numero_de_horas_)");
  precHistMenu->addAction(tr("Máximo"))->setProperty("added_text", "prec_max_historico_grid('_Nome_da_grade_', _Numero_de_horas_)");
  precHistMenu->addAction(tr("Média" ))->setProperty("added_text", "prec_media_historico_grid('_Nome_da_grade_', _Numero_de_horas_)");

  gridHistMenu->addMenu(avgHistMenu);
  gridHistMenu->addMenu(precHistMenu);

  QMenu* pointMenu = new QMenu(tr("Op. zonal p/ pontos"), _parent);
  pointMenu->addAction(tr("Mínimo"            ))->setProperty("added_text", "minimo('_Nome_do_plano_', '_atributo_', _..._)");
  pointMenu->addAction(tr("Máximo"            ))->setProperty("added_text", "maximo('_Nome_do_plano_', '_atributo_', _..._)");
  pointMenu->addAction(tr("Média"             ))->setProperty("added_text", "media('_Nome_do_plano_', '_atributo_', _..._)");  
  pointMenu->addAction(tr("Número de amostras"))->setProperty("added_text", "conta_amostras('_Nome_do_plano_', '_atributo_', _..._)"); 

  QMenu* pointHistMenu = new QMenu(tr("Op. histórico p/ pontos"), _parent);
  pointHistMenu->addAction(tr("Soma" ))->setProperty("added_text", "soma_historico_pcd('_Nome_do_plano_', '_atributo_', '_ID_', _horas_)"); 
  pointHistMenu->addAction(tr("Média"))->setProperty("added_text", "media_historico_pcd('_Nome_do_plano_', '_atributo_', '_ID_', _horas_)"); 

  QMenu* pointDiffMenu = new QMenu(tr("Op. p/ dados pontuais de ocorrências"), _parent);

  QMenu* pointDiffMenuSimple = new QMenu(tr("Sem agregação"), pointDiffMenu);
  QString simpleParams = "('_Nome_do_plano_', _buffer_do_Poligono_, _horas_, '_restricaoSQL_', '_atributo_')";
  pointDiffMenuSimple->addAction(tr("Mínimo"  ))->setProperty("added_text", "minimo_pontos" + simpleParams);
  pointDiffMenuSimple->addAction(tr("Máximo"  ))->setProperty("added_text", "maximo_pontos" + simpleParams);
  pointDiffMenuSimple->addAction(tr("Média"   ))->setProperty("added_text", "media_pontos"  + simpleParams);
  pointDiffMenuSimple->addAction(tr("Contagem"))->setProperty("added_text", "contagem_pontos('_Nome_do_plano_', _buffer_do_Poligono_, _horas_, '_restricaoSQL_')");

  QMenu* pointDiffMenuAg = new QMenu(tr("Com agregação"), pointDiffMenu);
  QString aggregateParams = "('_Nome_do_plano_', _buffer_do_Poligono_, _horas_, '_restricaoSQL_', '_atributo_', _buffer_agregacao_)";
  pointDiffMenuAg->addAction(tr("Mínimo / Agregação(mínimo)"  ))->setProperty("added_text", "minimo_pontos_agrega_min" + aggregateParams);
  pointDiffMenuAg->addAction(tr("Mínimo / Agregação(máximo)"  ))->setProperty("added_text", "minimo_pontos_agrega_max" + aggregateParams);
  pointDiffMenuAg->addAction(tr("Mínimo / Agregação(média)"   ))->setProperty("added_text", "minimo_pontos_agrega_med" + aggregateParams);
  pointDiffMenuAg->addAction(tr("Máximo / Agregação(mínimo)"  ))->setProperty("added_text", "maximo_pontos_agrega_min" + aggregateParams);
  pointDiffMenuAg->addAction(tr("Máximo / Agregação(máximo)"  ))->setProperty("added_text", "maximo_pontos_agrega_max" + aggregateParams);
  pointDiffMenuAg->addAction(tr("Máximo / Agregação(média)"   ))->setProperty("added_text", "maximo_pontos_agrega_med" + aggregateParams);
  pointDiffMenuAg->addAction(tr("Média  / Agregação(mínimo)"  ))->setProperty("added_text", "media_pontos_agrega_min"  + aggregateParams);
  pointDiffMenuAg->addAction(tr("Média  / Agregação(máximo)"  ))->setProperty("added_text", "media_pontos_agrega_max"  + aggregateParams);
  pointDiffMenuAg->addAction(tr("Média  / Agregação(média)"   ))->setProperty("added_text", "media_pontos_agrega_med"  + aggregateParams);
  pointDiffMenuAg->addAction(tr("Contagem  / Agregação"       ))->setProperty("added_text", "contagem_pontos_agrega('_Nome_do_plano_', _buffer_do_Poligono_, _horas_, '_restricaoSQL_', _buffer_agregacao_)");

  pointDiffMenu->addMenu(pointDiffMenuSimple);
  pointDiffMenu->addMenu(pointDiffMenuAg);

  zoneOpMenu->addMenu(gridMenu);
  zoneOpMenu->addMenu(gridBandMenu);
  zoneOpMenu->addMenu(gridHistMenu);
  zoneOpMenu->addMenu(pointDiffMenu);
  zoneOpMenu->addMenu(pointMenu);
  zoneOpMenu->addMenu(pointHistMenu);
  zoneOpMenu->addAction(tr("Influência p/ pontos"))->setProperty("added_text", "influencia_pcd('_Nome_do_plano_')");
  zoneOpMenu->addAction(tr("Adiciona valor ao polígono"))->setProperty("added_text", "add_value(_Valor_, '_Nome_identificacao_')");

  connect(zoneOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  _ui->analysisZonaisBtn->setMenu(zoneOpMenu);
  _ui->analysisZonaisBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche o menu de operadores zonais
void MainDialogAnalysisTab::fillPCDZoneOperatorMenu()
{
  QMenu* zoneOpMenu = new QMenu(tr("Operadores Zonais"), _parent);

  QMenu* pointMenu = new QMenu(tr("Op. zonal p/ pontos"), _parent);
  pointMenu->addAction(tr("Mínimo"            ))->setProperty("added_text", "minimo('_atributo_', _..._)");
  pointMenu->addAction(tr("Máximo"            ))->setProperty("added_text", "maximo('_atributo_', _..._)");
  pointMenu->addAction(tr("Média"             ))->setProperty("added_text", "media('_atributo_', _..._)");

  QMenu* pointHistMenu = new QMenu(tr("Op. histórico p/ pontos"), _parent);
  pointHistMenu->addAction(tr("Soma" ))->setProperty("added_text", "soma_historico_pcd('_atributo_', _horas_)");
  pointHistMenu->addAction(tr("Média"))->setProperty("added_text", "media_historico_pcd('_atributo_', _horas_)");

  zoneOpMenu->addMenu(pointMenu);
  zoneOpMenu->addMenu(pointHistMenu);

  zoneOpMenu->addAction(tr("Adiciona valor ao PCD"))->setProperty("added_text", "add_value(_Valor_, '_Nome_identificacao_')");

  connect(zoneOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  _ui->analysisPCDZonaisBtn->setMenu(zoneOpMenu);
  _ui->analysisPCDZonaisBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche menu com operador de amostragem
void MainDialogAnalysisTab::fillSampleOperatorMenu()
{
  QMenu* sampleOpMenu = new QMenu(tr("Operadores de amostragem"), _parent);
  sampleOpMenu->addAction(tr("Amostrar ponto"))->setProperty("added_text", "amostra('_Nome_da_grade_')");
  
  connect(sampleOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  _ui->analysisSampleBtn->setMenu(sampleOpMenu);
  _ui->analysisSampleBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche menu com resultados de alerta padrões
void MainDialogAnalysisTab::fillAlertLevelsMenu()
{
  QPixmap pixmap(10, 10);
  QMenu*  levelsMenu = new QMenu(tr("Níveis de Alerta"), _parent);
  levelsMenu->addAction(Utils::warningLevelToString(0))->setProperty("added_text", "return 0 -- Normal");
  pixmap.fill(QColor(0, 0, 255));
  levelsMenu->addAction(QIcon(pixmap), Utils::warningLevelToString(1))->setProperty("added_text", "return 1 -- Observacao");
  pixmap.fill(QColor(255, 255, 0));
  levelsMenu->addAction(QIcon(pixmap), Utils::warningLevelToString(2))->setProperty("added_text", "return 2 -- Atencao");
  pixmap.fill(QColor(255, 127, 0));
  levelsMenu->addAction(QIcon(pixmap), Utils::warningLevelToString(3))->setProperty("added_text", "return 3 -- Alerta");
  pixmap.fill(QColor(255, 0, 0));
  levelsMenu->addAction(QIcon(pixmap), Utils::warningLevelToString(4))->setProperty("added_text", "return 4 -- Alerta maximo");

  levelsMenu->addSeparator();
  levelsMenu->addAction(tr("Nível de alerta"))->setProperty("added_text", "nivel_alerta('_Nome_da_analise_', '_Nome_do_campo_de_ligacao_', _Valor_de_ligacao_)");
  
  connect(levelsMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  _ui->analysisLevelsBtn->setMenu(levelsMenu);
  _ui->analysisLevelsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche menu com funções padrão colocadas no ambiente
void MainDialogAnalysisTab::fillFunctionsMenu()
{
  // Menu com operações padrão
  QMenu* funcOpMenu = new QMenu(tr("Funções padrão"), _parent);
  funcOpMenu->addAction(tr("print"))->setProperty("added_text", "print(_valores_)");
  funcOpMenu->addAction(tr("tonumber"))->setProperty("added_text", "tonumber(_valor_)");
  funcOpMenu->addAction(tr("tostring"))->setProperty("added_text", "tostring(_valor_)");
  funcOpMenu->addAction(tr("type"))->setProperty("added_text", "type(_valor_)");
  funcOpMenu->addSeparator();
  connect(funcOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));
  
  // Menu com operações matemáticas
  QMenu* mathMenu = new QMenu(tr("Func. Matemáticas"), _parent);
  mathMenu->addAction(tr("abs"  ))->setProperty("added_text", "math.abs(_numero_)");
  mathMenu->addAction(tr("ceil" ))->setProperty("added_text", "math.ceil(_numero_)");
  mathMenu->addAction(tr("exp"  ))->setProperty("added_text", "math.exp(_numero_)");
  mathMenu->addAction(tr("floor"))->setProperty("added_text", "math.floor(_numero_)");
  mathMenu->addAction(tr("log"  ))->setProperty("added_text", "math.log(_numero_)");
  mathMenu->addAction(tr("log10"))->setProperty("added_text", "math.log10(_numero_)");
  mathMenu->addAction(tr("max"  ))->setProperty("added_text", "math.max(_lista_de_numeros_)");
  mathMenu->addAction(tr("min"  ))->setProperty("added_text", "math.min(_lista_de_numeros_)");
  mathMenu->addAction(tr("modf" ))->setProperty("added_text", "math.modf(_numero_)");
  mathMenu->addAction(tr("pow"  ))->setProperty("added_text", "math.pow(_numero_, _numero_)");
  mathMenu->addAction(tr("sqrt "))->setProperty("added_text", "math.sqrt(_numero_)");
  mathMenu->addSeparator();

  QMenu* trigMenu = new QMenu(tr("Trigonométricas"), _parent);
  trigMenu->addAction(tr("acos" ))->setProperty("added_text", "math.acos(_numero_)");
  trigMenu->addAction(tr("asin" ))->setProperty("added_text", "math.asin(_numero_)");
  trigMenu->addAction(tr("atan" ))->setProperty("added_text", "math.atan(_numero_)");
  trigMenu->addAction(tr("atan2"))->setProperty("added_text", "math.atan2(_numero_, _numero_)");
  trigMenu->addAction(tr("cos"  ))->setProperty("added_text", "math.cos(_numero_em_radianos_)");
  trigMenu->addAction(tr("deg"  ))->setProperty("added_text", "math.deg(_numero_em_radianos_)");
  trigMenu->addAction(tr("pi"   ))->setProperty("added_text", "math.pi");
  trigMenu->addAction(tr("rad"  ))->setProperty("added_text", "math.rad(_numero_em_graus_)");
  trigMenu->addAction(tr("sin"  ))->setProperty("added_text", "math.sin(_numero_em_radianos_)");
  trigMenu->addAction(tr("tan"  ))->setProperty("added_text", "math.tan(_numero_em_radianos_)");
  QMenu* hiperMenu = new QMenu(tr("Hiperbólicas"), _parent);
  hiperMenu->addAction(tr("cosh" ))->setProperty("added_text", "math.cosh(_numero_)");
  hiperMenu->addAction(tr("sinh" ))->setProperty("added_text", "math.sinh(_numero_)");
  hiperMenu->addAction(tr("tanh" ))->setProperty("added_text", "math.tanh(_numero_)");

  mathMenu->addMenu(trigMenu);
  mathMenu->addMenu(hiperMenu);
  funcOpMenu->addMenu(mathMenu);
  
  // Associa menu com o botão
  _ui->analysisFunctionsBtn->setMenu(funcOpMenu);
  _ui->analysisFunctionsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche menu com comandos padrão em Lua
void MainDialogAnalysisTab::fillCommandsMenu()
{
  QMenu* cmdOpMenu = new QMenu(tr("Comandos"), _parent);
  cmdOpMenu->addAction(tr("if then end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nend\n");
  cmdOpMenu->addAction(tr("if then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  cmdOpMenu->addAction(tr("if then elseif then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelseif _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  cmdOpMenu->addSeparator();
  cmdOpMenu->addAction(tr("and"))->setProperty("added_text", "(_condicao_ and _condicao_)");
  cmdOpMenu->addAction(tr("or"))->setProperty("added_text", "(_condicao_ or _condicao_)");
    
  connect(cmdOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  _ui->analysisCommandsBtn->setMenu(cmdOpMenu);
  _ui->analysisCommandsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche o menu de atributos do objeto monitorado
void MainDialogAnalysisTab::fillAttributesMenu(const RiskMap* map)
{
  assert(map);

  // Prepara o menu.  Se é a primeira vez, cria. Senão limpa itens e 
  // preenche menu antigo com novas entradas
  QMenu* attributesMenu;
  if(_ui->analysisAtributesBtn->menu())
  {
    attributesMenu = _ui->analysisAtributesBtn->menu();
    attributesMenu->clear();
  }
  else
  {
    attributesMenu = new QMenu(tr("Atributos"), _parent);
    _ui->analysisAtributesBtn->setMenu(attributesMenu);
    _ui->analysisAtributesBtn->setPopupMode(QToolButton::InstantPopup);

    connect(attributesMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));
  }
  // Preenche menu com atributos do plano
  for(int i=0, num=map->numAttributes(); i<num; i++)
  {
    QString name = map->attributeName(i);
    QString item_name = name + "  (" + Utils::columnTypeToString(map->attributeType(i)) + ")";
    attributesMenu->addAction(item_name)->setProperty("added_text", name);
  }
}

//! Preenche o menu de atributos do PCD
void MainDialogAnalysisTab::fillPCDAttributesMenu(WeatherGrid* pcd)
{
  assert(pcd);

  // Prepara o menu.  Se é a primeira vez, cria. Senão limpa itens e
  // preenche menu antigo com novas entradas
  QMenu* attributesMenu;
  if(_ui->analysisPCDAttributesBtn->menu())
  {
    attributesMenu = _ui->analysisPCDAttributesBtn->menu();
    attributesMenu->clear();
  }
  else
  {
    attributesMenu = new QMenu(tr("Atributos"), _parent);
    _ui->analysisPCDAttributesBtn->setMenu(attributesMenu);
    _ui->analysisPCDAttributesBtn->setPopupMode(QToolButton::InstantPopup);

    connect(attributesMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));
  }

  attributesMenu->addAction("object_id")->setProperty("added_text", "object_id");

  std::vector<std::string> attributes = pcd->data().pcd_attributes;
  // Preenche menu com atributos do plano
  for(size_t i=0, num=attributes.size(); i<num; i++)
  {
    QString name = QString::fromStdString(attributes.at(i));
    attributesMenu->addAction(name)->setProperty("added_text", name);
  }

}

//! Preenche o menu de atributos do espaço celular
void MainDialogAnalysisTab::fillCellularSpaceAttributesMenu(const CellularSpace* cs)
{
	assert(cs);

	// Prepara o menu.  Se é a primeira vez, cria. Senão limpa itens e 
	// preenche menu antigo com novas entradas
	QMenu* attributesMenu;
	if(_ui->analysisCellularSpaceAtributesBtn->menu())
	{
		attributesMenu = _ui->analysisCellularSpaceAtributesBtn->menu();
		attributesMenu->clear();
	}
	else
	{
		attributesMenu = new QMenu(tr("Atributos"), _parent);
		_ui->analysisCellularSpaceAtributesBtn->setMenu(attributesMenu);
		_ui->analysisCellularSpaceAtributesBtn->setPopupMode(QToolButton::InstantPopup);

		connect(attributesMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));
	}

	// Preenche menu com atributos do plano
	for(int i=0, num=cs->numAttributes(); i<num; i++)
	{
		QString name = cs->attributeName(i);
		QString item_name = name + "  (" + Utils::columnTypeToString(cs->attributeType(i)) + ")";
		attributesMenu->addAction(item_name)->setProperty("added_text", "_cell_." + name);
	}
}

//! Preenche menu com funções TerraME
void MainDialogAnalysisTab::fillTerraMEFunctionsMenu()
{
	QMenu* menu = new QMenu(tr("Funções TerraME"), _parent);

	QString cellularSpace = tr("-- Define e carrega o espaco celular. \n"
							   "-- Nao altere os parametros de conexao com a base de dados. \n"
							   "-- Eles serao preenchidos automaticamente. \n");
	
	cellularSpace += "cs = CellularSpace { \n"
					 "         dbType = \"%%dbType%%\", \n"
					 "         host = \"%%host%%\", \n"
					 "         database = \"%%database%%\", \n"
					 "         user = \"%%user%%\", \n"
					 "         password = \"%%password%%\", \n"
					 "         layer = \"_layer_\", \n"
					 "         theme = \"_theme_\", \n"
					 "         select = { \"_atributo_\", \"_..._\" } \n"
					 "     } \n"
					 "cs:load()\n";

	// Primeiro elemento do menu será a referência para o espaço celular da análise
	menu->addAction("CellularSpace")->setProperty("added_text", cellularSpace);
	menu->addSeparator();

	menu->addAction("createMooreNeighborhood")->setProperty("added_text", "createMooreNeighborhood(_CellularSpace_, _index_)");
	menu->addAction("forEachCell")->setProperty("added_text", "forEachCell(_CellularSpace_, _function_)");
	menu->addAction("forEachNeighbor")->setProperty("added_text", "forEachNeighbor(_cell_, _function_, _index_)");
	menu->addAction("synchronize")->setProperty("added_text", "_object_:synchronize()");
	menu->addAction("Trajectory")->setProperty("added_text", "Trajectory(_CellularSpace_, _function_filter_, _function_order_)");
	menu->addSeparator();

	QString save2PNGComment = tr("-- O valor %%imageOutputPath%% sera preenchido automaticamente com o\n"
								 "-- diretorio de imagens definido pela interface de Administracao.\n");

	menu->addAction("save2PNGd")->setProperty("added_text", save2PNGComment + "save2PNGd(_CellularSpace_, _t_, '%%imageOutputPath%%'..'_filename_', '_attr_', _attr_value_, _attr_color_)");
	menu->addAction("save2PNGc")->setProperty("added_text", save2PNGComment + "save2PNGc(_CellularSpace_, _t_, '%%imageOutputPath%%'..'_filename_', '_attr_', _attr_range_, _attr_clims_, _slices_)");

	connect(menu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

	_ui->analysisTerraMEFunctionsBtn->setMenu(menu);
	_ui->analysisTerraMEFunctionsBtn->setPopupMode(QToolButton::InstantPopup);
}

// Função comentada na classe base
QString MainDialogAnalysisTab::verifyAndEnableChangeMsg()
{
  return tr("As alterações efetuadas na tela de regras de análise\n"
            "ainda não foram salvas.  Deseja salvar as alterações?");
}

//! Slot chamado quando a linha corrente é alterada na lista de análises
void MainDialogAnalysisTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
  if(_ignoreChangeEvents)
    return;

  QModelIndexList selected_indexes = selected.indexes();
  
  // Se usuário clicou na lista fora de qq. item, remarca item anterior
  if(!selected_indexes.count())
  {
    if(oldSelection.indexes().count()) // Evita loop infinito se não existir seleção anterior...
      _ui->analysisListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    return;
  }
  
  // Obtem a linha selecionada    
  int row = selected_indexes[0].row();

  // Verifica se estamos apenas voltando à mesma opção atual.  Ocorre 
  // quando uma troca de regra foi cancelada
  if(row == _currentRuleIndex)
    return;

  bool ok = true;
  
  // Verifica se os dados atuais na tela foram modificados
  // e em caso positivo se podemos efetuar a troca de dados
  if(_currentRuleIndex != -1)
    ok = verifyAndEnableChange(false);

  if(ok)
  {
    // Operação permitida.  Troca dados na tela
    if(_newRule)
    {
      // Estamos tratando a seleção de uma análise recém incluida na lista
      assert(row == _anaList->count());
      clearFields(false);
    }
    else 
    {
      // Estamos tratando uma seleção normal feita pelo usuário
      setFields(_anaList->at(row));
    }
    _currentRuleIndex = row;
  }
  else
  {
    // Operação foi cancelada.  Devemos reverter à regra original
    _ui->analysisListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    _ui->analysisListWidget->setCurrentRow(_currentRuleIndex);
  }    
}

//! Slot chamdo quando o objeto monitorado é alterado
void MainDialogAnalysisTab::riskMapChanged(int row)
{
  // Habilita ou desabilita menu de atributos e botão de wizard
  if(row <= 0)
  {
    _ui->analysisAtributesBtn->setEnabled(false);
    _ui->analysisWizardBtn->setEnabled(false);
    _ui->analysisCreateLuaBtn->setVisible(false);
    return;
  }
  _ui->analysisAtributesBtn->setEnabled(true);
  _ui->analysisWizardBtn->setEnabled(true);
  fillAttributesMenu(_riskMapList->at(row-1));
  
  if (_ignoreChangeEvents)
	  return;

  setRuleChanged();
}

//! Slot chamdo quando o PCD é alterado
void MainDialogAnalysisTab::PCDChanged(int row)
{
  // Habilita ou desabilita menu de atributos e botão de wizard
  if(row <= 0)
  {
    _ui->analysisPCDAttributesBtn->setEnabled(false);
    _ui->analysisWizardBtn->setEnabled(false);
    _ui->analysisCreateLuaBtn->setVisible(false);
    return;
  }
  _ui->analysisPCDAttributesBtn->setEnabled(true);
  _ui->analysisWizardBtn->setEnabled(true);
  fillPCDAttributesMenu(_pcdList->at(row-1));

  if (_ignoreChangeEvents)
	  return;

  setRuleChanged();
}

//! Slot chamdo quando a vista é alterada
void MainDialogAnalysisTab::ViewChanged(int row)
{
  if(row <= 0)
    return;

	if (_ignoreChangeEvents)
	  return;

  setRuleChanged();
}


//! Slot chamdo quando o espaço celular é alterado
void MainDialogAnalysisTab::cellularSpaceChanged(int row)
{
	// Habilita ou desabilita menu de atributos
	if(row <= 0)
	{
		_ui->analysisCellularSpaceAtributesBtn->setEnabled(false);
		return;
	}

	_ui->analysisCellularSpaceAtributesBtn->setEnabled(true);
	fillCellularSpaceAttributesMenu(_cellularSpaceList->at(row-1));

	if (_ignoreChangeEvents)
		return;

	setRuleChanged();
}

//! Slot chamado quando o usuério pressiona o botão de criação do arquivo Lua.
void MainDialogAnalysisTab::wizardCreateLua()
{
    if(_ui->analysisCellularSpaceCmb->currentIndex() == 0)
	{
        QMessageBox::warning(_parent, "", tr("Espaço celular não foi selecionado!"));
		return;
    }

    int index = _ui->analysisCellularSpaceCmb->currentIndex() - 1;
	CellularSpace* cs = _cellularSpaceList->at(index);
    int celularSpaceId = cs->id();
    std::vector<std::string> result;

    if(!_manager->getColumnsCelularSpace(celularSpaceId, result))
    {
        QMessageBox::warning(_parent, "", tr("Erro ao carregar o espaço celular!"));
        return;
    }

    AnalysisCreateLuaDlg dlg(_manager, celularSpaceId, result);
	dlg.exec();
	//Pega as strings do lua.
	QString script = dlg.getLuaFileLinesString();
	if(script.isEmpty())
		return;

	_ui->analysisScriptTed->setPlainText(script);

}

//! Slot chamado quando o usuário pressiona o botão de assistente de análise
void MainDialogAnalysisTab::wizardRequested()
{
  RiskMap* map = _riskMapList->at(_ui->analysisRiskCmb->currentIndex() - 1);

  // Recarregamos a lista de grids, usada pela função fillSelectedGrids,
  // para que peguemos informações consistentes a respeito das fontes de dados
  _weatherGridList->clear();
  _manager->loadGridList();
  
  // Então, usamos essas fontes
  QList<WeatherGrid*> selgrids;
  fillSelectedGrids(selgrids, _ui->analysisGridListWidget);

  WizardDlg dlg(map, selgrids);
  dlg.exec();
  QString script = dlg.script();
  if(script.isEmpty())
    return;
  _ui->analysisScriptTed->setPlainText(script);
}

//! Slot chamado quando o usuário pressiona o botão de validação de sintaxe do script
void MainDialogAnalysisTab::syntaxCheckRequested()
{
  QString script = _ui->analysisScriptTed->toPlainText().trimmed();
  QString err;
  int lin;
  
  if(!LuaUtils::checkSyntax(script.toLatin1().data(), err, lin))
  {
    if(lin)
      QtUtils::selectLine(_ui->analysisScriptTed, lin);
    QMessageBox::warning(_parent, tr("validação de sintaxe..."), err);
  }  
  else
    QMessageBox::warning(_parent, tr("validação de sintaxe..."), tr("Validação Ok"));  
}

//! Slot chamado quando o usuário pressiona o botão de cruzamento de planos
void MainDialogAnalysisTab::crossRequested()
{
  CrossDlg dlg;
  dlg.setFields(_manager, _listAddMapId);

  if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
  {
    bool changed = false;

    dlg.getFields(_listAddMapId, changed);

	if (changed)
	  setRuleChanged();
  }

}

//! Slot chamado quando o usuário pressiona o botão de configuração dos icones de alerta
void MainDialogAnalysisTab::alertIconRequested()
{
  AlertIconDlg dlg;

  dlg.setFields(_manager->alertIconList(), _alertIcons);

  if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
  {
    bool changed = false;

    dlg.getFields(_alertIcons, changed);

    if (changed)
      setRuleChanged();
  }

}

//! Slot chamado quando o usuário pressiona o botão para definição das operações de preenchimento de células (disponível apenas para análises TerraME)
void MainDialogAnalysisTab::fCellOpRequested()
{
	if(_ui->analysisTerraMEListWidget->count() == 0)
	{
		// Só faz sentido chamar a interface de definição de operações de preenchimento se houver plano de entrada
		QMessageBox::warning(_parent, tr("Erro..."), tr("Plano de entrada ainda não foi definido."));
		return;
	}
	
	AnalysisFillCellOpDlg dlg(_manager, _ui->analysisTerraMEListWidget);

	dlg.setFields(_manager, _listFillOperations);

	if (dlg.exec()== QDialog::Accepted)
	{
		bool changed = false;

		dlg.getFields(_listFillOperations, changed);

		if (changed)
			setRuleChanged();
	}
}

//! Slot chamado quando o usuário pressiona o botão de adicionar grid
void MainDialogAnalysisTab::addGridRequested()
{
  // Descobre em qual lista estamos inserindo dados
  wsAnalysisType anaType = (wsAnalysisType) _ui->analysisWidgetStack->currentIndex();
  QListWidget* gridlist;

  if(anaType == WS_ANALYSISTYPE_RISK)
	  gridlist = _ui->analysisGridListWidget;
  else if(anaType == WS_ANALYSISTYPE_MODEL)
	  gridlist = _ui->analysisModelGridListWidget;
  else
	  gridlist = _ui->analysisTerraMEListWidget;

  // Obtem Ids dos grids selecionados (para que o diálogo apresente apenas grids novos)
  QList<int> selgrids;
  for(int i=0, num=(int)gridlist->count(); i<num; i++)
  {
    int id = gridlist->item(i)->data(Qt::UserRole).toInt();
    selgrids.append(id);
  }

  _weatherGridList->clear();
  _manager->loadGridList();

  // Se a analise corrente ja estiver na base de dados, obter referencia para ela
  Analysis* currentAnalysis = NULL;
  if(_currentRuleIndex >= 0 && !_newRule)
  {
	  currentAnalysis = _anaList->at(_currentRuleIndex);
  }

  // Se a análise for do tipo TerraME, por enquanto não vamos permitir a adição de planos de entrada que não sejam do tipo raster.
  // Se a analise for baseada em modelo, somente dados matriciais sao aceitos como planos de entrada.
  if(anaType == WS_ANALYSISTYPE_TERRAME || anaType == WS_ANALYSISTYPE_MODEL)
  {
	  for(int i=0, num=(int)_weatherGridList->count(); i<num; i++)
	  {
		  WeatherGrid* weatherGrid = _weatherGridList->at(i);
		  if(weatherGrid->geometry() != WS_WDSGEOM_RASTER)
		  {
			  // Se o plano de entrada não é raster, então o plano ainda não está em selgrids. Vamos inserir...
			  selgrids.append(weatherGrid->id());
		  }
		  else if(anaType == WS_ANALYSISTYPE_MODEL && weatherGrid->data().format == WS_WDSFFMT_Model 
					&& currentAnalysis && weatherGrid->name() == currentAnalysis->name())
		  {
			  // Se a analise for baseada em modelo, ela nao pode ser fonte de dados para ela mesma.
			  selgrids.append(weatherGrid->id());
		  }
	  }
  }

  // Chama o diálogo de seleção
  // Caso ainda tenha alguma fonte de dado...
  if(_weatherGridList->count() > selgrids.size())
  {
    // ...mostramos a janela para que o usuário escolha
	// qual ele irá adicionar
	GridListDlg dlg(_weatherGridList, selgrids);
	dlg.exec();
	WeatherGrid* newgrid = dlg.selectedGrid();
	if(!newgrid)  // Usuario cancelou a operação
		return;

	// Adiciona novo grid ao final da lista
	const char* icon_name;
	if (newgrid->geometry() == WS_WDSGEOM_RASTER) 
	{
		if (newgrid->data().format == WS_WDSFFMT_Additional_Map)
			icon_name = ":/data/icons/novo_grid.png";
		else if (newgrid->data().format == WS_WDSFFMT_Model)
			icon_name = ":/data/icons/analizemodel.png";
		else if (newgrid->data().format == WS_WDSFFMT_Surface)
			icon_name = ":/data/icons/surface.png";
		else
			icon_name = ":/data/icons/dado_grid.png";
	}
	else if (newgrid->geometry() == WS_WDSGEOM_POINTS)
		icon_name = ":/data/icons/dado_pontual.png";
	else if (newgrid->geometry() == WS_WDSGEOM_POINTSDIFF)
		icon_name = ":/data/icons/dado_pontual_diferente.png";

	QListWidgetItem* item = new QListWidgetItem(QIcon(icon_name), newgrid->name());
	item->setData(Qt::UserRole, newgrid->id());
	gridlist->addItem(item);

	//Se a análise for do tipo TerraME, vamos selecionar uma operação de preenchimento de células padrão
	if(anaType == WS_ANALYSISTYPE_TERRAME)
	{
		wsFillOperation fillOp;
		fillOp.weatherDataSourceID = newgrid->id();
		fillOp.column = newgrid->name().toStdString();

		//Os valores padrão sugeridos para os parâmetros "operação" e "horas" vão depender do tipo do dado
		if( (newgrid->data().format == WS_WDSFFMT_GrADS || newgrid->data().format == WS_WDSFFMT_OGC_WCS) 
			 && newgrid->data().grads_numBands > 1)
		{
			fillOp.opType = WS_FILLOPERATIONTYPE_MAX_PN;
			fillOp.hours = 48;
		}
		else
		{
			fillOp.opType = WS_FILLOPERATIONTYPE_MAX;
			fillOp.hours = 0;
		}

		_listFillOperations.push_back(fillOp);
	}

	setRuleChanged();
  }
}

//! Slot chamado quando o usuário pressiona o botão de remover grid
void MainDialogAnalysisTab::removeGridRequested()
{
	// Descobre de qual lista estamso removendo o dado
	wsAnalysisType anaType = (wsAnalysisType) _ui->analysisWidgetStack->currentIndex();
	QListWidget* gridlist;

	if(anaType == WS_ANALYSISTYPE_RISK)
		gridlist = _ui->analysisGridListWidget;
	else if(anaType == WS_ANALYSISTYPE_MODEL)
		gridlist = _ui->analysisModelGridListWidget;
	else
		gridlist = _ui->analysisTerraMEListWidget;

	if(gridlist->currentRow() < 0)
	{
		QMessageBox::warning(_parent, tr("Erro..."), tr("Selecione um grid na lista ao lado."));
		return;
	}

	if(anaType == WS_ANALYSISTYPE_TERRAME)
	{
		// Vamos remover também a operação de preenchimento associada a este dado
		QListWidgetItem *item = gridlist->takeItem(gridlist->currentRow());

		int wdsID = item->data(Qt::UserRole).toInt();
		std::vector<struct wsFillOperation>::iterator it = _listFillOperations.begin();
	 	
		for(int i = _listFillOperations.size() - 1; i >= 0; --i)
	 	{
	 		if(_listFillOperations.at(i).weatherDataSourceID == wdsID)
	 			_listFillOperations.erase(it + i);
	 	}

		delete item;
	}
	else
		delete gridlist->takeItem(gridlist->currentRow());

	setRuleChanged();
}

//! Slot chamado quando o usuário pressiona o botão de configurar grade de saída
void MainDialogAnalysisTab::configOutputGridRequested()
{
	GridConfigDlg dlg;
	dlg.setFields(_manager, _gridOutputConfig);

	if (dlg.exec()== QDialog::Accepted)
	{
		bool changed = false;
		dlg.getFields(_gridOutputConfig, changed);
		if (changed)
			setRuleChanged();
	}
}

/*! \brief Slot chamado quando o usuário seleciona uma propriedade ou 
           operador zonal via menu de contexto
           
Adiciona o texto associado ao item de menu selecionado no script.
Para isso, consulta a propriedade de nome "added_text" do item de menu
*/
void MainDialogAnalysisTab::addTextToScript(QAction* act)
{
  QString value = act->property("added_text").toString();

  _ui->analysisScriptTed->insertPlainText(value); 
  _ui->analysisScriptTed->setFocus(); 
}

//! Slot chamado quando o usuário deseja criar uma nova análise
void MainDialogAnalysisTab::addNewAnalysisRequested()
{
  // Verifica se podemos ir para nova analise
  if(!verifyAndEnableChange(false))
    return;
    
  // Ok. Podemos criar nova entrada.
  // 1) Gera um nome temporário para a análise
  QString name = tr("Nova análise");
  
  // 2) Descobre o tipo de análise
  wsAnalysisType anaType;
  QString ico;
  bool image = false;

  if(sender() == _ui->analysisAddRiskBtn)
  {	  anaType = WS_ANALYSISTYPE_RISK;
      ico = ":/data/icons/analise_plano de risco.png";
	  image = true;
  }
  else if(sender() == _ui->analysisAddModelBtn)
  {
	  anaType = WS_ANALYSISTYPE_MODEL;
	  ico = ":/data/icons/analizemodel.png";
  }
  else if(sender() == _ui->analysisAddPCDBtn)
  {
	  anaType = WS_ANALYSISTYPE_PCD;
	  ico = ":/data/icons/dado_pontual.png";
	  image = true;
  }
  else
  {
	  // Verificar se durante a compilação do TerraMA2 foi utilizado o TerraME
#ifndef TERRAME_ENABLED
	  QMessageBox::warning(_parent, tr("Erro..."), tr("Sistema não foi compilado para uso do TerraME."));
	  return;
#endif

#ifdef _WIN64
	  QMessageBox::warning(_parent, tr("Erro..."), tr("Sistema não foi compilado para uso do TerraME."));
	  return;
#endif

	  // Verificar o driver de banco de dados. Atualmente o TerraME não é compatível com PostgreSQL
	  if(_manager->getConfigData()->db()._driver != ConfigData::DRIVER_MySQL)
	  {
		  QMessageBox::warning(_parent, tr("Erro..."), tr("TerraME não é compatível com o banco de dados PostgreSQL.\nUtilize o MySQL."));
		  return;
	  }

	  anaType = WS_ANALYSISTYPE_TERRAME;
	  ico = ":/data/icons/terrame.png";
  }
  
  // 3) Inclui nome na lista de análises e seleciona esta análise
  //    Antes de selecionar, marca que estamos tratando de uma nova regra
  _newRule = true;
  _ui->analysisListWidget->addItem(new QListWidgetItem(QIcon(ico), name));
  _ui->analysisListWidget->setCurrentRow(_ui->analysisListWidget->count()-1);
  
  // 4) Tratador de seleção já limpou a tela.  Habilita os widgets corretos
  //    de acordo com o uso de objeto monitorado
  enableFields(true);
  _ui->analysisWidgetStack->setCurrentIndex(anaType);
  showScriptButtons(anaType);
  _ui->analysisCreateImageCbx->setEnabled(image);
  _ui->analysisCreateImageLabel->setEnabled(image);
  
  // 5) Reseta os parâmetros de configuração da grade de saída
  resetGridOutputConfig();

  // 6) Preenche campo com o nome da análise e marca a análise como modificada
  _ui->analysisNameLed->setText(name);
  _ui->analysisNameLed->setFocus();
  _ui->analysisNameLed->selectAll();
  _listAddMapId.clear();
  _listFillOperations.clear();
  setRuleChanged();

  // 6) Reseta os icones de alerta a analise de PCD
  resetAlertIcons();
}

//! Slot chamado quando o usuário deseja remover a análise atual
void MainDialogAnalysisTab::removeAnalysisRequested()
{
  if(_currentRuleIndex == -1)
    return;
    
  // Verifca com o usuário se ele deseja realmente remover o plano
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(_parent, tr("Remover análise..."), 
                                 tr("Deseja realmente remover esta análise?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No);
  if(answer == QMessageBox::No)
    return;

  // Remove objeto do servidor.  Se não deu certo, retorna e mantém o estado atual
  bool  refreshNeeded = false;
  if(!_newRule)
  {
    int analysisID = _anaList->at(_currentRuleIndex)->id();
	bool deleteLayers = false;
    
	//Se a analise for baseada em modelo, verificar se usuario deseja deletar tambem os layers gerados pela analise
	if(_anaList->at(_currentRuleIndex)->getAnalysisType() == WS_ANALYSISTYPE_MODEL)
	{
		QMessageBox::StandardButton answer;
		answer = QMessageBox::question(_parent, tr("Remover análise..."), 
										tr("Deseja remover também os layers gerados por esta análise?"),
										QMessageBox::Yes | QMessageBox::No,
										QMessageBox::No);
		if(answer == QMessageBox::Yes)
			deleteLayers = true;
	}

    if(!_anaList->deleteAnalysis(_currentRuleIndex, deleteLayers))
      return;
    
    refreshNeeded = true;
    
    emit analysisRuleDeleted(analysisID);
  }
  
  // Remove objeto da lista (interface) e desmarca seleção
  _ignoreChangeEvents = true;
  delete _ui->analysisListWidget->takeItem(_currentRuleIndex);    
  _ui->analysisListWidget->setCurrentRow(-1);
  _currentRuleIndex = -1;
  _ignoreChangeEvents = false;
  

  // Desmarca indicadores de modificação e nova regra
  clearRuleChanged();  
  _newRule = false;

  // Seleciona a primeira linha ou limpa campos se não houver nenhuma entrada na lista.
  if(_ui->analysisListWidget->count())
    _ui->analysisListWidget->setCurrentRow(0);
  else
  {
    clearFields(false);  
    enableFields(false);
  }  

  if(refreshNeeded)
    _ui->refreshAct->trigger();
}

//! Slot chamado quando o usuário clica no botao de mascara
void MainDialogAnalysisTab::menuMaskClick(QAction* actMenu)
{
  _ui->analysisOutputLed->setText(_ui->analysisOutputLed->text() + actMenu->text().left(2));
  setRuleChanged();
}

void MainDialogAnalysisTab::additionalMapChanged()
{
}

void MainDialogAnalysisTab::analysisDoubleClicked( QListWidgetItem * )
{
	// Verifca com o usuário se ele deseja realmente executar o boletim
	QMessageBox::StandardButton answer;
	answer = QMessageBox::question(_parent, tr("Executar análises..."), 
		tr("Deseja realmente executar as análises ?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);

	if(answer == QMessageBox::No)
		return;

	_manager->runAllAnalyses(true,0,0,0,0,0,0);
}

void MainDialogAnalysisTab::resetGridOutputConfig()
{
  _gridOutputConfig.interpolationMethod = WS_INTERPOLATION_NN_METHOD;
  _gridOutputConfig.resBase             = WS_OUTPUT_GRID_RES_MAX;
  _gridOutputConfig.resBaseMap          = 0;
  _gridOutputConfig.resBaseMapIsAddMap  = false;
  _gridOutputConfig.resX                = 0.0;
  _gridOutputConfig.resY                = 0.0;
  _gridOutputConfig.resBaseMapIsAddMap  = false;
  _gridOutputConfig.roiBaseMap          = 0;
  _gridOutputConfig.roiBaseMapIsAddMap  = false;
  _gridOutputConfig.x1                  = 0.0;
  _gridOutputConfig.y1                  = 0.0;
  _gridOutputConfig.x2                  = 0.0;
  _gridOutputConfig.y2                  = 0.0;
  _gridOutputConfig.dummy               = -99999;
}

void MainDialogAnalysisTab::resetAlertIcons()
{
  _alertIcons.imageLevel0 = 0;
  _alertIcons.imageLevel1 = 0;
  _alertIcons.imageLevel2 = 0;
  _alertIcons.imageLevel3 = 0;
  _alertIcons.imageLevel4 = 0;
}
