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
#include "Log.hpp"

/*!
     * \brief The ParserOGR class is responsible for dealing with the OGR files.
     *
     * The DataSetAdapter is responsible for creating a new daset without primary key.
     *
     *
*/

namespace terrama2
{
  namespace collector
  {

    /*!
       \brief The ParserOGR class is a generic Parser for OGR files.

       It can parse CSV and shapefiles, specific formated data must be processed in derived classes.

     */
    class ParserOGR : public Parser
    {
      public:
      //! Default constructor
        ParserOGR() : Parser(){}
        //! Default destructor
        virtual ~ParserOGR(){}

        /**
            \brief \copybrief Parser::read()
            */
        virtual void read(DataFilterPtr filter,
                          std::vector<terrama2::collector::TransferenceData>& transferenceDataVec) override;

    protected:
        /*!
           \brief Creates a te::da::DataSetTypeConverter that allow changes in the structure of the te::da::DataSet.

           Removes the column FID from the dataset.

           \param datasetType Original te::da::DataSetType.
           \return Shared pointer to a te::da::DataSetTypeConverter
         */
        virtual std::shared_ptr<te::da::DataSetTypeConverter> getConverter(const std::shared_ptr<te::da::DataSetType>& datasetType);

        /*!
           \brief Adapt the columns and data format based on the specific semantic of the data.

           Does nothing, must be overridden.
         */
        virtual void adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter){ }
        //! \brief Add all columns of the \a datasetType to the \a converter
        virtual void addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType);       
    };
  }
}



#endif //__TERRAMA2_COLLECTOR_PARSEROGR_HPP__
