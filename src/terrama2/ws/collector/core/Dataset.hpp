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
  \file terrama2/ws/collector/core/Dataset.hpp

  \brief Represents a data series

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_DATASET_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_DATASET_HPP__

//STL
#include <memory>

//QT
#include <QObject>
#include <QTimer>
#include <QList>


namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {

        class DataProvider;
        class Data;

        /*
        * \brief Class that reresents a data series, controls the time interval between each collection.
        * Emits a signal when it's time to collect a new data.
        */
        class Dataset : public QObject
        {
          Q_OBJECT
        public:
          Dataset(int id,
                  std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider,
                  QList<std::shared_ptr<terrama2::ws::collector::core::Data>> dataList);

          ~Dataset();

          std::shared_ptr<terrama2::ws::collector::core::DataProvider> getDataProvider() const;

          /**
           * \brief Returns the list of data that compose this dataset.
           * \return The list of data
           */
          QList<std::shared_ptr<terrama2::ws::collector::core::Data>> getDataList() const;

        public slots:
          /**
           * \brief Slots that will be called when it's time to collect a new data.
           */
          void timeoutSlot();

        signals:
          /**
           * \brief Signal that will be emmited by the timeout slot in order to notify the collector service.
           */
          void timerSignal(terrama2::ws::collector::core::Dataset*);

        protected:
          int id_;
          QTimer timer_;
          std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider_;
          QList<std::shared_ptr<terrama2::ws::collector::core::Data>> dataList_;

        };
      } // core
    } // collector
  } // ws
} // terrama2

#endif // __TERRAMA2_WS_COLLECTOR_CORE_DATASET_HPP__
