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
  \file terrama2/collector/TsFilter.hpp

  \brief Tests for the TestFilter class.

  \author Jano Simas
*/

#include "TsFilter.hpp"

//terrama2
#include <terrama2/collector/Filter.hpp>

//QT
#include <QStringList>

void TsFilter::TestFilterNamesExact()
{
  std::string exact("exact");
  terrama2::collector::Filter filter;
  filter.setMask(exact);

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  names = filter.filterNames(names);

  QVERIFY(names.size() == 1);
  QCOMPARE(names.at(0), exact);
}

void TsFilter::TestEmptyMask()
{
  terrama2::collector::Filter filter;

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  std::vector<std::string> output = filter.filterNames(names);

  QCOMPARE(output, names);
}

//QTEST_MAIN(TsFilter)
#include "TsFilter.moc"
