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
  \file terrama2/gui/config/GridConfigDialog.cpp

  \brief Definition of Class GridConfigDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

// TerraMA2
#include "GridConfigDialog.hpp"
#include "Services.hpp"
#include "WeatherGridList.hpp"
#include "AdditionalMapList.hpp"

// STL
#include <math.h>
#include <TeDefines.h>

// QT
#include <QMessageBox>





GridConfigDialog::GridConfigDialog(QWidget *parent, Qt::WFlags f) :  QDialog(parent, f)
{
	setupUi(this);

	connect(resBaseCmb, SIGNAL(currentIndexChanged(int)), SLOT(resBaseChangeRequest(int)));
	connect(roiBaseCmb, SIGNAL(currentIndexChanged(int)), SLOT(roiBaseChangeRequest(int)));

	connect(okBtn,     SIGNAL(clicked()), SLOT(checkAndAccept()));
	connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

	dummyEdit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, dummyEdit));

	resXEdit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, resXEdit));
	resYEdit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, resYEdit));

	x1Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, x1Edit));
	y1Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, y1Edit));
	x2Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, x2Edit));
	y2Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, y2Edit));
}

GridConfigDialog::~GridConfigDialog()
{
}

//! Inicializa dialogo
void GridConfigDialog::setFields(Services* manager, const struct wsGridOutputConfig & config)
{
  _startConfig = config;

  // Grids
  AdditionalMapList* addMapList      = manager->additionalMapList();
  WeatherGridList*   weatherGridList = manager->weatherGridList();

  // interpolação
  interpCmb->setCurrentIndex((int) config.interpolationMethod);

  // dummy
  dummyEdit->setText(QString::number(config.dummy));

  // resBaseMap
  resBaseCmb->setCurrentIndex((int) config.resBase);

  // resBaseMapCmb
  resBaseMapCmb->clear();

  for(int i=0; i < weatherGridList->size(); i++)
  {
    WeatherGrid* it = weatherGridList->at(i);
    
    int index = resBaseMapCmb->count();

    if(it->data().format != WS_WDSFFMT_Additional_Map)
    {
			resBaseMapCmb->addItem(it->name());
			resBaseMapCmb->setItemData(index, it->id(), Qt::UserRole);
			resBaseMapCmb->setItemData(index, false, Qt::UserRole+1);
		}
		if(!config.resBaseMapIsAddMap && it->id() == (int)config.resBaseMap) resBaseMapCmb->setCurrentIndex(index);
  }

  for(int i=0; i < addMapList->size(); i++)
  {
    AdditionalMap* addMap = addMapList->at(i);

    int index = resBaseMapCmb->count();

    if(addMap->theme().geometry == WS_LAYERGEOM_RASTER || addMap->theme().geometry == WS_LAYERGEOM_RASTERFILE)
    {
			resBaseMapCmb->addItem(addMap->name());
			resBaseMapCmb->setItemData(index, addMap->id(), Qt::UserRole);
			resBaseMapCmb->setItemData(index, true, Qt::UserRole+1);
    }
    if(config.resBaseMapIsAddMap && addMap->id() == (int)config.resBaseMap) resBaseMapCmb->setCurrentIndex(index);
  }

	// resX, resY
	resXEdit->setText(QString::number(config.resX));
	resYEdit->setText(QString::number(config.resY));

	//roiBaseCmb
	roiBaseCmb->setCurrentIndex((int) config.roiBase);

	// roiBaseMapCmb
	roiBaseMapCmb->clear();

	for(int i=0; i < weatherGridList->size(); i++)
  {
    WeatherGrid* it = weatherGridList->at(i);

    int index = roiBaseMapCmb->count();

    if(it->data().format != WS_WDSFFMT_Additional_Map)
    {
			roiBaseMapCmb->addItem(it->name());
			roiBaseMapCmb->setItemData(index, it->id(), Qt::UserRole);
			roiBaseMapCmb->setItemData(index, false, Qt::UserRole+1);
		}
		if(!config.roiBaseMapIsAddMap && it->id() == (int)config.roiBaseMap) roiBaseMapCmb->setCurrentIndex(index);
  }

  for(int i=0; i < addMapList->size(); i++)
  {
    AdditionalMap* addMap = addMapList->at(i);

    int index = roiBaseMapCmb->count();

    if(addMap->theme().geometry == WS_LAYERGEOM_RASTER || addMap->theme().geometry == WS_LAYERGEOM_RASTERFILE)
    {
			roiBaseMapCmb->addItem(addMap->name());
			roiBaseMapCmb->setItemData(index, addMap->id(), Qt::UserRole);
			roiBaseMapCmb->setItemData(index, true, Qt::UserRole+1);
    }
    if(config.roiBaseMapIsAddMap && addMap->id() == (int)config.roiBaseMap) roiBaseMapCmb->setCurrentIndex(index);
  }

	// x1, y1, x2, y2
	x1Edit->setText(QString::number(config.x1));
	y1Edit->setText(QString::number(config.y1));
	x2Edit->setText(QString::number(config.x2));
	y2Edit->setText(QString::number(config.y2));

	// Não deveria ser necessario mas não funciona sem a seguinte linha
	resBaseChangeRequest((int) config.resBase);
	roiBaseChangeRequest((int) config.roiBase);
}

