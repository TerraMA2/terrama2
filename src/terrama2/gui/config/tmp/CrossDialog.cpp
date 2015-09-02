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
  \file terrama2/gui/config/CrossDialog.cpp

  \brief Definition of Class CrossDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

// STL
#include <assert.h>
#include <algorithm>

// TerraMA2
#include "CrossDialog.hpp"
#include "CrossAttrDialog.hpp"
#include "Services.hpp"
#include "AdditionalMapList.hpp"
#include "ComboBoxDelegate.hpp"

// QT
#include <QHeaderView>



//! Construtor
CrossDialog::CrossDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  // Conecta sinal para alterar qual dos Mapas Adicionais é o Mapa de Fundo
  connect(crossBackgroundCmbBox, SIGNAL(currentIndexChanged(int)), SLOT(setChanged()));

  // Conecta sinal para preencher lista de atributos do Mapa
  connect(lwAdditionalMaps, SIGNAL(currentRowChanged(int)), SLOT(listAddMapsRowChanged(int)));

  connect(crossAddBtn, SIGNAL(clicked()), SLOT(addCross()));
  connect(crossDelBtn, SIGNAL(clicked()), SLOT(delCross()));

  connect(lwAdditionalMaps, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(addCross()));

  connect(selectBtn,   SIGNAL(clicked()), SLOT(selectAttribute()));
  connect(deselectBtn, SIGNAL(clicked()), SLOT(deselectAttribute()));

  connect(lwAdditionalMapsAttributes,         SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(selectAttribute()));
  connect(lwAdditionalMapsAttributesSelected, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(deselectAttribute()));

  connect(tblwCrossList,	SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(itemUpdate(QTableWidgetItem*)));

  // Configuração dos headers da tabela de mapas selecionados
  tblwCrossList->horizontalHeader()->setHighlightSections(false);
  tblwCrossList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  ComboBoxDelegate* cmbDelegate = new ComboBoxDelegate();
  cmbDelegate->insertItem(tr("Alerta"));
  cmbDelegate->insertItem(tr("Não Visível"));
  tblwCrossList->setItemDelegateForColumn(1, cmbDelegate);

  ComboBoxDelegate* cmbPrint = new ComboBoxDelegate();
  cmbPrint->insertItem(QObject::tr("Não"));
  cmbPrint->insertItem(tr("Sim"));
  tblwCrossList->setItemDelegateForColumn(3, cmbPrint);

  connect(tblwCrossList, SIGNAL(cellChanged(int, int)), SLOT(setChanged(int, int)));
}

//! Destrutor
CrossDialog::~CrossDialog()
{
}

//! Inicializa dialogo
void CrossDialog::setFields(Services* manager, std::vector<struct wsAddMapDisplayConfig> listAddMapId)
{
  _manager = manager;

  _additionalMapList         = NULL;
  _ignoreChangeEvents        = false;
  _currentCross = -1;
  _changed = false;
  
  int backgroundMapID = 0;
  // Para cada um dos mapas selecionados, verificamos se ele é o mapa
  // de fundo. Caso seja, não o adicionamos na lista de mapas (é administrado separadamente)
  for(unsigned int i = 0; i < listAddMapId.size(); i++)
  {
	  if(listAddMapId.at(i).addMap.baseTheme.geometry == WS_LAYERGEOM_RASTER ||
		 listAddMapId.at(i).addMap.baseTheme.geometry == WS_LAYERGEOM_RASTERFILE)
	  {
		 backgroundMapID = listAddMapId.at(i).addMap.id;
	  }
	  else
	  {
		 _listAddMapId.push_back(listAddMapId.at(i));
	  }
  }

  // Carregamos a janela
  load();

  // Após a carga da janela, verificamos quem é o índice atual
  // da combo de mapas de fundo
  crossBackgroundCmbBox->blockSignals(true);
  for (int i = 0; i < crossBackgroundCmbBox->count(); i++)
  {
	  if(crossBackgroundCmbBox->itemData(i) == backgroundMapID)
	  {
		  crossBackgroundCmbBox->setCurrentIndex(i);
	  }
  }
  crossBackgroundCmbBox->blockSignals(false);
}

