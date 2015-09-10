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
  \file terrama2/ws/collector/server/Collector.cpp

  \brief Aquire data from server.

  \author Jano Simas
*/


#include "Collector.hpp"
#include "../../../core/DataSet.hpp"

//Boost
#include <boost/log/trivial.hpp>

bool terrama2::ws::collector::server::Collector::isCollecting() const
{
  LockMutex lock(mutex_);
  if(lock.tryLock())
    return false;
  else
    return true;
}

bool terrama2::ws::collector::server::Collector::collect(const DataSetTimerPtr datasetTimer)
{
  LockMutex lock(mutex_);
  if(lock.tryLock())
  {
    //JANO: implement collect
    //thread....

    return true;
  }
  else
    return false;
}
