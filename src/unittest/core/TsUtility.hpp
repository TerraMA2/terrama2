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
  \file unittest/core/TsUtility.hpp

  \brief Tests for Core Utility class

  \author Vinicius Campanha
*/

//TerraMA2


//QT
#include <QtTest/QTest>


class TsUtility : public QObject
{
  Q_OBJECT

public:


private slots:

  void testTimerNoFrequencyException();
  void testTimerInvalidUnitException();
  void testFrequencyTimer();
  void testFrequencyTimerBase();
  void testFrequencyTimerFirstExecutionEarly();
  void testFrequencyTimerFirstExecutionLater();
  void testScheduleTimer();

  void testTimeUtilsAddMonth();
  void testTimeUtilsAddDay();
  void testTimeUtilsAddYear();
  void testTimeUtilsScheduleSeconds();

  void ValidMask();
  void invalidMask();
  void testTerramaMask2Regex();
  void testTerramaMaskMatch();

  void testValidDataSetName();
  void testValidDataSetNameCompress();
  void testValidDataSetNameCompressError();
  void testValidDataSetName2DigitsYear();
  void testValidDataSetName2DigitsYear1900();
  void testIgnoreArchiveExtension();
  void testSpecialCharacters();
};
