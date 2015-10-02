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

#include "TsDataFilter.hpp"

//terrama2
#include <terrama2/collector/DataFilter.hpp>

#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/Filter.hpp>

//QT
#include <QStringList>

void TsDataFilter::TestFilterNamesExact()
{
  terrama2::core::DataProviderPtr provider(new terrama2::core::DataProvider("TestProvider", terrama2::core::DataProvider::UNKNOWN_TYPE));
  terrama2::core::DataSetPtr      dataset (new terrama2::core::DataSet     (provider, "TestDataSet", terrama2::core::DataSet::UNKNOWN_TYPE));
  terrama2::core::DataSetItemPtr  dataItem(new terrama2::core::DataSetItem (dataset, terrama2::core::DataSetItem::UNKNOWN_TYPE));
  terrama2::core::FilterPtr       filter(new terrama2::core::Filter(dataItem));
  std::vector<terrama2::core::DataSetPtr>     datasets     = { dataset };
  std::vector<terrama2::core::DataSetItemPtr> datasetitems = { dataItem };
  provider->setDataSets(datasets);
  dataset->setDataSetItemList(datasetitems);
  dataItem->setFilter(filter);

  std::string exact("exact");
  dataItem->setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
  QCOMPARE(names.at(0), exact);
}

void TsDataFilter::TestEmptyMask()
{
  terrama2::core::DataProviderPtr provider(new terrama2::core::DataProvider("TestProvider", terrama2::core::DataProvider::UNKNOWN_TYPE));
  terrama2::core::DataSetPtr      dataset (new terrama2::core::DataSet     (provider, "TestDataSet", terrama2::core::DataSet::UNKNOWN_TYPE));
  terrama2::core::DataSetItemPtr  dataItem(new terrama2::core::DataSetItem (dataset, terrama2::core::DataSetItem::UNKNOWN_TYPE));
  terrama2::core::FilterPtr       filter(new terrama2::core::Filter(dataItem));
  std::vector<terrama2::core::DataSetPtr>     datasets     = { dataset };
  std::vector<terrama2::core::DataSetItemPtr> datasetitems = { dataItem };
  provider->setDataSets(datasets);
  dataset->setDataSetItemList(datasetitems);
  dataItem->setFilter(filter);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  std::vector<std::string> output = datafilter.filterNames(names);

  QCOMPARE(output, names);
}

//QTEST_MAIN(TsDataFilter)
#include "TsDataFilter.moc"
