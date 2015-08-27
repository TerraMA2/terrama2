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
  \file terrama2/gui/config/MainDialogAditionalTab.cpp

  \brief Definition of Class MainDialogAditionalTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>

// QT  
#include <QMessageBox>

// TerraMA2
#include "MainDialogAditionalTab.hpp"
#include "projectionDlg.h"
#include "Services.hpp"
#include "AdditionalMapList.hpp"
#include "utils.h"


#include <ComboBoxDelegate.h>


//! Construtor.  Prepara interface e estabelece conexões
MainDlgAditionalTab::MainDlgAditionalTab(MainDlg* main_dialog, Services* manager)
  : MainDlgTab(main_dialog, manager)
{
  _additionalMapList         = NULL;
  _additionalMapChanged      = false;
  _newAdditionalMap          = false;
  _ignoreChangeEvents        = false;
  _currentAdditionalMapIndex = -1;
  _currentTypeAdditionalMap = MDAT_TYPE_NONE;

  // Seta parâmetros da tabela de atributos
  _ui->additionalMapAttributesTableTwi->resizeRowsToContents();
  _ui->additionalMapAttributesTableTwi->setAlternatingRowColors(true);

  // Criamos a combo da tabela de atributos
  ComboBoxDelegate* cmbDelegate = new ComboBoxDelegate();
  cmbDelegate->insertItem(tr("Não"));
  cmbDelegate->insertItem(tr("Sim"));
  _ui->additionalMapAttributesTableTwi->setItemDelegateForColumn(2, cmbDelegate);

  // Para monitorar a mudança de regra na lista de objetos monitorados, estamos
  // usando um sinal do modelo de seleção e não o tradicional currentRowChanged()
  // Isso é feito pois em currentRowChanged() não conseguimos voltar para a seleção
  // anterior caso o usuário deseje cancelar a troca.
  connect(_ui->additionalMapListWidget->selectionModel(), 
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(listItemSelectionChanged(const QItemSelection&, const QItemSelection&)));
  
  connect(_ui->projectionAditionalBtn, SIGNAL(clicked()), SLOT(projectionDialogRequested()));

  // Conecta sinais tratados de maneira generica por MainDlgTab
  connect(_ui->additionalMapSaveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  connect(_ui->additionalMapCancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));

  // Conecta sinais para detectar dados alterados
  connect(_ui->additionalMapNameLed,			SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapAuthorLed,			SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapInstitutionLed,		SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapDescriptionTed,		SIGNAL(textChanged()), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapCreationDateDed,	SIGNAL(dateChanged(const QDate&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapExpirationDateDed,	SIGNAL(dateChanged(const QDate&)), SLOT(setAdditionalMapChanged()));

  // Conecta sinais para detectar dados alterados Raster
  connect(_ui->additionalMapLinhasGridLed,			SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapColunasGridLed,			SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapResXGridLed,			SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));
  connect(_ui->additionalMapResYGridLed,			SIGNAL(textEdited(const QString&)), SLOT(setAdditionalMapChanged()));

  // Define os tipos para validação (raster)
  _ui->additionalMapLinhasGridLed->setValidator(new QIntValidator(_ui->additionalMapLinhasGridLed));
  _ui->additionalMapColunasGridLed->setValidator(new QIntValidator(_ui->additionalMapColunasGridLed));
  _ui->additionalMapResXGridLed->setValidator(new QDoubleValidator(_ui->additionalMapResXGridLed));
  _ui->additionalMapResYGridLed->setValidator(new QDoubleValidator(_ui->additionalMapResYGridLed));

  // limita edicao em 6 casas decimais
  ((QDoubleValidator*)_ui->additionalMapResXGridLed->validator())->setDecimals(6);
  ((QDoubleValidator*)_ui->additionalMapResYGridLed->validator())->setDecimals(6);

  // Conecta sinais para adicionar e remover Mapas Adicionais
  connect(_ui->additionalMapNewGridBtn,			SIGNAL(clicked()), SLOT(insertAdditionalMapGridRequested()));
  connect(_ui->additionalMapNewVectBtn,			SIGNAL(clicked()), SLOT(insertAdditionalMapVectRequested()));
  connect(_ui->additionalMapDelMapBtn,			SIGNAL(clicked()), SLOT(removeAdditionalMapRequested()));

  // Conecta sinais do combo de Temas
  connect(_ui->cboBoxAdditionalMapTheme,	SIGNAL(currentIndexChanged(int)), SLOT(comboItemChanged(int)));

  // Conecta sinais da tabela de atributos
  connect(_ui->additionalMapAttributesTableTwi, SIGNAL(cellChanged(int, int)), SLOT(setAdditionalMapChanged()));

  _ui->stwgAddPlanTheme->setCurrentIndex(0);
  

  // Prepara Tab de Planos adicionais
//  connect(_ui->additionalMapListWidget, SIGNAL(currentRowChanged(int)), SLOT(listItemChanged(int)));
//  connect(_ui->projectionAditionalBtn, SIGNAL(clicked()), SLOT(projectionDialogRequested()));
//  _ui->additionalMapListWidget->setCurrentRow(0);
}


//! Destrutor
MainDlgAditionalTab::~MainDlgAditionalTab()
{
}

// Funcao comentada na classe base
void MainDlgAditionalTab::load()
{
  // Carrega novas informações
  loadAdditionalMapData();

  // Limpa dados
  clearFields(true);
  _currentAdditionalMapIndex = -1;

  // Preenche a lista de objetos monitorados.
  if(_additionalMapList->count()) 
  {
    enableFields(true);

    for(int i=0, count=(int)_additionalMapList->count(); i<count; i++)
    {
      AdditionalMap* additionalMap = _additionalMapList->at(i);
	  struct wsTheme theme =  additionalMap->theme();

	  assert(theme.id != -1);

      if ( (theme.geometry == WS_LAYERGEOM_RASTER) || (theme.geometry == WS_LAYERGEOM_RASTERFILE) )
        _ui->additionalMapListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/novo_grid.png"), additionalMap->name()));
	  else
        _ui->additionalMapListWidget->addItem(new QListWidgetItem(QIcon(":/data/icons/info_vect.png"), additionalMap->name()));

    }
    _ui->additionalMapListWidget->setCurrentRow(0);
  }
  else
    enableFields(false);  // Se não há entradas na lista, desabilita campos
}

// Funcao comentada na classe base
bool MainDlgAditionalTab::dataChanged()
{
  return _additionalMapChanged;
}

// Funcao comentada na classe base
bool MainDlgAditionalTab::validate(QString& err)
{
  // Nome do mapa é obrigatório
  if(_ui->additionalMapNameLed->text().trimmed().isEmpty())
  {
    err = tr("Nome do mapa adicional não foi preenchido!");
    return false;
  }

  if(_ui->cboBoxAdditionalMapTheme->isEnabled() && _ui->cboBoxAdditionalMapTheme->currentIndex()==0)
  {
    err = tr("Nenhum tema foi selecionado!");
    return false;
  }

  return true;
}

// Funcao comentada na classe base
bool MainDlgAditionalTab::save()
{
  AdditionalMap additionalMap;
  if(!_newAdditionalMap)
    additionalMap = *(_additionalMapList->at(_currentAdditionalMapIndex));
  
  // Carrega dados da interface
  getFields(&additionalMap);
  
  bool ok;
  bool isGrid = _currentTypeAdditionalMap == MDAT_TYPE_GRID;
  
  // Salva
  if(!_newAdditionalMap)
    ok = _additionalMapList->updateAdditionalMap(additionalMap.id(), &additionalMap);
  else
    ok = _additionalMapList->addNewAdditionalMap(&additionalMap, isGrid);  
  
  // Se a operação de salvar não deu certo, retorna false e mantém o estado atual
  if(!ok)
    return false;
  
  // Atualiza nome da entrada atual da lista que pode ter sido alterado
  _ui->additionalMapListWidget->item(_currentAdditionalMapIndex)->setText(additionalMap.name());  

  // Vamos recarregar a lista de temas do banco
  _manager->loadAdditionalMapThemeData();
  loadAdditionalMapData();

  // Atualiza estado para dados não alterados
  clearAdditionalMapChanged();
  _newAdditionalMap = false;
  
  emit additionalMapDatabaseChanged();

  return true;
}

// Funcao comentada na classe base
void MainDlgAditionalTab::discardChanges(bool restore_data)
{
  if(_newAdditionalMap)
  {
    // Estamos descartando um mapa recém criado que não foi salvo na base
    // 1) Remove entrada da lista (interface).  Deve ser a última linha
    assert(_currentAdditionalMapIndex == _ui->additionalMapListWidget->count()-1);
    _ignoreChangeEvents = true;
    delete _ui->additionalMapListWidget->takeItem(_currentAdditionalMapIndex);    
    _ui->additionalMapListWidget->setCurrentRow(-1);
    _currentAdditionalMapIndex = -1;
    _ignoreChangeEvents = false;

    // 2) Desmarca indicador de novo mapa e de dados modificados
    _newAdditionalMap = false;
    clearAdditionalMapChanged();  
    
    // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
    if(restore_data)
    {
      if(_ui->additionalMapListWidget->count())
        _ui->additionalMapListWidget->setCurrentRow(0);
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
      setFields(_additionalMapList->at(_currentAdditionalMapIndex));
    else  
      clearAdditionalMapChanged(); 
  }  
}

//! Slot chamado quando a linha corrente é alterada na lista de mapas
void MainDlgAditionalTab::listItemSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
{
  if(_ignoreChangeEvents)
    return;

  QModelIndexList selected_indexes = selected.indexes();
  
  // Se usuário clicou na lista fora de qq. item, remarca item anterior
  if(!selected_indexes.count())
  {
    if(oldSelection.indexes().count()) // Evita loop infinito se não existir seleção anterior...
      _ui->additionalMapListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    return;
  }
  
  // Obtem a linha selecionada    
  int row = selected_indexes[0].row();

  // Verifica se estamos apenas voltando à mesma opção atual.  Ocorre 
  // quando uma troca de mapa foi cancelada
  if(row == _currentAdditionalMapIndex)
    return;

  bool ok = true;
  
  // Verifica se os dados atuais na tela foram modificados
  // e em caso positivo se podemos efetuar a troca de dados
  if(_currentAdditionalMapIndex != -1)
    ok = verifyAndEnableChange(false);

  if(ok)
  {
    // Operação permitida.  Troca dados na tela
    if(_newAdditionalMap)
    {
      // Estamos tratando a seleção de uma análise recém incluida na lista
      assert(row == _additionalMapList->count());
      clearFields(false);
    }
    else 
    {
      // Estamos tratando uma seleção normal feita pelo usuário
      setFields(_additionalMapList->at(row));
    }  
    _currentAdditionalMapIndex = row;
  }
  else
  {
    // Operação foi cancelada.  Devemos reverter à regra original
    _ui->additionalMapListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
    _ui->additionalMapListWidget->setCurrentRow(_currentAdditionalMapIndex);
  }    
}

//! Slot chamado quando o botão de alterar projeções é pressionado
void MainDlgAditionalTab::projectionDialogRequested()
{
  ProjectionDlg dlg;
  wsProjectionParams tempProjectionParams;

  if(!_newAdditionalMap)
    tempProjectionParams = _additionalMapList->at(_currentAdditionalMapIndex)->theme().projection;
  else
  if ( (_ui->cboBoxAdditionalMapTheme->currentIndex()>0) ) {
    if (_currentTypeAdditionalMap == MDAT_TYPE_VECTOR)
      tempProjectionParams = _additionalMapThemeVector.at(_ui->cboBoxAdditionalMapTheme->currentIndex()-1).projection;
    else
    if (_currentTypeAdditionalMap == MDAT_TYPE_GRID)
      tempProjectionParams = _additionalMapThemeGrid.at(_ui->cboBoxAdditionalMapTheme->currentIndex()-1).projection;
    else
      return;
  }
  else
    return;

  dlg.setFields(tempProjectionParams);
  dlg.setEnabledFields(false);
  dlg.exec();
}

// Função comentada na classe base
QString MainDlgAditionalTab::verifyAndEnableChangeMsg()
{
  return tr("As alterações efetuadas na tela de mapas adicionais\n"
            "ainda não foram salvas.  Deseja salvar as alterações?");
}

/*! \brief Indica que os dados mostrados estão atualizados com o servidor. 

Desabilita os botões de salvar e cancelar
*/
void MainDlgAditionalTab::clearAdditionalMapChanged()
{
  _additionalMapChanged = false;
  _ui->additionalMapSaveBtn->setEnabled(false);
  _ui->additionalMapCancelBtn->setEnabled(false);
  _parent->statusBar()->clearMessage();

  _ui->additionalMapNewGridBtn->setEnabled(true);
  _ui->additionalMapNewVectBtn->setEnabled(true);

  _ui->cboBoxAdditionalMapTheme->setEnabled(false);
  
  emit editAdditionalMapFinished();
}

/*! \brief Indica que algum dos dados apresentados foi alterado.  

Habilita botões de salvar e cancelar
*/
void MainDlgAditionalTab::setAdditionalMapChanged()
{
  if(_ignoreChangeEvents)
    return;

  _additionalMapChanged = true;
  _ui->additionalMapSaveBtn->setEnabled(true);
  _ui->additionalMapCancelBtn->setEnabled(true);
  if(_newAdditionalMap)
    _parent->statusBar()->showMessage(tr("Novo mapa adicional."));
  else
    _parent->statusBar()->showMessage(tr("Mapa adicional alterado."));

  _ui->additionalMapNewGridBtn->setEnabled(false);
  _ui->additionalMapNewVectBtn->setEnabled(false);

  _ui->cboBoxAdditionalMapTheme->setEnabled(_newAdditionalMap);
  
  emit editAdditionalMapStarted();
}

/*! \brief Limpa a interface.  

Flag indica se a lista de mapas também deve ser limpa
*/
void MainDlgAditionalTab::clearFields(bool clearlist)
{
  _ignoreChangeEvents = true;

  // Lista de regras
  if(clearlist)
    _ui->additionalMapListWidget->clear();

  // Campos do plano
  _ui->additionalMapNameLed->clear();
  _ui->additionalMapInstitutionLed->clear();
  _ui->additionalMapAuthorLed->clear();
  _ui->additionalMapDescriptionTed->clear();
  _ui->additionalMapCreationDateDed->setDate(QDate::currentDate());
  _ui->additionalMapExpirationDateDed->setDate(QDate::currentDate().addYears(1));

  // Tabela de atributos
  _ui->additionalMapAttributesTableTwi->clearContents();
  _ui->additionalMapAttributesTableTwi->setRowCount(0);
  
  _ignoreChangeEvents = false;

  _ui->additionalDataWidgetStack->setEnabled(false);

  // Dados na ficha estão atualizados...
  clearAdditionalMapChanged();
}

//! Preenche a interface com os dados de um mapa
void MainDlgAditionalTab::setFields(const AdditionalMap* additionalMap)
{
  _ignoreChangeEvents = true; 

  if (!_newAdditionalMap)
  {
     enum wsLayerGeometry geometry;
	   geometry = additionalMap->theme().geometry;

  	 if ( (geometry == WS_LAYERGEOM_RASTER) || (geometry == WS_LAYERGEOM_RASTERFILE) )
  	 {
        _currentTypeAdditionalMap = MDAT_TYPE_GRID;
  	 }
	   else
	   {
        _currentTypeAdditionalMap = MDAT_TYPE_VECTOR;
  	 }
  }

  // Preenche dados comuns
  _ui->additionalMapNameLed->setText(additionalMap->name());
  _ui->additionalMapAuthorLed->setText(additionalMap->author());
  _ui->additionalMapInstitutionLed->setText(additionalMap->institution());
  _ui->additionalMapDescriptionTed->setPlainText(additionalMap->description());
  _ui->additionalMapCreationDateDed->setDate(additionalMap->creationDate());
  _ui->additionalMapExpirationDateDed->setDate(additionalMap->expirationDate());

  _addMapProperties = additionalMap->attrProperties();
  setThemeFields(additionalMap->theme());

  _ignoreChangeEvents = false;

  // Dados na ficha estão atualizados...
  clearAdditionalMapChanged();
}

//! Interface para procura o indice do tema pelo Id (Grid Ou Vector)
void MainDlgAditionalTab::fillComboBox(const std::vector<struct wsTheme>& _additionalMapTheme)
{
   _ui->cboBoxAdditionalMapTheme->clear();
   _ui->cboBoxAdditionalMapTheme->addItem("Nenhum");
   for (unsigned i=0; i<_additionalMapTheme.size(); i++)
   _ui->cboBoxAdditionalMapTheme->addItem(QString::fromStdString(_additionalMapTheme.at(i).name));
}


//! Interface para procura o indice do tema pelo Id (Grid Ou Vector)
int MainDlgAditionalTab::findThemeIndex(enum mdatAdditionalType typeTheme, int id)
{
	int ret = -1;

	if (typeTheme == MDAT_TYPE_GRID)
		ret = auxFindThemeIndex(_additionalMapThemeGrid, id);

	else if (typeTheme == MDAT_TYPE_VECTOR)
		ret = auxFindThemeIndex(_additionalMapThemeVector, id);

	return ret;
}

//! Procura o indice do tema pelo Id
int MainDlgAditionalTab::auxFindThemeIndex(const std::vector<struct wsTheme>& mapThemeList, int id)
{
	int nRet = -1;
	for (unsigned nPos=0; nPos < mapThemeList.size() && nRet == -1; nPos++)
	{
		struct wsTheme wstAux;

		wstAux = mapThemeList.at(nPos);

		if (wstAux.id == id)
			nRet = nPos;
	}

	return nRet;
}

//! Trata selecao do combo de temas
void MainDlgAditionalTab::comboItemChanged(int index)
{
	if (_ignoreChangeEvents)
		return;

	if (_currentTypeAdditionalMap == MDAT_TYPE_GRID)
		_ui->additionalDataWidgetStack->setCurrentIndex(1);

	else if (_currentTypeAdditionalMap == MDAT_TYPE_VECTOR)
		_ui->additionalDataWidgetStack->setCurrentIndex(0);

	if (index<=0)
		_ui->additionalDataWidgetStack->setEnabled(false);
	else
	{
		_ui->additionalDataWidgetStack->setEnabled(true);

		if (_currentTypeAdditionalMap == MDAT_TYPE_GRID)
			setThemeFields(_additionalMapThemeGrid.at(index-1));

		else if (_currentTypeAdditionalMap == MDAT_TYPE_VECTOR)
		{
			// Cuida primeiro das propriedades dos atributos dos mapas adicionais...
			_addMapProperties.clear();
			for (unsigned int i = 0; i < _additionalMapThemeVector.at(index-1).attributes.size(); i++)
			{
				wsRiskMapProperty property;
				property.attr = property.alias = _additionalMapThemeVector.at(index-1).attributes.at(i).name;
				property.order = i;
				property.visible = true;

				_addMapProperties.push_back(property);
			}

			//... depois atualiza os campos do tema
			setThemeFields(_additionalMapThemeVector.at(index-1));
		}

		else
			assert(false);
	}

	setAdditionalMapChanged();
}

//! Preenche a interface com o Temas selecionado
void MainDlgAditionalTab::setThemeFields(const struct wsTheme& theme)
{
  if (theme.id == -1)
  {
    _ui->additionalDataWidgetStack->setEnabled(false);
  }
  else
  {
    if((theme.geometry == WS_LAYERGEOM_RASTER) || (theme.geometry == WS_LAYERGEOM_RASTERFILE) )
    {
      _currentTypeAdditionalMap = MDAT_TYPE_GRID;

      _ui->additionalDataWidgetStack->setCurrentIndex(1);

	    QString aux;

	    aux.setNum(theme.nLines);	   _ui->additionalMapLinhasGridLed->setText(aux);
	    aux.setNum(theme.nColumns);  _ui->additionalMapColunasGridLed->setText(aux);
	    aux.setNum(theme.resX, 'f'); _ui->additionalMapResXGridLed->setText(aux);
	    aux.setNum(theme.resY, 'f'); _ui->additionalMapResYGridLed->setText(aux);
    }
    else
    {
      _currentTypeAdditionalMap = MDAT_TYPE_VECTOR;

      _ui->additionalDataWidgetStack->setCurrentIndex(0);

	  //Carregamos a tabela de atributos do mapa adicional:
	  int numattr = theme.attributes.size();
	  _ui->additionalMapAttributesTableTwi->setRowCount(numattr);
	  
	  for(int i = 0; i<numattr; i++)
	    {
		  QString type = Utils::columnTypeToString(theme.attributes.at(i).type);
		  _ui->additionalMapAttributesTableTwi->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(theme.attributes.at(i).name)));
		  _ui->additionalMapAttributesTableTwi->setItem(i, 1, new QTableWidgetItem(type));
		  _ui->additionalMapAttributesTableTwi->item(i, 0)->setFlags(Qt::ItemIsEnabled);	// Evitam que essas duas colunas sejam
		  _ui->additionalMapAttributesTableTwi->item(i, 1)->setFlags(Qt::ItemIsEnabled);	// editáveis.
		  _ui->additionalMapAttributesTableTwi->setItem(i, 2, new QTableWidgetItem(_addMapProperties.at(i).visible? tr("Sim") : tr("Não")));
		  _ui->additionalMapAttributesTableTwi->item(i,2)->setData(Qt::UserRole, _addMapProperties.at(i).visible);
		  _ui->additionalMapAttributesTableTwi->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(_addMapProperties.at(i).alias)));
		  _ui->additionalMapAttributesTableTwi->setItem(i, 4, new QTableWidgetItem());
		  _ui->additionalMapAttributesTableTwi->item(i, 4)->setData(Qt::DisplayRole, (unsigned int)_addMapProperties.at(i).order);
		  _ui->additionalMapAttributesTableTwi->resizeRowToContents(i);
	    }
	  }

    if(_newAdditionalMap)
    {
	    int nPos = findThemeIndex(_currentTypeAdditionalMap, theme.id);
      _ui->stwgAddPlanTheme->setCurrentIndex(0);
      _ui->cboBoxAdditionalMapTheme->setCurrentIndex(nPos+1);

      //Altera o campo nome do plano adicional para o mesmo do tema
      _ui->additionalMapNameLed->setText(QString::fromStdString(theme.name));
	  }
	  else
	  {
      _ui->stwgAddPlanTheme->setCurrentIndex(1);
	    _ui->ledAdditionalPlanThemeName->setText(QString::fromStdString(theme.name));
	  }

    _ui->additionalDataWidgetStack->setEnabled(true);

  }
}

