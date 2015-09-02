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
  \file terrama2/gui/config/IntersectionDialog.cpp

  \brief Definition of Class IntersectionDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

// STL
#include <assert.h>

// TerraMA2
#include "IntersectionDialog.hpp"

// QT  
#include <QHeaderView>

//! Construtor
IntersectionDialog::IntersectionDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _intersectionChanged = false;
  _ignoreChangeEvents = false;

  _transparentPixmap = QPixmap(18, 18);
  _transparentPixmap.fill(Qt::transparent);

  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(goToGridPageBtn,  SIGNAL(clicked()), SLOT(goToGridPageSlot()));
  connect(goToThemePageBtn, SIGNAL(clicked()), SLOT(goToThemePageSlot()));

  connect(bandLed, SIGNAL(textChanged(const QString&)), SLOT(setIntersectionChanged()));
  connect(bandLed, SIGNAL(textChanged(const QString&)), SLOT(setBandIntersection()));

  QRegExp regExp("^[0-9]+(-[0-9]+)?(,[0-9]+(-[0-9]+)?)*$");
  bandLed->setValidator(new QRegExpValidator(regExp, bandLed));

  connect(themeList, SIGNAL(currentRowChanged(int)), SLOT(changeRowTheme()));

  vectorAttributesTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  vectorAttributesTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
  connect(vectorAttributesTableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(enableStaticVectorIntersection(QTableWidgetItem*)));

  dynamicGridsTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  dynamicGridsTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
  connect(dynamicGridsTableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(enableDynamicGridIntersection(QTableWidgetItem*)));
  
  okBtn->setEnabled(false);
}

//! Destrutor
IntersectionDialog::~IntersectionDialog()
{
}

void IntersectionDialog::goToGridPageSlot()
{
	layerTypeStc->setCurrentWidget(dynamicGridListPage);
	geometryStc->setCurrentWidget(dynamicGridPage);
}

void IntersectionDialog::goToThemePageSlot()
{
	layerTypeStc->setCurrentWidget(themeListPage);

	if(themeList->count() > 0)
		changeRowTheme(); //Para atualizar a interface com os dados do elemento que ja estiver selecionado
	else
		geometryStc->setCurrentWidget(emptyPage);
}

void IntersectionDialog::setFields(const wsWeatherDataSourceIntersection& interParams, std::vector<struct wsTheme> themes, WeatherGridList* weatherGridList)
{
	bool lOldStat = _ignoreChangeEvents;
	_ignoreChangeEvents = true;

	_themes = themes;

	// Preencher a lista de temas estaticos (matriciais e vetoriais)
	themeList->clear();
	for(size_t i=0,size=themes.size(); i<size; ++i)
	{
		wsTheme theme = themes.at(i);

		if (theme.geometry == WS_LAYERGEOM_POLYGONS || theme.geometry == WS_LAYERGEOM_LINES || theme.geometry == WS_LAYERGEOM_POINTS || theme.geometry == WS_LAYERGEOM_RASTER)
		{
			QListWidgetItem* item = new QListWidgetItem(QIcon(_transparentPixmap), QString((theme.name).c_str()));
			item->setData(Qt::UserRole, theme.id);
			themeList->addItem(item);

			if (theme.geometry == WS_LAYERGEOM_RASTER)
			{
				wsWeatherDataSourceIntersectionRaster wdsR = findIntersectionRaster(interParams.rasterIntersection, theme.id);
				_interRasterMap.insert( std::pair<int,wsWeatherDataSourceIntersectionRaster>(theme.id,wdsR));

				// Se alguma banda estiver selecionada para este item, adicionar icone para informar isto ao usuario
				if(!wdsR.bandFilterString.empty())
					item->setIcon(QIcon(":/global/icons/ok.png"));
			}
			else
			{
				// Guardar os atributos do tema vetorial que estão selecionados para interseção
				std::vector<std::string> intersectionAttributes = findIntersectionVectorAttributes(interParams.vectorIntersection, theme.id);

				QSet<QString> attributes;
				for(int k = 0; k < intersectionAttributes.size(); ++k)
				{
					attributes.insert(QString::fromStdString(intersectionAttributes.at(k)));
				}

				_interVectorMap.insert(theme.id, attributes);

				// Se algum atributo estiver selecionado para este item, adicionar icone para informar isto ao usuario
				if(attributes.size() > 0)
					item->setIcon(QIcon(":/global/icons/ok.png"));
			}
		}		
	}

	// Preencher a tabela de grades dinamicas
	dynamicGridsTableWidget->clearContents();
	if(weatherGridList)
	{
		int weatherGridListSize = weatherGridList->count();

		// Guardar o id das fontes que ja estao selecionadas
		for(int i = 0; i < interParams.dataSourceIntersection.size(); ++i)
		{
			_interDataSource.insert(interParams.dataSourceIntersection.at(i));
		}
		
		for(int i = 0; i < weatherGridListSize; ++i)
		{
			WeatherGrid* weatherGrid = weatherGridList->at(i);

			// Vamos considerar como grade dinamica: fontes de dados matriciais, 
			// grades obtidas por analises de modelo e grades obtidas por interpolacao de PCDs
			wsWeatherDataSourceFileFormat dsFormat = weatherGrid->data().format;
			if(dsFormat != WS_WDSFFMT_ASCII_Grid && dsFormat != WS_WDSFFMT_TIFF && dsFormat != WS_WDSFFMT_GrADS && 
				dsFormat != WS_WDSFFMT_OGC_WCS && dsFormat != WS_WDSFFMT_Model && dsFormat != WS_WDSFFMT_Surface)
			{
				continue;
			}

			// Fontes de dados matriciais com mais de uma banda nao devem estar disponiveis para intersecao
			// (durante a intersecao so eh considerada uma banda dos dados coletados pelas fontes de dados)
			if(weatherGrid->data().grads_numBands > 1)
			{
				continue;
			}
			
			// Inserir linha na ultima posicao da tabela de grades dinamicas
			QString attrName = (weatherGrid ? weatherGrid->name() : "");
			if(!attrName.isEmpty())
			{
				int rowCount = dynamicGridsTableWidget->rowCount();
				dynamicGridsTableWidget->insertRow(rowCount);

				QString iconName;
				if(dsFormat == WS_WDSFFMT_Model)
					iconName = ":/data/icons/analizemodel.png";
				else if(dsFormat == WS_WDSFFMT_Surface)
					iconName = ":/data/icons/surface.png";
				else
					iconName = ":/data/icons/dado_grid.png";

				QTableWidgetItem *item = new QTableWidgetItem(QIcon(iconName), attrName);
				item->setData(Qt::UserRole, weatherGrid->id());

				dynamicGridsTableWidget->setItem(rowCount, 0, item);
				dynamicGridsTableWidget->setItem(rowCount, 1, new QTableWidgetItem());

				if(_interDataSource.contains(weatherGrid->id()))
					dynamicGridsTableWidget->item(rowCount, 1)->setIcon(QIcon(":/global/icons/ok.png"));
			}
		}
	}

	// Exibir a pagina de temas estaticos
	layerTypeStc->setCurrentWidget(themeListPage);
	if(themeList->count() > 0)
		themeList->setCurrentRow(0);
	else
		geometryStc->setCurrentWidget(emptyPage);

	_ignoreChangeEvents = lOldStat;
}

