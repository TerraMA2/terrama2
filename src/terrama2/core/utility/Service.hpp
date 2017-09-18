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
  \file terrama2/core/Service.hpp

  \brief Core service for TerraMA² process.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_SERVICE_HPP__
#define __TERRAMA2_CORE_SERVICE_HPP__

#include "../Typedef.hpp"
#include "../Shared.hpp"
#include "../data-model/Schedule.hpp"
#include "../data-model/Filter.hpp"
#include "ProcessLogger.hpp"

//STL
#include <vector>
#include <mutex>
#include <future>
#include <queue>

//Qt
#include <QObject>
#include <QJsonDocument>

namespace te
{
  namespace dt
  {
    class TimeInstantTZ;
  } /* dt */
} /* te */

namespace terrama2
{
  namespace core
  {
    /*!
    \brief Contains information about the execution of a process.
     */
    struct ExecutionPackage
    {
      RegisterId registerId = 0;
      ProcessId processId = 0;
      std::shared_ptr<te::dt::TimeInstantTZ> executionDate;
    };

    /*!
    \brief Tags used when returning values at the end of a process.
    */
    namespace ReturnTags
    {
      const QString PROCESS_ID = "process_id";//!< ID of the process finished, int.
      const QString RESULT = "result";//!< Result of te process, boolean.
      const QString AUTOMATIC = "automatic";//!< Flag if other processes should be started after this, boolean.
      const QString EXECUTION_DATE = "execution_date";//!< Reference date of the process, string.
    } /* ReturnTags */

    /*!
      \brief The Service provides thread and time management for processes.

      This class is used to manage thread sync and timer listening for derived TerraMA2 services.

      The Service has a main thread that will check for new data to be processed
      and a threadpool that will be allocated to actively process the data.

      ## Threadpool ##

      The number of threads available can be set at the start() of the service or updated later
      by updateNumberOfThreads().

      If the number of threads is set to 0 the service will
      try to identify the concurrency capabilities of the hardware, if this fails
      the number of threads will be set to 1.

      ## Process queue ##

      The processes are placed on a queue during processNextData().
      This method must be overloaded.

      Queued processes will be executed automatically by the threadpool.

    */
    class Service : public QObject
    {
        Q_OBJECT

      public:
        //! Default constructor
        Service();
        /*!
          \brief Destructor

          Stop the Service and destroys it.
        */
        virtual ~Service();

        /*!
           \brief Starts the server.
           \param threadNumber Number of threads to process tasks.

           Starts the server, starts to process waiting tasks.

           If the number of threads is 0 (default), this method will try to identify the number of processors,
           if it's not possible, only one thread will be created.

         */
        virtual void start(size_t threadNumber = 0);
        virtual void getStatus(QJsonObject& obj) const {};

      signals:
        void serviceFinishedSignal();

        /*!
       * \brief Signal emitted when a full process is complete in service
       * \param answer A JSON with process info
       */
        void processFinishedSignal(QJsonObject answer);

        /*!
        * \brief Signal emitted with the result of the validation of a process information.
        * \param answer A JSON with the valitadion result.
        */
        void validateProcessSignal(QJsonObject answer);

      public slots:

        virtual void addToQueue(ProcessId processId, std::shared_ptr<te::dt::TimeInstantTZ> startTime) noexcept = 0;

        /*!
           \brief  Stops the service.

           \note Incomplete tasks might be lost and will be restarted when the service is started again.

         */
        void stopService() noexcept;
        void stop(bool holdStopSignal) noexcept;

        /*!
          \brief Updates the number of process threads in the thread pool

          May wait for threads to finish current processing before changes.

          \param numberOfThreads Number of threads desired, if 0 the maximum number of threads allowed by the system the will be used.
        */
        virtual void updateNumberOfThreads(size_t numberOfThreads = 0) noexcept final;

        virtual void addProcessToSchedule(ProcessPtr process) noexcept;
        void setLogger(std::shared_ptr<ProcessLogger> logger) noexcept;

        virtual void updateAdditionalInfo(const QJsonObject& obj) noexcept = 0;

      protected:
        void updateFilterDiscardDates(terrama2::core::Filter& filter, std::shared_ptr<ProcessLogger> logger, ProcessId processId) const;

        TimerPtr createTimer(const terrama2::core::Schedule& schedule, ProcessId processId, std::shared_ptr<te::dt::TimeInstantTZ> lastProcess) const;
        /*!
           \brief Returns true if the main loop should continue.
           \return True if there is data to be tasked.
         */
        virtual bool hasDataOnQueue() noexcept;

        /*!
           \brief Watches for data that needs to be processed.
         */
        void mainLoopThread() noexcept;
        /*!
          \brief Add next task to the processing queue.
          \return True if there is more data to be processed.
         */
        bool processNextData();

        virtual void prepareTask(const ExecutionPackage& executionPackage) = 0;

        //! Process a queued task.
        void processingTaskThread(const std::shared_ptr< const ProcessLogger > logger) noexcept;

        //! Verifys if the number of threads is greater than 0.
        size_t verifyNumberOfThreads(size_t numberOfThreads) const;

        //! Sends the process finished signal
        void sendProcessFinishedSignal(const ProcessId processId, std::shared_ptr<te::dt::TimeInstantTZ> executionDate, const bool success, QJsonObject jsonAnswer = QJsonObject());

        /*!
          \brief Verifies if there is job to be done in the waiting queue and add it to the processing queue.
        */
        void notifyWaitQueue(ProcessId processId);

        bool stop_;
        std::mutex  mutex_; //!< Mutex for thread safety
        std::future<void> mainLoopThread_; //!< Thread that holds the loop of processing queued dataset.
        std::condition_variable mainLoopCondition_; //!< Wait condition for the loop thread. Wakes when new data is available or the service is stopped.
        std::map<ProcessId, std::queue<ExecutionPackage> > waitQueue_; //!< Wait queue to store que process that are already being processed.
        std::vector<ProcessId> processingQueue_; //!< Queue with process currently being processed.
        std::vector<ExecutionPackage> processQueue_;
        std::queue<std::packaged_task<void()> > taskQueue_; //!< Queue for tasks.
        std::vector<std::future<void> > processingThreadPool_; //!< Pool of processing threads
        std::condition_variable processingThreadCondition_; //!< Wait condition for the processing thread. Wakes when new tasks are available or the service is stopped.

        std::map<ProcessId, terrama2::core::TimerPtr> timers_;//!< List of running Collector timers
        std::shared_ptr< ProcessLogger > logger_;//!< process logger
    };
  }
}
#endif //__TERRAMA2_CORE_SERVICE_HPP__
