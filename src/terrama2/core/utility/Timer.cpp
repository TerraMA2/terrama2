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
  \file terrama2/core/utility/Timer.cpp

  \brief

  \author Jano Simas
*/

#include "Timer.hpp"

struct terrama2::core::Timer::Impl
{
  Impl()
    : schedule_(0,0,0) {}

    Schedule             dataSchedule_;
    QTimer               timer_;//<! Timer to next collection.
    te::dt::TimeDuration schedule_;//<! Schedule to next collection.
    uint64_t             processId_;
};

terrama2::core::Timer::Timer(const Schedule& dataSchedule, uint64_t processId)
{

  impl_ = new Impl();
  impl_->dataSchedule_ = dataSchedule;
  impl_->processId_ = processId;

  prepareTimer(dataSchedule);
}

terrama2::core::Timer::~Timer()
{
  delete impl_;
}

void terrama2::core::Timer::timeoutSlot() const
{
  emit timerSignal(impl_->processId_);
}

void terrama2::core::Timer::scheduleSlot() const
{
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

  if(impl_->schedule_.getHours() == now.time_of_day().hours()
     && impl_->schedule_.getMinutes() == now.time_of_day().minutes())
  {
    emit timerSignal(impl_->processId_);
  }
}

void terrama2::core::Timer::prepareTimer(const Schedule& dataSchedule)
{
  //FIXME: implement prepareTimer, needs unit conversion
  // te::dt::TimeDuration frequency = dataSchedule.dataFrequency();
  // int seconds = frequency.getTimeDuration().total_seconds();
  // if(seconds > 0)
  // {
  //   connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
  //   impl_->timer_.start(seconds*1000);
  // }
  // else
  // {
  //   impl_->schedule_ = dataSchedule.schedule();
  //
  //   long seconds = impl_->schedule_.getTimeDuration().total_seconds();
  //   if(seconds <= 0)
  //   {
  //     QString errMsg = QObject::tr("Invalid collect frequency.");
  //     TERRAMA2_LOG_ERROR() << errMsg;
  //     throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(errMsg);
  //   }
  //
  //   connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(scheduleSlot()), Qt::UniqueConnection);
  //   impl_->timer_.start(60000);//check each minute
  // }
}

uint64_t terrama2::core::Timer::processId() const
{
  return impl_->processId_;
}