void IntersectionDialog::getFields(wsWeatherDataSourceIntersection *interParams, bool& changed)
{
	changed = _intersectionChanged;
	interParams->rasterIntersection.clear();
	interParams->vectorIntersection.clear();
	interParams->dataSourceIntersection.clear();

	for(size_t i=0,size=_themes.size(); i<size; ++i)
	{
		wsTheme theme = _themes.at(i);

		if (theme.geometry == WS_LAYERGEOM_RASTER)
		{
			wsWeatherDataSourceIntersectionRaster wds = _interRasterMap[theme.id];
			interParams->rasterIntersection.push_back(wds);
		}
		else
		{
			wsWeatherDataSourceIntersectionVector wdsIntersectionVector;
			wdsIntersectionVector.themeID = theme.id;

			// Recuperar quais atributos do tema vetorial foram selecionados
			QSet<QString> attributes = _interVectorMap[theme.id];
			QSetIterator<QString> it(attributes);
			while(it.hasNext())
			{
				wdsIntersectionVector.intersectionAttributes.push_back(it.next().toStdString());
			}

			// Inserir configuração de interseção do tema vetorial
			interParams->vectorIntersection.push_back(wdsIntersectionVector);
		}		
	}

	// Obter as grades dinamicas selecionadas para intersecao
	QSetIterator<int> it(_interDataSource);
	while(it.hasNext())
	{
		interParams->dataSourceIntersection.push_back(it.next());
	}
}

void IntersectionDialog::changeRowTheme()
{
	bool lOldStat = _ignoreChangeEvents;
	_ignoreChangeEvents = true;

	int themeId = themeList->currentItem()->data(Qt::UserRole).toInt();
	
	wsTheme theme;
	theme.id = -1;
	for(size_t i=0; i<_themes.size(); ++i)
	{
		theme = _themes.at(i);
		if(theme.id == themeId)
			break;
	}

	if(theme.id == -1)
		return;

	if (theme.geometry == WS_LAYERGEOM_POLYGONS || theme.geometry == WS_LAYERGEOM_LINES || theme.geometry == WS_LAYERGEOM_POINTS)
	{
		geometryStc->setCurrentWidget(vectorPage);
		
		// Limpar a tabela de atributos
		int numattr = theme.attributes.size();
		vectorAttributesTableWidget->clearContents();
		vectorAttributesTableWidget->setRowCount(numattr);

		// Carregar a tabela de atributos
		QSet<QString> interAttributes = _interVectorMap[themeId];
		for(int i = 0; i<numattr; i++)
		{
			QString attrName = QString::fromStdString(theme.attributes.at(i).name);
			vectorAttributesTableWidget->setItem(i, 0, new QTableWidgetItem(attrName));
			vectorAttributesTableWidget->setItem(i, 1, new QTableWidgetItem());

			if(interAttributes.contains(attrName))
				vectorAttributesTableWidget->item(i, 1)->setIcon(QIcon(":/global/icons/ok.png"));
		}
	}
	else if (theme.geometry == WS_LAYERGEOM_RASTER)
	{
		geometryStc->setCurrentWidget(rasterPage);
		wsWeatherDataSourceIntersectionRaster wds = _interRasterMap[themeId];
		bandLed->setText(QString::fromStdString(wds.bandFilterString));
	}

	_ignoreChangeEvents = lOldStat;
}

