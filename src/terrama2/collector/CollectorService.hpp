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
  \file terrama2/collector/CollectorService.hpp

  \brief Manages the collection of data in the appropriate time.

  \author Jano Simas, Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_COLLECTOR_COLLECTORSERVICE_HPP__
#define __TERRAMA2_COLLECTOR_COLLECTORSERVICE_HPP__


#include "../core/DataProvider.hpp"
#include "../core/DataSet.hpp"

#include "Collector.hpp"

// QT
#include <QObject>
#include <QMap>

// STL
#include <memory>
#include <cstdint>

//Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core
  {
    class DataSet;
  }
  namespace collector
  {


    /*!
          \class CollectorService

          \brief Defines the base abstraction of a collector service.

          The collector service is a singleton responsible for
          scheduling collectors for each active dataset.

          Once this service starts collecting data it will
          remains in a loop waiting for a notification that new datasets
          must be collected or .
         */
    class CollectorService : public QObject, public boost::noncopyable
    {
        Q_OBJECT

      public:
        /*!
            * \brief Constructor
            */
        CollectorService(QObject* parent = nullptr);
        ~CollectorService();

        /*!
         * \brief Creates a processloop thread and wait for signals.
         */
        void start();

        /*!
             * \brief Contains an infinite loop that will keep the service collecting data.
             *
             * For each provider type verifies if the first provider in the queue is acquiring new data,
             * in case it's collecting moves to next type of provider, when it's done remove it from the queue,
             * in case it's not collecting, starts the collection calling the collect method.
             * It allows multiples providers to collect at the same time but only one provider of each type.
             */
        void processingLoop();

        /*!
             * \brief Creates an instace of a collector of appropriate type for the dataProvider.
             * \param dataProvider The shared pointer to the data provider
             * \return Collector to the DataProvider.
             */
        CollectorPtr addProvider(const core::DataProviderPtr dataProvider);

        /*!
             * \brief Creates a new DataSetTimer for the DataSet and listen to it's timer signal.
             * \param dataset The shared pointer to the dataset
             *
             * \return DataSetTimer for the DataSet.
             */
        DataSetTimerPtr addDataset(const core::DataSetPtr dataset);

      public slots:

        /*!
             * \brief Slot to be called when a DataSetTimer times out.
             */
        void addToQueueSlot(const uint64_t datasetId);

      private:
        /*!
             * \brief Start do collect queued datasets
             * \param firstCollectorInQueue Fist collector in queue for DataProvider::Kind.
             */
        void assignCollector(CollectorPtr firstCollectorInQueue);

        bool stop_;
        QMap<core::DataProvider::Kind, QList<CollectorPtr>>  collectorQueueMap_;
        QMap<CollectorPtr, QList<uint64_t /*DataSetId*/>>    datasetQueue_;

        QMap<int /*DataSetId*/, DataSetTimerPtr>             datasetTimerLst_;

        std::mutex  mutex_;
        std::thread loopThread_;
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_COLLECTORSERVICE_HPP__
