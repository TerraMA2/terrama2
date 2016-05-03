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
    uint64_t             processId_;
    std::shared_ptr< te::dt::TimeInstantTZ > lastEmit_;
};

terrama2::core::Timer::Timer(const Schedule& dataSchedule, uint64_t processId, std::shared_ptr< ProcessLogger > log)
{

  impl_ = new Impl();
  impl_->dataSchedule_ = dataSchedule;
  impl_->processId_ = processId;
  impl_->lastEmit_ = log->getLastProcessTimestamp();

  prepareTimer(dataSchedule);
}

terrama2::core::Timer::~Timer()
{
  delete impl_;
}

void terrama2::core::Timer::timeoutSlot()
{
  emit timeoutSignal(impl_->processId_);

  impl_->lastEmit_ = terrama2::core::TimeUtils::now();
  prepareTimer(impl_->dataSchedule_);
}

void terrama2::core::Timer::prepareTimer(const Schedule& dataSchedule)
{
  double secondsToStart = 0;

  if(dataSchedule.frequency > 0)
  {
    double timerSeconds = frequencySeconds(dataSchedule);

    std::shared_ptr < te::dt::TimeInstantTZ > nowTZ = terrama2::core::TimeUtils::now();

    double secondsSinceLastProcess = 0;

    if(impl_->lastEmit_)
      secondsSinceLastProcess = *nowTZ.get() - *impl_->lastEmit_.get();

    secondsToStart = timerSeconds - secondsSinceLastProcess;
  }
  else if(dataSchedule.schedule > 0)
  {
    secondsToStart = scheduleSeconds(dataSchedule);
  }
  else
  {
    QString errMsg = QObject::tr("No frequency or schedule informed.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

   if(secondsToStart > 0)
   {
     // Timer with X seconds
     connect(&impl_->timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
     impl_->timer_.setInterval(secondsToStart*1000);
     int j = impl_->timer_.interval();
     impl_->timer_.start(secondsToStart*1000);
   }
   else
   {
     timeoutSlot();
   }
}

double terrama2::core::Timer::frequencySeconds(const Schedule& dataSchedule)
{
  te::common::UnitOfMeasurePtr uom = te::common::UnitsOfMeasureManager::getInstance().find(dataSchedule.frequencyUnit);

  if(!uom)
  {
    QString errMsg = QObject::tr("Invalid unit frequency.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  double secondsFrequency = dataSchedule.frequency * te::common::UnitsOfMeasureManager::getInstance().getConversion(dataSchedule.frequencyUnit,"second");

  return secondsFrequency;
}

double terrama2::core::Timer::scheduleSeconds(const Schedule& dataSchedule)
{
  te::common::UnitOfMeasurePtr uom = te::common::UnitsOfMeasureManager::getInstance().find(dataSchedule.scheduleUnit);

  if(!uom)
  {
    QString errMsg = QObject::tr("Invalid schedule unit.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  if(uom->getName() == "WEEK")
  {
    std::shared_ptr < te::dt::TimeInstantTZ > nowTZ = terrama2::core::TimeUtils::now();

    boost::gregorian::greg_weekday gw(dataSchedule.schedule);
    boost::gregorian::date d(boost::date_time::next_weekday(nowTZ->getTimeInstantTZ().date(), gw));
    boost::posix_time::ptime pt(d, boost::posix_time::hours(00));
    boost::local_time::local_date_time dt(pt, nowTZ->getTimeInstantTZ().zone());
    te::dt::TimeInstantTZ day(dt);

    std::cout << nowTZ->toString() << std::endl;
    std::cout << day.toString() << std::endl;
    std::cout << day - *nowTZ.get() << std::endl;

    int days = (day - *nowTZ.get()) / (3600 * 24);
    int hours = ((day - *nowTZ.get()) % (3600 * 24)) / 3600;
    int minutes = (((day - *nowTZ.get()) % (3600 * 24)) % 3600) / 60;
    int seconds = ((((day - *nowTZ.get()) % (3600 * 24)) % 3600) % 60);

    std::cout << days << " days " << hours <<":" <<minutes <<":" <<seconds << std::endl;

    return day - *nowTZ.get();
  }
  else if(uom->getName() == "MONTH")
  {

  }
  else if(uom->getName() == "YEAR")
  {
    std::shared_ptr < te::dt::TimeInstantTZ > nowTZ = terrama2::core::TimeUtils::now();

    if(dataSchedule.schedule > nowTZ->getTimeInstantTZ().date().day_of_year())
    {
      int days = dataSchedule.schedule - nowTZ->getTimeInstantTZ().date().day_of_year();

      std::shared_ptr< te::dt::TimeInstantTZ > nextDate(new te::dt::TimeInstantTZ(*nowTZ.get()));

      terrama2::core::TimeUtils::addDay(nextDate, days);

      boost::posix_time::ptime pt(nextDate->getTimeInstantTZ().date(), boost::posix_time::hours(00));
      boost::local_time::local_date_time dt(pt, nextDate->getTimeInstantTZ().zone());
      te::dt::TimeInstantTZ day(dt);

      std::cout << nowTZ->toString() << std::endl;
      std::cout << day.toString() << std::endl;
      std::cout << day - *nowTZ.get() << std::endl;

      int d = (day - *nowTZ.get()) / (3600 * 24);
      int hours = ((day - *nowTZ.get()) % (3600 * 24)) / 3600;
      int minutes = (((day - *nowTZ.get()) % (3600 * 24)) % 3600) / 60;
      int seconds = ((((day - *nowTZ.get()) % (3600 * 24)) % 3600) % 60);
      std::cout << d << " days " << hours <<":" <<minutes <<":" <<seconds << std::endl;

      return day - *nowTZ.get();
    }
    else if (dataSchedule.schedule < nowTZ->getTimeInstantTZ().date().day_of_year())
    {

    }
    else
    {
      return 0;
    }
  }
  else
  {
    QString errMsg = QObject::tr("Unknow schedule unit.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidFrequencyException() << terrama2::ErrorDescription(errMsg);
  }

  // VINICIUS: implement
//  assert(0);
//  throw InvalidFrequencyException() << terrama2::ErrorDescription("Not implemented");
//  return double();
  return 0.0;
}

uint64_t terrama2::core::Timer::processId() const
{
  return impl_->processId_;
}
