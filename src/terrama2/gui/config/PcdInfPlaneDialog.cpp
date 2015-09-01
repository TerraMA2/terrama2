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
  \file terrama2/gui/config/PcdInfPlaneDialog.cpp

  \brief Definition of methods in class PcdInfPlaneDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

// TerraMA2
#include "PcdInfPlaneDialog.hpp"
#include "Utils.hpp"

PcdInfPlaneDialog::PcdInfPlaneDialog(std::vector<struct wsPCDTheme> pcdThemes, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
	setupUi(this);

	okBtn->setEnabled(false);
	
	_pcdThemes = pcdThemes;
	fillPCDThemeCmb();

	connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
	connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));
	
	connect(themeCmb, SIGNAL(currentIndexChanged(int)), SLOT(pcdThemeChanged(int)));
}

PcdInfPlaneDialog::~PcdInfPlaneDialog()
{
}

/*! Preenche combobox com os temas disponíveis para informar localização das PCDs
*/
void PcdInfPlaneDialog::fillPCDThemeCmb()
{
	themeCmb->clear();	
	for(unsigned i=0; i<_pcdThemes.size(); i++)
		themeCmb->addItem(QString::fromStdString(_pcdThemes.at(i).name));
	
	if(themeCmb->count() > 0)
	{
		themeCmb->setCurrentIndex(0);
		pcdThemeChanged(0);
		okBtn->setEnabled(true);
	}
}

//! Slot chamado quando o tema selecionado para uma PCD mudou. Popula lista de atributos do tema
void PcdInfPlaneDialog::pcdThemeChanged(int index)
{
	attributeCmb->clear();
	
	struct wsPCDTheme& themeData = _pcdThemes.at(index);
    for(unsigned i=0; i<themeData.attributes.size(); i++)
    {
		QString name = QString::fromStdString(themeData.attributes[i].name + "  (" + Utils::columnTypeToString(themeData.attributes[i].type).toStdString() + ")");
	    attributeCmb->addItem(name);
	}  

	attributeCmb->setCurrentIndex(0);
}

void PcdInfPlaneDialog::getFields(wsPCDInfPlaneAttributes *pcdInfPlaneAttributes)
{
	if(themeCmb->count() > 0)
	{
		wsPCDTheme currentPCDTheme = _pcdThemes.at(themeCmb->currentIndex());

		pcdInfPlaneAttributes->themeID = currentPCDTheme.id;
		pcdInfPlaneAttributes->attributeName = currentPCDTheme.attributes.at(attributeCmb->currentIndex()).name;
		pcdInfPlaneAttributes->mask = maskLed->text().trimmed().toStdString();
	}
	else
		pcdInfPlaneAttributes->themeID = -1;
}
