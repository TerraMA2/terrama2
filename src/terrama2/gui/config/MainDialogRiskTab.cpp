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
  \file terrama2/gui/config/MainDialogRiskTab.cpp

  \brief Definition of methods in class MainDialogRiskTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>

// QT  
#include <QMessageBox>

// TerraMA2
#include "MainDialogRiskTab.hpp"
#include "ProjectionDialog.hpp"
#include "Services.hpp"
#include "RiskMapList.hpp"
#include "Utils.hpp"

#include <ComboBoxDelegate.h>

//! Construtor.  Prepara interface e estabelece conex�es
MainDialogRiskTab::MainDialogRiskTab(MainDlg* main_dialog, Services* manager)
  : MainDialogTab(main_dialog, manager)
{
  _riskMapList		  = NULL;
  _mapChanged		  = false;
  _newMap			  = false;
  _ignoreChangeEvents = false;
  _currentMapIndex    = -1;

  // Seta par�metros da tabela de atributos
  _ui->attributesTableWidget->resizeRowsToContents();
  _ui->attributesTableWidget->setAlternatingRowColors(true);
  // Cria o delegate da coluna de visibilidade
  ComboBoxDelegate* cmbDelegate = new ComboBoxDelegate();
  cmbDelegate->insertItem(tr("N�o"));
  cmbDelegate->insertItem(tr("Sim"));
  _ui->attributesTableWidget->setItemDelegateForColumn(2, cmbDelegate);

  // Queremos sempre mostrar o nome dos atributos
  _ui->comboBoxAttrIdentificacao->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  // Para monitorar a mudan�a de regra na lista de objetos monitorados, estamos
  // usando um sinal do modelo de sele��o e n�o o tradicional currentRowChanged()
  // Isso � feito pois em currentRowChanged() n�o conseguimos voltar para a sele��o
  // anterior caso o usu�rio deseje cancelar a troca.
  connect(_ui->riskListWidget->selectionModel(), 
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));
  
  connect(_ui->projectionRiskBtn, SIGNAL(clicked()), SLOT(projectionDialogRequested()));

  // Conecta sinais tratados de maneira generica por MainDlgTab
  connect(_ui->riskSaveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  connect(_ui->riskCancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

  // Conecta sinais para detectar dados alterados
  connect(_ui->riskNameLed,				  SIGNAL(textEdited(const QString&)), SLOT(setMapChanged()));
  connect(_ui->riskAuthorLed,			  SIGNAL(textEdited(const QString&)), SLOT(setMapChanged()));
  connect(_ui->riskInstitutionLed,		  SIGNAL(textEdited(const QString&)), SLOT(setMapChanged()));
  connect(_ui->riskDescriptionTed,		  SIGNAL(textChanged()), SLOT(setMapChanged()));
  connect(_ui->riskCreationDateDed,		  SIGNAL(dateChanged(const QDate&)), SLOT(setMapChanged()));
  connect(_ui->riskExpirationDateDed,	  SIGNAL(dateChanged(const QDate&)), SLOT(setMapChanged()));
  connect(_ui->comboBoxAttrIdentificacao, SIGNAL(currentIndexChanged(int)), SLOT(setMapChanged()));
  connect(_ui->attributesTableWidget,	  SIGNAL(cellChanged(int, int)), SLOT(setMapChanged()));

  // Conecta sinais do combo de Temas
  connect(_ui->cboBoxRiskTheme, SIGNAL(currentIndexChanged(int)), SLOT(comboItemChanged(int)));

  // Conecta sinais para adicionar e remover objetos monitorados
  connect(_ui->riskInsertBtn,			SIGNAL(clicked()), SLOT(insertRiskRequested()));
  connect(_ui->riskDeleteBtn,			SIGNAL(clicked()), SLOT(removeRiskRequested()));
}

//! Destrutor
MainDialogRiskTab::~MainDialogRiskTab()
{
}

// Funcao comentada na classe base
void MainDialogRiskTab::load()
{
  // Carrega novas informa��es
  _riskMapList = _manager->riskMapList();

  assert(_riskMapList);

  fillComboBoxTheme();

  // Limpa dados
  clearFields(true);
  _currentMapIndex    = -1;


  // Preenche a lista de objetos monitorados.
  if(_riskMapList->count()) 
  {
    enableFields(true);

    for(int i=0, count=(int)_riskMapList->count(); i<count; i++)
    {
      RiskMap* map = _riskMapList->at(i);
      _ui->riskListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/plano de risco.png"), map->name()));
    }
    _ui->riskListWidget->setCurrentRow(0);
  }
  else
    enableFields(false);  // Se n�o h� entradas na lista, desabilita campos
}

//! preenche combo com RiskMapTheme
void MainDialogRiskTab::fillComboBoxTheme()
{
  _riskMapList->loadRiskMapThemeData();
  _riskMapTheme = _riskMapList->getRiskMapThemes();
  _ui->cboBoxRiskTheme->clear();
  _ui->cboBoxRiskTheme->addItem(tr("Nenhum"));
  for (unsigned i=0; i<_riskMapTheme.size(); i++)
	  _ui->cboBoxRiskTheme->addItem(QString::fromStdString(_riskMapTheme.at(i).name));
}

// Funcao comentada na classe base
bool MainDialogRiskTab::dataChanged()
{
  return _mapChanged;
}

// Funcao comentada na classe base
bool MainDialogRiskTab::validate(QString& err)
{
  // Nome do mapa � obrigat�rio
  if(_ui->riskNameLed->text().trimmed().isEmpty())
  {
    err = tr("Nome do objeto monitorado n�o foi preenchido!");
    return false;
  }

  if ( _ui->cboBoxRiskTheme->isEnabled() && ( _ui->cboBoxRiskTheme->currentIndex() == 0 ) )
  {
    err = tr("Nenhum tema foi selecionado!");
    return false;
  }

  for (int i = 0; i < _ui->attributesTableWidget->rowCount(); i++)
  {
	  if(_ui->attributesTableWidget->item(i, 4)->data(Qt::DisplayRole).toInt() < 0)
	  {
		  err = tr("O atributo %1 possui ordem negativa!").arg(_ui->attributesTableWidget->item(i, 0)->data(Qt::DisplayRole).toString());
		  return false;
	  }
  }

  return true;
}

// Funcao comentada na classe base
bool MainDialogRiskTab::save()
{
  RiskMap map;
  if(!_newMap)
    map = *(_riskMapList->at(_currentMapIndex));
  
  // Carrega dados da interface
  getFields(&map);
  
  bool ok;
  
  // Salva
  if(!_newMap)
    ok = _riskMapList->updateRiskMap(map.id(), &map);
  else
    ok = _riskMapList->addNewRiskMap(&map);  
  
  // Se a opera��o de salvar n�o deu certo, retorna false e mant�m o estado atual
  if(!ok)
    return false;
  
  // Recarrega os dados de temas
  _manager->loadRiskMapThemeData();
  _riskMapList = _manager->riskMapList();

  // Atualiza nome da entrada atual da lista que pode ter sido alterado
  _ui->riskListWidget->item(_currentMapIndex)->setText(map.name());  
  
  // Atualiza estado para dados n�o alterados
  clearMapChanged();
  _newMap = false;

  return true;
}

// Funcao comentada na classe base
void MainDialogRiskTab::discardChanges(bool restore_data)
{
  if(_newMap)
  {
    // Estamos descartando um mapa rec�m criado que n�o foi salvo na base
    // 1) Remove entrada da lista (interface).  Deve ser a �ltima linha
    assert(_currentMapIndex == _ui->riskListWidget->count()-1);
    _ignoreChangeEvents = true;
    delete _ui->riskListWidget->takeItem(_currentMapIndex);    
    _ui->riskListWidget->setCurrentRow(-1);
    _currentMapIndex = -1;
    _ignoreChangeEvents = false;

    // 2) Desmarca indicador de novo mapa e de dados modificados
    _newMap = false;
    clearMapChanged();  
    
    // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
    if(restore_data)
    {
      if(_ui->riskListWidget->count())
        _ui->riskListWidget->setCurrentRow(0);
      else
      {
        clearFields(false);  
        enableFields(false);
      }  
    }
  }
  else
  {
    // Estamos descartando as edi��es feitas em uma an�lise antiga
    if(restore_data)
      setFields(_riskMapList->at(_currentMapIndex));
    else  
      clearMapChanged(); 
  }  
}

/*! \brief Limpa a interface.  

Flag indica se a lista de mapas tamb�m deve ser limpa
*/
void MainDialogRiskTab::clearFields(bool clearlist)
{
  _ignoreChangeEvents = true;

  // Lista de regras
  if(clearlist)
    _ui->riskListWidget->clear();

  // Campos do plano
  _ui->riskNameLed->clear();
  _ui->riskAuthorLed->clear();
  _ui->riskInstitutionLed->clear();
  _ui->riskDescriptionTed->clear();
  _ui->riskCreationDateDed->setDate(QDate::currentDate());
  _ui->riskExpirationDateDed->setDate(QDate::currentDate().addYears(1));

  // Tabela de atributos
  _ui->attributesTableWidget->clearContents();
  _ui->attributesTableWidget->setRowCount(0);

  
  _ignoreChangeEvents = false;

  // Dados na ficha est�o atualizados...
  clearMapChanged();
}

//! Habilita ou desabilita campos da interface
void MainDialogRiskTab::enableFields(bool mode)
{
  // Campos
  _ui->riskNameLed->setEnabled(mode);
  _ui->riskAuthorLed->setEnabled(mode);
  _ui->riskInstitutionLed->setEnabled(mode);
  _ui->riskDescriptionTed->setEnabled(mode);
  _ui->riskCreationDateDed->setEnabled(mode);
  _ui->riskExpirationDateDed->setEnabled(mode);
  _ui->attributesTableWidget->setEnabled(mode);
  _ui->comboBoxAttrIdentificacao->setEnabled(mode);
  
  // Bot�es
  _ui->projectionRiskBtn->setEnabled(mode);
  _ui->riskDeleteBtn->setEnabled(mode);
}

//! Preenche a interface com os dados de um mapa
void MainDialogRiskTab::setFields(const RiskMap* map)
{
  _ignoreChangeEvents = true; 

  // Preenche dados comuns
  _ui->riskNameLed->setText(map->name());
  _ui->riskAuthorLed->setText(map->author());
  _ui->riskInstitutionLed->setText(map->institution());
  _ui->riskDescriptionTed->setPlainText(map->description());
  _ui->riskCreationDateDed->setDate(map->creationDate());
  _ui->riskExpirationDateDed->setDate(map->expirationDate());
  
  // Dizemos quem s�o as propriedades de atributo do objeto monitorado
  // e carregamos os campos do tema na tabela
  _riskMapProperties = map->attrProperties();
  setThemeFields(map->riskMapTheme());

  // A essa altura, os dados da combo do atributo de identifica��o
  // j� foram carregados. Resta-nos colocar o �ndice atual da combo
  // de acordo com o escolhido.
  _ui->comboBoxAttrIdentificacao->setCurrentIndex(0);
  for (int i=0; i<_ui->comboBoxAttrIdentificacao->count(); i++)
  {
	  if(_ui->comboBoxAttrIdentificacao->itemText(i) == map->nameAttr())
	  {
		_ui->comboBoxAttrIdentificacao->setCurrentIndex(i);
	  }
  }

  _ignoreChangeEvents = false;

  // Dados na ficha est�o atualizados...
  clearMapChanged();
}

//! Procura o indice do tema pelo Id
int MainDialogRiskTab::findThemeIndex(int id)
{
	int nRet = -1;
	for (unsigned nPos=0; nPos < _riskMapTheme.size() && nRet == -1; nPos++)
	{
		struct wsRiskMapTheme wsrmtAux;

		wsrmtAux = _riskMapTheme.at(nPos);

		if (wsrmtAux.id == id)
			nRet = nPos;
	}

	return nRet;
}

//! Preenche a interface com o Temas selecionado
void MainDialogRiskTab::setThemeFields(const struct wsRiskMapTheme& theme)
{
  int nPos = findThemeIndex(theme.id);
  bool oldStat = _ignoreChangeEvents;

  _ignoreChangeEvents = true;

  if ( nPos == -1 )
  {
    _ui->stwgRskPlanTheme->setCurrentIndex(1);
	_ui->ledRiskPlanThemeName->setText(QString::fromStdString(theme.name));
  }
  else
  {
    _ui->stwgRskPlanTheme->setCurrentIndex(0);
    _ui->cboBoxRiskTheme->setCurrentIndex(nPos+1);

    //Altera o campo Nome do plano para o mesmo do tema
    _ui->riskNameLed->setText(QString::fromStdString(theme.name));
  }

  // Preenche tabela de atributos e a combo do atributo de identifica��o
  int numattr = theme.attributes.size();
  _ui->attributesTableWidget->setRowCount(numattr);

  // Primeiro adicionamos o valor "<Nenhum>" para a combo de
  // atributos de identifica��o
  _ui->comboBoxAttrIdentificacao->clear();
  _ui->comboBoxAttrIdentificacao->addItem("<Nenhum>");

  for(int i = 0; i<numattr; i++)
  {
    QString type = Utils::columnTypeToString(theme.attributes.at(i).type);
	_ui->attributesTableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(theme.attributes.at(i).name)));
	_ui->attributesTableWidget->setItem(i, 1, new QTableWidgetItem(type));
	_ui->attributesTableWidget->item(i, 0)->setFlags(Qt::ItemIsEnabled);	// Evitam que essas duas colunas sejam
	_ui->attributesTableWidget->item(i, 1)->setFlags(Qt::ItemIsEnabled);	// edit�veis.
	_ui->attributesTableWidget->setItem(i, 2, new QTableWidgetItem(_riskMapProperties.at(i).visible? tr("Sim") : tr("N�o")));
	_ui->attributesTableWidget->item(i,2)->setData(Qt::UserRole, _riskMapProperties.at(i).visible);
	_ui->attributesTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(_riskMapProperties.at(i).alias)));
	_ui->attributesTableWidget->setItem(i, 4, new QTableWidgetItem());
	_ui->attributesTableWidget->item(i, 4)->setData(Qt::DisplayRole, (unsigned int)_riskMapProperties.at(i).order);

	// Queremos deixar os atributos selecion�veis tamb�m na combobox de
	// identificadores de alerta:
	_ui->comboBoxAttrIdentificacao->addItem(QString::fromStdString(theme.attributes.at(i).name));
  }
  _ui->attributesTableWidget->resizeRowsToContents();

  _ignoreChangeEvents = oldStat;
}

