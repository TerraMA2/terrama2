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
  \file terrama2/gui/admin/MainDialog.hpp

  \brief Admin main dialog - Class that implements the actions of the main dialog

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef __TERRAMA2_INTERNAL_GUI_ADMIN_MAINDIALOG_HPP__
#define __TERRAMA2_INTERNAL_GUI_ADMIN_MAINDIALOG_HPP__

// QT
// #include "ConfigDataManager.hpp"
// #include "DbManager.hpp"
#include "ui_MainDialogForm.h"

class QItemSelection;
 
class MainDialog : public QMainWindow, private Ui::MainDialogForm
{
Q_OBJECT

public:
  MainDialog(QWidget* parent = 0, Qt::WindowFlags f = 0 );
  ~MainDialog();

signals: 

protected:
	void closeEvent(QCloseEvent* close);

private slots:
  // void newRequested();
  // void openRequested();
  // void saveRequested();
  // void saveAsRequested();
  // void renameRequested();
  // void removeRequested();
  // void cancelRequested();
  
  // void numInstancesChanged(int val);
  // void reloadInstancesTableData();

  // void testSendMailRequested();
  
  // void showConsoles();
  // void manageServices();

  // void dbCheckConnectionRequested();
  // void dbCreateDatabaseRequested();
  
  // void loadBalanceReloadData();
  // void configListSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection);
  
  // void setDataChanged();
  // void clearDataChanged();

  // void studyChkClicked(bool);

  // void generatePackage();
  // void selectLogo();
  // void selectOutputPath();
  // void selectPDFPath();
  
private:
  // void setupDataChanged();
  // void setupValidators();
  // void load();
  // bool save(QString newfilename, QString& err);
  // bool validateDbData(QString& err);
  // bool validateInstanceTableData(QString& err);
  // bool validate(QString& err);
  // void setDialogData(const ConfigData& data);
  // void setInstanceTableData(const ConfigData::Analysis& data);

  // void getDialogData(ConfigData& data);
  // void getLoadBalanceData(QList<int>& instances);
  // void clearDialogData();
  // void enableFields(bool mode);
  // void discardChanges(bool restore_data);
  // void showFileName();
  
  // bool validateAndSaveChanges(bool newfile);
  // bool verifyAndEnableChange();  

  // bool validateCustomPreview();  

  // ConfigDataManager _configManager;      //!< Gerenciador de configurações disponíveis
  // int               _currentConfigIndex; //!< Indice da config. corrente
  // bool              _newData;            //!< Indica se a configuração corrente é uma nova configuração
  // bool              _dataChanged;        //!< Idica que os dados da config. corrente foram alterados
  // bool              _ignoreChangeEvents; //!< Indica que modificações devem ser ignoradas por setDataChanged
  // QString           _name;               //!< Nome da config. corrente
  // QList<DbManager::AnaData> _loadBalData;  //!< Lista com informações de analise lida da base de dados
};


#endif // __TERRAMA2_INTERNAL_GUI_ADMIN_MAINDIALOG_HPP__

