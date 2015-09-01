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
  \file terrama2/gui/config/CrossAttrDialog.cpp

  \brief Definition of Class CrossAttrDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

// TerraMA2
#include "CrossAttrDialog.hpp"

// STL
#include <algorithm>

CrossAttrDialog::CrossAttrDialog(QWidget* parent, Qt::WFlags f) : QDialog(parent, f)
{
  setupUi(this);

  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(selectBtn,   SIGNAL(clicked()), SLOT(selectAttribute()));
  connect(deselectBtn, SIGNAL(clicked()), SLOT(deselectAttribute()));

  connect(lwAdditionalMapsAttributes,         SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(selectAttribute()));
  connect(lwAdditionalMapsAttributesSelected, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(deselectAttribute()));
}

//! Destrutor
CrossAttrDialog::~CrossAttrDialog()
{
}

//! Inicializa dialogo
void CrossAttrDialog::setFields(struct wsAddMapDisplayConfig admCfgAdd)
{
  setWindowTitle(QString::fromStdString(admCfgAdd.addMap.name));

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
  lwAdditionalMapsAttributes->setEnabled(lwAdditionalMapsAttributes->count()>0);
  lwAdditionalMapsAttributesSelected->setEnabled(lwAdditionalMapsAttributesSelected->count()>0);

  _changed = false;
}

//! Retorna resultado do dialogo
void CrossAttrDialog::getFields(struct wsAddMapDisplayConfig &admCfgAdd, bool &changed)
{
  admCfgAdd.attributes.clear();

  // Atributos selecionados
  for(int i=0; i<lwAdditionalMapsAttributesSelected->count(); i++)
    admCfgAdd.attributes.push_back(lwAdditionalMapsAttributesSelected->item(i)->text().toStdString());

  changed = _changed;
}

void CrossAttrDialog::selectAttribute()
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

  _changed = true;
}

void CrossAttrDialog::deselectAttribute()
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

  _changed = true;
}