//! Preenche o mapa com os dados da interface, que j� devem ter sido validados
void MainDialogRiskTab::getFields(RiskMap* map)
{
  // Preenche dados comuns
  map->setName(_ui->riskNameLed->text().trimmed());
  map->setAuthor(_ui->riskAuthorLed->text().trimmed());
  map->setInstitution(_ui->riskInstitutionLed->text().trimmed());
  map->setDescription(_ui->riskDescriptionTed->toPlainText().trimmed());
  map->setCreationDate(_ui->riskCreationDateDed->date());
  map->setExpirationDate(_ui->riskExpirationDateDed->date());

  // Precisamos passar os dados da tabela para a _riskMapProperties
  _riskMapProperties.clear();
  for (int i = 0; i < _ui->attributesTableWidget->rowCount(); i++)
  {
	  wsRiskMapProperty property;
	  property.attr = _ui->attributesTableWidget->item(i, 0)->text().toStdString();
	  property.visible = _ui->attributesTableWidget->item(i, 2)->text() == tr("Sim")? true: false;
	  property.alias = _ui->attributesTableWidget->item(i, 3)->text().toStdString();
	  property.order = _ui->attributesTableWidget->item(i, 4)->data(Qt::DisplayRole).toInt();
	  _riskMapProperties.push_back(property);
  }
  map->setAttrProperties(_riskMapProperties);

  // Preenchemos o dado referente ao tema
  if ( (_newMap) && (_ui->cboBoxRiskTheme->currentIndex()>0) )
  {
	map->setRiskMapTheme(_riskMapTheme.at(_ui->cboBoxRiskTheme->currentIndex()-1) );
  }

  // Preenchemos o dado do atributo de identifica��o
  if(_ui->comboBoxAttrIdentificacao->currentText() == "<Nenhum>")
	  map->setNameAttr("");
  else
	  map->setNameAttr(_ui->comboBoxAttrIdentificacao->currentText());

}

