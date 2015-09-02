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
  \file terrama2/gui/config/MainDialogCellularSpaceTab.hpp

  \brief Definition of Class MainDialogCellularSpaceTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Gustavo Sampaio
*/

#ifndef _MAINDLG_CELLULARSPACE_TAB_H_
#define _MAINDLG_CELLULARSPACE_TAB_H_

// TerraMA2
#include "MainDialogTab.hpp"

class CellularSpace;
class CellularSpaceList;

/*!
\brief Classe responsável pelo tratamento da interface da ficha de espaços celulares, na janela principal
*/      
class MainDialogCellularSpaceTab : public MainDialogTab
{
	Q_OBJECT

public:
	MainDialogCellularSpaceTab(MainDlg* main_dialog, Services* manager);

	virtual ~MainDialogCellularSpaceTab();

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
	void setCellularSpaceChanged();

	void comboItemChanged(int index);

	void insertCellularSpaceRequested();
	void removeCellularSpaceRequested();

private:
	void clearFields(bool clearlist);
	void setFields(const CellularSpace* cs);
	void setThemeFields(const struct wsTheme& theme);
	void getFields(CellularSpace* cs);
	void enableFields(bool mode);
	void clearCellularSpaceChanged();
	void fillComboBoxTheme();

	int findThemeIndex(int id);

	std::vector<struct wsTheme> _cellThemes;	// Lista de temas disponiveis para o espaço celular

	CellularSpaceList* _cellularSpaceList; //!< Referência para lista com dados de espaços celulares, mantida pelo gerenciador de serviços

	int   _currentCellularSpaceIndex;    //!< Indice do espaço celular atual na lista de espaços celulares
	bool  _ignoreChangeEvents;			 //!< Indica que o slot que monitora mudanças está temporariamente desligado
	bool  _cellularSpaceChanged;		 //!< Indica que algum dos dados apresentados foi alterado pelo usuário
	bool  _newCellularSpace;			 //!< Indica que o espaco celular atual é novo

signals:
	//! Chamado quando o usuário inicia a criação/edição de um espaço celular.
	void editCellularSpaceStarted();

	//! Chamado quando o usuário termina a criação/edição de um espaço celular (confirmando ou cancelando).
	void editCellularSpaceFinished();
};

#endif