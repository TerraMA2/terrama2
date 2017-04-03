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
  \file terrama2/services/alert/core/Utils.hpp

  \brief Utility funtions for core classes.

  \author Vinicius Campanha
 */


// TerraMA2
#include "Utils.hpp"
#include "../../../core/utility/Utils.hpp"

std::string terrama2::services::alert::core::validPropertyDateName(const std::shared_ptr<te::dt::DateTime> dt)
{
  std::stringstream ss;
  boost::local_time::local_time_facet* oFacet(new boost::local_time::local_time_facet("%Y%m%d %H%M%S %z"));
  ss.imbue(std::locale(ss.getloc(), oFacet));
  auto dateTimeTZ = std::dynamic_pointer_cast<te::dt::TimeInstantTZ>(dt);
  ss << dateTimeTZ->getTimeInstantTZ();

  return terrama2::core::createValidPropertyName(ss.str());
}