/*! \brief Indica que algum dos dados apresentados foi alterado.  

Habilita bot�es de salvar e cancelar
*/
void MainDialogRiskTab::setMapChanged()
{
  if(_ignoreChangeEvents)
    return;

  _mapChanged = true;
  _ui->riskSaveBtn->setEnabled(true);
  _ui->riskCancelBtn->setEnabled(true);
  if(_newMap)
    _parent->statusBar()->showMessage(tr("Novo objeto monitorado."));
  else
    _parent->statusBar()->showMessage(tr("objeto monitorado alterado."));

  _ui->riskInsertBtn->setEnabled(false);
  
  emit editRiskMapStarted();
}

/*! \brief Indica que os dados mostrados est�o atualizados com o servidor. 

Desabilita os bot�es de salvar e cancelar
*/
void MainDialogRiskTab::clearMapChanged()
{
  _ui->cboBoxRiskTheme->setEnabled(false);

  _mapChanged = false;
  _ui->riskSaveBtn->setEnabled(false);
  _ui->riskCancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();

  _ui->riskInsertBtn->setEnabled(true);
  
  emit editRiskMapFinished();
}

//! Slot chamado quando o bot�o de alterar proje��es � pressionado
void MainDialogRiskTab::projectionDialogRequested()
{
  ProjectionDlg dlg;
  wsProjectionParams tempProjectionParams;

  if(!_newMap)
    tempProjectionParams = _riskMapList->at(_currentMapIndex)->riskMapTheme().projection;
  else
  if ( (_ui->cboBoxRiskTheme->currentIndex()>0) )
    tempProjectionParams = _riskMapTheme.at(_ui->cboBoxRiskTheme->currentIndex()-1).projection;
  else
    return;
  
  dlg.setFields(tempProjectionParams);
  dlg.setEnabledFields(false);
  dlg.exec();

/*  if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
  {
    bool changed;
    dlg.getFields(tempProjectionParams, changed);
	if (changed)
	  setMapChanged();
  }*/
}

