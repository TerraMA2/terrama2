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
  \file terrama2/gui/config/AlertIconSetDialog.cpp

  \brief Definition of methods in class AlertIconSetDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Fred

*/

// TerraMA2
#include "AlertIconSetDialog.hpp"
#include "AlertIconList.hpp"
#include "Services.hpp"

// QT
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>

AlertIconSetDialog::AlertIconSetDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f),
  _parent(parent), _changed(false)
{
  setupUi(this);

  connect(addBtn,    SIGNAL(clicked()), SLOT(addImage()));
  connect(removeBtn, SIGNAL(clicked()), SLOT(removeImage()));
  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));

  listWidget->setItemDelegate(new ListDelegate(listWidget));
}


//! Dialog initializer
void AlertIconSetDialog::setFields(AlertIconList *alertIconList)
{
  listWidget->clear();
  foreach(AlertIcon *icon, (QList<AlertIcon *>) *alertIconList)
  {
    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setData(Qt::DecorationRole, icon->pixmap());
    listWidget->addItem(item);
  }

  _alertIconList = alertIconList;
}

//! Retrieve dialog output
void AlertIconSetDialog::getFields(bool& changed)
{
  changed = _changed;
}


//! It is a slot triggered on add button to the image set
void AlertIconSetDialog::addImage()
{
  QStringList files = QFileDialog::getOpenFileNames(this, tr("Selecione as imagens"),
                                                    ".", tr("Imagens (*.png)"));

  QStringList::Iterator it = files.begin();

  QMessageBox::StandardButton answer = QMessageBox::No;

  while(it != files.end())
  {
    QPixmap pixmap = QPixmap(*it, "PNG");
    if(pixmap.size().width() > maxAlertIconSize || pixmap.size().height() > maxAlertIconSize)
    {

      if(answer != QMessageBox::YesToAll && answer != QMessageBox::NoToAll)
      {
        answer =
            QMessageBox::question(NULL, QObject::tr("Redimensionar a imagem"),
                                  QObject::tr("O tamanho da imagem %1 ultrapassa o recomendado, "
                                              "deseja redimensiona-la?").arg(QFileInfo(*it).fileName()),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll,
                                  QMessageBox::Yes);

      }

      if(answer == QMessageBox::Yes || answer == QMessageBox::YesToAll)
        pixmap = pixmap.scaled(maxAlertIconSize, maxAlertIconSize, Qt::KeepAspectRatio);
    }

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    int dummy;
    if(!_alertIconList->addIcon((uchar *)bytes.data(), bytes.size(), pixmap, dummy))
      return;

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setData(Qt::DecorationRole, pixmap);
    listWidget->addItem(item);

    ++it;
  }

  _changed = files.size() > 0;
}

//! It is a slot triggered on remove button of image set
void AlertIconSetDialog::removeImage()
{
  QList<QListWidgetItem *> selection = listWidget->selectedItems();

  foreach(QListWidgetItem *item, selection)
  {
    int row = listWidget->row(item);

    if(!_alertIconList->deleteIcon(row))
      return;

    delete listWidget->takeItem(row);
  }

  _changed = selection.size() > 0;
}

