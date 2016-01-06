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
  \file terrama2/collector/ParserOGR.hpp

  \brief Parsers OGR data and create a terralib DataSet.

  \author Jano Simas
  \author Evandro Delatin
*/

#ifndef __TERRAMA2_COLLECTOR_PARSEROGR_HPP__
#define __TERRAMA2_COLLECTOR_PARSEROGR_HPP__

// STL
#include <mutex>

// TerraLib
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>

// TerraMA2
#include "Parser.hpp"


namespace terrama2
{
  namespace collector
  {
    /*!
         * \brief The ParserOGR class is responsible for dealing with the OGR files.
         *
         * The DataSetAdapter is responsible for creating a new daset without primary key.
         *
         *
    */

    class ParserOGR : public Parser
    {
      public:
        ParserOGR() : Parser(){}
        virtual ~ParserOGR(){}

        virtual void read(const terrama2::core::DataSetItem& datasetitem,
                          const std::string& uri,
                          DataFilterPtr filter,
                          std::vector<std::shared_ptr<te::da::DataSet> >& datasetVec,
                          std::shared_ptr<te::da::DataSetType>& datasetType) override;

        virtual std::shared_ptr<te::da::DataSetTypeConverter> getConverter(const std::shared_ptr<te::da::DataSetType>& datasetType);

        virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter){ }
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType);       
    };
  }
}



#endif //__TERRAMA2_COLLECTOR_PARSEROGR_HPP__
