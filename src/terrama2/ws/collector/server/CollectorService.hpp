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
  \file terrama2/ws/collector/server/CollectorService.hpp

  \brief Implementation of a collector service.

  \author Jano Simas, Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_WS_COLLECTOR_APPSERVER_COLLECTORSERVICE_HPP__
#define __TERRAMA2_WS_COLLECTOR_APPSERVER_COLLECTORSERVICE_HPP__


#include "../core/DataProvider.hpp"

// QT
#include <QObject>
#include <QMap>

// STL
#include <memory>

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {
        class Dataset;
      }

      namespace appserver
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
        class CollectorService : public QObject
        {
            Q_OBJECT

        public:
            /*!
            * \brief Constructor
            */
            CollectorService(QObject* parent = nullptr);
            ~CollectorService(){}

            /**
             * \brief Contains an infinite loop that will keep the service collecting data.
             * For each provider type verifies if the first provider in the queue is acquiring new data,
             * in case it's collecting moves to next type of provider, when it's done remove it from the queue,
             * in case it's not collecting, starts the collection calling the collect method.
             * It allows multiples providers to collect at the same time but only one provider of each type.
             */
            void start();

            /**
             * \brief Adds a new data provider to the list.
             * \param dataProvider The shared pointer to the data provider
             */
            void addProvider(std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider);

             /**
             * \brief Adds a new dataset to the list.
             * \param dataset The shared pointer to the dataset
             */
            void addDataset(std::shared_ptr<terrama2::ws::collector::core::Dataset> dataset);

        public slots:
            /**
             * \brief Slot to stop the collector service.
             */
            void stop();

        private slots:
            /**
             * \brief Slot to be .
             */
            void addToQueueSlot(terrama2::ws::collector::core::Dataset* dataset);

        private:
            bool stop_;
            QList<std::shared_ptr<terrama2::ws::collector::core::DataProvider>> dataProviderLst_;
            QList<std::shared_ptr<terrama2::ws::collector::core::Dataset>> datasetLst_;
            QMap<terrama2::ws::collector::core::DataProviderType, QList<std::shared_ptr<terrama2::ws::collector::core::DataProvider>>> dataProviderQueueMap_;
            QMap<std::shared_ptr<terrama2::ws::collector::core::DataProvider>, QList<terrama2::ws::collector::core::Dataset*>> datasetQueue_datasetQueue_;
        };
      }
    }
  }
}

#endif //__TERRAMA2_WS_COLLECTOR_APPSERVER_COLLECTORSERVICE_HPP__
