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

  \brief Data Retriever FTP.

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


namespace terrama2
{
  namespace collector
  {
    class DataFilter;
    typedef std::shared_ptr<DataFilter> DataFilterPtr;

    /*!
       * \brief The DataRetrieverFTP class performs the download of
       * occurrences of files, PCD-TOA5, PCD_INPE, GRADES ETA15km.
    */

    class DataRetrieverFTP: public DataRetriever
    {
    public:
        /*!
         * \brief  Constructor DataRetrieverFTP
         * \param DataProvider dataprovider information.
         * \param Localization localization information. Ex. "file://".
         * \param Folder folder information where the files will be saved. Ex. "/tmp/".
         */
      explicit DataRetrieverFTP(const core::DataProvider& dataprovider, const std::string scheme = "file://", const std::string temporaryFolder = "/tmp/terrama2/");

      virtual bool isRetrivable() const noexcept override;

      //! Does nothing. In derived classes opens the connectin to the server.
      virtual void open() override;
      //! Initializes the Curl and check the URL to download.
      virtual bool isOpen() override;
      //! Does nothing. In derived classes closes the connection to the server.
      virtual void close() override;      
        /*!
         * \brief Retrieving remote data from FTP servers.
         * \param Filter to the data files.
         * \param Datasetitem datasetitem information.
         * \param Log_uris log information.
         * \return Returns the absolute path of the folder that contains the files that have been made the download.
         * \exception DataRetrieverError when could not perform the download files.
         * \exception DataRetrieverError when Unknown error, Could not perform the download files.
         */
      virtual std::string retrieveData(const terrama2::core::DataSetItem& datasetitem, DataFilterPtr filter, std::vector<terrama2::collector::TransferenceData>& transferenceDataVec) override;

        /*!
         * \brief Destructor - When Data Retrieve FTP destructor is called, it runs the removal of the temporary folder files.
         *
         */
     ~DataRetrieverFTP();

    private:
      std::vector<std::string> vectorNames_;
      std::string scheme_;
      std::string temporaryFolder_;
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;
  }
}

#endif //__TERRAMA2_COLLECTOR_DATARETRIEVERFTP_HPP__
