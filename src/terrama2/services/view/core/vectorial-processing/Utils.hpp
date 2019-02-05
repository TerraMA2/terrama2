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


#ifndef __TERRAMA2_SERVICES_VIEW_CORE_VECTOR_PROCESSING_UTILS_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_VECTOR_PROCESSING_UTILS_HPP__

// TerraMA2
#include "../View.hpp"

// TerraLib
#include <terralib/core/uri/URI.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// STL
#include <memory>
#include <string>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        namespace vp
        {
          std::vector<std::string> getIntersectionTables(te::da::DataSourceTransactor* transactor, const std::string& analysisTableNameResult);

          std::string prepareSQLIntersection(const std::vector<std::string>& listOfIntersectionTables,
                                             const std::string& monitoredTableName);

          std::unique_ptr<View::Legend> generateVectorProcessingLegend(const std::vector<std::string>& listOfIntersectionTables);
        }
      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_VIEW_CORE_VECTOR_PROCESSING_UTILS_HPP__
