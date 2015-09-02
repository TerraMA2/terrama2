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
  \file terrama2/gui/config/ConfigDialog.cpp

  \brief Definition of Class ConfigDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>

 // QT 
#include <QIntValidator>

// TerraMA2
#include "ConfigDialog.hpp"
#include "Services.hpp"


//! Construtor
ConfigDialog::ConfigDialog(Services* manager, bool isNew, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _manager = manager;
  _changed = false;

  if (isNew)
    setDefaultConfig();
  else
    loadServiceData();

  connect(saveBtn,   SIGNAL(clicked()), SLOT(saveConfig()));
  connect(saveBtn,   SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(cancelChanges()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(weatherDataAddressLed,			SIGNAL(textEdited(const QString&)), SLOT(setChanged()));
  connect(weatherDataPortLed,				SIGNAL(textEdited(const QString&)), SLOT(setChanged()));

  connect(analysisAddressLed,				SIGNAL(textEdited(const QString&)), SLOT(setChanged()));
  connect(analysisPortLed,  				SIGNAL(textEdited(const QString&)), SLOT(setChanged()));

  connect(planServiceAddressLed,			SIGNAL(textEdited(const QString&)), SLOT(setChanged()));
  connect(planServicePortLed,				SIGNAL(textEdited(const QString&)), SLOT(setChanged()));

  connect(planServiceAddressLed,			SIGNAL(textEdited(const QString&)), SLOT(setChanged()));
  connect(planServicePortLed,				SIGNAL(textEdited(const QString&)), SLOT(setChanged()));

  connect(notificationServiceAddressLed,	SIGNAL(textEdited(const QString&)), SLOT(setChanged()));
  connect(notificationServicePortLed,		SIGNAL(textEdited(const QString&)), SLOT(setChanged()));

  weatherDataPortLed->setValidator(new QIntValidator(weatherDataPortLed));
  analysisPortLed->setValidator(new QIntValidator(analysisPortLed));
  planServicePortLed->setValidator(new QIntValidator(planServicePortLed));
  notificationServicePortLed->setValidator(new QIntValidator(notificationServicePortLed));
}

//! Destrutor
ConfigDialog::~ConfigDialog()
{
}
//! Carrega para a interface dados (endereços) default
void ConfigDialog::setDefaultConfig()
{
  weatherDataAddressLed->setText("http://localhost/");
  weatherDataPortLed->setText(QString::number(1111));
  analysisAddressLed->setText("http://localhost/");
  analysisPortLed->setText(QString::number(2222));
  planServiceAddressLed->setText("http://localhost/");
  planServicePortLed->setText(QString::number(3333));
  notificationServiceAddressLed->setText("http://localhost/");
  notificationServicePortLed->setText(QString::number(4444));
}

//! Carrega para a interface os dados (endereços) contidos no gerenciador de serviços
void ConfigDialog::loadServiceData()
{
  weatherDataAddressLed->setText(_manager->weatherDataServiceAddress());
  weatherDataPortLed->setText(QString::number(_manager->weatherDataServicePort()));
  analysisAddressLed->setText(_manager->analysisServiceAddress());
  analysisPortLed->setText(QString::number(_manager->analysisServicePort()));
  planServiceAddressLed->setText(_manager->planServiceAddress());
  planServicePortLed->setText(QString::number(_manager->planServicePort()));
  notificationServiceAddressLed->setText(_manager->notificationServiceAddress());
  notificationServicePortLed->setText(QString::number(_manager->notificationServicePort()));
}

//! Grava a configuração do gerenciador de serviços dos endereços alteradas na interface
void ConfigDialog::saveServiceData()
{
  if (_manager!=NULL)
  {
    _manager->setWeatherDataServiceAddress(weatherDataAddressLed->text().trimmed(), weatherDataPortLed->text().toInt());
    _manager->setAnalysisServiceAddress(analysisAddressLed->text().trimmed(), analysisPortLed->text().toInt());
    _manager->setPlanServiceAddress(planServiceAddressLed->text().trimmed(), planServicePortLed->text().toInt());
    _manager->setNotificationServiceAddress(notificationServiceAddressLed->text().trimmed(), notificationServicePortLed->text().toInt());
  }
}

void ConfigDialog::saveConfig()
{
  if (_changed)
	saveServiceData();
}

void ConfigDialog::cancelChanges()
{
	_changed = false;
}

void ConfigDialog::setChanged()
{
  _changed = true;
}