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
  \file terrama2/gui/config/MainDialogUsersTab.cpp

  \brief Definition of Class MainDialogUsersTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Celso Luiz Ramos Cruz
*/

// STL
#include <assert.h>

// QT  
#include <QMessageBox>
#include <QStandardItemModel>
#include <QHeaderView>

// TerraMA2  
#include "ComboBoxDelegate.h"

#include "MainDialogUsersTab.hpp"
#include "Services.hpp"
#include "UserAnalysis.hpp"
#include "UserAnalysisList.hpp"
#include "AnalysisList.hpp"
#include "AnalysisListDialog.hpp"
#include "AddListDialog.hpp"
#include "AdditionalMapList.hpp"
#include "Utils.hpp"

//! Construtor.  Prepara interface e estabelece conexões
MainDialogUsersTab::MainDialogUsersTab(MainDialog* main_dialog, Services* manager)
  : MainDialogTab(main_dialog, manager)
{
  _userList    = NULL;
  _anaSelList = NULL;
  _userChanged = false;
  _newUser     = false;
  _ignoreChangeEvents = false;
  _currentUserIndex   = -1;

  // Criamos o delegate do combo que será usado para
  // a edição das colunas Email e Celular
  _comboDelegate = new ComboBoxDelegate();
  _comboDelegate->insertItem(Utils::warningLevelColor(1),Utils::warningLevelToString(1));
  _comboDelegate->insertItem(Utils::warningLevelColor(2),Utils::warningLevelToString(2));
  _comboDelegate->insertItem(Utils::warningLevelColor(3),Utils::warningLevelToString(3));
  _comboDelegate->insertItem(Utils::warningLevelColor(4),Utils::warningLevelToString(4));
  _comboDelegate->insertItem(Utils::warningLevelColor(5),Utils::warningLevelToString(5));

  // E para a edição da coluna Relatorio
  ComboBoxDelegate* reportComboDelegate = new ComboBoxDelegate();
  reportComboDelegate->insertItem(Utils::reportTypeToString(0));
  reportComboDelegate->insertItem(Utils::reportTypeToString(1));

  // Cria o modelo por trás da tabela
  _model = new QStandardItemModel(_listSelUserViews.size(), 4);
  _model->setHorizontalHeaderItem(0, new QStandardItem("Análise"));
  _model->setHorizontalHeaderItem(1, new QStandardItem("Email"));
  _model->setHorizontalHeaderItem(2, new QStandardItem("Celular"));
  _model->setHorizontalHeaderItem(3, new QStandardItem("Tipo"));

  _ui->tableView->setModel(_model);

  _ui->tableView->horizontalHeader()->setHighlightSections(false);
  _ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

  // Associamos o delegate que permite escolher o nível de alerta de
  // uma userview na tabela através de uma combo
  _ui->tableView->setItemDelegateForColumn(1, _comboDelegate);
  _ui->tableView->setItemDelegateForColumn(2, _comboDelegate);
  _ui->tableView->setItemDelegateForColumn(3, reportComboDelegate);

  connect(_ui->tableView->selectionModel(), 
	  SIGNAL(currentRowChanged ( const QModelIndex&, const QModelIndex& ) ),
	  SLOT(tableItemCurrentChanged(const QModelIndex&, const QModelIndex&)));

  // Conecta sinais tratados pela classe
  // Para monitorar a mudança de regra na lista de regras de análise, estamos
  // usando um sinal do modelo de seleção e não o tradicional currentRowChanged()
  // Isso é feito pois em currentRowChanged() não conseguimos voltar para a seleção
  // anterior caso o usuário deseje cancelar a troca.
  connect(_ui->userAnalysisListWidget->selectionModel(), 
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));

#ifdef _DEBUG
  connect(_ui->userAnalysisListWidget, SIGNAL(itemDoubleClicked ( QListWidgetItem * ) ), SLOT( userDoubleClicked(QListWidgetItem *)));
