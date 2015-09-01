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
\brief Classe responsável pelo tratamento da interface da ficha de
       análises da janela principal
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
  
  AnalysisList*			_anaList;          //!< Referência para lista com dados das regras de análise, mantida pelo gerenciador de serviços
  RiskMapList*			_riskMapList;      //!< Referência para lista com dados de objetos monitorados, mantida pelo gerenciador de serviços
  CellularSpaceList*	_cellularSpaceList;//!< Referência para lista com dados de espaços celulares, mantida pelo gerenciador de serviços 
  WeatherGridList* _weatherGridList;  //!< Referência para lista com dados de grids climáticos, mantida pelo gerenciador de serviços
  WeatherGridList* _pcdList;  //!< Referência para lista com dados de tipo PCD, mantida pelo gerenciador de serviços
  ViewList*        _viewList; //!< Referência para lista com vistas, mantida pelo gerenciador de serviços
  AlertIconList*   _alertIconList; //!< Referência para lista de imagens para usar como icones de alerta, mantida pelo gerenciador de serviços

  int   _currentRuleIndex;   //!< Indice da regra atual na lista de regras de análise
  bool  _ignoreChangeEvents; //!< Indica que o slot que monitora mudanças está temporariamente desligado
  bool  _ruleChanged; //!< Indica que algum dos dados apresentados foi alterado pelo usuário
  bool  _newRule;     //!< Indica que a regra atual é uma nova regra
  int   _numAnalysisModules;  //!< Indica quanto módulos de análise existem
  int     _conditionAnalysisId; //!< Indice da regra de análise da qual dependemos
  QString _conditionRule;       //!< Regra de condição para análises condicionadas

  std::vector<struct wsAddMapDisplayConfig> _listAddMapId;
  std::vector<struct wsFillOperation> _listFillOperations;

  wsGridOutputConfig _gridOutputConfig;
  wsAlertIcons       _alertIcons;
  
signals:
  //! Chamado quando o usuário inicia a criação/edição de uma regra de análise.
  void editAnalysisRuleStarted();
  
  //! Chamado quando o usuário termina a criação/edição de uma regra de análise
  //! (confirmando ou cancelando).
  void editAnalysisRuleFinished();
  
  //! Chamado quando uma análise foi excluída.
  void analysisRuleDeleted(int id);
};


#endif

