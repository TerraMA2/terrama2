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

// TerraMA2
#include "../core/DataProvider.hpp"
#include "../core/DataSet.hpp"
#include "DataSetTimer.hpp"

// QT
#include <QObject>

// STL
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <future>
#include <queue>

//Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace collector
  {
    // Forward declaration
    class Factory; // FIXME: redesign factory for global registration of concrete factories


    /*!
      \class CollectorService

      \brief Defines the base abstraction of a collector service.

      The collector service is responsible for scheduling collectors for each active dataset.

      Once the service starts collecting data it will
      remain in a loop waiting for notification of datasets to be collected.

      It may also be signaled of new datasets to be added to the collect list
      or removed from the list.
     */
    class CollectorService : public QObject, public boost::noncopyable
    {
      Q_OBJECT

    public:
      
      //! Constructor.
      CollectorService(QObject* parent = nullptr);

      //! Destructor.
      ~CollectorService();

      /*!
        \brief Start the main loop thread for collecting data.

        \param Number of collecting threads in the threadpool, if 0 trys to find number of system supported threads else 1 thread.

        \exception ServiceAlreadyRunnningError Raise when the service is already runnning.
        \exception UnableToStartServiceError Raise when the system can not create a thread or acquire resources to initialize the service.
       */
      void start(int threadNumber = 0);

    public slots:

      /*!
        \brief Stop the service and release the thread loop.

        \note It can block the caller waiting for any pending data collection.
       */
      void stop() noexcept;

      /*!
        \brief Register the dataprovider into the service.
        
        \param provider The dataprovider to be registered by the service.

        \note Doesn't add provider's dataset to the list of datasets to be collected.
       */
      void addProvider(const core::DataProvider& provider);

      /*!
        \brief Remove a DataProvider and all associated DataSet.

        The associated DataSet will be removed first.

          \note If the DataProvider does not exist nothing is done.

          \param DataProvider to be removed.
         */
      void removeProvider(const core::DataProvider& dataProvider);

      /*!
        \brief Updates a [DataProvider]{\ref terrama2::core::DataProvider}.

        \note No change is made to the associated [DataSet]{\ref terrama2::core::DataSet}.

        \note If the DataProvider does not exist nothing is done.

        \param Data provider to be updated.
       */
      void updateProvider(const core::DataProvider& dataProvider);

      /*!
        \brief Creates a new DataSetTimer for the DataSet and connect to it's timer signal.
        
        \param dataset The dataset
       
        \return DataSetTimer for the DataSet.

        \note Adds the dataset to queue to be collected.
       */
      void addDataset(const core::DataSet& dataset);

      /*!
        \brief Removes a [DataSet]{\ref terrama2::core::DataSet}.

        If the DataSet does not exist nothing is done.

        \param Dataset to be removed.
       */
      void removeDataset(const core::DataSet& dataset);
      void removeDatasetById(uint64_t datasetId);

      /*!
        \brief Updates a [DataSet]{\ref terrama2::core::DataSet}.

        The [DataSet]{\ref terrama2::core::DataSet} will be destroyed
        and recreated.

        If the DataSet does not exist nothing is done.

        \note Adds the dataset to queue to be collected.
        \param Dataset to be updated.
         */
      void updateDataset(const core::DataSet &dataset);

      //! Slot to be called when a DataSetTimer times out.
      void addToQueue(uint64_t datasetId);

    private:
      
      /*!
        \brief Start a new thread for collecting the datasets in the queue from the data-provider.

        \param provider Dataprovider .....
        \param datasets Fila de datasets do provider que iremos coletar....
       */
      void process(uint64_t provider, const std::vector<uint64_t>& datasets);

      static void collectAsThread(const terrama2::core::DataProvider& dataProvider, const std::list<terrama2::core::DataSet>& dataSetList);
      void threadProcess();

      /*!
         \brief Contains an infinite loop that will keep the service collecting data.

         For each provider type verifies if the first provider in the queue is acquiring new data,
         in case it's collecting moves to next type of provider, when it's done remove it from the queue,
         in case it's not collecting, starts the collection calling the collect method.
         It allows multiples providers to collect at the same time but only one provider of each type.
       */
      void processingLoop();

      /*!
        \brief Make connections with the [DataManager]{\ref terrama2::core::DataManager}.

        Listens to:
        - dataProviderAdded(DataProvider);
        - dataProviderRemoved(DataProvider);
        - dataProviderUpdated(DataProvider);
        - dataSetAdded(DataSet);
        - dataSetRemoved(DataSet);
        - dataSetUpdated(DataSet);
       */
      void connectDataManager();

    private:

      bool stop_;                                               //!< Controls the service thread.
      std::shared_ptr<Factory> factory_;                        //!< Factory for Parsers, Storagers and Retrievers
      std::map<uint64_t, core::DataProvider> dataproviders_;    //!< The list of data providers. [dataprovider-id] -> dataprovider.
      std::map<uint64_t, core::DataSet> datasets_;              //!< The list of dataset to be collected. [dataset-id] -> dataset.
      std::map<uint64_t, DataSetTimerPtr> timers_;              //!< The list of timers used to control the timeout for data collection. [dataset-id] -> [dataset-time].
      std::map<uint64_t, std::vector<uint64_t> > collectQueue_; //!< The queue of datasets to be collected by dataprovider. [dataprovider-id] -> [dataset-queue].

      std::mutex  mutex_;                                       //!< mutex to thread safety
      std::thread loopThread_;                                  //!< Thread that holds the loop of processing queued dataset.

      std::queue<std::packaged_task<void()> > taskQueue_;       //!< Pool of collecting tasks.
      std::vector<std::thread> threadPool_;                     //!< Pool of collecting threads

      void populateData();
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_COLLECTORSERVICE_HPP__
