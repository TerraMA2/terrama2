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
  \file terrama2/impl/DataRetrieverFTP.hpp

  \brief Data Retriever FTP.

 \author Evandro Delatin
*/


#ifndef __TERRAMA2_IMPL_DATARETRIEVERFTP_HPP__
#define __TERRAMA2_IMPL_DATARETRIEVERFTP_HPP__

// STL
#include <memory>
#include <cassert>

// TerraMA2
#include "../core/data-access/DataRetriever.hpp"
#include "../core/shared.hpp"

// LibCurl
#include <curl/curl.h>

//Qt
#include <QTemporaryDir>

namespace terrama2
{
  namespace core
  {
    /*!
       * \brief The DataRetrieverFTP class performs the download of
       * occurrences of files, PCD-TOA5, PCD_INPE, GRADES ETA15km.
       *
       * \warning The DataRetrieverFTP class only performs the download of files, but does not
       * perform the removal of the files after downloading.
    */
    class DataRetrieverFTP: public DataRetriever
    {
    public:
      /*!
       * \brief DataRetrieverFTP Constructor:
       *  Initializes the Curl and check the URL to download.
       * \param dataprovider Dataprovider information.
       * \param scheme information. Ex. "file://".
       * \param temporaryFolder Folder information where the files will be saved. Ex. "/tmp/".
       */
        explicit DataRetrieverFTP(DataProviderPtr dataprovider);

      /*!
       * \brief Destructor - When Data Retrieve FTP destructor is called, it runs the removal of the temporary folder files.
       *
       */
      virtual ~DataRetrieverFTP();

      //comments on parent
      virtual bool isRetrivable() const noexcept override;

      /*!
         * \brief Retrieving remote data from FTP servers.
         * \param Filter to the data files.
         * \param Datasetitem datasetitem information.
         * \param Log_uris log information.
         * \return Returns the absolute path of the folder that contains the files that have been made the download.
         * \exception DataRetrieverError when could not perform the download files.
         * \exception DataRetrieverError when Unknown error, Could not perform the download files.
         */     
        virtual std::string retrieveData(const std::string& mask, const Filter& filter) override;

      /*!
       * \brief write_response - data to be written in file.
       * Define our callback to get called when there's data to be written in file.
       * \param ptr - pointer to the data stream.
       * \param size - byte length of each data element.
       * \param nmemb - data elements.
       * \param data - data stream.
       * \return Returns the number of items that were successfully read.
       */
      static size_t write_response(void *ptr, size_t size, size_t nmemb, void *data);

      /*!
       * \brief write_vector - data to be written in vector.
       * Define our callback to get called when there's data to be written in vector.
       * \param ptr - pointer to the data stream.
       * \param size - byte length of each data element.
       * \param nmemb - data elements.
       * \param data - data stream.
       * \return Returns the number of items that were successfully read.
       */
      static size_t write_vector(void *ptr, size_t size, size_t nmemb, void *data);

    private:
      std::vector<std::string> vectorNames_; //! vector filtered names.
      std::string scheme_; //! scheme information. Ex. "file://".
      std::string temporaryFolder_; //! Folder information where the files will be saved. Ex. "/tmp/".
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;//!< Shared pointer to a DataRetriever.
  }
}

#endif //__TERRAMA2_IMPL_DATARETRIEVERFTP_HPP__