//! Preenche o mapa com os dados da interface, que já devem ter sido validados
void MainDlgAditionalTab::getFields(AdditionalMap* additionalMap)
{
  // Preenche dados comuns
  additionalMap->setName(_ui->additionalMapNameLed->text().trimmed());
  additionalMap->setAuthor(_ui->additionalMapAuthorLed->text().trimmed());
  additionalMap->setInstitution(_ui->additionalMapInstitutionLed->text().trimmed());
  additionalMap->setDescription(_ui->additionalMapDescriptionTed->toPlainText().trimmed());
  additionalMap->setCreationDate(_ui->additionalMapCreationDateDed->date());
  additionalMap->setExpirationDate(_ui->additionalMapExpirationDateDed->date());

  // Aqui setamos as propriedades dos atributos dos mapas adicionais
  std::vector<wsRiskMapProperty> addMapProperties;
  for (int i = 0; i < _ui->additionalMapAttributesTableTwi->rowCount(); i++)
  {
	  wsRiskMapProperty property;
	  property.attr = _ui->additionalMapAttributesTableTwi->item(i, 0)->text().toStdString();
	  property.visible = _ui->additionalMapAttributesTableTwi->item(i, 2)->text() == tr("Sim")? true: false;
	  property.alias = _ui->additionalMapAttributesTableTwi->item(i, 3)->text().toStdString();
	  property.order = _ui->additionalMapAttributesTableTwi->item(i, 4)->data(Qt::DisplayRole).toInt();
	  addMapProperties.push_back(property);
  }
  additionalMap->setAttrProperties(addMapProperties);

  if ( (_newAdditionalMap) && (_ui->cboBoxAdditionalMapTheme->currentIndex()>0) )
    getThemeFields(additionalMap);
}

