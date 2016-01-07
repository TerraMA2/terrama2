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
#include "Exception.hpp"
#include "Factory.hpp"

#include "../core/DataProvider.hpp"
#include "../core/DataManager.hpp"
#include "../core/DataSet.hpp"

struct terrama2::collector::DataSetTimer::Impl
{
    uint64_t         dataSetId_;//<! Id of the Dataset.
    QTimer           timer_;//<! Timer to next collection.
};

terrama2::collector::DataSetTimer::DataSetTimer(const terrama2::core::DataSet& dataSet)
{
  if(dataSet.id() == 0 || dataSet.name().empty())
  {
    throw InvalidDataSetException() << terrama2::ErrorDescription(tr("Invalid dataset in DataSetTimer constructor."));
  }

  impl_ = new Impl();
  impl_->dataSetId_ = dataSet.id();

  connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);

  prepareTimer(dataSet);
}

terrama2::collector::DataSetTimer::~DataSetTimer()
{
  delete impl_;
}

void terrama2::collector::DataSetTimer::timeoutSlot() const
{
  emit timerSignal(impl_->dataSetId_);
}

void terrama2::collector::DataSetTimer::prepareTimer(const terrama2::core::DataSet &dataSet)
{
  //JANO: implementar prepareTimer para schedule
  te::dt::TimeDuration frequency = dataSet.dataFrequency();
  long seconds = frequency.getTimeDuration().total_seconds();
  if(seconds > 0)
    impl_->timer_.start(seconds*1000);
  else
  {
    throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(QObject::tr("Invalid collect frequency."));
  }

}

uint64_t terrama2::collector::DataSetTimer::dataSet() const
{
  return impl_->dataSetId_;
}

