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
  \file terrama2/gui/config/MainDialogCellularSpaceTab.cpp

  \brief Definition of methods in class MainDialogCellularSpaceTab

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Gustavo Sampaio
*/

// STL
#include <assert.h>

// QT  
#include <QMessageBox>

// TerraMA2
#include "MainDialogCellularSpaceTab.hpp"
#include "ProjectionDialog.hpp"
#include "Services.hpp"
#include "CellularSpaceList.hpp"

//! Construtor.  Prepara interface e estabelece conex�es
MainDialogCellularSpaceTab::MainDialogCellularSpaceTab(MainDialog* main_dialog, Services* manager)
: MainDialogTab(main_dialog, manager)
{
	_cellularSpaceList		= NULL;
	_cellularSpaceChanged	= false;
	_newCellularSpace		= false;
	_ignoreChangeEvents		= false;
	_currentCellularSpaceIndex = -1;

	// Para monitorar a mudan�a de regra na lista de espa�os celulares, estamos
	// usando um sinal do modelo de sele��o e n�o o tradicional currentRowChanged()
	// Isso � feito pois em currentRowChanged() n�o conseguimos voltar para a sele��o
	// anterior caso o usu�rio deseje cancelar a troca.
	connect(_ui->cellularSpaceListWidget->selectionModel(), 
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));

	connect(_ui->projectionCellularSpaceBtn, SIGNAL(clicked()), SLOT(projectionDialogRequested()));

	// Conecta sinais tratados de maneira generica por MainDlgTab
	connect(_ui->cellularSpaceSaveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
	connect(_ui->cellularSpaceCancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

	// Conecta sinais para detectar dados alterados
	connect(_ui->cellularSpaceNameLed,				SIGNAL(textEdited(const QString&)), SLOT(setCellularSpaceChanged()));
	connect(_ui->cellularSpaceAuthorLed,			SIGNAL(textEdited(const QString&)), SLOT(setCellularSpaceChanged()));
	connect(_ui->cellularSpaceInstitutionLed,		SIGNAL(textEdited(const QString&)), SLOT(setCellularSpaceChanged()));
	connect(_ui->cellularSpaceDescriptionTed,		SIGNAL(textChanged()), SLOT(setCellularSpaceChanged()));
	connect(_ui->cellularSpaceCreationDateDed,		SIGNAL(dateChanged(const QDate&)), SLOT(setCellularSpaceChanged()));
	connect(_ui->cellularSpaceExpirationDateDed,	SIGNAL(dateChanged(const QDate&)), SLOT(setCellularSpaceChanged()));

	// Conecta sinais do combo de Temas
	connect(_ui->cboBoxCellularSpaceTheme, SIGNAL(currentIndexChanged(int)), SLOT(comboItemChanged(int)));

	// Conecta sinais para adicionar e remover espa�os celulares
	connect(_ui->cellularSpaceInsertBtn, SIGNAL(clicked()), SLOT(insertCellularSpaceRequested()));
	connect(_ui->cellularSpaceDeleteBtn, SIGNAL(clicked()), SLOT(removeCellularSpaceRequested()));
}

//! Destrutor
MainDialogCellularSpaceTab::~MainDialogCellularSpaceTab()
{
}

// Funcao comentada na classe base
void MainDialogCellularSpaceTab::load()
{
	// Carrega novas informa��es
	_cellularSpaceList = _manager->cellularSpaceList();

	assert(_cellularSpaceList);

	fillComboBoxTheme();

	// Limpa dados
	clearFields(true);
	_currentCellularSpaceIndex = -1;

	// Preenche a lista de espa�os celulares.
	if(_cellularSpaceList->count()) 
	{
		enableFields(true);

		for(int i=0, count=(int)_cellularSpaceList->count(); i<count; i++)
		{
			CellularSpace* cs = _cellularSpaceList->at(i);
			_ui->cellularSpaceListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/espaco_celular.png"), cs->name()));
		}
		_ui->cellularSpaceListWidget->setCurrentRow(0);
	}
	else
		enableFields(false);  // Se n�o h� entradas na lista, desabilita campos
}

//! Preenche combo com CellularSpaceTheme
void MainDialogCellularSpaceTab::fillComboBoxTheme()
{
	_cellularSpaceList->loadCellThemeData();
	_cellThemes = _cellularSpaceList->getCellThemes();
	_ui->cboBoxCellularSpaceTheme->clear();
	_ui->cboBoxCellularSpaceTheme->addItem(tr("Nenhum"));
	for (unsigned i=0; i<_cellThemes.size(); i++)
		_ui->cboBoxCellularSpaceTheme->addItem(QString::fromStdString(_cellThemes.at(i).name));
}

// Funcao comentada na classe base
bool MainDialogCellularSpaceTab::dataChanged()
{
	return _cellularSpaceChanged;
}

// Funcao comentada na classe base
bool MainDialogCellularSpaceTab::validate(QString& err)
{
	// Nome do espa�o celular � obrigat�rio
	if(_ui->cellularSpaceNameLed->text().trimmed().isEmpty())
	{
		err = tr("Nome do espa�o celular n�o foi preenchido!");
		return false;
	}

	if ( _ui->cboBoxCellularSpaceTheme->isEnabled() && ( _ui->cboBoxCellularSpaceTheme->currentIndex() == 0 ) )
	{
		err = tr("Nenhum tema foi selecionado!");
		return false;
	}

	return true;
}

// Funcao comentada na classe base
bool MainDialogCellularSpaceTab::save()
{
	CellularSpace cs;
	if(!_newCellularSpace)
		cs = *(_cellularSpaceList->at(_currentCellularSpaceIndex));

	// Carrega dados da interface
	getFields(&cs);

	bool ok;

	// Salva
	if(!_newCellularSpace)
		ok = _cellularSpaceList->updateCellularSpace(cs.id(), &cs);
	else
		ok = _cellularSpaceList->addNewCellularSpace(&cs);  

	// Se a opera��o de salvar n�o deu certo, retorna false e mant�m o estado atual
	if(!ok)
		return false;

	// Recarrega os dados de temas
	_manager->loadCellThemeData();
	_cellularSpaceList = _manager->cellularSpaceList();

	// Atualiza nome da entrada atual da lista que pode ter sido alterado
	_ui->cellularSpaceListWidget->item(_currentCellularSpaceIndex)->setText(cs.name());  

	// Atualiza estado para dados n�o alterados
	clearCellularSpaceChanged();
	_newCellularSpace = false;

	return true;
}

// Funcao comentada na classe base
void MainDialogCellularSpaceTab::discardChanges(bool restore_data)
{
	if(_newCellularSpace)
	{
		// Estamos descartando um espa�o celular rec�m criado que n�o foi salvo na base
		// 1) Remove entrada da lista (interface).  Deve ser a �ltima linha
		assert(_currentCellularSpaceIndex == _ui->cellularSpaceListWidget->count()-1);
		_ignoreChangeEvents = true;
		delete _ui->cellularSpaceListWidget->takeItem(_currentCellularSpaceIndex);    
		_ui->cellularSpaceListWidget->setCurrentRow(-1);
		_currentCellularSpaceIndex = -1;
		_ignoreChangeEvents = false;

		// 2) Desmarca indicador de novo espa�o celular e de dados modificados
		_newCellularSpace = false;
		clearCellularSpaceChanged();  

		// 3) Se precisamos restaurar os dados, marca a primeira linha da lista
		if(restore_data)
		{
			if(_ui->cellularSpaceListWidget->count())
				_ui->cellularSpaceListWidget->setCurrentRow(0);
			else
			{
				clearFields(false);  
				enableFields(false);
			}  
		}
	}
	else
	{
		// Estamos descartando as edi��es feitas em um espa�o celular antigo
		if(restore_data)
			setFields(_cellularSpaceList->at(_currentCellularSpaceIndex));
		else  
			clearCellularSpaceChanged(); 
	}  
}

/*! \brief Limpa a interface.  

Flag indica se a lista de espa�os celulares tamb�m deve ser limpa
*/
void MainDialogCellularSpaceTab::clearFields(bool clearlist)
{
	_ignoreChangeEvents = true;

	// Lista de regras
	if(clearlist)
		_ui->cellularSpaceListWidget->clear();

	// Campos do plano
	_ui->cellularSpaceNameLed->clear();
	_ui->cellularSpaceAuthorLed->clear();
	_ui->cellularSpaceInstitutionLed->clear();
	_ui->cellularSpaceDescriptionTed->clear();
	_ui->cellularSpaceCreationDateDed->setDate(QDate::currentDate());
	_ui->cellularSpaceExpirationDateDed->setDate(QDate::currentDate().addYears(1));

	_ignoreChangeEvents = false;

	// Dados na ficha est�o atualizados...
	clearCellularSpaceChanged();
}

//! Habilita ou desabilita campos da interface
void MainDialogCellularSpaceTab::enableFields(bool mode)
{
	// Campos
	_ui->cellularSpaceNameLed->setEnabled(mode);
	_ui->cellularSpaceAuthorLed->setEnabled(mode);
	_ui->cellularSpaceInstitutionLed->setEnabled(mode);
	_ui->cellularSpaceDescriptionTed->setEnabled(mode);
	_ui->cellularSpaceCreationDateDed->setEnabled(mode);
	_ui->cellularSpaceExpirationDateDed->setEnabled(mode);

	// Bot�es
	_ui->projectionCellularSpaceBtn->setEnabled(mode);
	_ui->cellularSpaceDeleteBtn->setEnabled(mode);
}

//! Preenche a interface com os dados de um espa�o celular
void MainDialogCellularSpaceTab::setFields(const CellularSpace* cs)
{
	_ignoreChangeEvents = true; 

	// Preenche dados comuns
	_ui->cellularSpaceNameLed->setText(cs->name());
	_ui->cellularSpaceAuthorLed->setText(cs->author());
	_ui->cellularSpaceInstitutionLed->setText(cs->institution());
	_ui->cellularSpaceDescriptionTed->setPlainText(cs->description());
	_ui->cellularSpaceCreationDateDed->setDate(cs->creationDate());
	_ui->cellularSpaceExpirationDateDed->setDate(cs->expirationDate());
	
	setThemeFields(cs->theme());

	_ignoreChangeEvents = false;

	// Dados na ficha est�o atualizados...
	clearCellularSpaceChanged();
}

//! Procura o indice do tema pelo Id
int MainDialogCellularSpaceTab::findThemeIndex(int id)
{
	int nRet = -1;
	for (unsigned nPos=0; nPos < _cellThemes.size() && nRet == -1; nPos++)
	{
		struct wsTheme wscstAux;

		wscstAux = _cellThemes.at(nPos);

		if (wscstAux.id == id)
			nRet = nPos;
	}

	return nRet;
}

//! Preenche a interface com o Temas selecionado
void MainDialogCellularSpaceTab::setThemeFields(const struct wsTheme& theme)
{
	int nPos = findThemeIndex(theme.id);
	bool oldStat = _ignoreChangeEvents;

	_ignoreChangeEvents = true;

	if ( nPos == -1 )
	{
		_ui->stwgCellularSpaceTheme->setCurrentIndex(1);
		_ui->ledCellularSpaceThemeName->setText(QString::fromStdString(theme.name));
	}
	else
	{
		_ui->stwgCellularSpaceTheme->setCurrentIndex(0);
		_ui->cboBoxCellularSpaceTheme->setCurrentIndex(nPos+1);
	}

	_ignoreChangeEvents = oldStat;
}

//! Preenche o espa�o celular com os dados da interface, que j� devem ter sido validados
void MainDialogCellularSpaceTab::getFields(CellularSpace* cs)
{
	// Preenche dados comuns
	cs->setName(_ui->cellularSpaceNameLed->text().trimmed());
	cs->setAuthor(_ui->cellularSpaceAuthorLed->text().trimmed());
	cs->setInstitution(_ui->cellularSpaceInstitutionLed->text().trimmed());
	cs->setDescription(_ui->cellularSpaceDescriptionTed->toPlainText().trimmed());
	cs->setCreationDate(_ui->cellularSpaceCreationDateDed->date());
	cs->setExpirationDate(_ui->cellularSpaceExpirationDateDed->date());

	// Preenchemos o dado referente ao tema
	if ( (_newCellularSpace) && (_ui->cboBoxCellularSpaceTheme->currentIndex()>0) )
	{
		cs->setThemeStructure(_cellThemes.at(_ui->cboBoxCellularSpaceTheme->currentIndex()-1) );
	}
}

/*! \brief Indica que algum dos dados apresentados foi alterado.  

Habilita bot�es de salvar e cancelar
*/
void MainDialogCellularSpaceTab::setCellularSpaceChanged()
{
	if(_ignoreChangeEvents)
		return;

	_cellularSpaceChanged = true;
	_ui->cellularSpaceSaveBtn->setEnabled(true);
	_ui->cellularSpaceCancelBtn->setEnabled(true);
	if(_newCellularSpace)
		_parent->statusBar()->showMessage(tr("Novo espa�o celular."));
	else
		_parent->statusBar()->showMessage(tr("Espa�o celular alterado."));

	_ui->cellularSpaceInsertBtn->setEnabled(false);

	emit editCellularSpaceStarted();
}

/*! \brief Indica que os dados mostrados est�o atualizados com o servidor. 

Desabilita os bot�es de salvar e cancelar
*/
void MainDialogCellularSpaceTab::clearCellularSpaceChanged()
{
	_ui->cboBoxCellularSpaceTheme->setEnabled(false);

	_cellularSpaceChanged = false;
	_ui->cellularSpaceSaveBtn->setEnabled(false);
	_ui->cellularSpaceCancelBtn->setEnabled(false);
	_parent->statusBar()->clearMessage();

	_ui->cellularSpaceInsertBtn->setEnabled(true);

	emit editCellularSpaceFinished();
}

//! Slot chamado quando o bot�o de proje��es � pressionado
void MainDialogCellularSpaceTab::projectionDialogRequested()
{
	ProjectionDlg dlg;
	wsProjectionParams tempProjectionParams;

	if(!_newCellularSpace)
		tempProjectionParams = _cellularSpaceList->at(_currentCellularSpaceIndex)->theme().projection;
	else
		if ( (_ui->cboBoxCellularSpaceTheme->currentIndex()>0) )
			tempProjectionParams = _cellThemes.at(_ui->cboBoxCellularSpaceTheme->currentIndex()-1).projection;
		else
			return;

	dlg.setFields(tempProjectionParams);
	dlg.setEnabledFields(false);
	dlg.exec();
}

// Fun��o comentada na classe base
QString MainDialogCellularSpaceTab::verifyAndEnableChangeMsg()
{
	return tr("As altera��es efetuadas na tela de espa�os celulares\n"
			  "ainda n�o foram salvas.  Deseja salvar as altera��es?");
}

//! Slot chamado quando a linha corrente � alterada na lista de espa�os celulares
void MainDialogCellularSpaceTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
	if(_ignoreChangeEvents)
		return;

	QModelIndexList selected_indexes = selected.indexes();

	// Se usu�rio clicou na lista fora de qq. item, remarca item anterior
	if(!selected_indexes.count())
	{
		if(oldSelection.indexes().count()) // Evita loop infinito se n�o existir sele��o anterior...
			_ui->cellularSpaceListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
		return;
	}

	// Obtem a linha selecionada    
	int row = selected_indexes[0].row();

	// Verifica se estamos apenas voltando � mesma op��o atual.  Ocorre 
	// quando uma troca de mapa foi cancelada
	if(row == _currentCellularSpaceIndex)
		return;

	bool ok = true;

	// Verifica se os dados atuais na tela foram modificados
	// e em caso positivo se podemos efetuar a troca de dados
	if(_currentCellularSpaceIndex != -1)
		ok = verifyAndEnableChange(false);

	if(ok)
	{
		// Opera��o permitida.  Troca dados na tela
		if(_newCellularSpace)
		{
			// Estamos tratando a sele��o de uma an�lise rec�m incluida na lista
			assert(row == _cellularSpaceList->count());
			clearFields(false);
		}
		else 
		{
			// Estamos tratando uma sele��o normal feita pelo usu�rio
			setFields(_cellularSpaceList->at(row));
		}  
		_currentCellularSpaceIndex = row;
	}
	else
	{
		// Opera��o foi cancelada.  Devemos reverter � regra original
		_ui->cellularSpaceListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
		_ui->cellularSpaceListWidget->setCurrentRow(_currentCellularSpaceIndex);
	}    
}

