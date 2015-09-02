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
  \file terrama2/gui/config/ColPointDialog.hpp

  \brief Definition of methods in class ColPointDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


#ifndef _ColPointDialog_H_
#define _ColPointDialog_H_

#include "ui_ColPointDialog.h"

#include "soapServPlanosProxy.h"

class ColPointDialog : public QDialog, private Ui::ColPointDialog
{
Q_OBJECT

public:
  ColPointDialog(QWidget* parent = 0, Qt::WFlags f = 0 );
  ~ColPointDialog();

  void setFields(const wsPCD& colPoint);
  void getFields(wsPCD* colPoint, bool& changed);

private slots:
  void setColPointChanged();
  
private:
  bool _ignoreChangeEvents;
  bool _collectionPointChanged;
};


#endif

