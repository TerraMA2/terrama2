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
         * \brief The Parser class interpret the origin format and returns a te::da::DataSet.
         *
         * This class is responsible for interpreting the data to use with te::da::DataSet,
         * if not compatible it will convert to a caompatible format.
         *
         */
    class Parser : public boost::noncopyable
    {
      public:

        /*!
             * \brief Reads the data refered in the uri and converts to a te::da::DataSet compatible format.
             * \param uri Uri to the temporary data.
             * \return Pointer to an te::da::DataSet, can be invalid.
             *
             * \exception TODO: Parser::read exception...
             */
        virtual void read(const std::string& uri,
                          DataFilterPtr filter,
                          std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                          std::shared_ptr<te::da::DataSetType> &datasetTypeVec) = 0;

    protected:
      std::mutex mutex_;
    };


    typedef std::shared_ptr<Parser> ParserPtr;
  }
}



#endif //__TERRAMA2_COLLECTOR_PARSER_HPP__