// Fun��o comentada na classe base
QString MainDialogRiskTab::verifyAndEnableChangeMsg()
{
  return tr("As altera��es efetuadas na tela de objetos monitorados\n"
            "ainda n�o foram salvas.  Deseja salvar as altera��es?");
}

//! Slot chamado quando a linha corrente � alterada na lista de mapas
void MainDialogRiskTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
  if(_ignoreChangeEvents)
    return;

  QModelIndexList selected_indexes = selected.indexes();
  
  // Se usu�rio clicou na lista fora de qq. item, remarca item anterior
  if(!selected_indexes.count())
  {
    if(oldSelection.indexes().count()) // Evita loop infinito se n�o existir sele��o anterior...
      _ui->riskListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    return;
  }
  
  // Obtem a linha selecionada    
  int row = selected_indexes[0].row();

  // Verifica se estamos apenas voltando � mesma op��o atual.  Ocorre 
  // quando uma troca de mapa foi cancelada
  if(row == _currentMapIndex)
    return;

  bool ok = true;
  
  // Verifica se os dados atuais na tela foram modificados
  // e em caso positivo se podemos efetuar a troca de dados
  if(_currentMapIndex != -1)
    ok = verifyAndEnableChange(false);

  if(ok)
  {
    // Opera��o permitida.  Troca dados na tela
    if(_newMap)
    {
      // Estamos tratando a sele��o de uma an�lise rec�m incluida na lista
      assert(row == _riskMapList->count());
      clearFields(false);
    }
    else 
    {
      // Estamos tratando uma sele��o normal feita pelo usu�rio
      setFields(_riskMapList->at(row));
    }  
    _currentMapIndex = row;
  }
  else
  {
    // Opera��o foi cancelada.  Devemos reverter � regra original
    _ui->riskListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    _ui->riskListWidget->setCurrentRow(_currentMapIndex);
  }    
}

