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
  \file terrama2/gui/config/FilterDialog.hpp

  \brief Definition of Class FilterDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#ifndef _FilterDialog_H_
#define _FilterDialog_H_

#include "ui_FilterDialog.h"

#include "soapH.h"

enum FilterDialogType {
	FULLFILTER = 0,
	DATEFILTER = 1,
	AREAFILTER = 2,
	NO_BANDFILTER = 3
};

class FilterDialog : public QDialog, private Ui::FilterDialog
{
Q_OBJECT

public:
  FilterDialog(FilterDialogType fType, QWidget* parent = 0, Qt::WFlags f = 0 );
  ~FilterDialog();
  void setFields(const wsWeatherDataSourceFilter& filterParams, std::vector<struct wsTheme>);
  void getFields(wsWeatherDataSourceFilter *filterParams, bool &changed);

private slots:
  void filterTypeToggled();
  void setFilterChanged();
  void enablePAThemeBox();
  void disablePAThemeBox();
  void setToday();
  
private:
	bool _filterChanged;
	bool _ignoreChangeEvents;
	void fillThemeList(std::vector<struct wsTheme>);
};


#endif

