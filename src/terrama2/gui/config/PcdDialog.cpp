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
#include "../../core/DataSetItem.hpp"

// Terralib
#include <terralib/qt/widgets/srs/SRSManagerDialog.h>

// QT
#include <QDoubleValidator>


struct terrama2::gui::config::PcdDialog::Impl
{
  Impl()
    :
      ui_(new Ui::PcdDialogForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }

  terrama2::core::DataSetItem pcdItem_;
  Ui::PcdDialogForm* ui_;
};

terrama2::gui::config::PcdDialog::PcdDialog(QWidget* parent, Qt::WindowFlags f)
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

terrama2::gui::config::PcdDialog::~PcdDialog()
{
  delete pimpl_;
}

void terrama2::gui::config::PcdDialog::fill(const terrama2::core::DataSetItem& pcdItem)
{
  pimpl_->pcdItem_ = pcdItem;

  auto metadata = pcdItem.metadata();

  pimpl_->ui_->ledColPointFileName->setText(pcdItem.mask().c_str());
  pimpl_->ui_->ledColPointLatitude->setText(metadata["LATITUDE"].c_str());
  pimpl_->ui_->ledColPointLongitude->setText(metadata["LONGITUDE"].c_str());
  pimpl_->ui_->activeCheckBox->setChecked(pcdItem.status() == terrama2::core::DataSetItem::ACTIVE);
  pimpl_->ui_->projectionTxt->setText(std::to_string(pcdItem.srid()).c_str());
  pimpl_->ui_->timeZoneCmb->setCurrentText(pcdItem.timezone().c_str());
}

terrama2::core::DataSetItem terrama2::gui::config::PcdDialog::getDataSetItem() const
{
  return pimpl_->pcdItem_;
}

void terrama2::gui::config::PcdDialog::onPcdChanged()
{
  pimpl_->ui_->okBtn->setEnabled(true);
}

void terrama2::gui::config::PcdDialog::onConfirmClicked()
{
  if (pimpl_->ui_->ledColPointFileName->text().trimmed().isEmpty())
  {
    reject();
    return;
  }

  auto metadata = pimpl_->pcdItem_.metadata();
  pimpl_->pcdItem_.setMask(pimpl_->ui_->ledColPointFileName->text().toStdString());
  metadata["LATITUDE"] = pimpl_->ui_->ledColPointLatitude->text().toStdString();
  metadata["LONGITUDE"] = pimpl_->ui_->ledColPointLongitude->text().toStdString();
  pimpl_->pcdItem_.setMetadata(metadata);

  pimpl_->pcdItem_.setStatus(pimpl_->ui_->activeCheckBox->isChecked() == true ? terrama2::core::DataSetItem::ACTIVE : terrama2::core::DataSetItem::INACTIVE);
  pimpl_->pcdItem_.setSrid(pimpl_->ui_->projectionTxt->text().toInt());
  pimpl_->pcdItem_.setTimezone(pimpl_->ui_->timeZoneCmb->currentText().toStdString());

  accept();
}

void terrama2::gui::config::PcdDialog::onProjectionClicked()
{
  te::qt::widgets::SRSManagerDialog srsDialog(this);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if (srsDialog.exec() == QDialog::Rejected)
    return;


  pimpl_->pcdItem_.setSrid((uint64_t) srsDialog.getSelectedSRS().first);
  pimpl_->ui_->projectionTxt->setText(std::to_string((uint64_t) srsDialog.getSelectedSRS().first).c_str());

  onPcdChanged();
}
