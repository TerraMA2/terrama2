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
  \file terrama2/collector/IntersectionOperation.hpp

  \brief Instersects the collected data with the intersection data configured in the dataset, in order to add more information to the collected data.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_COLLECTOR_INTERSECTIONOPERATION_HPP__
#define __TERRAMA2_COLLECTOR_INTERSECTIONOPERATION_HPP__


// STL
#include <memory>
#include <string>
#include <vector>

namespace te
{
  namespace da
  {
    class DataSet;
    class DataSetType;
  }

  namespace dt
  {
    class Property;
  }
}

namespace terrama2
{
  namespace core
  {
    class DataSet;
  }

  namespace services
  {
    namespace collector
    {
      /*
      void processIntersection(TransferenceData& transferenceData);

      te::da::DataSetType* createDataSetType(std::string newName,
                                             te::da::DataSetType* firstDt,
                                             std::vector<te::dt::Property*> firstProps,
                                             te::da::DataSetType* secondDt,
                                             std::vector<te::dt::Property*> secondProps);*/
    }
  }
}

#endif //__TERRAMA2_COLLECTOR_INTERSECTIONOPERATION_HPP__
