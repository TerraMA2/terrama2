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
  \author Vinicius Campanha
*/

//Terralib
#include <terralib/common/UnitsOfMeasureManager.h>

// TerraMA2
#include "Timer.hpp"
#include "ProcessLog.hpp"
#include "../Exception.hpp"
#include "../utility/Logger.hpp"
#include "../utility/TimeUtils.hpp"


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
  if(dataSchedule.frequency <= 0)
  {
    QString errMsg = QObject::tr("Invalid frequency.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  te::common::UnitOfMeasurePtr uom = te::common::UnitsOfMeasureManager::getInstance().find(dataSchedule.frequencyUnit);

  if(!uom)
  {
    QString errMsg = QObject::tr("Invalid unit frequency.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  double secondsFrequency = dataSchedule.frequency * te::common::UnitsOfMeasureManager::getInstance().getConversion(dataSchedule.frequencyUnit,"second");

  std::shared_ptr < te::dt::TimeInstantTZ > lastProcess = terrama2::core::ProcessLog::getLastProcessDate(processId());
  std::shared_ptr < te::dt::TimeInstantTZ > nowTZ = terrama2::core::TimeUtils::now();

  double secondsSinceLastProcess = 0;

  if(lastProcess)
    secondsSinceLastProcess = *nowTZ.get() - *lastProcess.get();

  double seconds = secondsFrequency - secondsSinceLastProcess;

   if(seconds > 0)
   {
     // Timer with X seconds
     connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
     impl_->timer_.start(seconds*1000);
   }
   else
   {
     // Timer with one second
     connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
     impl_->timer_.start(1*1000);
   }
}

uint64_t terrama2::core::Timer::processId() const
{
  return impl_->processId_;
}
