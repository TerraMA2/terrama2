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
  \file terrama2/ws/collector/core/DataProvider.hpp

  \brief DataProvider...

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_DATAPROVIDER_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_DATAPROVIDER_HPP__

// QT
#include <QMutex>
#include <QThread>

// STL
#include <string>

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {
        class Dataset;

        /**
         * \brief Enum that contains all types of data providers.
         */
        enum DataProviderType
        {
          FTP, FILE_SYSTEM, HTTP, WCS, WFS
        };

        /**
         * \brief Abstract class that represents a server, contains methods to collect the data of a given dataset.
         */
        class DataProvider : public QThread
        {
        public:
          /**
          * \brief Constructor
          */
          DataProvider();

          /**
          * \brief Destructor
          */
          virtual ~DataProvider();

          /**
          * \brief Returns if the data provider connection is open.
          * \return True if the connection is open.
          */
          virtual bool isOpen() const;

          /**
          * \brief This method will start the thread in order to collect a new data.
          */
          void collect(terrama2::ws::collector::core::Dataset* dataset);

          /**
          * \brief For each data in the current dataset calls the method getData.
          */
          void run();

          /**
          * \brief For each data in the current dataset calls the method getData.
          */
          virtual bool open();
          virtual bool close();
          bool isAquiring();
          virtual std::string getData(terrama2::ws::collector::core::Data* data) = 0;

        protected:
          terrama2::ws::collector::core::Dataset* dataset_;
          bool isOpen_;
          QMutex mutex_;

        };
      } // core
    } // collector
  } // ws
} // terrama2


#endif // __TERRAMA2_WS_COLLECTOR_CORE_DATAPROVIDER_HPP__
