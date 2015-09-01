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
  \file terrama2/gui/config/ColPointDialog.cpp

  \brief Definition of methods in class ColPointDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


// STL
#include <assert.h>

// QT
#include <QDoubleValidator>

// TerraMA2  
#include "ColPointDialog.hpp"

//! Construtor
ColPointDialog::ColPointDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _ignoreChangeEvents = false;
  _collectionPointChanged  = false;

  okBtn->setEnabled(false);

  // Connect
  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  // Conecta sinais para detectar dados alterados
  connect(ledColPointFileName,   SIGNAL(textEdited(const QString&)), SLOT(setColPointChanged()));
  connect(ledColPointLatitude,	 SIGNAL(textEdited(const QString&)), SLOT(setColPointChanged()));
  connect(ledColPointLongitude,  SIGNAL(textEdited(const QString&)), SLOT(setColPointChanged()));
  connect(activeCmb, SIGNAL(currentIndexChanged(const QString&)), SLOT(setColPointChanged()));

  ledColPointLatitude->setValidator(new QDoubleValidator(ledColPointLatitude));
  ledColPointLongitude->setValidator(new QDoubleValidator(ledColPointLongitude));
}

//! Destrutor
ColPointDialog::~ColPointDialog()
{
}

//! Preenche valores da interface conforme collectionPoint
void ColPointDialog::setFields(const wsPCD& colPoint)
{
  _ignoreChangeEvents = true;

  QString auxStr;

  ledColPointFileName->setText(QString::fromStdString(colPoint.fileName));
  auxStr.setNum(colPoint.latitude); ledColPointLatitude->setText(auxStr);
  auxStr.setNum(colPoint.longitude); ledColPointLongitude->setText(auxStr);
  colPoint.isActive ? activeCmb->setCurrentIndex(0) : activeCmb->setCurrentIndex(1);

  _ignoreChangeEvents = false;
}

//! Preenche collectionPoint conforme valores da interface
void ColPointDialog::getFields(wsPCD* colPoint, bool& changed)
{
  colPoint->fileName  = ledColPointFileName->text().trimmed().toStdString();
  colPoint->latitude  = ledColPointLatitude->text().toDouble();
  colPoint->longitude = ledColPointLongitude->text().toDouble();
  
  colPoint->isActive = (activeCmb->currentText() == tr("Sim")) ? true : false; 

  changed = _collectionPointChanged;
}

//! Marca flag indicando que houve alteração em algum campo
void ColPointDialog::setColPointChanged()
{
  if (_ignoreChangeEvents)
	  return;

  okBtn->setEnabled(true);
  _collectionPointChanged = true;
}