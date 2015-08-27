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
  \file terrama2/gui/config/AlertIconList.hpp

  \brief Definition of methods in class AlertIconList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Fred
*/

#ifndef _ALERT_ICON_SET_H
#define _ALERT_ICON_SET_H

// QT
#include <QObject>

// TerraMA2
#include "AlertIcon.hpp"

class Services;
class ServPlanos;

class AlertIconList : public QObject, public QList<AlertIcon*>
{
  Q_OBJECT

public:
  AlertIconList(Services *manager, ServPlanos *service);

  bool loadData();

  bool addIcon(uchar *cdata, int size, const QPixmap & pixmap, int &newImageID);
  bool deleteIcon(int index);

  int        findIconIndex(int iconId) const;
  AlertIcon* findIcon(int iconId) const;

private:
  AlertIconList(const AlertIconList&);             //!< Remove the copy-constructor
  AlertIconList& operator=(const AlertIconList&);  //!< Remove the attrib. operator

  Services*   _manager; //! Services handler
  ServPlanos* _service; //! An reference for analysis remote service access
};

#endif // _ALERT_ICON_SET_H
