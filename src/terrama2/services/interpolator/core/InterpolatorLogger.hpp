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
  \file interpolator/core/InterpolatorLogger.hpp

  \brief

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_COLLECTORLOG_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_COLLECTORLOG_HPP__

#include "../core/Typedef.hpp"
#include "../../../core/utility/ProcessLogger.hpp"

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        class InterpolatorLogger : public terrama2::core::ProcessLogger
        {
          Q_OBJECT
        public:


          InterpolatorLogger();

          virtual ~InterpolatorLogger() {}

          void addInput(std::string value, RegisterId registerID);

          void addOutput(std::string value, RegisterId registerID);

          virtual void setConnectionInfo(const te::core::URI& uri) override;

          virtual std::shared_ptr<ProcessLogger> clone() const override;

        };
      }
    }
  }
}
#endif // __TERRAMA2_SERVICES_INTERPOLATOR_CORE_COLLECTORLOG_HPP__
