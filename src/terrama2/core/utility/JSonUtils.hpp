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
#include "../data-model/Schedule.hpp"

#include "SemanticsManager.hpp"

//Qt
#include <QJsonObject>

namespace terrama2 {
  namespace core {
    DataProviderPtr fromDataProviderJson(QJsonObject json);
    DataSeriesPtr fromDataSeriesJson(QJsonObject json);
    void addBaseDataSetData(QJsonObject json, DataSet* dataset);
    DataSetPtr fromDataSetDcpJson(QJsonObject json);
    DataSetPtr fromDataSetOccurrenceJson(QJsonObject json);
    DataSetPtr fromDataSetGridJson(QJsonObject json);

    Schedule fromScheduleJson(QJsonObject json);


    QJsonObject toJson(DataProviderPtr dataProviderPtr);
    QJsonObject toJson(DataSeriesPtr dataSeriesPtr);
    QJsonObject toJson(DataSeriesSemantics semantics);

    QJsonObject toJson(DataSetPtr dataSetPtr, DataSeriesSemantics semantics);
    void addToJson(QJsonObject& obj, DataSetDcpPtr dataSetPtr);
    void addToJson(QJsonObject& obj, DataSetOccurrencePtr dataSetPtr);
    void addToJson(QJsonObject& obj, DataSetGridPtr dataSetPtr);

    QJsonObject toJson(Schedule schedule);
  } /* core */
} /* terrama2 */
