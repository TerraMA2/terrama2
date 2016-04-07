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
  \file terrama2/collector/AnalysisTimer.hpp

  \brief Signals when the dataset should be collected.

  \author Paulo R. M. Oliveira
*/

#include "AnalysisTimer.hpp"
#include "Analysis.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"

#include <terralib/datatype/TimeDuration.h>

struct terrama2::services::analysis::core::AnalysisTimer::Impl
{
  Impl()
    : schedule(0, 0, 0)
  {}

  AnalysisId analysisId; //<! Id of the Analysis.
  QTimer timer; //<! Timer to next collection.
  te::dt::TimeDuration schedule; //<! Schedule to next collection.
};

terrama2::services::analysis::core::AnalysisTimer::AnalysisTimer(const terrama2::services::analysis::core::Analysis& analysis)
{
  if(analysis.id == 0)
  {
    QString errMsg = tr("Invalid analysis in AnalysisTimer constructor.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw InvalidParameterException() << terrama2::ErrorDescription(errMsg);
  }

  impl_ = new Impl();
  impl_->analysisId = analysis.id;

  prepareTimer(analysis);
}

terrama2::services::analysis::core::AnalysisTimer::~AnalysisTimer()
{
  delete impl_;
}

uint64_t terrama2::services::analysis::core::AnalysisTimer::analysis() const
{
  return impl_->analysisId;
}

void terrama2::services::analysis::core::AnalysisTimer::timeoutSlot() const
{
  emit timerSignal(impl_->analysisId);
}

void terrama2::services::analysis::core::AnalysisTimer::scheduleSlot() const
{
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

  if(impl_->schedule.getHours() == now.time_of_day().hours()
     && impl_->schedule.getMinutes() == now.time_of_day().minutes())
  {
    emit timerSignal(impl_->analysisId);
  }
}

void terrama2::services::analysis::core::AnalysisTimer::prepareTimer(const terrama2::services::analysis::core::Analysis& analysis)
{
  terrama2::core::Schedule schedule = analysis.schedule;
  long frequency = schedule.frequency;
  // switch (schedule.frequencyUnit)
  // {
  //   case terrama2::core::SECOND:
  //     break;
  //   case terrama2::core::MINUTE:
  //     frequency *= 60;
  //     break;
  //   case terrama2::core::HOUR:
  //     frequency *= 60 * 60;
  //     break;
  //   case terrama2::core::DAY:
  //     frequency *= 60 * 60 * 24;
  //     break;
  //   case terrama2::core::WEEK:
  //     frequency *= 60 * 60 * 24 * 7;
  //     break;
  //   case terrama2::core::MONTH:
  //     frequency *= 60 * 60 * 24 * 30;
  //     break;
  // }

  connect(&impl_->timer, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
  impl_->timer.start(frequency*1000);
}
