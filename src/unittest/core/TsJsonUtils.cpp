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

#include "TsJsonUtils.hpp"

#include <terrama2/core/utility/JSonUtils.hpp>


#include <QJsonDocument>


void TsJsonUtils::OkFilter()
{
  std::string filterJson =  "{"
                              "\"class\": \"Filter\","
                              "\"frequency\": null,"
                              "\"frequency_unit\": null,"
                              "\"discard_before\": \"2017-07-17T09:00:00.000-03:00\","
                              "\"discard_after\": \"2017-07-25T09:05:00.000-03:00\","
                              "\"region\": null,"
                              "\"by_value\": null,"
                              "\"crop_raster\": null,"
                              "\"collector_id\": null,"
                              "\"data_series_id\": null"
                            "}";

  try
  {
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(filterJson.c_str(), &error);

    if(error.error != QJsonParseError::NoError)
      QFAIL("Error parsing filter json.");

    try
    {
      terrama2::core::fromFilterJson(jsonDoc.object(), nullptr);
    }
    catch (...)
    {
      QFAIL("Unexpected exception.");
    }

  }
  catch (...)
  {
    QFAIL("Unexpected exception.");
  }
}

void TsJsonUtils::emptyDiscardDateRange()
{
  std::string filterJson =  "{"
                              "\"class\": \"Filter\","
                              "\"frequency\": null,"
                              "\"frequency_unit\": null,"
                              "\"discard_before\": \"2017-07-25T09:05:00.000-03:00\","
                              "\"discard_after\": \"2017-07-17T09:00:00.000-03:00\","
                              "\"region\": null,"
                              "\"by_value\": null,"
                              "\"crop_raster\": null,"
                              "\"collector_id\": null,"
                              "\"data_series_id\": null"
                            "}";

  try
  {
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(filterJson.c_str(), &error);

    if(error.error != QJsonParseError::NoError)
      QFAIL("Error parsing filter json.");

    try
    {
      terrama2::core::fromFilterJson(jsonDoc.object(), nullptr);
      QFAIL("Exception expected.");
    }
    catch (const terrama2::core::JSonParserException&)
    {

    }
    catch (...)
    {
      QFAIL("Unexpected exception.");
    }

  }
  catch (...)
  {
    QFAIL("Unexpected exception.");
  }
}
