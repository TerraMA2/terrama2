/************************************************************************************
TerraMA² is a development platform of Monitoring, Analysis and Alert for
geoenvironmental extremes. Applied to natural disasters, quality of water / air / atmosphere,
tailings basins, epidemics and others.Copyright © 2012 INPE.

This code is part of the TerraMA² framework.
This framework is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose.
The library provided hereunder is on an "as is" basis, and the authors have no
obligation to provide maintenance, support, updates, enhancements,
or modifications.

In no event shall INPE or K2 SISTEMAS be held liable to any party for direct, indirect,
special, incidental, or consequential damages arising out of the use of this
library and its documentation.
*************************************************************************************/

#ifndef _ServicesDialog_H_
#define _ServicesDialog_H_

#include "ui_ServicesDialog.h"

#include "configData.h"
#include "configDataManager.h"

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