//! Retorna resultado do dialogo
void CrossDialog::getFields(std::vector<struct wsAddMapDisplayConfig>& listAddMapId, bool& changed)
{
  changed = _changed;

  if (_changed)
  {
    // Aqui, copiamos todos os mapas adicionais vetoriais
	// para a lista.
	listAddMapId.clear();

	// Aqui vamos atualizar o status dos displays ('alerta' ou 'não visível')
    // e o atributo que define se o mapa deve ser impresso em imagens geradas por objetos monitorados
	for (int i = 0; i < tblwCrossList->rowCount(); i++)
	{
	  for(unsigned int j = 0; j < _listAddMapId.size(); j++)
	  {
		if(tblwCrossList->item(i, 0)->data(Qt::UserRole).toInt() == _listAddMapId.at(i).addMap.id)
		{
			_listAddMapId.at(i).status = tblwCrossList->item(i, 1)->text() == tr("Alerta") ? 
															WS_STATUS_VISIBLE_IF_WARNING:
															WS_STATUS_NOT_VISIBLE;

			_listAddMapId.at(i).printImage = tblwCrossList->item(i, 3)->text() == tr("Sim") ? true : false; 
		}
	  }
	}
	listAddMapId = _listAddMapId;

	// Caso exista algum mapa de fundo selecionado, criamos uma DisplayConfig
	// para ele e adicionamos esse mapa à lista final
	if(crossBackgroundCmbBox->currentIndex() > 0)
	{
	  wsAddMapDisplayConfig backgroundMap;
	  for (int i = 0; i < _additionalMapList->size(); i++)
	  {
		if(_additionalMapList->at(i)->id() == crossBackgroundCmbBox->itemData(crossBackgroundCmbBox->currentIndex()).toInt())
		{
		  backgroundMap.addMap = _additionalMapList->at(i)->data();
		  backgroundMap.status = WS_STATUS_VISIBLE_IF_WARNING;
	      backgroundMap.background = true;
		  backgroundMap.printImage = false; //Por enquanto nao existe opcao para imprimir mapa de fundo
		}
	  }
	  listAddMapId.push_back(backgroundMap);
	}
  }
}


//! Carrega lista de mapas adicionais
void CrossDialog::load()
{
  // Carrega novas informações
  _additionalMapList = _manager->additionalMapList();
  assert(_additionalMapList);

  fillTableAddMapAvaible();
  fillTableAddMapSelected();
}

//! Preenche a lista de planos de adicionais disponíveis.
void CrossDialog::fillTableAddMapAvaible()
{
  _ignoreChangeEvents = true;
  crossBackgroundCmbBox->blockSignals(true);

  lwAdditionalMaps->clear();
  crossBackgroundCmbBox->clear();
  crossBackgroundCmbBox->addItem(tr("Nenhum"));

  for(int i=0; i < _additionalMapList->size(); i++)
  {
    AdditionalMap* addMap = _additionalMapList->at(i);

    if(addMap->theme().geometry != WS_LAYERGEOM_RASTER && 
	   addMap->theme().geometry != WS_LAYERGEOM_RASTERFILE)
    {
	  // Caso ele não seja uma mapa de raster (não possa ser um mapa de fundo),
	  // o adicionamos na lista de disponíveis caso não esteja selecionado.
      if (!selectedAddMapId(addMap->id()))
	  {
	    QListWidgetItem *lwiInc;
		QIcon *icnAdd = new QIcon(":/data/icons/info_vect.png");
		lwiInc = new QListWidgetItem(*icnAdd, addMap->name());
		lwiInc->setData(Qt::UserRole, addMap->id());
		lwAdditionalMaps->addItem(lwiInc);
	  }
	}
	else
	{
	  // Caso seja um raster, o colocamos na combo de mapas de fundo
	  crossBackgroundCmbBox->addItem(QIcon(":/data/icons/novo_grid.png"), addMap->name(), addMap->id());
	}
  }

  _ignoreChangeEvents = false;
  crossBackgroundCmbBox->blockSignals(false);
}

