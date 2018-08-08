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
  \file terrama2/core/DataSetMapper.cpp

  \brief

  \author Jano Simas
*/

#include <terralib/datatype/AbstractData.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <QObject>
#include <QString>
#include <limits>

#include "../../Exception.hpp"
#include "../Exception.hpp"
#include "DataSetMapper.hpp"
#include "Logger.hpp"


terrama2::core::DataSetMapper::DataSetMapper(std::shared_ptr<te::da::DataSet> dataSet, std::string pkProperty)
  : dataSet_(dataSet)
{
  lineMap_ = generateStringPkToLineMap(dataSet, pkProperty);
}

std::unordered_map<std::string, size_t> terrama2::core::DataSetMapper::generateStringPkToLineMap(std::shared_ptr<te::da::DataSet> dataSet, std::string pkProperty) const
{
  size_t pkPropertyPosition = std::numeric_limits<int>::max();
  for(size_t i = 0; i < dataSet->getNumProperties(); ++i)
  {
    if(dataSet->getPropertyName(i) == pkProperty)
    {
      pkPropertyPosition = i;
      break;
    }
  }

  if(pkPropertyPosition == std::numeric_limits<int>::max())
  {
    QString errMsg = QObject::tr("No property with name %1 found in dataSet.").arg(QString::fromStdString(pkProperty));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }

  std::unordered_map<std::string, size_t> pkMap;
  for(size_t i = 0; i < dataSet->size(); ++i)
  {
    dataSet->move(i);
    pkMap.emplace(dataSet->getValue(pkPropertyPosition)->toString(), i);
  }

  return pkMap;
}

std::unique_ptr< te::dt::AbstractData > terrama2::core::DataSetMapper::getValue(const std::string& key, const std::string& attribute) const
{
  size_t propertyPosition = std::numeric_limits<int>::max();
  for(size_t i = 0; i < dataSet_->getNumProperties(); ++i)
  {
    if(dataSet_->getPropertyName(i) == attribute)
    {
      propertyPosition = i;
      break;
    }
  }

  if(propertyPosition == std::numeric_limits<int>::max())
  {
    QString errMsg = QObject::tr("No property with name %1 found in dataSet.").arg(QString::fromStdString(attribute));
    TERRAMA2_LOG_WARNING() << errMsg;
    return nullptr;
  }

  dataSet_->move(lineMap_.at(key));
  if(dataSet_->isNull(propertyPosition))
    return nullptr;
  else
    return dataSet_->getValue(propertyPosition);
}
