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
  \file terrama2/core/data-access/DataRetriever.cpp

  \brief

  \author Jano Simas
 */

#include "DataRetriever.hpp"
#include "../utility/Logger.hpp"
#include "../Exception.hpp"

//Qt
#include <QString>
#include <QObject>

terrama2::core::DataRetriever::DataRetriever(DataProviderPtr dataProvider)
  : dataProvider_(dataProvider)
{
  if(!dataProvider_.get())
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataRetrieverException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataRetriever::retrieveData(const std::string& query,
                                                        const Filter& filter,
                                                        std::shared_ptr<terrama2::core::FileRemover> remover,
                                                        const std::string& temporaryFolder)
{
  QString errMsg = QObject::tr("Non retrievable DataRetriever.");
  throw NotRetrivableException() << ErrorDescription(errMsg);
}

te::dt::TimeInstantTZ terrama2::core::DataRetriever::lastDateTime() const
{
  QString errMsg = QObject::tr("Non retrievable DataRetriever.");
  throw NotRetrivableException() << ErrorDescription(errMsg);
}

bool terrama2::core::DataRetriever::isRetrivable() const
{
  return false;
}
terrama2::core::DataRetriever* terrama2::core::DataRetriever::make(DataProviderPtr dataProvider)
{
  return new DataRetriever(dataProvider);
}
