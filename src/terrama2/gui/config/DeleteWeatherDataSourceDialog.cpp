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
  \file terrama2/gui/config/DeleteWeatherDataSourceDialog.cpp

  \brief Definition of Class DeleteWeatherDataSourceDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/


// TerraMA2
#include "DeleteWeatherDataSourceDialog.hpp"

//! Construtor
DeleteWeatherDataSourceDialog::DeleteWeatherDataSourceDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
	setupUi(this);

	connect(yesBtn,     SIGNAL(clicked()), SLOT(accept()));
	connect(noBtn, SIGNAL(clicked()), SLOT(reject()));
}

//! Destrutor
DeleteWeatherDataSourceDialog::~DeleteWeatherDataSourceDialog()
{
}

bool DeleteWeatherDataSourceDialog::getDeleteLayersCbxValue()
{
	return deleteLayersCbx->isChecked();
}

void DeleteWeatherDataSourceDialog::setQuestionLabel(QString question)
{
	questionLabel->setText(question);
}