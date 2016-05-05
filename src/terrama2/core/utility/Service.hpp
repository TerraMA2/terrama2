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

//STL
#include <vector>
#include <mutex>
#include <future>
#include <queue>

//Qt
#include <QObject>

namespace terrama2
{
  namespace core
  {
    /*!
       \brief Base class for TerraMA2 services.
     */
    class Service : public QObject
    {
      Q_OBJECT

    public:
      //! Default constructor
      Service();
      //! Default destructor
      virtual ~Service();

      /*!
         \brief Starts the server.
         \param threadNumber Number of threads to process tasks.

         Starts the server, starts to process waiting tasks.

         If the number of threads is 0 (default), this method will try to identify the number of processors,
         if it's not possible, only one thread will be created.

       */
      void start(uint threadNumber = 0);

    public slots:
      /*!
         \brief  Stops the service.

         \note Incomplete tasks might be lost and will be restarted with a new Service::start.

       */
      void stop() noexcept;

      virtual void updateNumberOfThreads(int) = 0;

    protected:
      /*!
         \brief Returns true if the main loop should continue.
         \return True if there is data to be tasked OR is stop is true.
       */
      virtual bool mainLoopWaitCondition() = 0;

      /*!
         \brief Watches for data that needs to be processed.
       */
      void mainLoopThread() noexcept;
      /*!
         \brief Check if there is data to be processed.
         \return True if there is more data to be processed.
       */
      virtual bool checkNextData() = 0;

      //! Process a queued task.
      void processingTaskThread() noexcept;

      bool stop_;
      std::mutex  mutex_;                                       //!< Mutex for thread safety
      std::future<void> mainLoopThread_;                            //!< Thread that holds the loop of processing queued dataset.
      std::condition_variable mainLoopCondition_;                  //!< Wait condition for the loop thread. Wakes when new data is available or the service is stopped.

      std::queue<std::packaged_task<void()> > taskQueue_;       //!< Queue for tasks.
      std::vector<std::future<void> > processingThreadPool_;              //!< Pool of processing threads
      std::condition_variable processingThreadCondition_;                //!< Wait condition for the processing thread. Wakes when new tasks are available or the service is stopped.

    };
  }
}
#endif //__TERRAMA2_CORE_SERVICE_HPP__
