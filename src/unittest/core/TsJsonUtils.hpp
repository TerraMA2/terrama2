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
  \file unittest/core/TsJsonUtils.hpp

  \brief Tests for Core Json Utils

  \author Jano Simas
*/

#ifndef __TERRAMA2_UNITTEST_CORE_TSJSON_UTILS_HPP__
#define __TERRAMA2_UNITTEST_CORE_TSJSON_UTILS_HPP__


//QT
#include <QtTest/QTest>

class TsJsonUtils : public QObject
{
  Q_OBJECT

private slots:

  void OkFilter();
  void emptyDiscardDateRange();
  void emptyDiscardDate();

};

#endif // __TERRAMA2_UNITTEST_CORE_TSJSON_UTILS_HPP__
