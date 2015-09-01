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
  \file terrama2/gui/config/MainDialogUsersTab.hpp

  \brief Definition of Class MainDialogUsersTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Celso Luiz Ramos Cruz
*/



#ifndef _MAINDLG_USERS_TAB_H_
#define _MAINDLG_USERS_TAB_H_

// TerraMA2
#include "MainDialogTab.hpp"
#include "ComboBoxDelegate.h"
#include "soapModNotificacaoProxy.h"

// QT  
#include <QStandardItemModel>

class UserAnalysis;
class UserAnalysisList;
class AnalysisList;

/*!
\brief Classe respons�vel pelo tratamento da interface da ficha de
       usu�rios da janela principal
*/      
class MainDialogUsersTab : public MainDialogTab
{
Q_OBJECT

public:
  MainDialogUsersTab(MainDlg* main_dialog, Services* manager);

  virtual ~MainDialogUsersTab();

  virtual void load();
  virtual bool dataChanged();
  virtual bool validate(QString& err);
  virtual bool save();
  virtual void discardChanges(bool restore_data);

protected:
  virtual QString verifyAndEnableChangeMsg();

public slots:
  void analysisRuleDeleted(int analysisID);

private slots:
  void listItemSelectionChanged(const QItemSelection&, const QItemSelection&);
  void tableItemCurrentChanged(const QModelIndex&, const QModelIndex&);


  void addAnalysisRequested();
  void updateAnalysisRequested(int id, QString newName);
  void removeAnalysisRequested();

  void addFilterRequested();
  void removeFilterRequested();

  void setUserChanged();

  void userViewChanged(const QModelIndex &, const QModelIndex &);
  
  void addNewUserRequested();
  void removeUserRequested();

  void userDoubleClicked(QListWidgetItem *);

private:
  void clearFields(bool clearlist);
  void setFields(UserAnalysis* ua);
  void getFields(UserAnalysis* ua);
  void enableFields(bool mode);
  void clearUserChanged();

  int findAnaRule(AnalysisList* anaList, int id);
  int findSelectedList(const std::vector<wsUserView>& listAux, int id);
  void fillListAnalysis();
  void fillListNotification();
  
  UserAnalysisList*     _userList;      //!< Refer�ncia para lista com dados dos usu�rios, mantida pelo gerenciador de servi�os
  AnalysisList*         _anaSelList;    //!< Refer�ncia para lista com dados das regras de an�lise, mantida pelo gerenciador de servi�os
  int   _currentUserIndex;              //!< Indice do usu�rio atual na lista de usu�rios
  bool  _ignoreChangeEvents;            //!< Indica que o slot que monitora mudan�as est� temporariamente desligado
  bool  _userChanged;                   //!< Indica que algum dos dados apresentados foi alterado pelo usu�rio
  bool  _newUser;                       //!< Indica que o usu�rio � um novo usu�rio
  QStandardItemModel*   _model;			//!< Representa o modelo por tr�s da tabela
  ComboBoxDelegate*     _comboDelegate; //!< O delegate utilizado para a edi��o da tabela atrav�s de uma combo

  std::vector<struct wsUserView>_listSelUserViews; //!< lista de userviews selecionadas para o usu�rio corrente  

signals:
  //! Chamado quando o usu�rio inicia a cria��o/edi��o de um usu�rio.
  void editUserStarted();
  
  //! Chamado quando o usu�rio termina a cria��o/edi��o de um usu�rio
  //! (confirmando ou cancelando).
  void editUserFinished();
};


#endif

