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
  \file terrama2/gui/config/MainDialogBulletinTab.hpp

  \brief Definition of Class MainDialogBulletinTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Raphael Meloni
*/

#ifndef _MAINDLG_BULLETINS_TAB_H_
#define _MAINDLG_BULLETINS_TAB_H_

// TerraMA2
#include "MainDialogTab.hpp"

#include "soapModNotificacaoProxy.h"

class Bulletin;
class BulletinList;
class AnalysisList;
class UserAnalysisList;

/*!
\brief Classe responsável pelo tratamento da interface da ficha de
       usuários da janela principal
*/      
class MainDialogBulletinsTab : public MainDialogTab
{
Q_OBJECT

public:
  MainDialogBulletinsTab(MainDlg* main_dialog, Services* manager);

  virtual ~MainDialogBulletinsTab();

  virtual void load();
  virtual bool dataChanged();
  virtual bool validate(QString& err);
  virtual bool save();
  virtual void discardChanges(bool restore_data);

protected:
  virtual QString verifyAndEnableChangeMsg();

private slots:
  void listItemSelectionChanged(const QItemSelection&, const QItemSelection&);

  void setBulletinChanged();
  
  void addNewBulletinRequested();
  void removeBulletinRequested();

  void addAnalysisBulletinRequested();
  void removeAnalysisBulletinRequested();

  void addUsersBulletinRequested();
  void removeUsersBulletinRequested();

  void bulletinDoubleClicked(QListWidgetItem *);

private:
  void clearFields(bool clearlist);
  void setFields(Bulletin* bu);
  void getFields(Bulletin* bu);
  void enableFields(bool mode);
  void clearBulletinChanged();

  void fillBulletinsAnalysis();
  void fillBulletinsUsers();

  int findAnaRule(AnalysisList* anaList, int id, int& idRef, wsAnalysisType& analysisType);
  int findUser(UserAnalysisList* userList, std::string login);

  bool  _bulletinChanged;               //!< Indica que algum dos dados apresentados foi alterado pelo usuário
  
  BulletinList*     _bulletinList;      //!< Referência para lista com dados dos boletins, mantida pelo gerenciador de serviços
  int   _currentBulletinIndex;          //!< Indice do boletim atual na lista de boletins
  bool  _ignoreChangeEvents;            //!< Indica que o slot que monitora mudanças está temporariamente desligado
  
  bool  _newBulletin;                   //!< Indica que o boletim é um novo boletim

  std::vector<struct wsAnalysisRule>_listSelAnalysis; //!< lista de regras de analise selecionadas para o usuário corrente
  std::vector<struct wsUser>_listSelUser; //!< lista de user selecionadas para o usuário corrente

signals:
  //! Chamado quando o usuário inicia a criação/edição de um boletim.
  void editBulletinStarted();
  
  //! Chamado quando o usuário termina a criação/edição de um boletin
  //! (confirmando ou cancelando).
  void editBulletinFinished();    
};


#endif

