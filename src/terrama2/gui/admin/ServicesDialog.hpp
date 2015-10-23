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
*/

#ifndef __TERRAMA2_GUI_ADMIN_SERVICESDIALOG_HPP__
#define __TERRAMA2_GUI_ADMIN_SERVICESDIALOG_HPP__  

// TerraMA2
#include "../../ws/collector/client/Client.hpp"

// QT
#include "ui_ServicesDialogForm.h"

// Boost
#include <boost/noncopyable.hpp>  

struct CommonData;
class ConfigManager;
class AdminApp;

class ServicesDialog : public QDialog, private boost::noncopyable
{
 Q_OBJECT

public:
  ServicesDialog(AdminApp *adminapp, ConfigManager&, QString nameConfig);
  ~ServicesDialog();

private slots:
  void verifyRequested();
  void saveRequested();
  void execRequested();
  void closeRequested();
  void setDataChanged(int row, int col);
  void clearDataChanged();
   
private:
  void setLine(int line, const QString& module, const CommonData& data);
  void getLine(int line, CommonData& data);
  void getSelectedLines(QList<int>& list);
  void setDialogData(QString nameConfig); 
  bool runCmd(int line, QString cmd, QString param, QString& err);

 struct Impl;
 
 Impl* pimpl_;  //!< Pimpl idiom.

 ConfigManager& configManager_; //!< Configuration Manager
 QString idNameConfig_; //!<  current identifier in config settings manager
 bool changed_; //!< Flag indicating the data have been changed
 AdminApp* adminapp_;
 terrama2::ws::collector::client::Client* client;
};


#endif // __TERRAMA2_GUI_ADMIN_SERVICESDIALOG_HPP__

