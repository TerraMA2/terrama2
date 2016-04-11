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
  \file terrama2/core/data-access/DataAccessorOccurrence.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataAccessorOccurrence.hpp"
#include "DataRetriever.hpp"
#include "../utility/Logger.hpp"
#include "../Exception.hpp"

//TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/memory/DataSet.h>

//Qt
#include <QObject>

terrama2::core::OccurrenceSeriesPtr terrama2::core::DataAccessorOccurrence::getOccurrenceSeries(const Filter& filter)
{
  auto series = getSeries(filter);
  OccurrenceSeriesPtr occurrenceSeries = std::make_shared<OccurrenceSeries>();
  occurrenceSeries->addOccurrences(series);

  return occurrenceSeries;
}

te::dt::TimeInstantTZ terrama2::core::DataAccessorOccurrence::lastDateTime() const
{
  //TODO: implement lastDateTime
  assert(0);
}
