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
#include "Logger.hpp"
#include "TimeUtils.hpp"
#include "../data-model/Process.hpp"
#include "../Exception.hpp"


struct terrama2::core::Timer::Impl
{
  Impl()
    : schedule_{0,0,0},
      process_{nullptr} {}

  Schedule             dataSchedule_;
  QTimer               timer_;//<! Timer to next collection.
  te::dt::TimeDuration schedule_;//<! Schedule to next collection.
  ProcessPtr           process_;
  std::shared_ptr< te::dt::TimeInstantTZ > lastEmit_;
};

terrama2::core::Timer::Timer(ProcessPtr process, std::shared_ptr< te::dt::TimeInstantTZ > lastEmit)
{
  impl_ = std::unique_ptr<Impl>{new Impl{} };
  impl_->dataSchedule_ = process->schedule;
  impl_->process_ = process;
  impl_->lastEmit_ = lastEmit;

  prepareTimer(impl_->dataSchedule_);
}

terrama2::core::Timer::~Timer()
{
}

void terrama2::core::Timer::timeoutSlot() noexcept
{
  try
  {
    auto now = terrama2::core::TimeUtils::nowUTC();
    emit timeoutSignal(impl_->process_, now);

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

    if(impl_->lastEmit_ || dataSchedule.frequencyStartTime.empty())
    {
      // The timer never emitted before OR don't has a time to start
      double secondsSinceLastProcess = 0.0;

      if(impl_->lastEmit_)
        secondsSinceLastProcess = *nowTZ.get() - *impl_->lastEmit_.get();

      if(secondsSinceLastProcess > timerSeconds)
      {
        secondsToStart = 0;
      }
      else
      {
        secondsToStart = timerSeconds - secondsSinceLastProcess;
      }
    }
    else
    {
      // The timer never emitted before and has a time to start

      std::stringstream ss;

      ss.exceptions(std::ios_base::failbit);
      boost::gregorian::date_facet* facet = new boost::gregorian::date_facet();
      facet->format("%Y-%m-%d");
      ss.imbue(std::locale(ss.getloc(), facet));

      ss << nowTZ->getTimeInstantTZ().date();
      ss << "T";
      ss << dataSchedule.frequencyStartTime;

      try
      {
        auto startDate = terrama2::core::TimeUtils::stringToTimestamp(ss.str(), terrama2::core::TimeUtils::webgui_timefacet);
        if(*startDate < *nowTZ)
        {
          // If the time to start has already passed, set the start time to tomorrow
          terrama2::core::TimeUtils::addDay(startDate, 1);
        }
        secondsToStart = *startDate - *nowTZ;
      }
      catch(...)
      {
        QString errMsg = QObject::tr("Invalid frequency or schedule informed.");
        TERRAMA2_LOG_WARNING() << errMsg;
        throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
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

  // Timer with X seconds
  connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
  impl_->timer_.start(secondsToStart > 0 ? secondsToStart*1000 : 1000);
}

terrama2::core::ProcessPtr terrama2::core::Timer::process() const
{
  return impl_->process_;
}
