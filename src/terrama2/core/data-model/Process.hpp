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
  \file terrama2/core/data-model/Process.hpp

  \brief Base struct for process (collector, analysis...)

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_PROCESS_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_PROCESS_HPP__

#include "../Typedef.hpp"
#include "Schedule.hpp"

namespace terrama2
{
  namespace core
  {
    struct Process
    {
      ProcessId id = terrama2::core::InvalidId();//!< Unique identification.
      ServiceInstanceId serviceInstanceId = terrama2::core::InvalidId();//!< Collector service instace where the process should be executed.
      bool active = true;//!< Flag if the process is active.
      ProjectId projectId = 0;//!< Identification of the project owner of the process.

      Schedule schedule;//!< terrama2::core::Schedule of execution of the process.
    };
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATA_MODEL_PROCESS_HPP__