//! Slot chamado quando o usu�rio clica no botao para inserir um novo espa�o celular
void MainDialogCellularSpaceTab::insertCellularSpaceRequested()
{
	fillComboBoxTheme();

	QListWidgetItem* item = new QListWidgetItem;

	enableFields(true);
	clearFields(false);
	setCellularSpaceChanged();

	item->setText("New CellularSpace");
	item->setIcon(QIcon(":/data/icons/espaco_celular.png"));
	_newCellularSpace= true;
	_cellularSpaceChanged = true;
	_ui->cellularSpaceNameLed->setText("New CellularSpace");

	_ignoreChangeEvents = true;
	_ui->cellularSpaceListWidget->addItem(item);
	_ui->cellularSpaceListWidget->setCurrentItem(item);
	_ignoreChangeEvents = false;
	_currentCellularSpaceIndex = _ui->cellularSpaceListWidget->currentRow();

	_ui->stwgCellularSpaceTheme->setCurrentIndex(0);

	_ui->cboBoxCellularSpaceTheme->setEnabled(true);
}

//! Slot chamado quando o usu�rio clica no botao para excluir o espa�o celular atual
void MainDialogCellularSpaceTab::removeCellularSpaceRequested()
{
	int currentPos = _ui->cellularSpaceListWidget->currentRow();
	QListWidgetItem* oldItem = _ui->cellularSpaceListWidget->currentItem();

	bool ok = true;

	if (oldItem==NULL)
		return;

	// Remove objeto do servidor.  Se n�o deu certo, retorna e mant�m o estado atual
	if(!_newCellularSpace)
	{
		// Verifca com o usu�rio se ele deseja realmente remover o espa�o celular
		QMessageBox::StandardButton answer;
		answer = QMessageBox::question(_parent, tr("Remover espa�o celular..."), 
												tr("Deseja realmente remover este espa�o celular ?"),
												QMessageBox::Yes | QMessageBox::No,
												QMessageBox::No);
		
		if(answer == QMessageBox::No)
			return;

		ok = _cellularSpaceList->deleteCellularSpace(_cellularSpaceList->at(currentPos)->id());

		// Recarrega os dados de temas
		_manager->loadCellThemeData();
		_cellularSpaceList = _manager->cellularSpaceList();
	}

	if (ok)
	{
		// Remove objeto da lista (interface) e desmarca sele��o
		_ignoreChangeEvents = true;

		_ui->cellularSpaceListWidget->removeItemWidget(oldItem);
		delete oldItem;
		_ignoreChangeEvents = false;

		// Desmarca indicadores de modifica��o e novo espa�o celular
		clearCellularSpaceChanged();
		_newCellularSpace = false;

		if (_ui->cellularSpaceListWidget->count() <= currentPos)
			currentPos = _ui->cellularSpaceListWidget->count()-1;

		if (currentPos >= 0)
		{
			_ui->cellularSpaceListWidget->setCurrentRow(currentPos);
			_currentCellularSpaceIndex = currentPos;
			setFields(_cellularSpaceList->at(_currentCellularSpaceIndex));
		}
		else
		{
			clearFields(true);
			enableFields(false);  // Se n�o h� entradas na lista, desabilita campos
		}
	}  
}

//! Trata selecao do combo de temas
void MainDialogCellularSpaceTab::comboItemChanged(int index)
{
	if (_ignoreChangeEvents)
		return;

	setCellularSpaceChanged();
}