void IntersectionDialog::setIntersectionChanged()
{
	if (_ignoreChangeEvents)
		return;

	_intersectionChanged = true;
	okBtn->setEnabled(true);
}

//! Procura no vetor por uma estrutura wsWeatherDataSourceIntersectionRasterRetorna com o mesmo themeId passado como parâmetro e retorna uma cópia 
wsWeatherDataSourceIntersectionRaster IntersectionDialog::findIntersectionRaster( std::vector<wsWeatherDataSourceIntersectionRaster> interRaster, int themeId )
{
	wsWeatherDataSourceIntersectionRaster wdsIntersectionRaster;
	wdsIntersectionRaster.themeID = themeId;
	wdsIntersectionRaster.bandFilterString = "";

	for (size_t i = 0; i < interRaster.size(); i++)
	{
		wsWeatherDataSourceIntersectionRaster it = interRaster.at(i);
		if (it.themeID == themeId)
		{
			wdsIntersectionRaster.bandFilterString = it.bandFilterString;
			break;
		}
	}

	return wdsIntersectionRaster;
}

//! Retorna os nomes dos atributos que serão usados na interseção um determinado tema
std::vector<std::string> IntersectionDialog::findIntersectionVectorAttributes(std::vector<wsWeatherDataSourceIntersectionVector> interVector, int themeId)
{
	std::vector<std::string> intersectionAttributes;

	for (int i = 0; i < interVector.size(); i++)
	{
		wsWeatherDataSourceIntersectionVector it = interVector.at(i);
		if (it.themeID == themeId)
		{
			intersectionAttributes = it.intersectionAttributes;
			break;
		}
	}

	return intersectionAttributes;
}

//! Slot usado para definir quais bandas de um plano raster serão usadas na interseção
void IntersectionDialog::setBandIntersection()
{
	QListWidgetItem *themeListItem = themeList->currentItem();

	if(!themeListItem)
		return;
	
	int themeId = themeListItem->data(Qt::UserRole).toInt();

	wsWeatherDataSourceIntersectionRaster& wds = _interRasterMap[themeId];
	wds.bandFilterString = bandLed->text().toStdString();

	if(wds.bandFilterString.empty())
		themeListItem->setIcon(QIcon(_transparentPixmap));
	else
		themeListItem->setIcon(QIcon(":/global/icons/ok.png"));
}

//! Slot usado para definir se um atributo de um plano vetorial será usado na interseção
void IntersectionDialog::enableStaticVectorIntersection(QTableWidgetItem* item)
{
	QListWidgetItem *themeListItem = themeList->currentItem();

	if(!item || !themeListItem)
		return;

	setIntersectionChanged();

	int themeId = themeListItem->data(Qt::UserRole).toInt();
	QSet<QString>& interAttributes = _interVectorMap[themeId];

	int row = item->row();
	QString attrName = vectorAttributesTableWidget->item(row,0)->data(Qt::DisplayRole).toString();

	if(interAttributes.contains(attrName))
	{
		interAttributes.remove(attrName);
		vectorAttributesTableWidget->item(row,1)->setIcon(QIcon());
	}
	else
	{
		interAttributes.insert(attrName);
		vectorAttributesTableWidget->item(row,1)->setIcon(QIcon(":/global/icons/ok.png"));
	}	

	if(interAttributes.size() == 0)
		themeListItem->setIcon(QIcon(_transparentPixmap));
	else
		themeListItem->setIcon(QIcon(":/global/icons/ok.png"));
}

//! Slot usado para definir se uma grade dinamica sera usada na intersecao
void IntersectionDialog::enableDynamicGridIntersection(QTableWidgetItem* item)
{
	if(item != NULL)
	{
		setIntersectionChanged();

		int row = item->row();
		int dataSourceId = dynamicGridsTableWidget->item(row,0)->data(Qt::UserRole).toInt();

		if(_interDataSource.contains(dataSourceId))
		{
			_interDataSource.remove(dataSourceId);
			dynamicGridsTableWidget->item(row,1)->setIcon(QIcon());
		}
		else
		{
			_interDataSource.insert(dataSourceId);
			dynamicGridsTableWidget->item(row,1)->setIcon(QIcon(":/global/icons/ok.png"));
		}
	}
}