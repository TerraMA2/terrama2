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
  \file terrama2/ws/collector/server/DataSetTimer.cpp

  \brief Signals when the dataset should be collected..

  \author Jano Simas
*/

#include "DataSetTimer.hpp"
#include "DataProcessor.hpp"
#include "CollectorFactory.hpp"

#include "../../../core/DataSet.hpp"
#include "../../../core/DataProvider.hpp"

terrama2::ws::collector::server::DataSetTimer::DataSetTimer(terrama2::core::DataSetPtr dataSet)
  : dataSet_(dataSet)
{
  connect(&timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);

  populateDataLst();
  prepareTimer();
}

void terrama2::ws::collector::server::DataSetTimer::timeoutSlot() const
{
  emit timerSignal(dataSet_->id());
}

void terrama2::ws::collector::server::DataSetTimer::prepareTimer()
{
  //JANO: implementar prepareTimer
  timer_.start(10000);
}

void terrama2::ws::collector::server::DataSetTimer::populateDataLst()
{
//JANO: implementar populateDataLst
}


terrama2::ws::collector::server::CollectorPtr terrama2::ws::collector::server::DataSetTimer::getCollector() const
{
  auto dataProvider = getDataSet()->dataProvider();
  return CollectorFactory::instance().getCollector(dataProvider);
}
