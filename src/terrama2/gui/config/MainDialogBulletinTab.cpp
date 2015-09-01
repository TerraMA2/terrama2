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
  \file terrama2/gui/config/MainDialogBulletinTab.cpp

  \brief Definition of methods in class mainDlgBulletinTab

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Raphael Meloni
*/

// STL
#include <assert.h>

// QT
#include <QMessageBox>

// TerraMA2
#include "MainDialogBulletinTab.hpp"
#include "Services.hpp"
#include "Bulletin.hpp"
#include "BulletinList.hpp"
#include "AnalysisList.hpp"
#include "AnalysisListDialog.hpp"
#include "UserAnalysisList.hpp"
#include "UserListDialog.hpp"

//! Construtor.  Prepara interface e estabelece conexões
MainDialogBulletinsTab::MainDialogBulletinsTab(MainDlg* main_dialog, Services* manager)
  : MainDlgTab(main_dialog, manager)
{

  _bulletinList		= NULL;
  _bulletinChanged	= false;
  _newBulletin		= false;
  _ignoreChangeEvents = false;
  _currentBulletinIndex   = -1;
	
  // Conecta sinais tratados pela classe
  // Para monitorar a mudança de regra na lista de regras de análise, estamos
  // usando um sinal do modelo de seleção e não o tradicional currentRowChanged()
  // Isso é feito pois em currentRowChanged() não conseguimos voltar para a seleção
  // anterior caso o usuário deseje cancelar a troca.
  connect(_ui->bulletinListWidget->selectionModel(), 
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));

#ifdef _DEBUG
  connect(_ui->bulletinListWidget, SIGNAL(itemDoubleClicked ( QListWidgetItem * ) ), SLOT( bulletinDoubleClicked(QListWidgetItem *)));
