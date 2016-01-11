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
  \file terrama2/collector/ParserPcdToa5.hpp

  \brief Parser of PCD TOA5 file

  \author Jano Simas
  \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_PARSERPCDTOA5_HPP__
#define __TERRAMA2_COLLECTOR_PARSERPCDTOA5_HPP__

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
        * \brief The ParserPcdToa5 class is responsible for making the parser file in the format TOA5.
        *        
   */

  class ParserPcdToa5: public ParserOGR
  {
   public:
      /*!
        * \brief The Adapt method is responsible for manually change data from TOA5 file. This method first performs the remove
        * of the column "TIMESTAMP" of TOA5 file, then creates a new column or property "DateTime" type dt::DateTimeProperty
        * with value dt::TIME_INSTANT_TZ.
        *
        * \param convert DataSetTypeConverter is the type that is a pointer to the DataSetType that will be converted manually.
        *
        * After creating "DateTime" method is used "add" to add the new property in the dataset.
        * The add method has the following parameters:
        * \param PropertyPos is the position of the property will be converted;
        * \param DtProperty is converted property;
        * \param StringToTimestamp is the function that will be used to make the conversion of the property values.
        *       
        * \note DataSetType A note can be used to describe a set of data. It lists the attributes of the data set,
        * including their names and types.
        *
        */
    virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter) override;

      /*!
     * \brief StringToTimestamp is the function is responsible for handling the date and time fields
        * in the format timestamp TOA5 file.
        * The StringToTimestamp function has the following parameters:
        * \param te::da::DataSet* dataset is input data set;
        * \param std::vector<std::size_t>& indexes is list with the positions of the modified property on the input data set;
        * \param int dstType is type of target data.
        * \return te::dt::AbstractData* dt is adding a new data type in the system. Ex dateTime type timestamp.
        *
     */
    te::dt::AbstractData* StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType);
  };
 }
}


#endif // __TERRAMA2_COLLECTOR_PARSERPCDTOA5_HPP__
