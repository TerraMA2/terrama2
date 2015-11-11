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
  \file terrama2/collector/ParserPostgis.hpp

  \brief Parsers postgres/postgis data and create a terralib DataSet.

  \author Jano Simas
*/

#ifndef __TERRAMA2_COLLECTOR_PARSERPOSTGIS_HPP__
#define __TERRAMA2_COLLECTOR_PARSERPOSTGIS_HPP__

#include "Parser.hpp"

#include <mutex>

namespace terrama2
{
  namespace collector
  {

    class ParserPostgis : public Parser
    {
      public:
        ParserPostgis() : Parser(){}
        virtual ~ParserPostgis(){}

        /*!
         * \brief TODO: document ParserPostgis
         * \param uri
         * \return Dataset of the temporary file.
         *
         * \pre TerrLib should be initialized.
         *
         * \exception UnableToReadDataSetError Raised if could not open the datasource, dataset is empty, terralib exception.
         */
        virtual void read(const std::string& uri,
                          DataFilterPtr filter,
                          std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                          std::shared_ptr<te::da::DataSetType> &datasetTypePtr) override;

    };
  }
}



#endif //__TERRAMA2_COLLECTOR_PARSERPOSTGIS_HPP__
