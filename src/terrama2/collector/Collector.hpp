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
  \file terrama2/collector/Collector.hpp

  \brief Aquire data from server.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_COLLECTOR_HPP__
#define __TERRAMA2_COLLECTOR_COLLECTOR_HPP__

#include "../core/DataProvider.hpp"
#include "DataSetTimer.hpp"
#include "DataProcessor.hpp"

//Qt
#include <QObject>

//std
#include <mutex>
#include <thread>

//Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core {
    class DataSet;
    class Data;
  }

  namespace collector
  {


    /*!
         * \brief The Collector class is responsible for aquiring the data from a remote server.
         *
         * This class is an interface to open a connection with a [DataProvider](\ref terrama2::core::DataProvider)
         * and collect the data from a DataSetTimer (see [DataSet](\ref terrama2::core::DataSet)).
         *
         */
    class Collector : public QObject, public boost::noncopyable
    {
        Q_OBJECT

      public:
        /*!
             * \brief Constructor
             * \param dataProvider Server information for collecting.
             */
        Collector(const core::DataProviderPtr dataProvider, QObject* parent = nullptr);
        /*!
             * \brief Destructor
             */
        virtual ~Collector();

        /*!
             * \brief Type of the data provider.
             *
             * This information is used by the CollectorFactory to build a Collector
             * of right type.
             *
             * \return Data provider kind.
             */
        core::DataProvider::Kind kind() const { return dataProvider_->kind();}
        /*!
             * \brief Data provider containing the information of this collector.
             */
        core::DataProviderPtr dataProvider() const { return dataProvider_;}

        /*!
             * \brief Verifies if the collector is collecting.
             * \return Returns true if is collecting.
             */
        bool isCollecting() const;
        /*!
             * \brief Prepare and start to collect the data required by the [DataSet](\ref terrama2::core::DataSet).
             *
             * If the colelctor is avaiable will call collectAsThread in a new thread and return true.
             *
             * \return Return true if able to start collecting, false otherwise.
             *
             * \exception terrama2::collector::InvalidDataSetException Raise when called with an invalid dataset.
             * \exception terrama2::collector::InactiveDataSetException Raise when called with an inactive dataset.
             * \exception terrama2::collector::UnabletoGetLockException Raise when the processes is unable to get lock. Collector already collecting?
             */
        void collect(const DataSetTimerPtr datasetTimer);

        //! \brief Returns if the connection is open.
        virtual bool isOpen() const = 0;

        /*!
             * \brief Open the connection with the server.
             *
             * Trys to open the connection, returns false if fails
             *
             * \return True if the connection is open. If not appliable, returns true.
             */
        virtual bool open()  = 0;

        //! \brief Close the connection, if not open, does nothing.
        virtual void close() = 0;

      protected:
        //! \brief Aquired the data specified in dataProcessor.
        virtual void getData(const DataProcessorPtr dataProcessor) = 0;

        core::DataProviderPtr dataProvider_; //!< Data provider information.

      private:
        //! \brief Internal method to collect a dataset, should be started as a thread.
        void collectAsThread(const DataSetTimerPtr datasetTimer);

        mutable std::mutex mutex_; //!< Mutex for thread safety.
        std::thread        collectingThread_; //!< Thread for collecting.
    };

    typedef std::shared_ptr<Collector> CollectorPtr;
  }
}



#endif //__TERRAMA2_COLLECTOR_COLLECTOR_HPP__
