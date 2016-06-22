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
  \file terrama2/services/analysis/Analysis.hpp

  \brief This file contains include headers for the TerraMA2 analysis service module.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_ANALYSIS_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_HPP__

// TerraMA2
#include "core/Shared.hpp"
#include "core/Typedef.hpp"
#include "core/Analysis.hpp"
#include "core/AnalysisExceutor.hpp"
#include "core/AnalysisLogger.hpp"
#include "core/BufferMemory.hpp"
#include "core/Context.hpp"
#include "core/DataManager.hpp"
#include "core/Excpetion.hpp"
#include "core/JSonUtils.hpp"
#include "core/OperatorCache.hpp"
#include "core/PythonInterpreter.hpp"
#include "core/Service.hpp"
#include "core/Utils.hpp"
#include "dcp/Operator.hpp"
#include "dcp/history/Operator.hpp"
#include "occurrence/Operator.hpp"
#include "occurrence/aggregation/Operator.hpp"

namespace terrama2
{
  /*! \brief Namespace for the TerraMA2 services module. */
  namespace services
  {
    /*! \brief Namespace for the TerraMA2 analysis service module. */
    namespace analysis
    {
      /*! \brief Namespace for the TerraMA2 analysis service core classes. */
      namespace core
      {

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif  // __TERRAMA2_SERVICES_ANALYSIS_HPP__
