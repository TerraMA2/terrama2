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
  \file terrama2/collector/Parser.hpp

  \brief Parsers data and create a terralib DataSet.

  \author Jano Simas
*/

#ifndef __TERRAMA2_COLLECTOR_PARSER_HPP__
#define __TERRAMA2_COLLECTOR_PARSER_HPP__

//Terralib
#include "terralib/dataaccess/dataset/DataSet.h"
#include "terralib/dataaccess/dataset/DataSetType.h"

//Boost
#include <boost/noncopyable.hpp>

//std
#include <mutex>

namespace te
{
  namespace da
  {
    class DataSource;
  }
}

namespace terrama2
{
  namespace collector
  {
    class DataFilter;
    typedef std::shared_ptr<DataFilter> DataFilterPtr;
    /*!
          \brief The Parser class interprets the origin format and returns a te::da::DataSet.

          This class is responsible for interpreting the data to use with te::da::DataSet,
          if not compatible it will convert to a compatible format.

         */
    class Parser : public boost::noncopyable
    {
    public:

      /*!
       \brief Reads the data refered in the uri and converts to a te::da::DataSet compatible format.

       If the datatype is known the parser will identify the data and convert to better data types.
       ex: date string to Date format and lat long strings to point geometry.

       \param uri URI to the data.
       \param filter Used to identify the file/table names to be collected.
       \param datasetVec Will be populated with parsed datasets.
       \param datasetType DataSetType of found datasets

       \exception UnableToReadDataSetError Raised if could not open the dataset, read error message for more information.
       \pre Tarralib should be initialized.
       */

      virtual void read(const std::string& uri,
                        DataFilterPtr filter,
                        std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                        std::shared_ptr<te::da::DataSetType> &datasetType) = 0;

    protected:
      std::mutex mutex_;//TODO: needed?
    };


    typedef std::shared_ptr<Parser> ParserPtr;
  }
}



#endif //__TERRAMA2_COLLECTOR_PARSER_HPP__
