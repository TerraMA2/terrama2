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
  \file terrama2/collector/DataSetTimer.hpp

  \brief Signals when the dataset should be collected.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_DATASETTIMER_HPP__
#define __TERRAMA2_COLLECTOR_DATASETTIMER_HPP__

//Std
#include <memory>
#include <cstdint>

//Qt
#include <QObject>
#include <QTimer>

//BOOST
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core {
    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;
  }
  namespace collector
  {
    class Collector;
    typedef std::shared_ptr<Collector> CollectorPtr;

    class DataProcessor;
    typedef std::shared_ptr<DataProcessor> DataProcessorPtr;

    /*!
         * \brief The DataSetTimer class is a wrapper to a [DataSet](\ref terrama2::core::DataSet) with timer capabilities.
         *
         * The DataSetTimer class has an internal timer that emits a signal when
         * it's time to collect the data.
         *
         */
    class DataSetTimer : public QObject, public boost::noncopyable
    {
        Q_OBJECT

      public:
        DataSetTimer(core::DataSetPtr dataSet);
        ~DataSetTimer(){}

        /*!
             * \brief Recover the Collector from the CollectorFactory.
             * \return Collector for the DataSet.
             */
        CollectorPtr                  collector() const;
        //! \brief Returns the original DataSet.
        core::DataSetPtr              dataSet()   const;
        //! \brief List of DataProcessor that should be aquired and processed.
        std::vector<DataProcessorPtr> data()      const;

        bool isValid() const { return false; }

      signals:
        //! \brief Signal emited when the DataSet should be collected.
        void timerSignal(uint64_t DatasetID) const;

      private slots:
        //! \brief Slot called when the timer_ times out, emits timerSignal.
        void timeoutSlot() const;

      private:
        //! \brief Prepare and starts timer following the DataSet information.
        void prepareTimer();
        //! \brief Populates dataLst_ based on DataSet's Data information.
        void populateDataLst();

        core::DataSetPtr dataSet_;//<! Pointer to the Dataset.
        QTimer           timer_;//<! Timer to next collection.

        std::vector<DataProcessorPtr> dataLst_;
    };

    typedef std::shared_ptr<DataSetTimer> DataSetTimerPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_DATASETTIMER_HPP__
