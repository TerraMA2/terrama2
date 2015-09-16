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

struct terrama2::collector::DataSetTimer::Impl
{
    core::DataSetPtr dataSet_;//<! Pointer to the Dataset.
    QTimer           timer_;//<! Timer to next collection.

    std::vector<DataProcessorPtr> dataLst_;
};

terrama2::collector::DataSetTimer::DataSetTimer(terrama2::core::DataSetPtr dataSet)
{
  impl_ = new Impl();
  impl_->dataSet_ = dataSet;

  connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);

  populateDataLst();
  prepareTimer();
}

terrama2::collector::DataSetTimer::~DataSetTimer()
{
  delete impl_;
}

void terrama2::collector::DataSetTimer::timeoutSlot() const
{
  emit timerSignal(impl_->dataSet_->id());
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
  auto dataProvider = impl_->dataSet_->dataProvider();
  return CollectorFactory::getInstance().getCollector(dataProvider);
}

terrama2::core::DataSetPtr terrama2::collector::DataSetTimer::dataSet() const
{
  return impl_->dataSet_;
}

std::vector<terrama2::collector::DataProcessorPtr> terrama2::collector::DataSetTimer::data() const
{
  return impl_->dataLst_;
}
