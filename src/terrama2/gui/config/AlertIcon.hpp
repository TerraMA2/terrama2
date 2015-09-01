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
  \file terrama2/gui/config/AlertIcon.hpp

  \brief Definition of methods in class AlertIcon.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Fred
*/

#ifndef _ALERT_ICON_H_
#define _ALERT_ICON_H_

// TerraMA2
#include "soapServPlanosProxy.h"

// QT
#include <QPixmap>

class AlertIcon
{
public:
  AlertIcon(const wsImage &data);
  AlertIcon(const wsImage &data, const QPixmap &pixmap);

  //! Return the image identifier
  const int & id() const { return _data.id; }

  //! Return the pixelmap loaded from image data
  const QPixmap & pixmap() const { return _pixmap; }

  //! Return the struct that wraps the image
  const wsImage & data() const { return _data; }

private:
  QPixmap  _pixmap;
  wsImage  _data;
};

#endif // _ALERT_ICON_H_
