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
  \file terrama2/gui/config/MainDialogAditionalTab.hpp

  \brief Definition of Class MainDialogAditionalTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _MAINDLG_ADITIONAL_TAB_H_
#define _MAINDLG_ADITIONAL_TAB_H_

// TerraMA2
#include "MainDialogTab.hpp"

class AdditionalMap;
class AdditionalMapList;

enum mdatAdditionalType
{
	MDAT_TYPE_NONE,
	MDAT_TYPE_GRID,
	MDAT_TYPE_VECTOR,
};

/*!
\brief Classe responsável pelo tratamento da interface da ficha de
       análises da janela principal
*/      
class MainDialogAditionalTab : public MainDialogTab
{
Q_OBJECT

public:
  MainDialogAditionalTab(MainDlg* main_dialog, Services* manager);

  virtual ~MainDialogAditionalTab();

  virtual void load();
  virtual bool dataChanged();
  virtual bool validate(QString& err);
  virtual bool save();
  virtual void discardChanges(bool restore_data);

protected:
  virtual QString verifyAndEnableChangeMsg();

private slots:
  void listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection);

  void projectionDialogRequested();

  void setAdditionalMapChanged();

  void insertAdditionalMapGridRequested();
  void insertAdditionalMapVectRequested();
  void removeAdditionalMapRequested();

  void comboItemChanged(int index);

private:
  void clearFields(bool clearlist);
  void setFields(const AdditionalMap* additionalMap);
  void setThemeFields(const struct wsTheme& theme);
  void getFields(AdditionalMap* additionalMap);
  void getThemeFields(AdditionalMap* additionalMap);
  void enableFields(bool mode);
  void clearAdditionalMapChanged();

  int findThemeIndex(enum mdatAdditionalType typeTheme, int id);
  int auxFindThemeIndex(const std::vector<struct wsTheme>& mapThemeList, int id);

  void loadAdditionalMapData();

  void fillComboBox(const std::vector<struct wsTheme>& _additionalMapTheme);

  std::vector<struct wsTheme> _additionalMapThemeGrid;   // Lista de temas disponiveis tipo Grid para o mapa adicional
  std::vector<struct wsTheme> _additionalMapThemeVector; // Lista de temas disponiveis tipo Vector para o mapa adicional

  AdditionalMapList* _additionalMapList; //!< Referência para lista com dados de mapas adicional, mantida pelo gerenciador de serviços

  enum mdatAdditionalType _currentTypeAdditionalMap;     //!< Tipo de mapa adicional atual (-1 = nenhum, 0
  int   _currentAdditionalMapIndex;    //!< Indice do mapa adicional atual na lista de mapas
  std::vector<struct wsRiskMapProperty> _addMapProperties; // Lista de propriedades dos atributos do atual mapa adicional
  bool  _ignoreChangeEvents;           //!< Indica que o slot que monitora mudanças está temporariamente desligado
  bool  _additionalMapChanged;         //!< Indica que algum dos dados apresentados foi alterado pelo usuário
  bool  _newAdditionalMap;             //!< Indica que o mapa adicional atual é um novo mapa adicional
  
signals:
  //! Chamado quando o usuário inicia a criação/edição de um mapa adicional.
  void editAdditionalMapStarted();
  
  //! Chamado quando o usuário termina a criação/edição de um mapa adicional
  //! (confirmando ou cancelando).
  void editAdditionalMapFinished();

  //! Chamado quando o usuário salva ou apaga um mapa adicional do banco
  void additionalMapDatabaseChanged();
};


#endif

