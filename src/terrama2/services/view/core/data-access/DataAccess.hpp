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
  \file terrama2/services/view/core/data-access/DataAccess.hpp

  \brief Access data through TerraLib

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_DATAACCESS_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_DATAACCESS_HPP__

// TerraMA2
#include "../../../../core/Shared.hpp"
#include "../../../../core/data-model/Filter.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSetType.h>

//Qt
#include <QFileInfo>

// STL


namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        struct TableInfo
        {
            std::string tableName = "";
            std::string timestampPropertyName = "";

            std::unique_ptr< te::da::DataSetType > dataSetType;
        };

        class DataAccess
        {
          public:
            static QFileInfoList getFilesList(const std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr>& dataSeriesProvider,
                                              const terrama2::core::DataSetPtr dataSet,
                                              const terrama2::core::Filter& filter);

            static te::da::DataSetType* getFileDataSetType(const QFileInfo& fileInfo);

            static te::da::DataSetType* getDataSetType(const std::string& dataSourceURI,
                                                       const std::string& dataSetName,
                                                       const std::string& driver);

            static TableInfo getPostgisTableInfo(const std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr>& dataSeriesProvider,
                                                   const terrama2::core::DataSetPtr dataSet);
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_VIEW_CORE_DATAACCESS_HPP__
