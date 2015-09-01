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
  \file terrama2/gui/config/PcdWFSAttrsDialog.hpp

  \brief Definition of Class PcdWFSAAttrsDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Gustavo Sampaio
*/

/*!
* 
* \brief Definicao da classe PcdWFSAttrsDialog, que representa a interface onde o
*		 usuario ajusta alguns parametros necessarios para importacao de dados
*		 de uma serie do tipo PCD WFS.
*
*/

#ifndef _PcdWFSAttrsDialog_H_
#define _PcdWFSAttrsDialog_H_

// TerraMA2
#include "ui_PcdWFSAttrsDialog.h"
#include "soapH.h"

class PcdWFSAttrsDialog : public QDialog, private Ui::PcdWFSAttrsDialog
{
	Q_OBJECT

public:
	PcdWFSAttrsDialog(std::vector<wsDBColumn> attributes, QWidget* parent = 0, Qt::WFlags f = 0);
	~PcdWFSAttrsDialog();

	void setFields(QString wfsPCDIdAttr, QString wfsPCDTimestampAttr, QString wfsPCDTimestampMask);
	void getFields(QString &wfsPCDIdAttr, QString &wfsPCDTimestampAttr, QString &wfsPCDTimestampMask);

private:
	void setComboBoxIndex(QComboBox* combo, QString value);

private slots:
	void okBtnClicked();
	void menuMaskClicked(QAction* actMenu);
};

#endif