void GridConfigDialog::getFields(struct wsGridOutputConfig & config, bool& changed)
{
  // interpolação
	config.interpolationMethod = (wsInterpolationMethod)  interpCmb->currentIndex();

	// dummy
	bool ok;
	config.dummy = dummyEdit->text().toDouble(&ok);
	if(!ok) config.dummy = -99999;

	// resBaseMap
	config.resBase = (wsOutputResolutionBase) resBaseCmb->currentIndex();

	// resBaseMapCmb
	config.resBaseMap         = resBaseMapCmb->itemData(resBaseMapCmb->currentIndex(), Qt::UserRole).toInt();
	config.resBaseMapIsAddMap = resBaseMapCmb->itemData(resBaseMapCmb->currentIndex(), Qt::UserRole+1).toBool();

	// resX, resY
	config.resX = resXEdit->text().toDouble();
	config.resY = resYEdit->text().toDouble();

	//roiBaseCmb
	config.roiBase = (wsOutputROIBase) roiBaseCmb->currentIndex();

	// roiBaseMapCmb
	config.roiBaseMap         = roiBaseMapCmb->itemData(roiBaseMapCmb->currentIndex(), Qt::UserRole).toInt();
	config.roiBaseMapIsAddMap = roiBaseMapCmb->itemData(roiBaseMapCmb->currentIndex(), Qt::UserRole+1).toBool();

	// x1, y1, x2, y2
	config.x1 = x1Edit->text().toDouble();
	config.y1 = y1Edit->text().toDouble();
	config.x2 = x2Edit->text().toDouble();
	config.y2 = y2Edit->text().toDouble();

	changed =
			_startConfig.interpolationMethod != config.interpolationMethod ||
			_startConfig.resBase             != config.resBase             ||
			_startConfig.resBaseMap          != config.resBaseMap          ||
			_startConfig.resBaseMapIsAddMap  != config.resBaseMapIsAddMap  ||
			_startConfig.resX                != config.resX                ||
			_startConfig.resY                != config.resY                ||
			_startConfig.roiBase             != config.roiBase             ||
			_startConfig.roiBaseMap          != config.roiBaseMap          ||
			_startConfig.roiBaseMapIsAddMap  != config.roiBaseMapIsAddMap  ||
			_startConfig.x1                  != config.x1                  ||
			_startConfig.y1                  != config.y1                  ||
			_startConfig.x2                  != config.x2                  ||
			_startConfig.y2                  != config.y2                  ||
			_startConfig.dummy               != config.dummy;
}

void GridConfigDialog::resBaseChangeRequest(int index)
{
	if(index == WS_OUTPUT_GRID_RES_MAP)
	{
		stackedWidget->setCurrentIndex(0);
		resLbl->setText(tr("Nome:"));
		resLbl->setVisible(true);
		stackedWidget->setVisible(true);
	}
	else if(index == WS_OUTPUT_GRID_RES_CUSTOM)
	{
		stackedWidget->setCurrentIndex(1);
		resLbl->setText(tr("Valores:"));
		resLbl->setVisible(true);
		stackedWidget->setVisible(true);
	}
	else
	{
		resLbl->setVisible(false);
		stackedWidget->setVisible(false);
	}
}

void GridConfigDialog::roiBaseChangeRequest(int index)
{
	if(index == WS_OUTPUT_GRID_ROI_MAP)
	{
		stackedWidget_2->setCurrentIndex(0);
		roiLbl->setText(tr("Nome:"));
		roiLbl->setVisible(true);
		stackedWidget_2->setVisible(true);
	}
	else if(index == WS_OUTPUT_GRID_ROI_CUSTOM)
	{
		stackedWidget_2->setCurrentIndex(1);
		roiLbl->setText(tr("Valores:"));
		roiLbl->setVisible(true);
		stackedWidget_2->setVisible(true);
	}
	else
	{
		roiLbl->setVisible(false);
		stackedWidget_2->setVisible(false);
	}
}

void GridConfigDialog::checkAndAccept()
{
  if((wsOutputResolutionBase) resBaseCmb->currentIndex() == WS_OUTPUT_GRID_RES_CUSTOM)
  {
    if(fabs(resXEdit->text().toDouble()) < TeMINFLOAT || fabs(resYEdit->text().toDouble()) < TeMINFLOAT)
    {
      QMessageBox::warning(this, tr("Erro validando dados..."), tr("Resoluções devem ser diferentes de zero"));
      return;
    }
  }

  if((wsOutputROIBase) roiBaseCmb->currentIndex() == WS_OUTPUT_GRID_ROI_CUSTOM)
  {
    if(x2Edit->text().toDouble() - x1Edit->text().toDouble() < TeMINFLOAT)
    {
      QMessageBox::warning(this, tr("Erro validando dados..."), tr("x2 deve ser estritamente maior que x1"));
      return;
    }

    if(y2Edit->text().toDouble() - y1Edit->text().toDouble() < TeMINFLOAT)
    {
      QMessageBox::warning(this, tr("Erro validando dados..."), tr("y2 deve ser estritamente maior que y1"));
      return;
    }
  }

  // Dados validos
	accept();
}
