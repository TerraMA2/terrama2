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
  \file terrama2/core/utility/JSonUtils.hpp

  \brief

  \author Jano Simas
*/
#include "../Shared.hpp"
#include "../data-model/Risk.hpp"
#include "../data-model/Schedule.hpp"
#include "../data-model/DataManager.hpp"
#include "../data-model/Filter.hpp"

#include "SemanticsManager.hpp"

//Qt
#include <QJsonObject>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Creates a DataProvider from a QJsonObject.
      \see DataProvider for json structure
      \see [DataProvider at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/DataProvider) for more information.
      */
    DataProviderPtr fromDataProviderJson(QJsonObject json);

    /*!
      \brief Creates a DataSeries from a QJsonObject.
      \see DataSeries for json structure
      \see [DataSeries at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/DataSeries) for more information.
      */
    DataSeriesPtr fromDataSeriesJson(QJsonObject json);

    /*!
      \brief Add base DataSet information to derived classes when constructing from QJsonObject.
      */
    void addBaseDataSetData(QJsonObject json, std::shared_ptr<terrama2::core::DataSet> dataset);

    /*!
      \brief Creates a DataSet from a QJsonObject.
      \see DataSet for json structure
      \see [DataSetDcp at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/DataSet) for more information.
    */
    DataSetPtr fromDataSetJson(QJsonObject json);

    /*!
      \brief Creates a DataSetDcp from a QJsonObject.
      \see DataSetDcp for json structure
      \see [DataSetDcp at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/DataSetDcp) for more information.
      */
    DataSetPtr fromDataSetDcpJson(QJsonObject json);

    /*!
    \brief Creates a DataSetOccurrence from a QJsonObject.
    \see DataSetOccurrence for json structure
    \see [DataSetOccurrence at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/DataSetOccurrence) for more information.
      */
    DataSetPtr fromDataSetOccurrenceJson(QJsonObject json);

    /*!
      \brief Creates a DataSetGrid from a QJsonObject.
      \see DataSetGrid for json structure
      \see [DataSetGrid at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/DataSetGrid) for more information.
      */
    DataSetPtr fromDataSetGridJson(QJsonObject json);

    /*!
      \brief Creates a Schedule from a QJsonObject.
      \see Schedule for json structure
      \see [Schedule at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/Schedule) for more information.
      */
    Schedule fromScheduleJson(QJsonObject json);

    /*!
      \brief Creates a Filter from a QJsonObject.
      \see Filter for json structure
      \see [Filter at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/core/Filter) for more information.
      */
    Filter fromFilterJson(QJsonObject json, DataManager* dataManager);
    QJsonObject toJson(const Filter& filter);

    LegendPtr fromRiskJson(QJsonObject json);
    QJsonObject toJson(const terrama2::core::Risk& risk);

    /*!
      \brief Creates a QJsonObject from a DataProvider.
      \attention This is a function created for debug and tests
      */
    QJsonObject toJson(DataProviderPtr dataProviderPtr);

    /*!
      \brief Creates a QJsonObject from a DataSeries.
      \attention This is a function created for debug and tests
      */
    QJsonObject toJson(DataSeriesPtr dataSeriesPtr);

    /*!
      \brief Creates a QJsonObject from a DataSet.
      \attention This is a function created for debug and tests
      */
    QJsonObject toJson(DataSetPtr dataSetPtr, DataSeriesSemantics semantics);
    //! Add DataSetDcp information to base DataSet QJsonObject.
    void addToJson(QJsonObject& obj, DataSetDcpPtr dataSetPtr);
    //! Add DataSetOccurrence information to base DataSet QJsonObject.
    void addToJson(QJsonObject& obj, DataSetOccurrencePtr dataSetPtr);
    //! Add DataSetGrid information to base DataSet QJsonObject.
    void addToJson(QJsonObject& obj, DataSetGridPtr dataSetPtr);

    /*!
      \brief Creates a QJsonObject from a Schedule.
      \attention This is a function created for debug and tests
      */
    QJsonObject toJson(Schedule schedule);

    /*!
      \brief Creates a QJsonObject from an ReprocessingHistoricalData.
    */
    QJsonObject toJson(ReprocessingHistoricalDataPtr reprocessingHistoricalDataPtr);

    /*!
      \brief Creates an ReprocessingHistoricalData object from a QJsonObject.
      \see [ReprocessingHistoricalData at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/services/analysis/ReprocessingHistoricalData) for more information.
    */
    ReprocessingHistoricalDataPtr fromReprocessingHistoricalData(const QJsonObject& json);

  } /* core */
} /* terrama2 */