//! Preenche o tema com os dados da interface
void MainDlgAditionalTab::getThemeFields(AdditionalMap* additionalMap)
{
  assert(_newAdditionalMap);  // Ao editar um plano não podemos mudar o tema....
  
  struct wsTheme selectedTheme;

  if (_currentTypeAdditionalMap == MDAT_TYPE_GRID)
    selectedTheme = _additionalMapThemeGrid.at(_ui->cboBoxAdditionalMapTheme->currentIndex()-1);
  else if (_currentTypeAdditionalMap == MDAT_TYPE_VECTOR)
    selectedTheme = _additionalMapThemeVector.at(_ui->cboBoxAdditionalMapTheme->currentIndex()-1);
  else
	  assert(false);

  if ( (selectedTheme.geometry == WS_LAYERGEOM_RASTER) || (selectedTheme.geometry == WS_LAYERGEOM_RASTERFILE) )
  {
	  selectedTheme.nLines   = _ui->additionalMapLinhasGridLed->text().toInt();
	  selectedTheme.nColumns = _ui->additionalMapColunasGridLed->text().toInt();
	  selectedTheme.resX     = _ui->additionalMapResXGridLed->text().toDouble();
	  selectedTheme.resY     = _ui->additionalMapResYGridLed->text().toDouble();
  }

  additionalMap->setThemeStructure(selectedTheme);
}

