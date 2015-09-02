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
  \file terrama2/gui/config/AlertIcon.cpp

  \brief Definition of methods in class AlertIcon.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Fred
*/

// TerraMA2
#include "AlertIcon.hpp"

// QT
#include <QPixmap>

AlertIcon::AlertIcon(const wsImage &data)
{
  _data = data;
  _pixmap.loadFromData(QVector<unsigned char>::fromStdVector(data.data).data(), data.data.size(), "PNG");
}

AlertIcon::AlertIcon(const wsImage &data, const QPixmap &pixmap)
{
  _data = data;
  _pixmap = QPixmap(pixmap);
}
