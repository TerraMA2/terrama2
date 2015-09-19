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
  \file terrama2/gui/admin/WaitDialog.hpp

  \brief Services QT Dialog Header

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/
  
#ifndef __TERRAMA2_GUI_CORE_WAITDIALOG_HPP__
#define __TERRAMA2_GUI_CORE_WAITDIALOG_HPP__

#include "ui_WaitDialogForm.h"

//! Class responsible for wait dialog apresentation
class WaitDialog : public QDialog
{
Q_OBJECT

public:
  WaitDialog(QString msg, QWidget* parent = 0, Qt::WindowFlags f = 0 );
  ~WaitDialog();

public slots:
  //! Set message to dialog
  void setMsg(QString msg);
  
private:
  // Forward Declaration
  struct Impl;

  Impl* pimpl_; //< Pimpl idiom
};

#endif // __TERRAMA2_GUI_CORE_WAITDIALOG_HPP__

