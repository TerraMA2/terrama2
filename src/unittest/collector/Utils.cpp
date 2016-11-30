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
  \file unittest/collector/TestUtils.cpp

  \brief Utility functions to initialize e finalize terralib and TerraMA2 for tests.

  \author Paulo R. M. Oliveira, Jano Simas
*/


//terralib
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>

// QT
#include <QTest>
#include <QJsonDocument>
#include <QDebug>

// STL
#include <string>


const char* NO_EXCEPTION_THROWN = "No exception thrown";

const char* NO_EXCEPTION_EXPECTED = "No exception expected";

const char* WRONG_TYPE_EXCEPTION = "Wrong exception type";

const char* UNEXPECTED_BEHAVIOR = "Should not be here";

const char* CANT_ACCESS_DATA = "Can't access data";
