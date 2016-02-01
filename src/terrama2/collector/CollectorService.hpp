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
#include "../core/Service.hpp"
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
    class CollectorService : public core::Service
    {
      Q_OBJECT

    public:
      CollectorService();

      //comments on base class
      virtual ~CollectorService(){}


    public slots:
      /*!
          \brief Register the dataprovider.

          \param provider The dataprovider to be registered by the service.

          \note Doesn't add provider's dataset to the list of datasets to be collected.
         */
      void addProvider(const core::DataProvider& provider);

      /*!
          \brief Remove a core::DataProvider and all associated core::DataSet from the CollectorService_old list.

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
          \brief Removes a core::DataSet from the CollectorService_old list.

          \note If the core::DataSet does not exist nothing is done.

          \param Dataset to be removed.
         */
      void removeDataset(const core::DataSet& dataset);

      /*!
          \brief Removes a core::DataSet from the CollectorService_old list.

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


    protected:
      //comments on base class
      virtual bool mainLoopWaitCondition() noexcept override;
      //comments on base class
      virtual bool checkNextData() override;

      /*!
         \brief Creates a task and place on queue.

         Places the task on queue waiting for the next free thread to execute.

         \param provider core::DataProvider id .
         \param datasets Vector of core::DataSet id that are on schedule to colelct.
       */
      virtual void prepareTask(const uint64_t provider, const std::vector<uint64_t>& datasets);

      /*!
          \brief Retrieves, parse, filter and store the data from each dataset.
          \param dataProvider DataProvider of the data
          \param dataSetList List of DataSets to be collected.
         */
      static void collect(const terrama2::core::DataProvider& dataProvider, const std::vector<core::DataSet>& dataSetList);

      void connectDataManager();
      void populateData();                  //!< Populate data based on core::DataSet and core::DataProvider already in the core::DataManager.

      std::map<uint64_t, core::DataProvider> dataproviders_;    //!< The list of data providers. [dataprovider-id] -> dataprovider.
      std::map<uint64_t, core::DataSet> datasets_;              //!< The list of dataset to be collected. [dataset-id] -> dataset.
      std::map<uint64_t, DataSetTimerPtr> timers_;              //!< The list of timers used to control the timeout for data collection. [dataset-id] -> [dataset-time].
      std::map<uint64_t, std::vector<uint64_t> > collectQueue_; //!< The queue of datasets to be collected by dataprovider. [dataprovider-id] -> [dataset-queue].
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_COLLECTORSERVICE_HPP__
