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
  \file terrama2/core/utility/Verify.cpp

  \brief Utility functions for easy consistency check

  \author Jano Simas
*/

#include "Verify.hpp"
#include "Logger.hpp"

#include "../Exception.hpp"

#include <QObject>
#include <QString>


//TODO: improve message to help identify where the error accurred

void terrama2::core::verify::srid(int srid_, bool log)
{
  if(srid_ <= 0 || srid_ > 998999)
  {
    QString errMsg = QObject::tr("Invalid SRID.");
    if(log)
      TERRAMA2_LOG_ERROR() << errMsg;
    throw VerifyException() << terrama2::ErrorDescription(errMsg);
  }
}

void terrama2::core::verify::date(const std::shared_ptr<te::dt::TimeInstant>& date)
{
  if(!date.get())
  {
    QString errMsg = QObject::tr("Invalid Date/Time.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw VerifyException() << terrama2::ErrorDescription(errMsg);
  }

  verify::date(date->getTimeInstant());
}

void terrama2::core::verify::date(const std::shared_ptr<te::dt::TimeInstantTZ>& date)
{
  if(!date.get())
  {
    QString errMsg = QObject::tr("Invalid Date/Time.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw VerifyException() << terrama2::ErrorDescription(errMsg);
  }

  verify::date(date->getTimeInstantTZ());
}

void terrama2::core::verify::date(const boost::local_time::local_date_time& date)
{
  QString errMsg = QObject::tr("Invalid Timezone.");
  if(!date.zone())
  {
    TERRAMA2_LOG_ERROR() << errMsg;
    throw VerifyException() << terrama2::ErrorDescription(errMsg);
  }

  verify::date(static_cast<const boost::posix_time::ptime::base_time&>(date));
}

void terrama2::core::verify::date(const boost::posix_time::ptime::base_time& date)
{
  QString errMsg = QObject::tr("Invalid Date/Time.");
  if(date.is_special())
  {
    TERRAMA2_LOG_ERROR() << errMsg;
    throw VerifyException() << terrama2::ErrorDescription(errMsg);
  }
}
