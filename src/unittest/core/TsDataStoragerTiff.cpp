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
  \file unittest/core/TsUtility.cpp

  \brief Tests for TsDatatStoragerTiff class

  \author Vinicius Campanha
*/

//TerraMA2
#include "TsDataStoragerTiff.hpp"
#include <terrama2/impl/DataStoragerTiff.cpp>
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>


// GMock
#include <gtest/gtest.h>

class DerivedDataStoragerTiff : public terrama2::core::DataStoragerTiff
{
  public:

    DerivedDataStoragerTiff(terrama2::core::DataSeriesPtr dataSeries, terrama2::core::DataProviderPtr dataProvider)
      : terrama2::core::DataStoragerTiff(dataSeries, dataProvider)
    { }

    virtual ~DerivedDataStoragerTiff() = default;

  public:

    virtual std::string replaceMask(const std::string& mask,
                            std::shared_ptr<te::dt::DateTime> timestamp,
                            terrama2::core::DataSetPtr dataSet) const
    {
      return terrama2::core::DataStoragerTiff::replaceMask(mask, timestamp, dataSet);
    }
};

terrama2::core::DataProviderPtr outputDataProvider()
{
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->uri = "file://" + TERRAMA2_DATA_DIR + "/";

  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->active = true;
  dataProvider->id = 1;
  dataProvider->name = "DataProvider queimadas local";
  dataProvider->dataProviderType = "FILE";

  return dataProviderPtr;
}

void TsDataStoragerTiff::testReplaceMask()
{
  try
  {
    DerivedDataStoragerTiff ds(std::make_shared<terrama2::core::DataSeries>(), outputDataProvider());

    terrama2::core::DataSet* dataset = new terrama2::core::DataSet();
    terrama2::core::DataSetPtr dataSetPtr(dataset);

    dataset->format.emplace("timezone", "-3");

    {
      auto dateTime = terrama2::core::TimeUtils::stringToTimestamp("2017-05-22T16:12:00.123456789MST-03", terrama2::core::TimeUtils::webgui_timefacet);

      std::string mask = ds.replaceMask("%YY%MM%DD_%hh%mm", dateTime, dataSetPtr);

      if(mask != "170522_1912")
        QFAIL("Error at replaceMask!");
    }

    {
      auto dateTime = terrama2::core::TimeUtils::stringToTimestamp("17-05-22T16:12:00.123456789MST-03", "%y-%m-%dT%H:%M:%S%F%ZP");

      std::string mask = ds.replaceMask("%YY%MM%DD_%hh%mm", dateTime, dataSetPtr);

      if(mask != "170522_1912")
        QFAIL("Error at replaceMask!");
    }
  }
  catch(...)
  {
    QFAIL("Should not be here!");
  }
}
