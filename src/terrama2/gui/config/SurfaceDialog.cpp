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
  \file terrama2/gui/config/SurfaceDialog.cpp

  \brief Definition of Class SurfaceDialog.cpp

  \author Raphael Willian da Costa  
  
*/

// TerraMA2
#include "ui_SurfaceDialogForm.h"
#include "SurfaceDialog.hpp"

// QT
#include <QMenu>

struct SurfaceDialog::Impl
{
  Impl()
    : ui_(new Ui::SurfaceDialogForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::SurfaceDialogForm* ui_;
};

SurfaceDialog::SurfaceDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  //disable all widget
  for(auto widget: pimpl_->ui_->groupBox->findChildren<QWidget*>())
    widget->setEnabled(false);

  pimpl_->ui_->interpCmb->addItem(tr("Vizinho mais próximo"));
  pimpl_->ui_->interpCmb->addItem(tr("K-vizinhos mais próximos"));
  pimpl_->ui_->interpCmb->addItem(tr("K-vizinhos mais próximos ponderados"));

  QMenu* menuMask = new QMenu(tr("Máscaras"), this);
  menuMask->addAction(tr("%a - ano com dois digitos"));
  menuMask->addAction(tr("%A - ano com quatro digitos"));
  menuMask->addAction(tr("%d - dia com dois digitos"));
  menuMask->addAction(tr("%M - mes com dois digitos"));
  menuMask->addAction(tr("%h - hora com dois digitos"));
  menuMask->addAction(tr("%m - minuto com dois digitos"));
  menuMask->addAction(tr("%s - segundo com dois digitos"));

  pimpl_->ui_->fileAnaGridMaskBtn->setMenu(menuMask);
  pimpl_->ui_->fileAnaGridMaskBtn->setPopupMode(QToolButton::InstantPopup);

  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(onMenuMaskClicked(QAction*)));
  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), SLOT(accept()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));

}

SurfaceDialog::~SurfaceDialog()
{
  delete pimpl_;
}

void SurfaceDialog::onMenuMaskClicked(QAction* action)
{
  pimpl_->ui_->outputGridEdit->setText(pimpl_->ui_->outputGridEdit->text() + action->text().left(2));
}