//! Habilita ou desabilita campos da interface
void MainDlgAditionalTab::enableFields(bool mode)
{
  // Campos
  _ui->additionalMapNameLed->setEnabled(mode);
  _ui->additionalMapAuthorLed->setEnabled(mode);
  _ui->additionalMapInstitutionLed->setEnabled(mode);
  _ui->additionalMapDescriptionTed->setEnabled(mode);
  _ui->additionalMapCreationDateDed->setEnabled(mode);
  _ui->additionalMapExpirationDateDed->setEnabled(mode);
  _ui->additionalMapAttributesTableTwi->setEnabled(mode);
  // Combo
  _ui->cboBoxAdditionalMapTheme->setEnabled(mode);
  // Stack
  _ui->additionalDataWidgetStack->setEnabled(mode);
  // Botões
  _ui->projectionAditionalBtn->setEnabled(mode);

  _ui->additionalMapDelMapBtn->setEnabled(mode);
}



//! Slot chamado quando o usuário clica no botao para inserir um novo Mapa adicional Tipo Grid
void MainDlgAditionalTab::insertAdditionalMapGridRequested()
{
	// Falta diferenciar o Tipo (Grid ou Vect)
  QListWidgetItem* item = new QListWidgetItem;

  _currentTypeAdditionalMap = MDAT_TYPE_GRID;
  _additionalMapList->loadAdditionalMapThemeData();
  _additionalMapThemeGrid = _additionalMapList->getAdditionalMapThemesGrid();
  fillComboBox(_additionalMapThemeGrid);

  enableFields(true);
  clearFields(false);

  item->setText("New Additional Map");
  item->setIcon(QIcon(":/data/icons/novo_grid.png"));
  _newAdditionalMap= true;
  _additionalMapChanged = true;
  _ui->additionalMapNameLed->setText("New Additional Map");

  setAdditionalMapChanged();

  _ignoreChangeEvents = true;
  _ui->additionalMapListWidget->addItem(item);
  _ui->additionalMapListWidget->setCurrentItem(item);
  _ignoreChangeEvents = false;
  _currentAdditionalMapIndex = _ui->additionalMapListWidget->currentRow();

  _ui->stwgAddPlanTheme->setCurrentIndex(0);
  _ui->cboBoxAdditionalMapTheme->setCurrentIndex(0);
}

