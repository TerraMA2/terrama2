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
  \file terrama2/gui/config/AddListDialog.cpp

  \brief Definition of methods in class AddListDialog

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Raphael Meloni
*/

// TerraMA2
#include "AddListDialog.hpp"

// STL
#include <assert.h>

/*! \brief Construtor

\param parent   Widget Parent Dialog
\param f        Flags for build Dialog
\
*/
AddListDialog::AddListDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  // Preenche lista de grids
  listWidget->clear();

  // Bind the signals
  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));
}

//! Destructor
AddListDialog::~AddListDialog()
{
}

bool AddListDialog::setFields(std::vector<wsNotificationFilter> notifications, AdditionalMapList* addList )
{
  notificationList.clear();

  for (int i = 0; i < addList->size(); i++)
  {
    AdditionalMap* addMap = addList->at(i);
    if (addMap->theme().geometry == WS_LAYERGEOM_POLYGONS)
    {
      bool include = true;
      for (int j = 0; j < notifications.size(); j++)
      {
        if (notifications[j].additionalMapId == addMap->id())
        {
          include = false;
          break;
        }
      }

      if (!include)
        continue;

      wsNotificationFilter wsnot;
      wsnot.additionalMapId = addMap->id();
      wsnot.additionalMapName = addMap->name().toStdString();

      notificationList.push_back(wsnot);
    }
  }

  fillListNotification();

  return notificationList.size() > 0;
}

void AddListDialog::getFields( std::vector<wsNotificationFilter>& notifications, bool& changed )
{
  for (int i = 0; i < listWidget->count(); i++)
  {
    QListWidgetItem* item = listWidget->item(i);
    if (item->isSelected())
    {
      int id = item->data(Qt::UserRole).toInt();
      for (int j = 0; j < notificationList.size(); j++)
      {
        if (notificationList[j].additionalMapId == id)
        {
          changed = true;
          notifications.push_back(notificationList[j]);
          break;
        }
      }
    }
  }
}

void AddListDialog::fillListNotification()
{
  listWidget->clear();
  for (size_t i = 0; i < notificationList.size(); i++)
  {
    QString ico = ":/data/icons/additional_layer.png";
    wsNotificationFilter wsnot = notificationList.at(i);
    QListWidgetItem* item = new QListWidgetItem(QIcon(ico), QString::fromStdString(wsnot.additionalMapName));
    item->setData(Qt::UserRole, wsnot.additionalMapId);
    
    listWidget->addItem(item);
  }
}
