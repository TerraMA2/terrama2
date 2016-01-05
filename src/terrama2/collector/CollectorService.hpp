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
#include <vector>
#include <mutex>
#include <map>
#include <future>
#include <queue>
#include <condition_variable>

//Boost
#include <boost/noncopyable.hpp>

// Forward Declaration
namespace te
{
  namespace da
  {
    class DataSet;
  }
}

namespace terrama2
{
  namespace collector
  {
    /*!
      \class CollectorService

      \brief Manages the collection of data in the appropriate time.

      The collector service is responsible for scheduling the collection of each active dataset.

      Once the service starts collecting data it will
      remain in a loop waiting for notification of datasets to be collected.

      It may also be signaled of new datasets to be added to the collect list
      or removed from the list.
     */
    class CollectorService : public QObject, public boost::noncopyable
    {
      Q_OBJECT

    public:

      //! Creates a CollectorService object, does not start the service.
      CollectorService(QObject* parent = nullptr);

      //! Stops and finishes current running service.
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
          \brief Register the dataprovider.

          \param provider The dataprovider to be registered by the service.

          \note Doesn't add provider's dataset to the list of datasets to be collected.
         */
      void addProvider(const core::DataProvider& provider);

      /*!
          \brief Remove a core::DataProvider and all associated core::DataSet from the CollectorService list.

          The associated core::DataSet will be removed first.

            \note If the DataProvider does not exist nothing is done.

            \param DataProvider to be removed.
           */
      void removeProvider(const core::DataProvider& dataProvider);

      /*!
          \brief Updates a core::DataProvider.

          \note No change is made to the associated core::DataSet.

          \note If the core::DataProvider does not exist nothing is done.

          \param Data provider to be updated.
         */
      void updateProvider(const core::DataProvider& dataProvider);

      /*!
          \brief Creates a new DataSetTimer for the core::DataSet and connect to it's timer signal.

          \param dataset The dataset

          \return DataSetTimer for the core::DataSet.

          \note Adds the dataset to queue to be collected.

          \pre The core::DataSet must have an id and be active.
         */
      void addDataset(const core::DataSet& dataset);

      /*!
          \brief Removes a core::DataSet from the CollectorService list.

          \note If the core::DataSet does not exist nothing is done.

          \param Dataset to be removed.
         */
      void removeDataset(const core::DataSet& dataset);

      /*!
          \brief Removes a core::DataSet from the CollectorService list.

          \note If the core::DataSet does not exist nothing is done.

          \param Id of the core::Dataset to be removed.
         */
      void removeDatasetById(uint64_t datasetId);

      /*!
          \brief Updates a core::DataSet.

          The core::DataSet will be destroyed
          and recreated.

          \note If the DataSet does not exist nothing is done.

          \note Adds the dataset to queue to be collected.
          \param Dataset to be updated.
        */
      void updateDataset(const core::DataSet &dataset);

      //! Slot to be called when a DataSetTimer times out.
      void addToQueue(uint64_t datasetId);

    private:

      /*!
          \brief Start a new thread for collecting the datasets in the queue from the data-provider.

          \param provider Provide identifier.
          \param datasets Queue of datasets from the provider that will be collected.
         */
      void process(const uint64_t provider, const std::vector<uint64_t>& datasets);

      /*!
          \brief Retrieves, parse, filter and store the data from each dataset.
          \param dataProvider DataProvider of the data
          \param dataSetList List of DataSets to be collected.
         */
      static void collect(const terrama2::core::DataProvider& dataProvider, const std::list<terrama2::core::DataSet>& dataSetList);

      /*!
         * \brief Thread that will check for new collecting tasks and start next task.
         */
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
          \brief Connect with core::DataManager signals.

          Listens to:
          - dataProviderAdded(const core::DataProvider&);
          - dataProviderRemoved(const core::DataProvider&);
          - dataProviderUpdated(const core::DataProvider&);
          - dataSetAdded(const core::DataSet&);
          - dataSetRemoved(uint64_t);
          - dataSetUpdated(const core::DataSet&);
         */
      void connectDataManager();

    private:

      bool stop_;                                               //!< Controls the service thread.
      std::map<uint64_t, core::DataProvider> dataproviders_;    //!< The list of data providers. [dataprovider-id] -> dataprovider.
      std::map<uint64_t, core::DataSet> datasets_;              //!< The list of dataset to be collected. [dataset-id] -> dataset.
      std::map<uint64_t, DataSetTimerPtr> timers_;              //!< The list of timers used to control the timeout for data collection. [dataset-id] -> [dataset-time].
      std::map<uint64_t, std::vector<uint64_t> > collectQueue_; //!< The queue of datasets to be collected by dataprovider. [dataprovider-id] -> [dataset-queue].

      std::mutex  mutex_;                                       //!< Mutex for thread safety
      std::future<void> loopThread_;                            //!< Thread that holds the loop of processing queued dataset.
      std::condition_variable loop_condition_;                  //!< Wait condition for the loop thread. Wakes when new data is available or the service is stopped.

      std::queue<std::packaged_task<void()> > taskQueue_;       //!< Queue for collecting tasks.
      std::vector<std::future<void> > threadPool_;              //!< Pool of collecting threads
      std::condition_variable thread_condition_;                //!< Wait condition for the collecting thread. Wakes when new tasks are available or the service is stopped.

      void populateData();                  //!< Populate data based on core::DataSet and core::DataProvider already in the core::DataManager.

    };
  }
}

#endif //__TERRAMA2_COLLECTOR_COLLECTORSERVICE_HPP__
