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
  \file terrama2/collector/ParserPcdInpe.hpp

  \brief Parser of occurrences of fires file

  \author Jano Simas
  \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_PARSERPCDINPE_HPP__
#define __TERRAMA2_COLLECTOR_PARSERPCDINPE_HPP__

// TerraMA2
#include "Parser.hpp"
#include "ParserOGR.hpp"

namespace terrama2
{
 namespace collector
 {
   /*!
        * \brief The ParserPcdINPE class is responsible for making the parser file in the format PCD INPE.        
        *
   */

  class ParserPcdInpe: public ParserOGR
  {
   public:
      /*!
     * \brief The Adapt method is responsible for manually change data from PcdINPE file.
     *
     *  This first method performs remove the column "N/A" PCD INPE file, then create a new property "DateTime" type dt::DateTimeProperty
     * with value dt::TIME_INSTANT_TZ.
     * \param converter DataSetTypeConverter is the type that is a pointer to the DataSet Type that will be converted manually.
     *
     * After creating "DateTime" a search is conducted to find the correct column "N/A" PCD INPE file.
     *
     */
    virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter) override;

    //! \brief Add all columns of the \a datasetType to the \a converter
    virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) override;

      /*!
     * \brief StringToTimestamp is the function is responsible for handling the date and time fields
        * in the format timestamp PCD INPE file.
        * The StringToTimestamp function has the following parameters:
        * \param te::da::DataSet* dataset is input data set;
        * \param std::vector<std::size_t>& indexes is list with the positions of the modified property on the input data set;
        * \param int dstType is type of target data.
        * \return te::dt::AbstractData* dt is adding a new data type in the system. Ex. dateTime type timestamp.
        *
     */
    te::dt::AbstractData* StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType);
  };
 }
}


#endif // __TERRAMA2_COLLECTOR_PARSERPCDINPE_HPP__
