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


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_ANALYSISTIMER_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_ANALYSISTIMER_HPP__

//Std
#include <memory>
#include <cstdint>

//Qt
#include <QObject>
#include <QTimer>

//BOOST
#include <boost/noncopyable.hpp>

#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/data-model/DataSet.hpp"

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {

        struct Analysis;

        /*!
              \brief The DataSetTimer class is a timer to a core::DataSet.

              The DataSetTimer class has an internal timer that emits a signal when
              it's time to collect the data.

             */
        class AnalysisTimer : public QObject
        {
            Q_OBJECT

          public:
            /*!
              \brief Constructor, start the timer for collecting.

              \exception terrama2::collector::InvalidDataSetException Raise when constructed with an invalid core::DataSet.
              \exception InvalidCollectFrequencyException Raised if collect frequency equals or lesser then zero.
             */
            AnalysisTimer(const terrama2::services::analysis::core::Analysis& analysis);
            ~AnalysisTimer();

            //! Returns the id of the Analysis.
            uint64_t analysis() const;

          signals:

            //! Signal emited when the Analysis should be executed.
            void timerSignal(uint64_t analysisId) const;

          private slots:

            //! Slot called when the timer times out, emits timerSignal.
            void timeoutSlot() const;
            void scheduleSlot() const;

          private:
            /*!
              \brief Prepare and starts timer following the Analysis information.

              If a data frequency was set in the Analysis, the timeout signal will be emited every time the interval has passed,
              else, a schedule is set and the timeout signal is emited at the time set once a day.

              \note The schedule only consider time of day.
              \note The schedule precision is 1 minute, seconds will be ignored.

              \exception InvalidCollectFrequencyException Raised if collect frequency equals or lesser then zero.
            */
            void prepareTimer(const terrama2::services::analysis::core::Analysis& analysis);

            struct Impl;
            Impl* impl_;
        };

        typedef std::shared_ptr<AnalysisTimer> AnalysisTimerPtr; //!< Shared pointer for AnalysisTimer.

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2


#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_ANALYSISTIMER_HPP__
