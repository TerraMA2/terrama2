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
  \file terrama2/gui/config/PcdDialog.cpp

  \brief Class responsible to handle the PCD insertion/modification file

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/

// TerraMA2
#include "ui_PcdDialogForm.h"
#include "PcdDialog.hpp"

// QT
#include <QDoubleValidator>

struct PcdDialog::Impl
{
  Impl()
    : ui_(new Ui::PcdDialogForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::PcdDialogForm* ui_;
};

PcdDialog::PcdDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  pimpl_->ui_->okBtn->setEnabled(false);

  // Connect
  connect(pimpl_->ui_->okBtn,     SIGNAL(clicked()), SLOT(onConfirmClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(pimpl_->ui_->ledColPointFileName,   SIGNAL(textEdited(const QString&)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->ledColPointLatitude,   SIGNAL(textEdited(const QString&)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->ledColPointLongitude,  SIGNAL(textEdited(const QString&)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->activeCmb, SIGNAL(currentIndexChanged(const QString&)), SLOT(onPcdChanged()));

  pimpl_->ui_->ledColPointLatitude->setValidator(new QDoubleValidator(pimpl_->ui_->ledColPointLatitude));
  pimpl_->ui_->ledColPointLongitude->setValidator(new QDoubleValidator(pimpl_->ui_->ledColPointLongitude));
}

PcdDialog::~PcdDialog()
{
  delete pimpl_;
}

void PcdDialog::fill(const PCD& pcd)
{
  pimpl_->ui_->ledColPointFileName->setText(pcd.file);
  pimpl_->ui_->ledColPointLatitude->setText(pcd.latitude);
  pimpl_->ui_->ledColPointLongitude->setText(pcd.longitude);
  pimpl_->ui_->activeCmb->setCurrentIndex(pcd.active ? 0 : 1);
}

void PcdDialog::fillObject(PCD &pcd)
{
  pcd.file = pimpl_->ui_->ledColPointFileName->text();
  pcd.latitude = pimpl_->ui_->ledColPointLatitude->text();
  pcd.longitude = pimpl_->ui_->ledColPointLongitude->text();
  pcd.active = pimpl_->ui_->activeCmb->currentIndex() == 0;
}

void PcdDialog::onPcdChanged()
{
  pimpl_->ui_->okBtn->setEnabled(true);
}

void PcdDialog::onConfirmClicked()
{
  if (pimpl_->ui_->ledColPointFileName->text().trimmed().isEmpty())
    reject();
  accept();
}
