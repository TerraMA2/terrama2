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
  \file terrama2/gui/config/GridConfigDialog.hpp

  \brief Definition of Class GridConfigDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#ifndef _GridConfigDialog_H_
#define _GridConfigDialog_H_

#include <soapH.h>

#include "ui_GridConfigDialog.h"

class WeatherGridList;
class Services;

class GridConfigDialog : public QDialog, private Ui::GridConfigDialog
{
  Q_OBJECT
  
public:
	GridConfigDialog(QWidget *parent = 0, Qt::WFlags f = 0);
  ~GridConfigDialog();
  
	void setFields(Services *manager, const wsGridOutputConfig &config);
	void getFields(struct wsGridOutputConfig & config, bool& changed);

private slots:
	void resBaseChangeRequest(int index);
	void roiBaseChangeRequest(int index);

	void checkAndAccept();

private:
	struct wsGridOutputConfig _startConfig;
};

#endif
