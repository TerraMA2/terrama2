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
  \file terrama2/gui/config/CrossDialog.hpp

  \brief Definition of Class CrossDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#ifndef _CrossDialog_H_
#define _CrossDialog_H_

#include "ui_CrossDialog.h"

class Services;
class AdditionalMapList;


class CrossDialog : public QDialog, private Ui::CrossDialog
{
Q_OBJECT

public:
  CrossDialog(QWidget* parent = 0, Qt::WFlags f = 0);
  ~CrossDialog();

  void setFields(Services* manager, std::vector<struct wsAddMapDisplayConfig> listAddMapId);
  void getFields(std::vector<struct wsAddMapDisplayConfig>& listAddMapId, bool& changed);

private slots:
  void listAddMapsRowChanged(int index);
  void addCross();
  void delCross();
  void selectAttribute();
  void deselectAttribute();
  void itemUpdate(QTableWidgetItem *item);
  void setChanged(int row = -1, int column = -1);

  
private:
  bool _ignoreChangeEvents;
  bool _changed;
  int _currentCross;

  Services* _manager;
  AdditionalMapList* _additionalMapList;
  std::vector<struct wsAddMapDisplayConfig> _listAddMapId;
  int _backgroundMapId;

  void load();
  bool selectedAddMapId(int id);
  bool findAddMapId(int id, wsAddMapDisplayConfig& admCfg);

  void fillTableAddMapAvaible();
  void fillTableAddMapSelected();

  std::vector<struct wsAddMapDisplayConfig>::iterator findItemListAddMapId(int id);

};


#endif

