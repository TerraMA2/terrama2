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
  \file terrama2/services/analysis/core/ThreadPool.hpp

  \brief Pool of threads to execute the the analysis.

  \author Paulo R. M. Oliveira
*/
#ifndef TERRAMA2_SERVICES_ANALYSIS_CORE_THREADPOOL_HPP
#define TERRAMA2_SERVICES_ANALYSIS_CORE_THREADPOOL_HPP

// TerraMA2
#include "Analysis.hpp"
#include "DataManager.hpp"
#include "PythonInterpreter.hpp"
#include "Shared.hpp"

// Python
#include <Python.h>

// STL
#include <future>
#include <thread>
#include <queue>
#include <vector>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        class ThreadPool
        {
          public:

            /*!
              \brief The constructor launches the given amount of worker threads.
              \param size_t Number of threads.
            */
            ThreadPool(size_t);


            /*!
             \brief Add new work item to the pool
             \param f The function to be executed.
             \param args Parameters to the given function.
           */
            template<class F, class... Args>
            auto enqueue(F&& f, Args&& ... args)
                    -> std::future<typename std::result_of<F(Args...)>::type>;


            /*!
              \brief The destructor joins all threads.
            */
            ~ThreadPool();

            /*!
              \brief Return the number of threads.
              \return The number of threads.
            */
            size_t numberOfThreads() const;

          private:

            std::vector<std::thread> workers; //!< Keep track of threads so we can join them.
            std::queue<std::function<void()> > tasks; //!< The task queue.
            std::mutex queue_mutex; //!< Mutex to control access to queue.
            std::condition_variable condition; //!< Condition variable to notify workers when there is work to be done.
            bool stop; //!< Flag to indicate that the thread pool must stop the worker thread
        };


        inline ThreadPool::ThreadPool(size_t threads)
                : stop(false)
        {
          for(size_t i = 0; i < threads; ++i)
            workers.emplace_back(
                    [this]
                    {
                      for(; ;)
                      {
                        std::function<void()> task;

                        {
                          std::unique_lock<std::mutex> lock(this->queue_mutex);
                          this->condition.wait(lock,
                                               [this]
                                               { return this->stop || !this->tasks.empty(); });
                          if(this->stop && this->tasks.empty())
                            return;
                          task = std::move(this->tasks.front());
                          this->tasks.pop();
                        }

                        task();
                      }
                    }
            );
        }

        template<class F, class... Args>
        auto ThreadPool::enqueue(F&& f, Args&& ... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
        {
          using return_type = typename std::result_of<F(Args...)>::type;

          auto task = std::make_shared<std::packaged_task<return_type()> >(
                  std::bind(std::forward<F>(f), std::forward<Args>(args)...)
          );

          std::future<return_type> res = task->get_future();
          {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if(stop)
            {
              QString errMsg = QObject::tr("Enqueue on stopped ThreadPool");
              throw terrama2::InitializationException() << ErrorDescription(errMsg);
            }

            tasks.emplace([task]()
                          { (*task)(); });
          }
          condition.notify_one();
          return res;
        }

        inline ThreadPool::~ThreadPool()
        {
          {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
          }
          condition.notify_all();
          for(std::thread& worker: workers)
            worker.join();
        }

        inline size_t ThreadPool::numberOfThreads() const
        {
          return workers.size();
        }


      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //TERRAMA2_SERVICES_ANALYSIS_CORE_THREADPOOL_HPP
