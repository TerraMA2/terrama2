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
  \file terrama2/gui/config/MainDialogAnalysisTab.hpp

  \brief Definition of Class MainDialogAnalysisTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _MAINDLG_ANALYSIS_TAB_H_
#define _MAINDLG_ANALYSIS_TAB_H_

#include "MainDialogTab.hpp"
#include "RiskMap.hpp"
#include "CellularSpace.hpp"

class Analysis;
class WeatherGrid;
class AnalysisList;
class RiskMapList;
class CellularSpaceList;
class WeatherGridList;
class ViewList;
class AlertIconList;

/*!
\brief Classe respons�vel pelo tratamento da interface da ficha de
       an�lises da janela principal
*/      
class MainDialogAnalysisTab : public MainDialogTab
{
Q_OBJECT

public:
  MainDialogAnalysisTab(MainDlg* main_dialog, Services* manager);

  virtual ~MainDialogAnalysisTab();

  virtual void load();
  virtual bool dataChanged();
  virtual bool validate(QString& err);
  virtual bool save();
  virtual void discardChanges(bool restore_data);

protected:
  virtual QString verifyAndEnableChangeMsg();

private slots:
  void listItemSelectionChanged(const QItemSelection&, const QItemSelection&);
  void riskMapChanged(int row);
  void PCDChanged(int row);
  void ViewChanged(int row);
  void cellularSpaceChanged(int row);
  void additionalMapChanged();
  void wizardCreateLua();
  void wizardRequested ();
  void syntaxCheckRequested();
  void crossRequested  ();
  void alertIconRequested();
  void fCellOpRequested();
  void addGridRequested();
  void removeGridRequested();
  void configOutputGridRequested();
  void conditionChangeRequested();
  void setRuleChanged();
  void addTextToScript(QAction* act);
  
  void addNewAnalysisRequested();
  void removeAnalysisRequested();

  void deleteMapListInCombo(const int& index, const wsRiskMap& data);
  void updateMapListInCombo(const int& index, const wsRiskMap& data);
  void insertMapListInCombo(const int& index, const wsRiskMap& data);

  void deleteCellularSpaceListInCombo(const int&);
  void updateCellularSpaceListInCombo(const int& index, const wsCellularSpace& data);
  void insertCellularSpaceListInCombo(const int& index, const wsCellularSpace& data);

  void updatePCDList();
  void updateWeatherDataSourceInList(int id, QString newName);

  void menuMaskClick(QAction* actMenu);

  void analysisDoubleClicked(QListWidgetItem *);

private:
  void clearFields(bool clearlist);
  void setFields(const Analysis* ana);
  void getFields(Analysis* ana);
  void enableFields(bool mode);
  void showScriptButtons(wsAnalysisType anaType);
  void fillZoneOperatorMenu();
  void fillPCDZoneOperatorMenu();
  void fillAlertLevelsMenu();
  void fillFunctionsMenu();
  void fillCommandsMenu();
  void fillSampleOperatorMenu();
  void fillAttributesMenu(const RiskMap* map);
  void fillPCDAttributesMenu(WeatherGrid* pcd);
  void fillCellularSpaceAttributesMenu(const CellularSpace* cs);
  void fillTerraMEFunctionsMenu();
  void clearRuleChanged();
  void fillSelectedGrids(QList<WeatherGrid*>& sellist, QListWidget* gridlist);
  void fillInstanceList();
  void resetGridOutputConfig();
  void resetAlertIcons();
  
  AnalysisList*			_anaList;          //!< Refer�ncia para lista com dados das regras de an�lise, mantida pelo gerenciador de servi�os
  RiskMapList*			_riskMapList;      //!< Refer�ncia para lista com dados de objetos monitorados, mantida pelo gerenciador de servi�os
  CellularSpaceList*	_cellularSpaceList;//!< Refer�ncia para lista com dados de espa�os celulares, mantida pelo gerenciador de servi�os 
  WeatherGridList* _weatherGridList;  //!< Refer�ncia para lista com dados de grids clim�ticos, mantida pelo gerenciador de servi�os
  WeatherGridList* _pcdList;  //!< Refer�ncia para lista com dados de tipo PCD, mantida pelo gerenciador de servi�os
  ViewList*        _viewList; //!< Refer�ncia para lista com vistas, mantida pelo gerenciador de servi�os
  AlertIconList*   _alertIconList; //!< Refer�ncia para lista de imagens para usar como icones de alerta, mantida pelo gerenciador de servi�os

  int   _currentRuleIndex;   //!< Indice da regra atual na lista de regras de an�lise
  bool  _ignoreChangeEvents; //!< Indica que o slot que monitora mudan�as est� temporariamente desligado
  bool  _ruleChanged; //!< Indica que algum dos dados apresentados foi alterado pelo usu�rio
  bool  _newRule;     //!< Indica que a regra atual � uma nova regra
  int   _numAnalysisModules;  //!< Indica quanto m�dulos de an�lise existem
  int     _conditionAnalysisId; //!< Indice da regra de an�lise da qual dependemos
  QString _conditionRule;       //!< Regra de condi��o para an�lises condicionadas

  std::vector<struct wsAddMapDisplayConfig> _listAddMapId;
  std::vector<struct wsFillOperation> _listFillOperations;

  wsGridOutputConfig _gridOutputConfig;
  wsAlertIcons       _alertIcons;
  
signals:
  //! Chamado quando o usu�rio inicia a cria��o/edi��o de uma regra de an�lise.
  void editAnalysisRuleStarted();
  
  //! Chamado quando o usu�rio termina a cria��o/edi��o de uma regra de an�lise
  //! (confirmando ou cancelando).
  void editAnalysisRuleFinished();
  
  //! Chamado quando uma an�lise foi exclu�da.
  void analysisRuleDeleted(int id);
};


#endif

