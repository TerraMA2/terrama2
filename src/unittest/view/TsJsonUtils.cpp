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
    view->imageName = "ImageName";
    view->imageType = te::map::ImageType(2);
    view->imageResolutionWidth = 800;
    view->imageResolutionHeight = 600;
    view->srid = 4326;

    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:39:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:45:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    view->filtersPerDataSeries.emplace(1, filter);
    view->filtersPerDataSeries.emplace(2, filter);
    view->filtersPerDataSeries.emplace(3, filter);
    view->filtersPerDataSeries.emplace(4, filter);

    view->dataSeriesList.push_back(1);
    view->dataSeriesList.push_back(2);
    view->dataSeriesList.push_back(3);
    view->dataSeriesList.push_back(4);

    view->stylesPerDataSeries.emplace(2, "style");

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
    view->imageName = "ImageName";
    view->imageType = te::map::ImageType(2);
    view->imageResolutionWidth = 800;
    view->imageResolutionHeight = 600;
    view->srid = 4326;

    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:39:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:45:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    view->dataSeriesList.push_back(1);

    view->stylesPerDataSeries.emplace(1, "style");

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

    QCOMPARE(viewBackPtr->dataSeriesList.size(), viewPtr->dataSeriesList.size());

    for(uint32_t i = 0; i < viewPtr->dataSeriesList.size(); i++)
    {
      QCOMPARE(viewBackPtr->dataSeriesList.at(i), viewPtr->dataSeriesList.at(i));
    }

    QCOMPARE(viewBackPtr->filtersPerDataSeries.size(), viewPtr->filtersPerDataSeries.size());

    // TODO: enable when convert filter/json is implemented
/*    for(auto& it : viewPtr->filtersPerDataSeries)
    {
      QCOMPARE(*viewBackPtr->filtersPerDataSeries.at(it.first).discardBefore, *it.second.discardBefore);
      QCOMPARE(*viewBackPtr->filtersPerDataSeries.at(it.first).discardAfter, *it.second.discardAfter);
    }
*/
    QCOMPARE(viewBackPtr->stylesPerDataSeries.size(), viewPtr->stylesPerDataSeries.size());

    for(auto& it : viewPtr->stylesPerDataSeries)
    {
      QCOMPARE(viewBackPtr->stylesPerDataSeries.at(it.first), it.second);
    }
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
