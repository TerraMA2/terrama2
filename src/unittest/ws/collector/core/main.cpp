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
  \file terrama2/unittest/ws/core/main.cpp

  \brief Main class for tests in the WebService Core class.

  \author Vinicius Campanha
*/

// TerraMA2 Test
#include "TestCore.hpp"
#include "Utils.hpp"

int main(int argc, char **argv)
{
  initializeTerraMA2();

  TestCore testCore;
  int ret = QTest::qExec(&testCore, argc, argv);

  finalizeTerraMA2();

  return ret;
}