//! Slot chamado quando o usu�rio clica no botao para inserir um novo objeto monitorado
void MainDialogRiskTab::insertRiskRequested()
{
  fillComboBoxTheme();

  QListWidgetItem* item = new QListWidgetItem;

  enableFields(true);
  clearFields(false);
  setMapChanged();

  item->setText("New Risk Map");
  item->setIcon(QIcon(":/data/icons/plano de risco.png"));
  _newMap= true;
  _mapChanged = true;
  _ui->riskNameLed->setText("New Risk Map");

  _ignoreChangeEvents = true;
  _ui->riskListWidget->addItem(item);
  _ui->riskListWidget->setCurrentItem(item);
  _ignoreChangeEvents = false;
  _currentMapIndex = _ui->riskListWidget->currentRow();

  _ui->comboBoxAttrIdentificacao->clear();
  _ui->comboBoxAttrIdentificacao->addItem("<Nenhum>");

  _ui->stwgRskPlanTheme->setCurrentIndex(0);

  _ui->cboBoxRiskTheme->setEnabled(true);
}

//! Slot chamado quando o usu�rio clica no botao para excluir o objeto monitorado atual
void MainDialogRiskTab::removeRiskRequested()
{
	int currentPos = _ui->riskListWidget->currentRow();
	QListWidgetItem* oldItem = _ui->riskListWidget->currentItem();

	bool ok = true;

	if (oldItem==NULL)
		return;

  // Remove objeto do servidor.  Se n�o deu certo, retorna e mant�m o estado atual
  if(!_newMap)
  {
	// Verifca com o usu�rio se ele deseja realmente remover o servidor remoto
	QMessageBox::StandardButton answer;
    answer = QMessageBox::question(_parent, tr("Remover objeto monitorado..."),
                                    tr("Deseja realmente remover este objeto monitorado ?"),
									QMessageBox::Yes | QMessageBox::No,
									QMessageBox::No);
	if(answer == QMessageBox::No)
		return;

	ok = _riskMapList->deleteRiskMap(_riskMapList->at(currentPos)->id());

	// Recarrega os dados de temas
	_manager->loadRiskMapThemeData();
	_riskMapList = _manager->riskMapList();
  }

  if (ok)
  {
	// Remove objeto da lista (interface) e desmarca sele��o
	_ignoreChangeEvents = true;

	_ui->riskListWidget->removeItemWidget(oldItem);
	delete oldItem;
	_ignoreChangeEvents = false;

    // Desmarca indicadores de modifica��o e novo objeto monitorado
	clearMapChanged();
	_newMap = false;

	if (_ui->riskListWidget->count() <= currentPos)
		currentPos = _ui->riskListWidget->count()-1;

	if (currentPos >= 0)
	{
		_ui->riskListWidget->setCurrentRow(currentPos);
		_currentMapIndex = currentPos;
		setFields(_riskMapList->at(_currentMapIndex));
	}
	else
	{
		clearFields(true);
		enableFields(false);  // Se n�o h� entradas na lista, desabilita campos
	}
  }  
}

//! Trata selecao do combo de temas
void MainDialogRiskTab::comboItemChanged(int index)
{
	if (_ignoreChangeEvents)
		return;

	if (index>0)
	{
		_riskMapProperties.clear();
		for (unsigned int i = 0; i < _riskMapTheme.at(index-1).attributes.size(); i++)
		{
			wsRiskMapProperty property;
			property.attr = property.alias = _riskMapTheme.at(index-1).attributes.at(i).name;
			property.order = i;
			property.visible = true;

			_riskMapProperties.push_back(property);
		}
		setThemeFields(_riskMapTheme.at(index-1));
	}


    setMapChanged();
}

