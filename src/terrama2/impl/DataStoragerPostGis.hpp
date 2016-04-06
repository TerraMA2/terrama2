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
  \file terrama2/core/data-access/DataStoragerPostGis.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_POSTGIS_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_POSTGIS_HPP__

//TerraMA2
#include "../core/data-access/DataStorager.hpp"
#include "../core/utility/Logger.hpp"

//QT
#include <QString>
#include <QObject>

namespace terrama2
{
  namespace core
  {
    class DataStoragerPostGis : public DataStorager
    {
    public:
      DataStoragerPostGis(DataProviderPtr outputDataProvider)
       : DataStorager(outputDataProvider) {}
      ~DataStoragerPostGis() {}

      static DataStorager* make(DataProviderPtr dataProvider);

      virtual void store(Series series, DataSetPtr outputDataSet) const override;

    protected:
      std::string getTableName(DataSetPtr dataSet) const
      {
        try
        {
          return dataSet->format.at("table_name");
        }
        catch (...)
        {
          QString errMsg = QObject::tr("Undefined table name in dataset: %1.").arg(dataSet->id);
          TERRAMA2_LOG_ERROR() << errMsg;
          throw UndefinedTagException() << ErrorDescription(errMsg);
        }
      }
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_POSTGIS_HPP__
