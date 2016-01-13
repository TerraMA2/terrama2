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
  \file terrama2/gui/config/PcdWfsDialog.cpp

  \brief Definition of PcdWfsDialog for handling WFS among PCD

  \author Raphael Willian da Costa
*/


// TerraMA2
#include "PcdWfsDialog.hpp"
#include "ui_PcdWfsDialogForm.h"

// QT
#include <QMessageBox>
#include <QMenu>


struct terrama2::gui::config::PcdWfsDialog::Impl
{
  Impl()
    : ui_(new Ui::PcdWfsDialogForm)
  {

  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::PcdWfsDialogForm* ui_;
};


terrama2::gui::config::PcdWfsDialog::PcdWfsDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  connect(pimpl_->ui_->okBtn,     SIGNAL(clicked()), SLOT(onOkBtnClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  QMenu* menuMask = new QMenu(tr("Masks"), this);
  menuMask->addAction(tr("d    - day without left zero (from 1 to 31)"));
  menuMask->addAction(tr("dd   - day with left zero (01 to 31)"));
  menuMask->addAction(tr("M    - month without left zero (1 to 12)"));
  menuMask->addAction(tr("MM   - month with left zero (01 to 12)"));
  menuMask->addAction(tr("yy   - Year with only two digits"));
  menuMask->addAction(tr("yyyy - Year with four digits"));
  menuMask->addAction(tr("H    - Hour without left zero (0 to 23)"));
  menuMask->addAction(tr("HH   - Hour with left zero (00 a 23)"));
  menuMask->addAction(tr("m    - Minute without left zero (0 a 59)"));
  menuMask->addAction(tr("mm   - Minute with left zero (00 a 59)"));
  menuMask->addAction(tr("s    - Second without left zero (0 a 59)"));
  menuMask->addAction(tr("ss   - Second with left zero (00 a 59)"));

  pimpl_->ui_->wfsTimestampMaskToolBtn->setMenu(menuMask);
  pimpl_->ui_->wfsTimestampMaskToolBtn->setPopupMode(QToolButton::InstantPopup);

  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(onMenuMaskClicked(QAction*)));
}

terrama2::gui::config::PcdWfsDialog::~PcdWfsDialog()
{
  delete pimpl_;
}

void terrama2::gui::config::PcdWfsDialog::onOkBtnClicked()
{
  if(pimpl_->ui_->wfsIdAttrCmb->currentIndex() == -1)
    QMessageBox::warning(this, tr("TerraMA2 Error"), tr("There is no selected attribute ID."));
  else if(pimpl_->ui_->wfsTimestampAttrCmb->currentIndex() == -1)
    QMessageBox::warning(this, tr("TerraMA2 Error"), tr("There is no selected Date/Time attribute."));
  else if(pimpl_->ui_->wfsTimestampMaskLed->text().isEmpty())
    QMessageBox::warning(this, tr("TerraMA2 Error"), tr("It must fill a mask value."));
  else
    accept();
}

void terrama2::gui::config::PcdWfsDialog::onMenuMaskClicked(QAction* actMenu)
{
  QString selectedMaskText = actMenu->text();
  int separatorIndex = selectedMaskText.indexOf("-");
  QString maskItem = selectedMaskText.left(separatorIndex - 1).trimmed();

  pimpl_->ui_->wfsTimestampMaskLed->setText(pimpl_->ui_->wfsTimestampMaskLed->text() + maskItem);
}
