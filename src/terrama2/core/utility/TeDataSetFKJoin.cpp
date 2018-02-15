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
  \file terrama2/services/alert/core/Alert.hpp

  \brief

  \author Jano Simas
 */

#include "TeDataSetFKJoin.hpp"
#include "Logger.hpp"
#include "../Exception.hpp"

#include <terralib/dataaccess/dataset/ForeignKey.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/PrimaryKey.h>

#include <QString>
#include <QObject>

terrama2::core::TeDataSetFKJoin::TeDataSetFKJoin( std::shared_ptr<te::da::DataSetType> referrerDataSetType,
                                                  std::shared_ptr<te::da::DataSet> referrerDataSet,
                                                  std::string referrerAttribute,
                                                  std::shared_ptr<te::da::DataSetType> referredDataSetType,
                                                  std::shared_ptr<te::da::DataSet> referredDataSet,
                                                  std::string referredAttribute)
 :  _referrerDataSetType(referrerDataSetType),
    _referrerDataSet(referrerDataSet),
    _referrerPropertyName(referrerAttribute),
    _referredDataSetType(referredDataSetType),
    _referredDataSet(referredDataSet)
{
  fillPKMap(referredAttribute, referredDataSet);
}

void terrama2::core::TeDataSetFKJoin::fillPKMap(std::string referredPropertyName, std::shared_ptr<te::da::DataSet> referredDataSet)
{
  for (size_t i = 0; i < referredDataSet->size(); i++) {
    referredDataSet->move(i);

    std::unique_ptr<te::dt::AbstractData> val(referredDataSet->getValue(referredPropertyName));
    if(val)
      _referredPKMap.emplace(val->toString(), i);
    else
    {
      QString errMsg = QObject::tr("Invalid property: %1.").arg(QString::fromStdString(referredPropertyName));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }
  }
}

std::unique_ptr<te::dt::AbstractData> terrama2::core::TeDataSetFKJoin::getValue(std::string propertyName) const
{
  auto fk = _referrerDataSet->getValue(_referrerPropertyName);
  auto pos = _referredPKMap.at(fk->toString());
  _referredDataSet->move(pos);
  return _referredDataSet->getValue(propertyName);
}

te::dt::Property* terrama2::core::TeDataSetFKJoin::getProperty(std::string propertyName) const
{
  return _referredDataSetType->getProperty(propertyName);
}
