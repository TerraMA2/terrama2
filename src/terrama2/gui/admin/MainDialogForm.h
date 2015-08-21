/************************************************************************************
TerraMA² is a development platform of Monitoring, Analysis and Alert for
geoenvironmental extremes. Applied to natural disasters, quality of water / air / atmosphere,
tailings basins, epidemics and others.Copyright © 2012 INPE.

This code is part of the TerraMA² framework.
This framework is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose.
The library provided hereunder is on an "as is" basis, and the authors have no
obligation to provide maintenance, support, updates, enhancements,
or modifications.

In no event shall INPE or K2 SISTEMAS be held liable to any party for direct, indirect,
special, incidental, or consequential damages arising out of the use of this
library and its documentation.
*************************************************************************************/

/*!
 * \file mainDlg.h
 *
 * \brief Definicao da classe MainDlg
 *
 * \author Carlos Augusto Teixeira Mendes
 * \date january, 2009 
 * \version $Id$
 *
 */
 
#ifndef _MAINDLG_H_
#define _MAINDLG_H_

#include "ui_MainDialogForm.h"

#include "configDataManager.h"
#include "dbManager.h"

class QItemSelection;

/*! \brief Classe que implementa as açoes do diálogo principal
*/      
class MainDialogForm : public QMainWindow, private Ui::MainDlg
{
Q_OBJECT

public:
  MainDlg(QWidget* parent = 0, Qt::WFlags f = 0 );
  ~MainDlg();

signals: 

protected:
	void closeEvent(QCloseEvent* close);

private slots:
  void newRequested();
  void openRequested();
  void saveRequested();
  void saveAsRequested();
  void renameRequested();
  void removeRequested();
  void cancelRequested();
  
  void numInstancesChanged(int val);
  void reloadInstancesTableData();

  void testSendMailRequested();
  
  void showConsoles();
  void manageServices();

  void dbCheckConnectionRequested();
  void dbCreateDatabaseRequested();
  
  void loadBalanceReloadData();
  void configListSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection);
  
  void setDataChanged();
  void clearDataChanged();

  void studyChkClicked(bool);

  void generatePackage();
  void selectLogo();
  void selectOutputPath();
  void selectPDFPath();
  
private:
  void setupDataChanged();
  void setupValidators();
  void load();
  bool save(QString newfilename, QString& err);
  bool validateDbData(QString& err);
  bool validateInstanceTableData(QString& err);
  bool validate(QString& err);
  void setDialogData(const ConfigData& data);
  void setInstanceTableData(const ConfigData::Analysis& data);

  void getDialogData(ConfigData& data);
  void getLoadBalanceData(QList<int>& instances);
  void clearDialogData();
  void enableFields(bool mode);
  void discardChanges(bool restore_data);
  void showFileName();
  
  bool validateAndSaveChanges(bool newfile);
  bool verifyAndEnableChange();  

  bool validateCustomPreview();  

  ConfigDataManager _configManager;      //!< Gerenciador de configurações disponíveis
  int               _currentConfigIndex; //!< Indice da config. corrente
  bool              _newData;            //!< Indica se a configuração corrente é uma nova configuração
  bool              _dataChanged;        //!< Idica que os dados da config. corrente foram alterados
  bool              _ignoreChangeEvents; //!< Indica que modificações devem ser ignoradas por setDataChanged
  QString           _name;               //!< Nome da config. corrente
  QList<DbManager::AnaData> _loadBalData;  //!< Lista com informações de analise lida da base de dados
};


#endif

