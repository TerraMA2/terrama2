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
  \file unittest/view/TsJsonUtils.cpp

  \brief Tests for View JsonUtils class

  \author Vinicius Campanha
*/

// TerraLib
#include <terralib/se.h>
#include <terralib/maptools/GroupingAlgorithms.h>

//TerraMA2
#include "TsJsonUtils.hpp"
#include "Utils.hpp"
#include <terrama2/services/view/core/JSonUtils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>

// STD
#include <algorithm>

void TsJsonUtils::testToJSon()
{
  try
  {
    terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
    terrama2::services::view::core::ViewPtr viewPtr(view);

    view->viewName = "TestView";
    view->id = 1;
    view->projectId = 1;
    view->serviceInstanceId = 1;
    view->active = true;

    // DataSeries List
    view->dataSeriesID = 1;

    // Legend
    terrama2::services::view::core::View::Legend::Rule  rule;

    rule.title = "Class 1";
    rule.value = "1";
    rule.color = "#000000";
    rule.isDefault = true;

    terrama2::services::view::core::View::Legend* legend = new terrama2::services::view::core::View::Legend();

    legend->operation = terrama2::services::view::core::View::Legend::OperationType::VALUE;
    legend->classify = terrama2::services::view::core::View::Legend::ClassifyType::RAMP;

    legend->metadata.emplace("band_number", "0");
    legend->metadata.emplace("column", "");

    legend->rules.push_back(rule);

    view->legend.reset(legend);

    // Schedule
    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    // Filter
    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:39:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:45:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    view->filter = filter;

    QJsonObject obj = terrama2::services::view::core::toJson(viewPtr);

  }
  catch(const terrama2::Exception& e)
  {
    QString message(*boost::get_error_info<terrama2::ErrorDescription>(e));
    QFAIL(message.toStdString().c_str());
  }
  catch(boost::exception& e)
  {
    QFAIL(boost::diagnostic_information(e).c_str());
  }
  catch(std::exception& e)
  {
    QFAIL(e.what());
  }
  catch(...)
  {
    QFAIL("Unknow exception!");
  }
}

void TsJsonUtils::testGoNBackJSon()
{
  try
  {
    terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
    terrama2::services::view::core::ViewPtr viewPtr(view);

    view->viewName = "TestView";
    view->id = 1;
    view->projectId = 1;
    view->serviceInstanceId = 1;
    view->active = true;

    // DataSeries List
    view->dataSeriesID = 1;

    // Legend
    terrama2::services::view::core::View::Legend::Rule  rule;

    rule.title = "Class 1";
    rule.value = "1";
    rule.color = "#000000";
    rule.isDefault = true;

    terrama2::services::view::core::View::Legend* legend = new terrama2::services::view::core::View::Legend();

    legend->operation = terrama2::services::view::core::View::Legend::OperationType::VALUE;
    legend->classify = terrama2::services::view::core::View::Legend::ClassifyType::RAMP;

    legend->metadata.emplace("band_number", "0");
    legend->metadata.emplace("column", "");

    legend->rules.push_back(rule);

    view->legend.reset(legend);

    // Schedule
    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    // Filter
    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:39:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:45:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    view->filter = filter;

    QJsonObject obj = terrama2::services::view::core::toJson(viewPtr);

    terrama2::services::view::core::ViewPtr viewBackPtr = terrama2::services::view::core::fromViewJson(obj);

    QCOMPARE(viewBackPtr->viewName, viewPtr->viewName);
    QCOMPARE(viewBackPtr->id, viewPtr->id);
    QCOMPARE(viewBackPtr->projectId, viewPtr->projectId);
    QCOMPARE(viewBackPtr->serviceInstanceId, viewPtr->serviceInstanceId);
    QCOMPARE(viewBackPtr->active, viewPtr->active);
    QCOMPARE(viewBackPtr->imageName, viewPtr->imageName);
    QCOMPARE(viewBackPtr->imageType, viewPtr->imageType);
    QCOMPARE(viewBackPtr->imageResolutionWidth, viewPtr->imageResolutionWidth);
    QCOMPARE(viewBackPtr->imageResolutionHeight, viewPtr->imageResolutionHeight);
    QCOMPARE(viewBackPtr->srid, viewPtr->srid);

    QCOMPARE(viewBackPtr->schedule.id, viewPtr->schedule.id);
    QCOMPARE(viewBackPtr->schedule.frequency, viewPtr->schedule.frequency);
    QCOMPARE(viewBackPtr->schedule.frequencyUnit, viewPtr->schedule.frequencyUnit);

    QCOMPARE(viewBackPtr->dataSeriesID, viewPtr->dataSeriesID);

    QCOMPARE(viewBackPtr->legend->operation, viewPtr->legend->operation);
    QCOMPARE(viewBackPtr->legend->classify, viewPtr->legend->classify);

    QCOMPARE(viewBackPtr->legend->metadata.size(), viewPtr->legend->metadata.size());

    if(!std::equal(viewBackPtr->legend->metadata.begin(), viewBackPtr->legend->metadata.end(), viewPtr->legend->metadata.begin()))
      QFAIL("Fail!");

    QCOMPARE(viewBackPtr->legend->rules.size(), viewPtr->legend->rules.size());

    for(uint i = 0; i < viewBackPtr->legend->rules.size(); i++)
    {
      QCOMPARE(viewBackPtr->legend->rules.at(i).title, viewPtr->legend->rules.at(i).title);
      QCOMPARE(viewBackPtr->legend->rules.at(i).value, viewPtr->legend->rules.at(i).value);
      QCOMPARE(viewBackPtr->legend->rules.at(i).color, viewPtr->legend->rules.at(i).color);
      QCOMPARE(viewBackPtr->legend->rules.at(i).isDefault, viewPtr->legend->rules.at(i).isDefault);
    }

    // TODO: enable when convert filter/json is implemented
/*
     QCOMPARE(*viewBackPtr->filter.discardBefore, viewPtr->filter.discardBefore);
      QCOMPARE(*viewBackPtr->filter.discardAfter, viewPtr->filter.discardAfter);

*/
  }
  catch(const terrama2::Exception& e)
  {
    QString message(*boost::get_error_info<terrama2::ErrorDescription>(e));
    QFAIL(message.toStdString().c_str());
  }
  catch(boost::exception& e)
  {
    QFAIL(boost::diagnostic_information(e).c_str());
  }
  catch(std::exception& e)
  {
    QFAIL(e.what());
  }
  catch(...)
  {
    QFAIL("Unknow exception!");
  }
}
