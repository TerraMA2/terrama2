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
  \file terrama2/collector/ParserFirePoint.hpp

  \brief Parser of occurrences of fires file

  \author Jano Simas
  \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_PARSERFIREPOINT_HPP__
#define __TERRAMA2_COLLECTOR_PARSERFIREPOINT_HPP__

// TerraMA2
#include "Parser.hpp"
#include "ParserOGR.hpp"

// STL
#include <mutex>


namespace terrama2
{
 namespace collector
 {
   /*!
        * \brief The ParserFirePoint class is responsible for dealing with the occurrence of files.
        *
        * The DataSetAdapter is used to add the semantic geometry (generates point by coordinates Lat and Long)
        * and dating the occurrence files.
        *
   */

  class ParserFirePoint: public ParserOGR
  {
   public:
    virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter) override;
    te::dt::AbstractData* StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType);
  };
 }
}


#endif // __TERRAMA2_COLLECTOR_PARSERFIREPOINT_HPP__
