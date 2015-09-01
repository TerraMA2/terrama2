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
  \file terrama2/gui/config/WizardDialog.hpp

  \brief Definition WizardDialog.hpp class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/


#ifndef _WIZARDDLG_H_
#define _WIZARDDLG_H_

#include <QList>

#include "ui_wizardDlg.h"

class RiskMap;
class WeatherGrid;

/*! \brief Class responsible for dialog actions of assistant to create the analyse role
*/
class WizardDlg : public QDialog, private Ui::WizardDlg
{
Q_OBJECT

public:
  WizardDlg(const RiskMap* riskmap, const QList<WeatherGrid*>& sellist, QWidget* parent = 0, Qt::WFlags f = 0 );
  ~WizardDlg();

  QString script();

private slots:
  void okRequested();
  void addRuleRequested();
  void removeRuleRequested();
  void cpcGridChanged(int index);
  void cpcPCDChanged(int index);

private:
  QString _script;
  QList<WeatherGrid*> _sellist;

  void generateGenericScript();
  bool generateCPCScript();
};

#endif

