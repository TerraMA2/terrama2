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
#include "../core/Logger.hpp"

struct terrama2::collector::DataSetTimer::Impl
{
  Impl()
    : schedule_(0,0,0) {}

    uint64_t             dataSetId_;//<! Id of the Dataset.
    QTimer               timer_;//<! Timer to next collection.
    te::dt::TimeDuration schedule_;//<! Schedule to next collection.
};

terrama2::collector::DataSetTimer::DataSetTimer(const terrama2::core::DataSet& dataSet)
{
  if(dataSet.id() == 0 || dataSet.name().empty())
  {
    QString errMsg = tr("Invalid dataset in DataSetTimer constructor.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
  }

  impl_ = new Impl();
  impl_->dataSetId_ = dataSet.id();

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

void terrama2::collector::DataSetTimer::scheduleSlot() const
{
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

  if(impl_->schedule_.getHours() == now.time_of_day().hours()
     && impl_->schedule_.getMinutes() == now.time_of_day().minutes())
  {
    emit timerSignal(impl_->dataSetId_);
  }
}

void terrama2::collector::DataSetTimer::prepareTimer(const terrama2::core::DataSet &dataSet)
{
  te::dt::TimeDuration frequency = dataSet.dataFrequency();
  int seconds = frequency.getTimeDuration().total_seconds();
  if(seconds > 0)
  {
    connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
    impl_->timer_.start(seconds*1000);
  }
  else
  {
    impl_->schedule_ = dataSet.schedule();

    long seconds = impl_->schedule_.getTimeDuration().total_seconds();
    if(seconds <= 0)
    {
      QString errMsg = QObject::tr("Invalid collect frequency.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(errMsg);
    }

    connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(scheduleSlot()), Qt::UniqueConnection);
    impl_->timer_.start(60000);//check each minute
  }

}

uint64_t terrama2::collector::DataSetTimer::dataSet() const
{
  return impl_->dataSetId_;
}