//! Slot chamado quando o usuário clica no botao para inserir um novo Mapa adicional Tipo Vect
void MainDlgAditionalTab::insertAdditionalMapVectRequested()
{
  QListWidgetItem* item = new QListWidgetItem;

  _currentTypeAdditionalMap = MDAT_TYPE_VECTOR;
  _additionalMapList->loadAdditionalMapThemeData();
  _additionalMapThemeVector = _additionalMapList->getAdditionalMapThemesVector();
  fillComboBox(_additionalMapThemeVector);

  enableFields(true);
  clearFields(false);

  item->setText("New Additional Map");
  item->setIcon(QIcon(":/data/icons/info_vect.png"));
  _newAdditionalMap= true;
  _additionalMapChanged = true;
  _ui->additionalMapNameLed->setText("New Additional Map");

  setAdditionalMapChanged();

  _ignoreChangeEvents = true;
  _ui->additionalMapListWidget->addItem(item);
  _ui->additionalMapListWidget->setCurrentItem(item);
  _ignoreChangeEvents = false;
  _currentAdditionalMapIndex = _ui->additionalMapListWidget->currentRow();

  _ui->stwgAddPlanTheme->setCurrentIndex(0);
  _ui->cboBoxAdditionalMapTheme->setCurrentIndex(0);
}

