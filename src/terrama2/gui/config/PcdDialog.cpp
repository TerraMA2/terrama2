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

  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ui_PcdDialogForm.h"
#include "PcdDialog.hpp"

// Terralib
#include <terralib/qt/widgets/srs/SRSManagerDialog.h>

// QT
#include <QDoubleValidator>

struct PcdDialog::Impl
{
  Impl()
    : srid_(0),
      ui_(new Ui::PcdDialogForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }

  uint64_t srid_;
  Ui::PcdDialogForm* ui_;
};

PcdDialog::PcdDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  pimpl_->ui_->okBtn->setEnabled(false);

  // Connect
  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), SLOT(onConfirmClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));
  connect(pimpl_->ui_->projectionBtn, SIGNAL(clicked()), SLOT(onProjectionClicked()));

  connect(pimpl_->ui_->ledColPointFileName, SIGNAL(textEdited(const QString&)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->timeZoneCmb, SIGNAL(currentIndexChanged(int)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->ledColPointLongitude, SIGNAL(textEdited(const QString&)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->ledColPointLongitude, SIGNAL(textEdited(const QString&)), SLOT(onPcdChanged()));
  connect(pimpl_->ui_->activeCheckBox, SIGNAL(stateChanged(int)), SLOT(onPcdChanged()));

  pimpl_->ui_->ledColPointLatitude->setValidator(new QDoubleValidator(pimpl_->ui_->ledColPointLatitude));
  pimpl_->ui_->ledColPointLongitude->setValidator(new QDoubleValidator(pimpl_->ui_->ledColPointLongitude));

  pimpl_->ui_->timeZoneCmb->setCurrentText("+00:00");
  pimpl_->ui_->projectionTxt->setText("0");

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
  pimpl_->ui_->activeCheckBox->setChecked(pcd.active);
  pimpl_->srid_ = pcd.srid;
  pimpl_->ui_->timeZoneCmb->setCurrentText(pcd.timezone);
  pimpl_->ui_->projectionTxt->setText(std::to_string(pcd.srid).c_str());
}

void PcdDialog::fillObject(PCD &pcd)
{
  pcd.file = pimpl_->ui_->ledColPointFileName->text();
  pcd.latitude = pimpl_->ui_->ledColPointLatitude->text();
  pcd.longitude = pimpl_->ui_->ledColPointLongitude->text();
  pcd.active = pimpl_->ui_->activeCheckBox->isChecked();
  pcd.srid = pimpl_->srid_;
  pcd.timezone = pimpl_->ui_->timeZoneCmb->currentText();

}

void PcdDialog::onPcdChanged()
{
  pimpl_->ui_->okBtn->setEnabled(true);
}

void PcdDialog::onConfirmClicked()
{
  if (pimpl_->ui_->ledColPointFileName->text().trimmed().isEmpty())
    {
      reject();
      return;
    }

  accept();
}

void PcdDialog::onProjectionClicked()
{
  te::qt::widgets::SRSManagerDialog srsDialog(this);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if (srsDialog.exec() == QDialog::Rejected)
    return;


  pimpl_->srid_ = (uint64_t) srsDialog.getSelectedSRS().first;
  pimpl_->ui_->projectionTxt->setText(std::to_string(pimpl_->srid_).c_str());

  onPcdChanged();
}
