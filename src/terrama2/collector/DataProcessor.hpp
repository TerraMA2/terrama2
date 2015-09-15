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

//Std
#include <memory>
#include <cstdint>
#include <string>

//Qt
#include <QObject>

//Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core {
    class Data;
    typedef std::shared_ptr<Data> DataPtr;
  }

  namespace collector
  {
    class Filter;
    typedef std::shared_ptr<Filter> FilterPtr;

    /*!
         * \brief The DataProcessor class is responsible to process and store the aquired data.
         *
         * The DataProcessor class receives an uri to the data aquired by a Collector
         * and call the appropriate processing and filtering objects. After this process
         * it is responsible for calling the appropriate storager.
         *
         */
    class DataProcessor : public QObject, public boost::noncopyable
    {
        Q_OBJECT

      public:
        //! Constructor
        DataProcessor(QObject* parent = nullptr);
        //! Destructor
        ~DataProcessor(){}

        core::DataPtr data() const;

        FilterPtr filter() const;

        /*!
             * \brief Calls the process and filtering objecs, after they finish calls the storager.
             * \param uri Uri to temporary data aquired by a collector.
             */
        void import(const std::string &uri);

    };

    typedef std::shared_ptr<DataProcessor> DataProcessorPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_DATAPROCESSOR_HPP__
