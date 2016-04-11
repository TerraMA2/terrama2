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

//Terralib
#include <terralib/common/UnitsOfMeasureManager.h>

// TerraMA2
#include "Timer.hpp"
#include "../Exception.hpp"
#include "../utility/Logger.hpp"


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
  enum
  {
    UOM_second =  1040
  };

  // Base of Time measure: second
  te::common::UnitOfMeasurePtr uomSecond(new te::common::UnitOfMeasure(UOM_second,"second", "s", te::common::MeasureType::Time));
  std::vector<std::string> secondAlternativeNames {"s", "sec", "ss", "seconds"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomSecond, secondAlternativeNames);

  te::common::UnitOfMeasurePtr uomMinute(new te::common::UnitOfMeasure(1, "minute", "min", te::common::MeasureType::Time, UOM_second, 60.0, 0.0, 0.0, 1.0));
  std::vector<std::string> minuteAlternativeNames {"min", "minutes"};

  te::common::UnitOfMeasurePtr uomHour(new te::common::UnitOfMeasure(2, "hour", "h", te::common::MeasureType::Time, UOM_second, 3600.0, 0.0, 0.0, 1.0));
  std::vector<std::string> hourAlternativeNames {"hh", "h", "hours"};

  te::common::UnitOfMeasurePtr uomDay(new te::common::UnitOfMeasure(3, "day", "d", te::common::MeasureType::Time, UOM_second, 86400.0, 0.0, 0.0, 1.0));
  std::vector<std::string> dayAlternativeNames {"d", "dd", "days"};

  te::common::UnitsOfMeasureManager::getInstance().insert(uomMinute, minuteAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomHour, hourAlternativeNames);
  te::common::UnitsOfMeasureManager::getInstance().insert(uomDay, dayAlternativeNames);

  te::common::UnitOfMeasurePtr uom = te::common::UnitsOfMeasureManager::getInstance().find(dataSchedule.frequencyUnit);

  if(!uom)
  {
    QString errMsg = QObject::tr("Invalid unit frequency.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  double seconds = dataSchedule.frequency * te::common::UnitsOfMeasureManager::getInstance().getConversion(dataSchedule.frequencyUnit,"second");

  if(seconds > 0)
  {
    connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
    impl_->timer_.start(seconds*1000);
  }
  else
  {
    QString errMsg = QObject::tr("Invalid collect frequency.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidCollectFrequencyException() << terrama2::ErrorDescription(errMsg);
  }
}

uint64_t terrama2::core::Timer::processId() const
{
  return impl_->processId_;
}