#endif // _DEBUG
  
  connect(_ui->userAnalysisAddSelectedAnalysysBtn,     SIGNAL(clicked()), SLOT(addAnalysisRequested()));
  connect(_ui->userAnalysisRemoveSelectedAnalysysBtn,  SIGNAL(clicked()), SLOT(removeAnalysisRequested()));

  connect(_ui->filterAdditionalAddBtn,     SIGNAL(clicked()), SLOT(addFilterRequested()));
  connect(_ui->filterAdditionalRemoveBtn,  SIGNAL(clicked()), SLOT(removeFilterRequested()));

  // Conecta sinais para adicionar e remover usuários
  connect(_ui->userAnalysisAddUserBtn,     SIGNAL(clicked()), SLOT(addNewUserRequested()));
  connect(_ui->userAnalysisRemoveUserBtn,  SIGNAL(clicked()), SLOT(removeUserRequested()));
  
  // Conecta sinais tratados de maneira generica por MainDlgTab
  connect(_ui->userAnalysisSaveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  connect(_ui->userAnalysisCancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

  // Conecta sinais para detectar dados alterados
  connect(_ui->userAnalysisLoginLed,     SIGNAL(textEdited(const QString&)), SLOT(setUserChanged()));
  connect(_ui->userAnalysisPasswordLed,  SIGNAL(textEdited(const QString&)), SLOT(setUserChanged()));
  connect(_ui->userAnalysisFullName,     SIGNAL(textEdited(const QString&)), SLOT(setUserChanged()));
  connect(_ui->userAnalysisCellPhoneLed, SIGNAL(textEdited(const QString&)), SLOT(setUserChanged()));
  connect(_ui->userAnalysisEMailLed,     SIGNAL(textEdited(const QString&)), SLOT(setUserChanged()));
  connect(_ui->rbGridDefCanvasSmall,	 SIGNAL(toggled(bool)),			     SLOT(setUserChanged()));
  connect(_ui->rbGridDefCanvasMedium,	 SIGNAL(toggled(bool)),			     SLOT(setUserChanged()));
  connect(_ui->rbGridDefCanvasLarge,	 SIGNAL(toggled(bool)),			     SLOT(setUserChanged()));
  connect(_model,						 SIGNAL(dataChanged (const QModelIndex &, const QModelIndex &)), 
										 SLOT(userViewChanged(const QModelIndex &, const QModelIndex &)));
}

//! Destrutor
MainDialogUsersTab::~MainDialogUsersTab()
{
}

// Funcao comentada na classe base
void MainDialogUsersTab::load()
{

  // Carrega novas informações
  _userList         = _manager->userAnalysisList();
  _anaSelList		= _manager->analysisList();
  assert(_userList);
  assert(_anaSelList);

  connect(_anaSelList, SIGNAL(afterUpdateAnalysis(int, QString)), SLOT(updateAnalysisRequested(int, QString)));
  
  // Limpa dados
  clearFields(true);
  _currentUserIndex   = -1;

  // Preenche lista de regras e mostra dados do primeiro usuário
  if(_userList->count()) 
  {
    enableFields(true);

    for(int i=0, count=(int)_userList->count(); i<count; i++)
    {
      UserAnalysis* ua = _userList->at(i);
      _ui->userAnalysisListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/usuario.png"), ua->login()));
    }
	if(_ui->userAnalysisListWidget->count() > 0)
		_ui->userAnalysisListWidget->setCurrentRow(0);
  }  
  else
    enableFields(false); // Se não há entradas na lista, desabilita campos
}

// Funcao comentada na classe base
bool MainDialogUsersTab::dataChanged()
{
  return _userChanged;
}

// Funcao comentada na classe base
bool MainDialogUsersTab::validate(QString& err)
{
  return true;
}

// Funcao comentada na classe base
bool MainDialogUsersTab::save()
{
  UserAnalysis ua;
  if(!_newUser)
    ua = *(_userList->at(_currentUserIndex));
  
  // Carrega dados da interface
  getFields(&ua);
  
  bool ok;
  
  // Salva
  if(!_newUser)
	  ok = _userList->updateUserAnalysis(&ua);
  else
	  ok = _userList->addNewUserAnalysis(&ua);
  
  // Se a operação de salvar não deu certo, retorna false e mantém o estado atual
  if(!ok)
    return false;
  
  // Atualiza nome da entrada atual da lista que pode ter sido alterado
  _ui->userAnalysisListWidget->item(_currentUserIndex)->setText(ua.login());  
  
  // Atualiza estado para dados não alterados
  clearUserChanged();
  _newUser = false;

  return true;
}

// Funcao comentada na classe base
void MainDialogUsersTab::discardChanges(bool restore_data)
{
  if(_newUser)
  {
    // Estamos descartando uma análise recém criada que não foi salva na base
    // 1) Remove entrada da lista (interface).  Deve ser a última linha
    assert(_currentUserIndex == _ui->userAnalysisListWidget->count()-1);
    _ignoreChangeEvents = true;
    delete _ui->userAnalysisListWidget->takeItem(_currentUserIndex);    
    _ui->userAnalysisListWidget->setCurrentRow(-1);
    _currentUserIndex = -1;
    _ignoreChangeEvents = false;

    // 2) Desmarca indicador de nova regra e de dados modificados
    _newUser = false;
    clearUserChanged();  
    
    // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
    if(restore_data)
    {
      if(_ui->userAnalysisListWidget->count())
        _ui->userAnalysisListWidget->setCurrentRow(0);
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
      setFields(_userList->at(_currentUserIndex));
    else  
      clearUserChanged(); 
  }  
}

/*! \brief Limpa a interface.  

Flag indica se a lista de regras e a lista de objetos monitorados também devem ser limpas
*/
void MainDialogUsersTab::clearFields(bool clearlist)
{
  _ignoreChangeEvents = true;

  // Lista de regras
  if(clearlist)
    _ui->userAnalysisListWidget->clear();

  _ui->userAnalysisLoginLed->clear();
  _ui->userAnalysisPasswordLed->clear();
  _ui->userAnalysisFullName->clear();
  _ui->userAnalysisCellPhoneLed->clear();
  _ui->userAnalysisEMailLed->clear();

  _ui->rbGridDefCanvasSmall->setChecked(false);
  _ui->rbGridDefCanvasMedium->setChecked(false);
  _ui->rbGridDefCanvasLarge->setChecked(true);

  _listSelUserViews.clear();
  fillListAnalysis();
  fillListNotification();

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearUserChanged();
}

//! Habilita ou desabilita campos da interface
void MainDialogUsersTab::enableFields(bool mode)
{
  // Campos
  _ui->userAnalysisLoginLed->setEnabled(mode);
  _ui->userAnalysisPasswordLed->setEnabled(mode);
  _ui->userAnalysisFullName->setEnabled(mode);
  _ui->userAnalysisCellPhoneLed->setEnabled(mode);
  _ui->userAnalysisEMailLed->setEnabled(mode);

  _ui->rbGridDefCanvasSmall->setEnabled(mode);
  _ui->rbGridDefCanvasMedium->setEnabled(mode);
  _ui->rbGridDefCanvasLarge->setEnabled(mode);

  _ui->userAnalysisAddSelectedAnalysysBtn->setEnabled(mode);
  _ui->userAnalysisRemoveSelectedAnalysysBtn->setEnabled(mode);

  _ui->userAnalysisRemoveUserBtn->setEnabled(mode);

  _ui->tableView->setEnabled(mode);

  _ui->filterGrb->setEnabled(mode);

  _ui->userAnalysisSaveBtn->setEnabled(mode);
  _ui->userAnalysisCancelBtn->setEnabled(mode);
}

//! Preenche a interface com os dados de uma regra de análise
void MainDialogUsersTab::setFields(UserAnalysis* ua)
{
  _ignoreChangeEvents = true; 

  // Preenche dados comuns
  _ui->userAnalysisLoginLed->setText(ua->login());
  _ui->userAnalysisPasswordLed->setText(ua->password());
  _ui->userAnalysisFullName->setText(ua->fullName());
  _ui->userAnalysisCellPhoneLed->setText(ua->cellPhone());
  _ui->userAnalysisEMailLed->setText(ua->email());

  if (ua->canvasSize() == WS_CANVASSIZE_SMALL)
    _ui->rbGridDefCanvasSmall->setChecked(true);
  else if (ua->canvasSize() == WS_CANVASSIZE_MEDIUM)
    _ui->rbGridDefCanvasMedium->setChecked(true);
  else if (ua->canvasSize() == WS_CANVASSIZE_LARGE)
    _ui->rbGridDefCanvasLarge->setChecked(true);
  else
    _ui->rbGridDefCanvasLarge->setChecked(true);

  // Preenche lista de grids
  _listSelUserViews.clear();
  _listSelUserViews = ua->listUserViewsbyUser();
 
  fillListAnalysis();
  _ui->tableView->resizeColumnsToContents();

  if (_listSelUserViews.size())
  {
	  QModelIndex index = _ui->tableView->model()->index(0,0);
	  _ui->tableView->setCurrentIndex( index );
  }
  fillListNotification();
  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearUserChanged();
}


//! Preenche a análise com os dados da interface, que já devem ter sido validados
void MainDialogUsersTab::getFields(UserAnalysis* ua)
{
  // Preenche dados comuns
  ua->setLogin(_ui->userAnalysisLoginLed->text().trimmed());
  ua->setPassword(_ui->userAnalysisPasswordLed->text().trimmed());
  ua->setFullName(_ui->userAnalysisFullName->text().trimmed());
  ua->setCellPhone(_ui->userAnalysisCellPhoneLed->text().trimmed());
  ua->setEmail(_ui->userAnalysisEMailLed->text().trimmed());

  ua->setCanvasSize(_ui->rbGridDefCanvasSmall->isChecked()? WS_CANVASSIZE_SMALL:
					_ui->rbGridDefCanvasLarge->isChecked()? WS_CANVASSIZE_LARGE:
															WS_CANVASSIZE_MEDIUM);

  ua->setUserViewsByUser(_listSelUserViews);
}

/*! \brief Indica que algum dos dados apresentados foi alterado.  

Habilita botões de salvar e cancelar
*/
void MainDialogUsersTab::setUserChanged()
{
  if(_ignoreChangeEvents)
    return;

  _userChanged = true;
  _ui->userAnalysisSaveBtn->setEnabled(true);
  _ui->userAnalysisCancelBtn->setEnabled(true);
  if(_newUser)
    _parent->statusBar()->showMessage(tr("Novo usuário."));
  else
    _parent->statusBar()->showMessage(tr("Usuário alterado."));

  _ui->userAnalysisAddUserBtn->setEnabled(false);

  _ui->userAnalysisLoginLed->setEnabled(_newUser);
  
  emit editUserStarted();
}

/*! \brief Indica que os dados mostrados estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar
*/
void MainDialogUsersTab::clearUserChanged()
{
  _userChanged = false;
  _ui->userAnalysisSaveBtn->setEnabled(false);
  _ui->userAnalysisCancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();

  _ui->userAnalysisLoginLed->setEnabled(false);

  _ui->userAnalysisAddUserBtn->setEnabled(true);
  
  emit editUserFinished();
}

// Função comentada na classe base
QString MainDialogUsersTab::verifyAndEnableChangeMsg()
{
  return tr("As alterações efetuadas na tela de usuários\n"
            "ainda não foram salvas.  Deseja salvar as alterações?");
}

//! Slot chamado quando a linha corrente é alterada na lista de análises
void MainDialogUsersTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
  if(_ignoreChangeEvents)
    return;

  QModelIndexList selected_indexes = selected.indexes();
  
  // Se usuário clicou na lista fora de qq. item, remarca item anterior
  if(!selected_indexes.count())
  {
    if(oldSelection.indexes().count()) // Evita loop infinito se não existir seleção anterior...
      _ui->userAnalysisListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    return;
  }
  
  // Obtem a linha selecionada    
  int row = selected_indexes[0].row();

  // Verifica se estamos apenas voltando à mesma opção atual.  Ocorre 
  // quando uma troca de regra foi cancelada
  if(row == _currentUserIndex)
    return;

  bool ok = true;
  
  // Verifica se os dados atuais na tela foram modificados
  // e em caso positivo se podemos efetuar a troca de dados
  if(_currentUserIndex != -1)
    ok = verifyAndEnableChange(false);

  if(ok)
  {
    // Operação permitida.  Troca dados na tela
    if(_newUser)
    {
      // Estamos tratando a seleção de uma análise recém incluida na lista
      assert(row == _userList->count());
      clearFields(false);
    }
    else 
    {
      // Estamos tratando uma seleção normal feita pelo usuário
      setFields(_userList->at(row));
    }  
    _currentUserIndex = row;
  }
  else
  {
    // Operação foi cancelada.  Devemos reverter à regra original
    _ui->userAnalysisListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    _ui->userAnalysisListWidget->setCurrentRow(_currentUserIndex);
  }
}

//! Slot chamado quando o usuário pressiona o botão de adicionar analises
void MainDialogUsersTab::addAnalysisRequested()
{
  _manager->analysisList()->clear();
  if(!_manager->loadAnalysisList())
	return;

  AnalysisList* anaList = _manager->analysisList();
  AnalysisListDlg dlg(false);

  if(dlg.setFields(_listSelUserViews, anaList))
  {
    if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
    {
      QList<int> newIDSelectedAnalyses;
      bool changed;

      dlg.getFields(newIDSelectedAnalyses, changed);

      if (changed)
      {
	    int cellphone = 0;
		int email = 0;
        int report = 0;
        dlg.getLevels(email, cellphone, report);

	    int pos;
		for (int i=0; i<newIDSelectedAnalyses.size(); i++)
	    {
		  pos = findAnaRule(anaList, newIDSelectedAnalyses.at(i));

		  if (pos != -1)
		  {
			  wsUserView newUserView;
			  newUserView.cellularWarningLevel = cellphone;
			  newUserView.emailWarningLevel = email;
			  newUserView.reportType = report;
			  newUserView.analysisName = anaList->at(pos)->name().toStdString();
			  newUserView.analysisID = newIDSelectedAnalyses.at(i);
			  _listSelUserViews.push_back(newUserView);
		  }
	    }

	    fillListAnalysis();
        setUserChanged();
	  }
	}
  }
  else
  {
	  QMessageBox::information(_parent, tr("Adicionar análises"), tr("Todas as análises já foram adicionadas a esse usuário."));
  }
}

int MainDialogUsersTab::findAnaRule(AnalysisList* anaList, int id)
{
   int ret=-1;

   for (int i=0; i<anaList->size() && ret==-1; i++)
   {
	   if (anaList->at(i)->id() == id)
		   ret = i;
   }

   return ret;
}

int MainDialogUsersTab::findSelectedList(const std::vector<wsUserView>& listAux, int id)
{
   int ret=-1;

   for (size_t i=0; i<listAux.size() && ret==-1; i++)
   {
	   if (listAux.at(i).analysisID == id)
		   ret = i;
   }

   return ret;
}


//! Slot chamado quando o usuário pressiona o botão de remover analises
void MainDialogUsersTab::removeAnalysisRequested()
{
  QStandardItemModel item;
  QVector<struct wsUserView> listAux;
  listAux = QVector<struct wsUserView>::fromStdVector(_listSelUserViews);
  int id;
  int niPos;
  bool removed = false;

  if(_ui->tableView->currentIndex().row() != -1)
  { 
	listAux.remove(_ui->tableView->currentIndex().row());
	removed = true;
  }

  if (removed)
  {
    setUserChanged();
	_listSelUserViews = listAux.toStdVector();
    _ignoreChangeEvents = true;
	fillListAnalysis();
	fillListNotification();
	_ignoreChangeEvents = false;
  }
}

//! Slot chamado quando deseja criar um novo usuário
void MainDialogUsersTab::addNewUserRequested()
{
  // Verifica se podemos ir para novo usuário
  if(!verifyAndEnableChange(false))
    return;
    
  // Ok. Podemos criar nova entrada.
  // 1) Gera um login temporário para o usuário
  QString login= tr("Novo usuário");
  
  // 2) Inclui nome na lista de usuários e seleciona este usuário
  //    Antes de selecionar, marca que estamos tratando de um novo usuário
  _newUser = true;
  _ui->userAnalysisListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/usuario.png"), login));

  _ui->userAnalysisListWidget->setCurrentRow(_ui->userAnalysisListWidget->count()-1);
  
  // 3) Tratador de seleção já limpou a tela.  Habilita os widgets corretos
  enableFields(true);

  // 4) Preenche campo com o login do usuário e marca o usuário como modificado
  clearFields(false);
  setUserChanged();
  _listSelUserViews.clear();
  fillListAnalysis();
  fillListNotification();

  _ui->userAnalysisLoginLed->setText(login);
  _ui->userAnalysisLoginLed->setFocus();
  _ui->userAnalysisLoginLed->selectAll();
}

//! Slot chamado quando deseja remover o usuário atual
void MainDialogUsersTab::removeUserRequested()
{
  if(_currentUserIndex == -1)
    return;
    
  // Verifca com o usuário se ele deseja realmente remover o plano
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(_parent, tr("Remover usuário..."), 
                                 tr("Deseja realmente remover este usuário ?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No);
  if(answer == QMessageBox::No)
    return;

  // Remove objeto do servidor.  Se não deu certo, retorna e mantém o estado atual
  if(!_newUser)
  {
	  if(!_userList->deleteUserAnalysisIndex(_currentUserIndex))
	  return;
  }
  
  // Remove objeto da lista (interface) e desmarca seleção
  _ignoreChangeEvents = true;
  delete _ui->userAnalysisListWidget->takeItem(_currentUserIndex);    
  _ui->userAnalysisListWidget->setCurrentRow(-1);
  _currentUserIndex = -1;
  _ignoreChangeEvents = false;

  // Desmarca indicadores de modificação e nova regra
  clearUserChanged();  
  _newUser = false;

  // Seleciona a primeira linha ou limpa campos se não houver nenhuma entrada na lista.
  if(_ui->userAnalysisListWidget->count())
    _ui->userAnalysisListWidget->setCurrentRow(0);
  else
  {
    clearFields(false);  
    enableFields(false);
  }
}

//! Preenche lista com as analises selecionadas.
void MainDialogUsersTab::fillListAnalysis()
{
	// Limpa as userviews da tabela
	while(_model->rowCount())
	{
		_model->removeRow(0);
	}
	_model->setRowCount(_listSelUserViews.size());

	// Cria-se uma linha na tabela para cada User View
	for(int i=0; i<(int)_listSelUserViews.size(); i++)
	{
		wsUserView userView = _listSelUserViews.at(i);

		Analysis* ana = _manager->analysisList()->findAnalysis(userView.analysisID);

        // Apenas objetos monitorados e com PCD podem ser inspecionadas
		QIcon  ico((ana && ana->getAnalysisType() == WS_ANALYSISTYPE_PCD) ?
								 ":/data/icons/dado_pontual.png" :
                                 ":/data/icons/analise_plano de risco.png");
		
		// Cria a célula que dá o nome de cada uma das análises de cada
		// User View e determina as propriedades dessa célula
		QStandardItem* item = new QStandardItem(ico, QString::fromStdString(userView.analysisName));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		_model->setItem(i, 0, item);
		_model->setData(_model->index(i, 0), userView.analysisID, Qt::UserRole);

		// Cria a célula de nível de alerta para emails
		_model->setData(_model->index(i, 1), userView.emailWarningLevel - 1, Qt::UserRole);
		_model->setData(_model->index(i, 1), Utils::warningLevelToString(userView.emailWarningLevel), Qt::DisplayRole);
		_model->setData(_model->index(i, 1), Utils::warningLevelColor(userView.emailWarningLevel), Qt::DecorationRole);

		// Cria a célula de nível de alerta para celulares
		_model->setData(_model->index(i, 2), userView.cellularWarningLevel - 1, Qt::UserRole);
		_model->setData(_model->index(i, 2), Utils::warningLevelToString(userView.cellularWarningLevel), Qt::DisplayRole);
		_model->setData(_model->index(i, 2), Utils::warningLevelColor(userView.cellularWarningLevel), Qt::DecorationRole);

		_model->setData(_model->index(i, 3), Utils::reportTypeToString(userView.reportType), Qt::DisplayRole);
		_model->setData(_model->index(i, 3), userView.reportType, Qt::UserRole);
	}
}

void MainDialogUsersTab::updateAnalysisRequested(int id, QString newName)
{
	// Primeiro, trocamos os nomes na estrutura de dados
	for(int i = 0; i < _userList->count(); i++)
	{
		for(size_t j = 0; j < _userList->at(i)->listUserViewsbyUser().size(); j++)
		{
			if(_userList->at(i)->listUserViewsbyUser().at(j).analysisID == id)
			{
				_userList->at(i)->listUserViewsbyUser().at(j).analysisName = newName.toStdString();
			}
		}
	}

	// Depois nos encarregamos da tabela atual
	for (int i = 0; i < _model->rowCount(); i++)
	{
		int idUserView = _model->item(i, 0)->data(Qt::UserRole).toInt();
		if(idUserView == id)
		{
			_model->item(i, 0)->setData(newName, Qt::DisplayRole);
		}
	}
}

void MainDialogUsersTab::analysisRuleDeleted(int analysisID)
{
  for (int i = 0; i < _userList->size(); i++)
  {
	bool removed = false;

	std::vector<wsUserView> userViews = _userList->at(i)->listUserViewsbyUser();
	std::vector<wsUserView> newUserViews(userViews);
	newUserViews.clear();

	for(int j=userViews.size()-1; j >= 0; j--)
	{
		if(userViews[j].analysisID != analysisID)
		{
			newUserViews.push_back(userViews[j]);
		}
		else
		{
			removed = true;
		}
	}

	if (removed)
	{
	  _userList->at(i)->setUserViewsByUser(newUserViews);
	}

	if(i == _currentUserIndex)
	{
		_listSelUserViews = newUserViews;
		fillListAnalysis();
	}
  }
}

void MainDialogUsersTab::userViewChanged(const QModelIndex & topLeftCell, const QModelIndex & bottomRightCell)
{
	for (int row = topLeftCell.row(); row <= bottomRightCell.row(); row++)
	{
		for (int col = topLeftCell.column(); col <= bottomRightCell.column(); col++)
		{
			// Pegamos cada um dos itens editados e seus valores
			QStandardItem* item = _model->item(row, col);
			int value = item->data(Qt::UserRole).toInt();

			// Caso esteja na coluna correta, desejamos atualizar os valores
			// alterados na lista de User Views da janela
			if(col == 1)
			{
				_listSelUserViews.at(row).emailWarningLevel = value + 1;
				setUserChanged();
			}
			else if(col == 2)
			{
				_listSelUserViews.at(row).cellularWarningLevel = value + 1;
				setUserChanged();
			}
			else if (col == 3)
			{
				_listSelUserViews.at(row).reportType = value;
				setUserChanged();
			}
		}
	}
}

//! Slot chamado quando o usuário pressiona o botão de adicionar filtro de notificacao
void MainDialogUsersTab::addFilterRequested()
{
	_manager->additionalMapList()->clear();
	if(!_manager->loadAdditionalMapData())
		return;

	int rowSelected = _ui->tableView->currentIndex().row();
	if (rowSelected < 0)
		return;

	std::vector<struct wsNotificationFilter> listNotification = _listSelUserViews[rowSelected].notificationFilter;

	AdditionalMapList* addList = _manager->additionalMapList();

	AddListDlg dlg(false);

	if(dlg.setFields(listNotification, addList))
	{
		if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
		{
			bool changed;
			dlg.getFields(listNotification, changed);

			if (changed)
			{
				_listSelUserViews[rowSelected].notificationFilter = listNotification;
				fillListNotification();
				setUserChanged();
			}
		}
	}
	else
	{
		QMessageBox::information(_parent, tr("Adicionar mapas adicionais"), tr("Não há mapa adicional com representação de polígonos disponível."));
	}
}

//! Slot chamado quando o usuário pressiona o botão de remover os filtros
void MainDialogUsersTab::removeFilterRequested()
{
	int rowSelected = _ui->tableView->currentIndex().row();
	if (rowSelected < 0)
		return;

	std::vector<struct wsNotificationFilter> listNotification = _listSelUserViews[rowSelected].notificationFilter;

	QStandardItemModel item;
	QVector<struct wsNotificationFilter> listAux;
	listAux = QVector<struct wsNotificationFilter>::fromStdVector(listNotification);
	int id;
	int niPos;
	bool removed = false;

	if(_ui->filterAdditionalLst->currentRow() != -1)
	{ 
		listAux.remove(_ui->filterAdditionalLst->currentRow());
		removed = true;
	}

	if (removed)
	{
		setUserChanged();
		listNotification = listAux.toStdVector();
		_listSelUserViews[rowSelected].notificationFilter = listNotification;
		fillListNotification();
	}	
}

void MainDialogUsersTab::fillListNotification()
{
	_ui->filterAdditionalLst->clear();
	int rowSelected = _ui->tableView->currentIndex().row();
	if (rowSelected < 0)
		return;

	if(rowSelected >= _listSelUserViews.size())
		return;

	std::vector<struct wsNotificationFilter> listNotification = _listSelUserViews[rowSelected].notificationFilter;
	for (size_t i = 0; i < listNotification.size(); i++)
	{
		_ui->filterAdditionalLst->addItem(QString::fromStdString(listNotification.at(i).additionalMapName));
	}
}

void MainDialogUsersTab::tableItemCurrentChanged( const QModelIndex& current, const QModelIndex& )
{
	if(_ignoreChangeEvents)
		return;

	//trocou a selecao da analise
	fillListNotification();
}

void MainDialogUsersTab::userDoubleClicked( QListWidgetItem * )
{
	// Verifca com o usuário se ele deseja realmente executar o boletim
	QMessageBox::StandardButton answer;
	answer = QMessageBox::question(_parent, tr("Executar notificações..."), 
		tr("Deseja realmente executar as notificações ?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);

	if(answer == QMessageBox::No)
		return;

	_manager->runAllNotifications(false);
}
