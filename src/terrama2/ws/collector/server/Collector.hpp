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
  \file terrama2/ws/collector/server/Collector.hpp

  \brief Implementation of collector.

  \author Jano Simas
*/


#ifndef __TERRAMA2_WS_COLLECTOR_SERVER_COLLECTOR_HPP__
#define __TERRAMA2_WS_COLLECTOR_SERVER_COLLECTOR_HPP__

#include "../../../core/DataProvider.hpp"
#include "DataSetTimer.hpp"
#include "DataProcessor.hpp"

//Qt
#include <QObject>

//std
#include <mutex>

namespace terrama2
{
  namespace core {
    class DataSet;
    class Data;
  }
  namespace ws
  {
    namespace collector
    {
      namespace server
      {

        class Collector : public QObject
        {
            Q_OBJECT

          public:
            Collector(core::DataProviderPtr dataProvider, QObject* parent = nullptr);
            virtual ~Collector(){}

            core::DataProvider::Kind kind()         const { return dataProvider_->kind();}
            core::DataProviderPtr    dataProvider() const { return dataProvider_;}

            bool isCollecting()                     const {}//JANO: implementar isCollecting
            bool collect(DataSetTimerPtr datasetTimer); //should run in thread

            virtual bool isOpen() const = 0;
            virtual bool open()  = 0;
            virtual void close() = 0;

          protected:
            virtual void getData(const DataProcessor&) = 0;

            std::mutex mutex_;

            core::DataProviderPtr dataProvider_;

            /*!
             * \brief Utility class to assert the mutex will be unlocked in the end of the process.
             */
            class LockMutex
            {
              public:
                LockMutex(std::mutex& mutex) : mutex_(mutex) { }
                ~LockMutex()   { mutex_.unlock(); }

                bool tryLock() { return mutex_.try_lock(); }
                void lock()    { mutex_.lock(); }
                void unLock()  { mutex_.unlock(); }

              private:
                std::mutex& mutex_;
            };
        };

        typedef std::shared_ptr<Collector> CollectorPtr;
      }
    }
  }
}


#endif //__TERRAMA2_WS_COLLECTOR_SERVER_COLLECTOR_HPP__
