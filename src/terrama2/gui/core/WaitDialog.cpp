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
  \file terrama2/gui/admin/ServicesDialog.cpp

  \brief Services QT Dialog

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

// TerraMA2
#include "WaitDialog.hpp"

// STL
#include <assert.h>

struct WaitDialog::Impl
{
  Impl() : ui_(new Ui::WaitDialogForm)
  {

  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::WaitDialogForm* ui_;
};


//! Constructor
WaitDialog::WaitDialog(QString msg, QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new WaitDialog::Impl)
{
  pimpl_->ui_->setupUi(this);
  setMsg(msg);
  show();
}

//! Destructor
WaitDialog::~WaitDialog()
{
  hide();
}

//! Slot to set a message in dialog
void WaitDialog::setMsg(QString msg)
{
  pimpl_->ui_->msgLbl->setText(msg);
}