#endif // _DEBUG

  // Conecta sinais para adicionar e remover boletins
  connect(_ui->bulletinAddBulletinBtn,     SIGNAL(clicked()), SLOT(addNewBulletinRequested()));
  connect(_ui->bulletinRemoveBulletinBtn,  SIGNAL(clicked()), SLOT(removeBulletinRequested()));

  //conect sinais para adicionar e remover analises ao boletim
  connect(_ui->addAnalysisBulletinBtn,     SIGNAL(clicked()), SLOT(addAnalysisBulletinRequested()));
  connect(_ui->removeAnalysisBulletinBtn,  SIGNAL(clicked()), SLOT(removeAnalysisBulletinRequested()));

  //conect sinais para adicionar e remover usuários ao boletim
  connect(_ui->addUsersBulletinBtn,     SIGNAL(clicked()), SLOT(addUsersBulletinRequested()));
  connect(_ui->removeUsersBulletinBtn,  SIGNAL(clicked()), SLOT(removeUsersBulletinRequested()));  
  
  // Conecta sinais tratados de maneira generica por MainDlgTab
  connect(_ui->bulletinSaveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  connect(_ui->bulletinCancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

  // Conecta sinais para detectar dados alterados
  connect(_ui->bulletinFullName,    SIGNAL(textEdited(const QString&)), SLOT(setBulletinChanged()));
  connect(_ui->buletinDescription,  SIGNAL(textChanged()), SLOT(setBulletinChanged()));
}

//! Destrutor
MainDialogBulletinsTab::~MainDialogBulletinsTab()
{
}

// Funcao comentada na classe base
void MainDialogBulletinsTab::load()
{

  // Carrega novas informações
  _bulletinList     = _manager->bulletinList();
  assert(_bulletinList);
  
  // Limpa dados
  clearFields(true);
  _currentBulletinIndex  = -1;

  // Preenche lista de regras e mostra dados do primeiro boletim
  if(_bulletinList->count()) 
  {
    enableFields(true);

	_ui->bulletinListWidget->clear();
	
    for(int i=0, count=(int)_bulletinList->count(); i<count; i++)
    {
      Bulletin* bu = _bulletinList->at(i);
      _ui->bulletinListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/boletim.png"), bu->name()));
    }

	if(_ui->bulletinListWidget->count() > 0)
		_ui->bulletinListWidget->setCurrentRow(0);
  }  
  else
    enableFields(false); // Se não há entradas na lista, desabilita campos
}

// Funcao comentada na classe base
bool MainDialogBulletinsTab::dataChanged()
{
  return _bulletinChanged;
}

// Funcao comentada na classe base
bool MainDialogBulletinsTab::validate(QString& err)
{
  return true;
}

// Funcao comentada na classe base
bool MainDialogBulletinsTab::save()
{
  Bulletin bu;
  if(!_newBulletin)
    bu = *(_bulletinList->at(_currentBulletinIndex));
  
  // Carrega dados da interface
  getFields(&bu);
  
  bool ok;
  
  // Salva
  if(!_newBulletin)
	  ok = _bulletinList->updateBulletin(&bu);
  else
	  ok = _bulletinList->addNewBulletin(&bu);
  
  // Se a operação de salvar não deu certo, retorna false e mantém o estado atual
  if(!ok)
    return false;
  
  // Atualiza nome da entrada atual da lista que pode ter sido alterado
  _ui->bulletinListWidget->item(_currentBulletinIndex)->setText(bu.name());  
  
  // Atualiza estado para dados não alterados
  clearBulletinChanged();
  _newBulletin = false;
	
  return true;
}

// Funcao comentada na classe base
void MainDialogBulletinsTab::discardChanges(bool restore_data)
{
  if(_newBulletin)
  {
    // Estamos descartando uma análise recém criada que não foi salva na base
    // 1) Remove entrada da lista (interface).  Deve ser a última linha
    assert(_currentBulletinIndex == _ui->bulletinListWidget->count()-1);
    _ignoreChangeEvents = true;
    delete _ui->bulletinListWidget->takeItem(_currentBulletinIndex);    
    _ui->bulletinListWidget->setCurrentRow(-1);
    _currentBulletinIndex = -1;
    _ignoreChangeEvents = false;

    // 2) Desmarca indicador de nova regra e de dados modificados
    _newBulletin = false;
    clearBulletinChanged();  
    
    // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
    if(restore_data)
    {
      if(_ui->bulletinListWidget->count())
        _ui->bulletinListWidget->setCurrentRow(0);
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
      setFields(_bulletinList->at(_currentBulletinIndex));
    else  
      clearBulletinChanged(); 
  }  
}

/*! \brief Limpa a interface.  

Flag indica se a lista de regras e a lista de objetos monitorados também devem ser limpas
*/
void MainDialogBulletinsTab::clearFields(bool clearlist)
{
  _ignoreChangeEvents = true;

  // Lista de regras
  if(clearlist)
    _ui->bulletinListWidget->clear();

  _ui->bulletinFullName->clear();
  _ui->buletinDescription->clear();

  _listSelAnalysis.clear();
  fillBulletinsAnalysis();

  _listSelUser.clear();
  fillBulletinsUsers();

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearBulletinChanged();
}

//! Habilita ou desabilita campos da interface
void MainDialogBulletinsTab::enableFields(bool mode)
{
  // Campos
  _ui->bulletinFullName->setEnabled(mode);
  _ui->buletinDescription->setEnabled(mode);
  
  _ui->bulletinRemoveBulletinBtn->setEnabled(mode);

  _ui->addAnalysisBulletinBtn->setEnabled(mode);
  _ui->removeAnalysisBulletinBtn->setEnabled(mode);

  _ui->addUsersBulletinBtn->setEnabled(mode);
  _ui->removeUsersBulletinBtn->setEnabled(mode);

  _ui->bulletinSaveBtn->setEnabled(mode);
  _ui->bulletinCancelBtn->setEnabled(mode);
}

//! Preenche a interface com os dados de um boletim
void MainDialogBulletinsTab::setFields(Bulletin* bu)
{
  _ignoreChangeEvents = true; 

  // Preenche dados comuns
  _ui->bulletinFullName->setText(bu->name());
  _ui->buletinDescription->setText(bu->description());

  //preenche as analises
  _listSelAnalysis.clear();
  _listSelAnalysis = bu->listAnalysisRulebyBulletin();
  fillBulletinsAnalysis();

  //preenche os usuários
  _listSelUser.clear();
  _listSelUser = bu->listUserbyBulletin();
  fillBulletinsUsers();

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearBulletinChanged();
}


//! Preenche o boletim com os dados da interface, que já devem ter sido validados
void MainDialogBulletinsTab::getFields(Bulletin* bu)
{	
  // Preenche dados comuns
  bu->setName(_ui->bulletinFullName->text().trimmed());
  bu->setDescription(_ui->buletinDescription->toPlainText().trimmed());

  bu->setAnalysisRuleByBulletin(_listSelAnalysis);
  bu->setUserByBulletin(_listSelUser);

}

/*! \brief Indica que algum dos dados apresentados foi alterado.  

Habilita botões de salvar e cancelar
*/
void MainDialogBulletinsTab::setBulletinChanged()
{
  if(_ignoreChangeEvents)
    return;

  _bulletinChanged = true;
  _ui->bulletinSaveBtn->setEnabled(true);
  _ui->bulletinCancelBtn->setEnabled(true);

  if(_newBulletin)
    _parent->statusBar()->showMessage(tr("Novo Boletim."));
  else
    _parent->statusBar()->showMessage(tr("Boletim alterado."));

  _ui->bulletinAddBulletinBtn->setEnabled(false);

  _ui->bulletinFullName->setEnabled(_newBulletin);
  
  emit editBulletinStarted();
}

/*! \brief Indica que os dados mostrados estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar
*/
void MainDialogBulletinsTab::clearBulletinChanged()
{
  _bulletinChanged = false;
  _ui->bulletinSaveBtn->setEnabled(false);
  _ui->bulletinCancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();

  _ui->bulletinFullName->setEnabled(false);

  _ui->bulletinAddBulletinBtn->setEnabled(true);
  emit editBulletinFinished();
}

// Função comentada na classe base
QString MainDialogBulletinsTab::verifyAndEnableChangeMsg()
{
  return tr("As alterações efetuadas na tela de boletins\n"
            "ainda não foram salvas.  Deseja salvar as alterações?");
}

//! Slot chamado quando a linha corrente é alterada na lista de boletins
void MainDialogBulletinsTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
  if(_ignoreChangeEvents)
    return;

  QModelIndexList selected_indexes = selected.indexes();
  
  // Se usuário clicou na lista fora de qq. item, remarca item anterior
  if(!selected_indexes.count())
  {
    if(oldSelection.indexes().count()) // Evita loop infinito se não existir seleção anterior...
      _ui->bulletinListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    return;
  }
  
  // Obtem a linha selecionada    
  int row = selected_indexes[0].row();

  // Verifica se estamos apenas voltando à mesma opção atual.  Ocorre 
  // quando uma troca de regra foi cancelada
  if(row == _currentBulletinIndex)
    return;

  bool ok = true;
  
  // Verifica se os dados atuais na tela foram modificados
  // e em caso positivo se podemos efetuar a troca de dados
  if(_currentBulletinIndex != -1)
    ok = verifyAndEnableChange(false);

  if(ok)
  {
    // Operação permitida.  Troca dados na tela
    if(_newBulletin)
    {
      // Estamos tratando a seleção de uma análise recém incluida na lista
      assert(row == _bulletinList->count());
      clearFields(false);
    }
    else 
    {
      // Estamos tratando uma seleção normal feita pelo usuário
      setFields(_bulletinList->at(row));
    }  
    _currentBulletinIndex = row;
  }
  else
  {
    // Operação foi cancelada.  Devemos reverter à regra original
    _ui->bulletinListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    _ui->bulletinListWidget->setCurrentRow(_currentBulletinIndex);
  }
}

//! Slot chamado quando deseja criar um novo boletim
void MainDialogBulletinsTab::addNewBulletinRequested()
{
	
  // Verifica se podemos ir para novo boletim
  if(!verifyAndEnableChange(false))
    return;
    
  // Ok. Podemos criar nova entrada.
  // 1) Gera um nome temporário para o boletim
  QString name= tr("Novo boletim");
  
  // 2) Inclui nome na lista de boletins e seleciona este boletim
  //    Antes de selecionar, marca que estamos tratando de um novo boletim
  _newBulletin = true;
  _ui->bulletinListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/boletim.png"), name));

  _ui->bulletinListWidget->setCurrentRow(_ui->bulletinListWidget->count()-1);
  
  // 3) Tratador de seleção já limpou a tela.  Habilita os widgets corretos
  enableFields(true);

  // 4) Preenche campo com o nome do boletim e marca o boletim como modificado
  clearFields(false);
  setBulletinChanged();

  _listSelAnalysis.clear();
  fillBulletinsAnalysis();

  _listSelUser.clear();
  fillBulletinsUsers();

  _ui->bulletinFullName->setText(name);
  _ui->bulletinFullName->setFocus();
  _ui->bulletinFullName->selectAll();
}

//! Slot chamado quando deseja remover o boletim atual
void MainDialogBulletinsTab::removeBulletinRequested()
{
  if(_currentBulletinIndex == -1)
    return;
    
  // Verifca com o usuário se ele deseja realmente remover o plano
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(_parent, tr("Remover boletim..."), 
                                 tr("Deseja realmente remover este boletim ?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No);
  if(answer == QMessageBox::No)
    return;

  // Remove objeto do servidor.  Se não deu certo, retorna e mantém o estado atual
  if(!_newBulletin)
  {
	  if(!_bulletinList->deleteBulletinIndex(_currentBulletinIndex))
	  return;
  }
  
  // Remove objeto da lista (interface) e desmarca seleção
  _ignoreChangeEvents = true;
  delete _ui->bulletinListWidget->takeItem(_currentBulletinIndex);    
  _ui->bulletinListWidget->setCurrentRow(-1);
  _currentBulletinIndex = -1;
  _ignoreChangeEvents = false;

  // Desmarca indicadores de modificação e nova regra
  clearBulletinChanged();  
  _newBulletin = false;

  // Seleciona a primeira linha ou limpa campos se não houver nenhuma entrada na lista.
  if(_ui->bulletinListWidget->count())
    _ui->bulletinListWidget->setCurrentRow(0);
  else
  {
    clearFields(false);  
    enableFields(false);
  }
}

//! Slot chamado quando deseja adicionar uma analise ao boletim
void MainDialogBulletinsTab::addAnalysisBulletinRequested()
{
	_manager->analysisList()->clear();
	if(!_manager->loadAnalysisList())
		return;

	AnalysisList* anaList = _manager->analysisList();
	AnalysisListDlg dlg (true);

	if(dlg.setFields(_listSelAnalysis, anaList))
	{
		if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
		{
			QList<int> newIDSelectedAnalyses;
			bool changed;

			dlg.getFields(newIDSelectedAnalyses, changed);

			if (changed)
			{
				bool hasAnotherRef = false;
				int idRef = -1;

				wsAnalysisType analysisType = WS_ANALYSISTYPE_RISK;

				if(_listSelAnalysis.size())
				{
					wsAnalysisRule rule = _listSelAnalysis.at(0);
					analysisType = rule.type;

					if (analysisType == WS_ANALYSISTYPE_RISK)
						idRef = rule.riskMap.id;
					else
						idRef = rule.pcd.id;
				}

				int pos;
				for (int i=0; i<newIDSelectedAnalyses.size(); i++)
				{
					pos = findAnaRule(anaList, newIDSelectedAnalyses.at(i), idRef, analysisType);

					if(pos == -2)
					{
						hasAnotherRef = true;
					}
					else if (pos != -1)
					{
						wsAnalysisRule rule = anaList->at(pos)->data();
						_listSelAnalysis.push_back(rule);
					}
				}

				fillBulletinsAnalysis();
				setBulletinChanged();

				if (hasAnotherRef)
				{
                    QMessageBox::information(_parent, tr("Adicionar análises"), tr("As análises selecionadas precisam conter o mesmo objeto monitorado ou mesma fonte de dados PCD."));
				}
			}
		}
	}
	else
	{
		QMessageBox::information(_parent, tr("Adicionar análises"), tr("Todas as análises já foram adicionadas a esse usuário."));
	}
}

//! Slot chamado quando deseja remover uma analise ao boletim
void MainDialogBulletinsTab::removeAnalysisBulletinRequested()
{
	QVector<struct wsAnalysisRule> listAux;
	listAux = QVector<struct wsAnalysisRule>::fromStdVector(_listSelAnalysis);
	bool removed = false;

	int cRow = _ui->analysisBulletinListWidget->currentRow();
	if(cRow != -1)
	{ 
		listAux.remove(cRow);
		removed = true;
	}

	if (removed)
	{
		setBulletinChanged();
		_listSelAnalysis = listAux.toStdVector();
		fillBulletinsAnalysis();
	}
}

//! Slot chamado quando deseja adicionar um usuário ao boletim
void MainDialogBulletinsTab::addUsersBulletinRequested()
{
	_manager->analysisList()->clear();
	if(!_manager->loadAnalysisList())
		return;

	UserAnalysisList* userList = _manager->userAnalysisList();
	UserListDlg dlg;

	if(dlg.setFields(_listSelUser, userList))
	{
		if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
		{
			QList<std::string> newIDSelectedAnalyses;
			bool changed;

			dlg.getFields(newIDSelectedAnalyses, changed);

			if (changed)
			{
				int pos;
				for (int i=0; i<newIDSelectedAnalyses.size(); i++)
				{
					pos = findUser(userList, newIDSelectedAnalyses.at(i));

					if (pos != -1)
					{
						wsUser rule = userList->at(pos)->data();
						_listSelUser.push_back(rule);
					}
				}

				fillBulletinsUsers();
				setBulletinChanged();
			}
		}
	}
	else
	{
		QMessageBox::information(_parent, tr("Adicionar análises"), tr("Todas as análises já foram adicionadas a esse usuário."));
	}
}

//! Slot chamado quando deseja remover um usuário ao boletim
void MainDialogBulletinsTab::removeUsersBulletinRequested()
{
	QVector<struct wsUser> listAux;
	listAux = QVector<struct wsUser>::fromStdVector(_listSelUser);
	bool removed = false;

	int cRow = _ui->userBulletinListWidget->currentRow();
	if(cRow != -1)
	{ 
		listAux.remove(cRow);
		removed = true;
	}

	if (removed)
	{
		setBulletinChanged();
		_listSelUser = listAux.toStdVector();
		fillBulletinsUsers();
	}
}

void MainDialogBulletinsTab::fillBulletinsAnalysis()
{
	_ui->analysisBulletinListWidget->clear();

	//preenche as analises
	for (unsigned int i = 0; i < _listSelAnalysis.size(); i++)
	{
		QString itemName = _listSelAnalysis.at(i).name.c_str();
        QString icon = _listSelAnalysis.at(i).type == WS_ANALYSISTYPE_RISK ? QString(":/data/icons/analise_plano de risco.png") : QString(":/data/icons/dado_pontual.png");
		_ui->analysisBulletinListWidget->addItem(new QListWidgetItem(QIcon(icon), itemName));
	}
}

void MainDialogBulletinsTab::fillBulletinsUsers()
{
	_ui->userBulletinListWidget-> clear();
	_ui->userBulletinListWidget->setCurrentRow(-1);

	//preenche os usuários
	for (unsigned int i = 0; i < _listSelUser.size(); i++)
	{
		QString itemName = _listSelUser.at(i).login.c_str();
		_ui->userBulletinListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/usuario.png"), itemName));
	}
}

int MainDialogBulletinsTab::findAnaRule(AnalysisList* anaList, int id, int& idRef, wsAnalysisType& analysisType)
{
	int ret=-1;

	for (int i=0; i<anaList->size() && ret==-1; i++)
	{
		Analysis* rule = anaList->at(i);
		if (rule->id() == id)
		{
			if(idRef == -1)
			{
				analysisType = rule->getAnalysisType();

				if (analysisType == WS_ANALYSISTYPE_RISK)
					idRef = rule->riskMapId();
				else if (analysisType == WS_ANALYSISTYPE_PCD)
					idRef = rule->pcdId();

				ret = i;
			}
			else if (rule->getAnalysisType() != analysisType)
			{
				ret = -2;
				break;
			}
			else if (rule->getAnalysisType() == WS_ANALYSISTYPE_RISK && idRef != rule->riskMapId() )
			{
				ret = -2;
				break;
			}
			else if (rule->getAnalysisType() == WS_ANALYSISTYPE_PCD && idRef != rule->pcdId() )
			{
				ret = -2;
				break;
			}
			else
				ret = i;
		}
	}

	return ret;
}

int MainDialogBulletinsTab::findUser(UserAnalysisList* userList, std::string login)
{
	int ret=-1;

	for (int i=0; i<userList->size() && ret==-1; i++)
	{
		if (userList->at(i)->login().toStdString() == login)
			ret = i;
	}

	return ret;
}

void MainDialogBulletinsTab::bulletinDoubleClicked( QListWidgetItem * )
{
	// Verifca com o usuário se ele deseja realmente executar o boletim
	QMessageBox::StandardButton answer;
	answer = QMessageBox::question(_parent, tr("Executar boletim..."), 
		tr("Deseja realmente executar os boletins ?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);
	
	if(answer == QMessageBox::No)
		return;

	_manager->runAllBulletin(true);
}
