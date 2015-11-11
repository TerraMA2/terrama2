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
  \file terrama2/gui/config/IntersectionDialog.cpp

  \brief Definition of Class IntersectionDialog.hpp

  \author Raphael Willian da Costa  
*/

// TerraMA2
#include "ui_IntersectionDialogForm.h"
#include "IntersectionDialog.hpp"

// QT
#include <QMessageBox>

struct IntersectionDialog::Impl
{
  Impl()
    : ui_(new Ui::IntersectionDialogForm)
  {

  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::IntersectionDialogForm* ui_;
};

IntersectionDialog::IntersectionDialog(QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), SLOT(onOkBtnClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));
}

IntersectionDialog::~IntersectionDialog()
{
  delete pimpl_;
}

void IntersectionDialog::onOkBtnClicked()
{
  QMessageBox::information(this, tr("TerraMA2"), tr("TerraMA2 Intersection"));
}
