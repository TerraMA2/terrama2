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
  \file terrama2/gui/admin/ServicesDialog.hpp

  \brief Services QT Dialog Header

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _ServicesDialog_H_
#define _ServicesDialog_H_

// QT
#include "ui_ServicesDialog.hpp"

// TerraMA2
#include "ConfigData.hpp"
#include "ConfigDataManager.hpp"

class ServicesDialog : public QDialog, private Ui::ServicesDialog
{
Q_OBJECT

public:
  ServicesDialog(ConfigDataManager& configManager, int id, QWidget* parent = 0, Qt::WFlags f = 0 );
  ~ServicesDialog();

private slots:
  void saveRequested();
  void execRequested();
  void verifyRequested();
  void closeRequested(); 
  void setDataChanged(int row, int col);
  void clearDataChanged();
    
private:
  void setLine(int line, const QString& module, const ConfigData::CommonData& data);
  void getLine(int line, ConfigData::CommonData& data);
  void getSelectedLines(QList<int>& list);

  void setDialogData(const ConfigData& config);
  void getDialogData(ConfigData& config);
  
  bool runCmd(int line, QString cmd, QString param, QString& err);
  
  ConfigDataManager& _configManager; //!< Gerenciador de configurações
  int                _id;            //!< identificador da cfg atual no gerenciador de configurações
  bool               _changed;       //!< Flag indicando se os dados foram ou não alterados
};


#endif

