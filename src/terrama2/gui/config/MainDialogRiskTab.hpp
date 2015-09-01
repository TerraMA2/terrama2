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
  \file terrama2/gui/config/MainDialogRiskTab.hpp

  \brief Definition of Class MainDialogRiskTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _MAINDLG_RISK_TAB_H_
#define _MAINDLG_RISK_TAB_H_

// TerraMA2
#include "MainDialogTab.hpp"

class RiskMap;
class RiskMapList;

/*!
\brief Classe responsável pelo tratamento da interface da ficha de
       objetos monitorados da janela principal
*/      
class MainDialogRiskTab : public MainDialogTab
{
Q_OBJECT

public:
  MainDialogRiskTab(MainDlg* main_dialog, Services* manager);

  virtual ~MainDialogRiskTab();

  virtual void load();
  virtual bool dataChanged();
  virtual bool validate(QString& err);
  virtual bool save();
  virtual void discardChanges(bool restore_data);

protected:
  virtual QString verifyAndEnableChangeMsg();

private slots:
  void listItemSelectionChanged(const QItemSelection&, const QItemSelection&);
  void projectionDialogRequested();
  void setMapChanged();

  void comboItemChanged(int index);

  void insertRiskRequested();
  void removeRiskRequested();

private:
  void clearFields(bool clearlist);
  void setFields(const RiskMap* map);
  void setThemeFields(const struct wsRiskMapTheme& theme);
  void getFields(RiskMap* map);
  void enableFields(bool mode);
  void clearMapChanged();
  void fillComboBoxTheme();
  
  int findThemeIndex(int id);

  std::vector<struct wsRiskMapTheme> _riskMapTheme;	// Lista de temas disponiveis para o objeto monitorado
  std::vector<struct wsRiskMapProperty> _riskMapProperties; // Lista de propriedades dos atributos dos objetos monitorados

  RiskMapList* _riskMapList; //!< Referência para lista com dados de objetos monitorados, mantida pelo gerenciador de serviços

  int   _currentMapIndex;    //!< Indice do mapa atual na lista de mapas
  bool  _ignoreChangeEvents; //!< Indica que o slot que monitora mudanças está temporariamente desligado
  bool  _mapChanged;         //!< Indica que algum dos dados apresentados foi alterado pelo usuário
  bool  _newMap;             //!< Indica que o mapa atual é um novo mapa
  
signals:
  //! Chamado quando o usuário inicia a criação/edição de um objeto monitorado.
  void editRiskMapStarted();
  
  //! Chamado quando o usuário termina a criação/edição de um objeto monitorado
  //! (confirmando ou cancelando).
  void editRiskMapFinished();
};


#endif

