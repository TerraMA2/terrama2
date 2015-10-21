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
  \file terrama2/collector/DataProcessor.hpp

  \brief Manages the processing, filtering and storaging of aquired data.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_DATAPROCESSOR_HPP__
#define __TERRAMA2_COLLECTOR_DATAPROCESSOR_HPP__

//TerraMA2
#include "../core/DataSetItem.hpp"

//Std
#include <memory>
#include <cstdint>
#include <string>

//Boost
#include <boost/noncopyable.hpp>

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
    class Factory;
    class DataFilter;
    typedef std::shared_ptr<DataFilter> DataFilterPtr;
    class Parser;
    typedef std::shared_ptr<Parser> ParserPtr;
    class Storager;
    typedef std::shared_ptr<Storager> StoragerPtr;

    /*!
         * \brief The DataProcessor class is responsible to process and store the aquired data.
         *
         * The DataProcessor class receives an uri to the data aquired by a Collector
         * and call the appropriate processing and filtering objects. After this process
         * it is responsible for calling the appropriate storager.
         *
         */
    class DataProcessor : public boost::noncopyable
    {
      public:
        //! Constructor
        DataProcessor(const core::DataSetItem& data);
        //! Destructor
        ~DataProcessor();

        /*!
         * \brief Data object being processed by this processor.
         * \return Shared pointer to the Data object.
         */
        core::DataSetItem data() const;

        /*!
         * \brief Filtering rules for the data.
         *
         * The filter object is able to filter file names to match the Data mask
         * and filter the data with according rules. [Filter]{\ref terrama2::collector::Filter}
         *
         * \pre The usage of this function assumes that the filtering rules are already set.
         *
         * \return Shared pointer to a filter object.
         */
        DataFilterPtr filter() const;

        /*!
             * \brief Calls the process and filtering objecs, after they finish calls the storager.
             * \param uri Uri to temporary data aquired by a collector.
             */
        void import(const std::string &uri);

      private:
        void initFilter();
        void initParser();
        void initStorager();


        struct Impl;
        Impl* impl_;
    };

    typedef std::shared_ptr<DataProcessor> DataProcessorPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_DATAPROCESSOR_HPP__
