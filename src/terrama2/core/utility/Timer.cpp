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

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

//Terralib
#include <terralib/common/UnitsOfMeasureManager.h>

// TerraMA2
#include "Timer.hpp"
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
    ProcessId             processId_;
    std::shared_ptr< te::dt::TimeInstantTZ > lastEmit_;
};

terrama2::core::Timer::Timer(const Schedule& dataSchedule, ProcessId processId, std::shared_ptr< te::dt::TimeInstantTZ > lastEmit)
{

  impl_ = new Impl();
  impl_->dataSchedule_ = dataSchedule;
  impl_->processId_ = processId;
  impl_->lastEmit_ = lastEmit;

  prepareTimer(dataSchedule);
}

terrama2::core::Timer::~Timer()
{
  delete impl_;
}

void terrama2::core::Timer::timeoutSlot() noexcept
{
  try
  {
    auto now = terrama2::core::TimeUtils::nowUTC();
    emit timeoutSignal(impl_->processId_, now);

    impl_->lastEmit_ = now;
    prepareTimer(impl_->dataSchedule_);
  }
  catch (...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::core::Timer::prepareTimer(const Schedule& dataSchedule)
{
  int64_t secondsToStart = 0;

  if(dataSchedule.frequency > 0)
  {
    double timerSeconds = terrama2::core::TimeUtils::frequencySeconds(dataSchedule);

    std::shared_ptr < te::dt::TimeInstantTZ > nowTZ = terrama2::core::TimeUtils::nowUTC();

    if(dataSchedule.frequencyStartTime.empty())
    {
      double secondsSinceLastProcess = 0;

      if(impl_->lastEmit_)
        secondsSinceLastProcess = *nowTZ.get() - *impl_->lastEmit_.get();

      secondsToStart = timerSeconds - secondsSinceLastProcess;
    }
    else
    {
      auto startDate = terrama2::core::TimeUtils::stringToTimestamp(dataSchedule.frequencyStartTime, terrama2::core::TimeUtils::webgui_timefacet);
      if(startDate->getTimeInstantTZ().date() <= nowTZ->getTimeInstantTZ().date())
      {
        auto now = nowTZ->getTimeInstantTZ().time_of_day();
        auto startTime = startDate->getTimeInstantTZ().time_of_day();

        while(startTime < now)
          startTime += boost::posix_time::seconds(timerSeconds);

        auto td = (startTime - now);
        secondsToStart = td.total_seconds();
      }
      else
      {
        secondsToStart = *startDate - *nowTZ;
      }

    }
  }

  else if(dataSchedule.schedule > 0)
  {
    secondsToStart = terrama2::core::TimeUtils::scheduleSeconds(dataSchedule, terrama2::core::TimeUtils::nowUTC());
  }
  else
  {
    QString errMsg = QObject::tr("No frequency or schedule informed.");
    TERRAMA2_LOG_WARNING() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

   if(secondsToStart > 0)
   {
     // Timer with X seconds
     connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
     impl_->timer_.start(secondsToStart*1000);
   }
   else
   {
     timeoutSlot();
   }
}

ProcessId terrama2::core::Timer::processId() const
{
  return impl_->processId_;
}
