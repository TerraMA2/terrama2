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
  \file terrama2/collector/DataSetTimer.cpp

  \brief Signals when the dataset should be collected..

  \author Jano Simas
*/

#include "DataSetTimer.hpp"
#include "DataProcessor.hpp"
#include "CollectorFactory.hpp"

#include "../core/DataSet.hpp"
#include "../core/DataProvider.hpp"

terrama2::collector::DataSetTimer::DataSetTimer(terrama2::core::DataSetPtr dataSet)
  : dataSet_(dataSet)
{
  connect(&timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);

  populateDataLst();
  prepareTimer();
}

void terrama2::collector::DataSetTimer::timeoutSlot() const
{
  emit timerSignal(dataSet_->id());
}

void terrama2::collector::DataSetTimer::prepareTimer()
{
  //JANO: implementar prepareTimer para schedule
//  timer_.start(dataSet_->dataFrequency());
}

void terrama2::collector::DataSetTimer::populateDataLst()
{
//JANO: implementar populateDataLst
}


terrama2::collector::CollectorPtr terrama2::collector::DataSetTimer::collector() const
{
  auto dataProvider = dataSet()->dataProvider();
  return CollectorFactory::instance().getCollector(dataProvider);
}

terrama2::core::DataSetPtr terrama2::collector::DataSetTimer::dataSet() const
{
  return dataSet_;
}

std::vector<terrama2::collector::DataProcessorPtr> terrama2::collector::DataSetTimer::data() const
{
  return dataLst_;
}
