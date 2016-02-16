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
  \file terrama2/collector/ParserPcdCemaden.hpp

  \brief Parser of Pcd of Cemaden

  \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_PARSERPCDCEMADEN_HPP__
#define __TERRAMA2_COLLECTOR_PARSERPCDCEMADEN_HPP__

// TerraMA2
#include "Parser.hpp"

namespace terrama2
{
  namespace collector
  {
    /*!
       \brief The ParserPcdCemaden class is responsible for making the parser of files in JSON format
              of pluviometric stations and hydrological the WebService CEMADEN.
     */
    class ParserPcdCemaden : public Parser
    {
        /*!
         \brief Reads the data refered in the uri and converts to a te::da::DataSet.

         If the datatype is known to the parser, it will identify the data and convert to better data types.
         ex: datetime string to DateTime format and lat, long strings to point geometry.

         \param filter Used to identify the file/table names to be collected.
         \param transferenceDataVec Will be populated with parsed datasets.

         \exception ParserPcdCemadenException when could not perform the download files.
         */

      public:
        virtual void read(DataFilterPtr filter,
                          std::vector<terrama2::collector::TransferenceData>& transferenceDataVec) override;

      /*!
         * \brief write_data - data to be written in stringstream.
         * Define our callback to get called when there's data to be written in stringstream.
         * \param ptr - pointer to the data stream.
         * \param size - byte length of each data element.
         * \param nmemb - data elements.
         * \param stream - data stream.
         * \return Returns the number of items that were successfully read.
       */
        static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

    };
  }
}

#endif // __TERRAMA2_COLLECTOR_PARSERPCDCEMADEN_HPP__
