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
  \file terrama2/collector/StoragerPostgis.hpp

  \brief Store a temporary terralib DataSet into the permanent PostGis storage area.

  \author Jano Simas
*/

#ifndef __TERRAMA2_COLLECTOR_STORAGERPOSTGIS_HPP__
#define __TERRAMA2_COLLECTOR_STORAGERPOSTGIS_HPP__

#include "Storager.hpp"

//Terralib
#include <terralib/dataaccess/datasource/DataSource.h>

namespace terrama2
{
  namespace collector
  {

    //! \brief The StoragerPostgis class specializes the Storager class to Postgres/Postgis DB.
    class StoragerPostgis : public Storager
    {
    public:
      //! Constructor. Stores metadata for later use.
      StoragerPostgis(const std::map<std::string, std::string>& storageMetadata);

      //Documented in parent class
      virtual void store(std::vector<TransferenceData>& transferenceDataVec) override;

    private:
      //! Internal method that commits data to the final storage area.
      void commitData(const std::string& destinationDataSetName,
                      std::shared_ptr<te::da::DataSource> datasourceDestination,
                      std::vector<TransferenceData>& transferenceDataVec);

    };
  }
}


#endif //__TERRAMA2_COLLECTOR_STORAGERPOSTGIS_HPP__
