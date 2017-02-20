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
  \file terrama2/impl/DataAccessorTxtFile.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "DataAccessorDCPCSV.hpp"
#include "../core/utility/Logger.hpp"

// TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/geometry/GeometryProperty.h>

//QT
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


void terrama2::core::DataAccessorDCPCSV::checkFields(DataSetPtr dataSet) const
{
  bool dateTime = false;

  QJsonArray array = getFields(dataSet);

  for(const auto& item : array)
  {
    const QJsonObject& obj = item.toObject();

    int type = dataTypes.at(obj.value("type").toString().toStdString());

    if(type == te::dt::DATETIME_TYPE)
      dateTime = true;
  }

  if(!dateTime)
  {
    QString errMsg = QObject::tr("Invalid fields: Missing dateTime field!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
  }
}

terrama2::core::DataAccessorPtr terrama2::core::DataAccessorDCPCSV::make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
{
  return std::make_shared<DataAccessorDCPCSV>(dataProvider, dataSeries);
}