//! Preenche a tabela de planos de adicionais selecionados.
void CrossDialog::fillTableAddMapSelected()
{
  QTableWidgetItem *itemMapName;
  QTableWidgetItem *itemAttribStatus;
  QTableWidgetItem *itemAttribDescription;
  QTableWidgetItem *itemPrint;
  wsAddMapDisplayConfig strAddMapId;

  tblwCrossList->blockSignals(true);

  tblwCrossList->setRowCount(_listAddMapId.size());

  for (unsigned i = 0; i < _listAddMapId.size(); i++)
  {
	strAddMapId = _listAddMapId.at(i);

	itemMapName           = new QTableWidgetItem(QString::fromStdString(strAddMapId.addMap.name));
	itemMapName->setData(Qt::UserRole, strAddMapId.addMap.id);
	itemMapName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	itemAttribStatus      = new QTableWidgetItem("n/d");
	itemPrint			  = new QTableWidgetItem("n/d");

	if (strAddMapId.status == WS_STATUS_NOT_VISIBLE)
	{
		itemAttribStatus->setText(tr("Não Visivel"));
		itemAttribStatus->setData(Qt::UserRole, 1);		// usado pelo índice da combo
	}
	else if (strAddMapId.status == WS_STATUS_VISIBLE_IF_WARNING)
	{
		itemAttribStatus->setText(tr("Alerta"));
		itemAttribStatus->setData(Qt::UserRole, 0);		// usado pelo índice da combo
	}

	if (strAddMapId.printImage)
	{
		itemPrint->setText(tr("Sim"));
		itemPrint->setData(Qt::UserRole, 1);		// usado pelo índice da combo
	}
	else
	{
		itemPrint->setText(QObject::tr("Não"));
		itemPrint->setData(Qt::UserRole, 0);		// usado pelo índice da combo
	}

	tblwCrossList->setItem(i, 0, itemMapName);
	tblwCrossList->setItem(i, 1, itemAttribStatus);

	QString attributes;
	if(strAddMapId.attributes.empty())
		attributes = tr("Nenhum");
	else
	{
		for(unsigned int j = 0; j < strAddMapId.attributes.size(); j++)
		{
			if(!attributes.isEmpty()) attributes += ", ";
			attributes += QString::fromStdString(strAddMapId.attributes.at(j));
		}
	}

	itemAttribDescription = new QTableWidgetItem(attributes);
	itemAttribDescription->setData(Qt::UserRole, strAddMapId.addMap.id);
	tblwCrossList->setItem(i, 2, itemAttribDescription);
	tblwCrossList->item(i,2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // evita que esse item seja editável

	tblwCrossList->setItem(i, 3, itemPrint);	
  }

  crossDelBtn->setEnabled(_listAddMapId.size()>0);
  crossAddBtn->setEnabled(lwAdditionalMaps->count()>0);

  tblwCrossList->setEnabled(_listAddMapId.size()>0);
  grpBoxAttribAvaible->setEnabled(lwAdditionalMaps->count()>0);

  lwAdditionalMapsAttributes->setEnabled(lwAdditionalMapsAttributes->count()>0);
  lwAdditionalMapsAttributesSelected->setEnabled(lwAdditionalMapsAttributesSelected->count()>0);

  tblwCrossList->blockSignals(false);
}

//! Procura AdditionalMap já foi selecionado
bool CrossDialog::selectedAddMapId(int id)
{
	bool ret = false;

	for (unsigned i=0; i< _listAddMapId.size() && !ret; i++)
	{
		if (_listAddMapId.at(i).addMap.id == id)
			ret = true;
	}

	return ret;
}

//! Procura AdditionalMap na lista de todos os mapas adicionais disponiveis
bool CrossDialog::findAddMapId(int id, wsAddMapDisplayConfig& admCfg)
{
	bool ret = false;

	for (int i=0; i< _additionalMapList->size() && !ret; i++)
	{
		if (_additionalMapList->at(i)->id() == id)
		{
			admCfg.addMap = _additionalMapList->at(i)->data();
			admCfg.status = WS_STATUS_VISIBLE_IF_WARNING; // default
			admCfg.background = false;
			admCfg.printImage = false;
			ret = true;
		}
	}

	return ret;
}

//! Trata mudanca de selecao de mapa adicional (carrega lista de atributos no novo mapa adicional selecionado
void CrossDialog::listAddMapsRowChanged(int index)
{
  if (_ignoreChangeEvents)
	  return;

  lwAdditionalMapsAttributes->clear();
  lwAdditionalMapsAttributesSelected->clear();

  if (index <0)
	  return;

  QListWidgetItem* item = lwAdditionalMaps->item(index);
  int id = item->data(Qt::UserRole).toInt();
  wsAddMapDisplayConfig admCfgAdd;

  if (findAddMapId(id, admCfgAdd))
  {
    const wsTheme& tema = admCfgAdd.addMap.baseTheme;
    // Preenche a lista de planos de adicionais.
    if(tema.attributes.size()>0) 
    {
      for(unsigned i=0; i < tema.attributes.size(); i++)
      {
        std::vector<std::string> &v = admCfgAdd.attributes;
        if(std::find(v.begin(), v.end(), tema.attributes.at(i).name) != v.end())
          lwAdditionalMapsAttributesSelected->addItem(new QListWidgetItem(QString::fromStdString(tema.attributes.at(i).name)) );
        else
          lwAdditionalMapsAttributes->addItem(new QListWidgetItem(QString::fromStdString(tema.attributes.at(i).name)) );
      }
    }
  }
  lwAdditionalMapsAttributes->setEnabled(lwAdditionalMapsAttributes->count()>0);
  lwAdditionalMapsAttributesSelected->setEnabled(lwAdditionalMapsAttributesSelected->count()>0);
}

//! Inclui associacao do Mapa Adicional
void CrossDialog::addCross()
{
  if (lwAdditionalMaps->selectedItems().size()>0)
  {
	for (int i=0; i<lwAdditionalMaps->selectedItems().size(); i++)
	{
	   QListWidgetItem *item = lwAdditionalMaps->selectedItems().at(i);
	   int idAdd = item->data(Qt::UserRole).toInt();

       wsAddMapDisplayConfig admCfgAdd;

	   if (findAddMapId(idAdd, admCfgAdd))
	   {
			 // Atributos selecionados
			 for(int j=0; j<lwAdditionalMapsAttributesSelected->count(); j++)
				 admCfgAdd.attributes.push_back(lwAdditionalMapsAttributesSelected->item(j)->text().toStdString());

			 _listAddMapId.push_back(admCfgAdd);
	   }

		lwAdditionalMaps->removeItemWidget(item);
		delete item;
	}

    fillTableAddMapSelected();
    emit setChanged();
  }
}

//! Remove associacao do Mapa Adicional
void CrossDialog::delCross()
{
  if (tblwCrossList->selectedItems().size())
  {
	for (int i=0; i<tblwCrossList->selectedItems().size(); i++)
	{
	   QTableWidgetItem *item = tblwCrossList->selectedItems().at(i);

	   if (item->column() == 0)
	   {
	     int idDel = item->data(Qt::UserRole).toInt();
		 std::vector<struct wsAddMapDisplayConfig>::iterator pos = findItemListAddMapId(idDel);

		 if (pos != _listAddMapId.end())
			 _listAddMapId.erase(pos);
	   }
	}

	// Aqui, como recarregamos a lista de disponíveis e ela zera o
	// índice ca combo de mapas de fundo, precisamos guardá-lo.
	int cmbBoxIndex = crossBackgroundCmbBox->currentIndex();
    fillTableAddMapAvaible();
    fillTableAddMapSelected();
	crossBackgroundCmbBox->setCurrentIndex(cmbBoxIndex);

    emit setChanged();
  }
}

void CrossDialog::selectAttribute()
{
  if(lwAdditionalMapsAttributes->selectedItems().size() > 0)
  {
    for (int i = 0; i < lwAdditionalMapsAttributes->selectedItems().size(); i++)
    {
      int row = lwAdditionalMapsAttributes->row(lwAdditionalMapsAttributes->selectedItems().at(i));
      lwAdditionalMapsAttributesSelected->addItem(lwAdditionalMapsAttributes->takeItem(row));
    }
  }
  lwAdditionalMapsAttributes->setEnabled(lwAdditionalMapsAttributes->count()>0);
  lwAdditionalMapsAttributesSelected->setEnabled(lwAdditionalMapsAttributesSelected->count()>0);
}

void CrossDialog::deselectAttribute()
{
  if(lwAdditionalMapsAttributesSelected->selectedItems().size() > 0)
  {
    for (int i = 0; i < lwAdditionalMapsAttributesSelected->selectedItems().size(); i++)
    {
      int row = lwAdditionalMapsAttributesSelected->row(lwAdditionalMapsAttributesSelected->selectedItems().at(i));
      lwAdditionalMapsAttributes->addItem(lwAdditionalMapsAttributesSelected->takeItem(row));
    }
  }
  lwAdditionalMapsAttributes->setEnabled(lwAdditionalMapsAttributes->count()>0);
  lwAdditionalMapsAttributesSelected->setEnabled(lwAdditionalMapsAttributesSelected->count()>0);
}

void CrossDialog::itemUpdate(QTableWidgetItem *item)
{
	if (item->column() != 2)
		return;

	int id = item->data(Qt::UserRole).toInt();

	std::vector<struct wsAddMapDisplayConfig>::iterator it = findItemListAddMapId(id);

	if (it == _listAddMapId.end())
		return;

	wsAddMapDisplayConfig &admCfgAdd = *it;

	CrossAttrDlg dlg;
	dlg.setFields(admCfgAdd);

  if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
  {
    bool changed;
    dlg.getFields(admCfgAdd, changed);

    if(changed)
      setChanged();

    // Atualizar o texto do item
    QString attributes;
    if(admCfgAdd.attributes.empty())
      attributes = tr("Nenhum");
    else
    {
      for(unsigned int j = 0; j < admCfgAdd.attributes.size(); j++)
      {
        if(!attributes.isEmpty()) attributes += ", ";
        attributes += QString::fromStdString(admCfgAdd.attributes.at(j));
      }
    }
    item->setText(attributes);
  }
}

void CrossDialog::setChanged(int row, int column)
{
	_changed = true;

	if(row >= 0 && (column == 1 || column == 3))
	{
		int id = tblwCrossList->item(row, 0)->data(Qt::UserRole).toInt();

		std::vector<struct wsAddMapDisplayConfig>::iterator it = findItemListAddMapId(id);

		if (it == _listAddMapId.end())
			return;

		wsAddMapDisplayConfig &admCfgAdd = *it;

		if(column == 1)
		{
			// Atualizar plano com o valor da coluna que define o Estado (Alerta ou Não Visível)
			admCfgAdd.status = tblwCrossList->item(row, column)->text() == tr("Alerta") ? WS_STATUS_VISIBLE_IF_WARNING : WS_STATUS_NOT_VISIBLE;
		}
		else if(column == 3)
		{
			// Atualizar plano com o valor da coluna Imprimir no Log
			admCfgAdd.printImage = tblwCrossList->item(row, column)->text() == tr("Sim") ? true : false;
		}
	}
}

//! Procura id do AdditionalMap na lista de todos os mapas adicionais selecionados
std::vector<struct wsAddMapDisplayConfig>::iterator CrossDialog::findItemListAddMapId(int id)
{
  bool achou = false;
  std::vector<struct wsAddMapDisplayConfig>::iterator ret;

  for (ret=_listAddMapId.begin(); ret< _listAddMapId.end() && !achou; ret++)
  {
    if (ret->addMap.id == id)
      achou = true;
  }

  if (achou)
    ret--;

  return ret;
}
