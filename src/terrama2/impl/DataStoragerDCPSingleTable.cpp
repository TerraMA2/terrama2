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
  \file terrama2/core/data-access/DataStoragerDCPSingleTable.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataStoragerDCPSingleTable.hpp"

#include "../core/data-model/DataSeries.hpp"

#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/datatype/SimpleData.h>

std::string terrama2::core::DataStoragerDCPSingleTable::getDataSetName(terrama2::core::DataSetPtr /*dataSet*/) const
{
  return "dcp_data_"+std::to_string(dataSeries_->id);
}

void terrama2::core::DataStoragerDCPSingleTable::adapt(terrama2::core::DataSetSeries& dataSetSeries, DataSetPtr outputDataSet) const
{
  auto memDataSet = std::static_pointer_cast<te::mem::DataSet>(dataSetSeries.syncDataSet->dataset());

  auto idProperty = new te::dt::StringProperty("dcp_id", te::dt::STRING);
  dataSetSeries.teDataSetType->add(idProperty);
  std::unique_ptr<te::dt::SimpleData<std::string> > abstractId(new te::dt::SimpleData<std::string>(std::to_string(outputDataSet->id)));
  memDataSet->add(idProperty->getName(), te::dt::STRING_TYPE, abstractId.get());
}