//! Slot chamado quando o usuário clica no botao para excluir o mapa adicional atual
void MainDlgAditionalTab::removeAdditionalMapRequested()
{
	int currentPos = _ui->additionalMapListWidget->currentRow();
	QListWidgetItem* oldItem = _ui->additionalMapListWidget->currentItem();

	bool ok = true;

	if (oldItem==NULL)
		return;

    
      // Remove objeto do servidor.  Se não deu certo, retorna e mantém o estado atual
  if(!_newAdditionalMap)
  {
	// Verifca com o usuário se ele deseja realmente remover o servidor remoto
	QMessageBox::StandardButton answer;
	answer = QMessageBox::question(_parent, tr("Remover mapa adicional..."), 
									tr("Deseja realmente remover este mapa adicional ?"),
									QMessageBox::Yes | QMessageBox::No,
									QMessageBox::No);
	if(answer == QMessageBox::No)
		return;

	// Precisamos dessas informações para retornar o tema
	// as combos da interface, agora que ele será liberado
	int themeId = _additionalMapList->at(currentPos)->theme().id;
	std::string themeName = _additionalMapList->at(currentPos)->theme().name;

	ok = _additionalMapList->deleteAdditionalMap(_additionalMapList->at(currentPos)->id());

	// Vamos recarregar a lista de temas do banco
	_manager->loadAdditionalMapThemeData();
	loadAdditionalMapData();
  }

  if (ok)
  {
	// Remove objeto da lista (interface) e desmarca seleção
	_ignoreChangeEvents = true;

	_ui->additionalMapListWidget->removeItemWidget(oldItem);
	delete oldItem;
	_ignoreChangeEvents = false;

    // Desmarca indicadores de modificação e novo objeto monitorado
	clearAdditionalMapChanged();
	_newAdditionalMap = false;

	if (_ui->additionalMapListWidget->count() <= currentPos)
		currentPos = _ui->additionalMapListWidget->count()-1;

	if (currentPos >= 0)
	{
		_ui->additionalMapListWidget->setCurrentRow(currentPos);
		_currentAdditionalMapIndex = currentPos;
		setFields(_additionalMapList->at(_currentAdditionalMapIndex));
	}
	else
	{
		clearFields(true);
		enableFields(false);  // Se não há entradas na lista, desabilita campos
	}
  }  

  emit additionalMapDatabaseChanged();
}

void MainDlgAditionalTab::loadAdditionalMapData()
{
	// Pega as informações da lista de mapas adicionais
	_additionalMapList = _manager->additionalMapList();
	assert(_additionalMapList);
	
	// Pega temas de grade
	_additionalMapThemeGrid = _additionalMapList->getAdditionalMapThemesGrid();

	// Pega temas vetoriais
	_additionalMapThemeVector = _additionalMapList->getAdditionalMapThemesVector();
}
