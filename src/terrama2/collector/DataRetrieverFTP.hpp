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
  \file terrama2/collector/DataRetrieverFTP.hpp

  \brief 

 \author Jano Simas
 \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_DATARETRIEVERFTP_HPP__
#define __TERRAMA2_COLLECTOR_DATARETRIEVERFTP_HPP__

// STL
#include <memory>
#include <cassert>

// TerraMA2
#include "DataRetriever.hpp"

// LibCurl
#include <curl/curl.h>

/*!
     * \brief The DataRetrieverFTP class performs the download of occurrences of files, PCD-TOA5, PCD_INPE, GRADES ETA15km.
     */

namespace terrama2
{
  namespace collector
  {
    class DataFilter;
    typedef std::shared_ptr<DataFilter> DataFilterPtr;

    class DataRetrieverFTP: public DataRetriever
    {
    public:
      explicit DataRetrieverFTP(const core::DataProvider& dataprovider);

      virtual void open() override;
      virtual bool isOpen() override;
      virtual void close() override;

      virtual std::string retrieveData(const terrama2::core::DataSetItem& datasetitem, DataFilterPtr filter, std::vector<std::string>& log_uris) override;

    private:
      CURL* curl;
      FILE* ftpfile;
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;
  }
}

#endif //__TERRAMA2_COLLECTOR_DATARETRIEVERFTP_HPP__
