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
  \file terrama2/impl/DataRetrieverStaticHTTP.hpp

  \brief Data Retriever HTTP.

 \author Jean Souza
*/


#ifndef __TERRAMA2_IMPL_DATARETRIEVER_STATIC_HTTP_HPP__
#define __TERRAMA2_IMPL_DATARETRIEVER_STATIC_HTTP_HPP__

// TerraMA2
#include "Config.hpp"
#include "DataRetrieverHTTP.hpp"
#include "../core/Shared.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
       * \brief The DataRetrieverJsonRest class performs the download of text files from a static address
    */
    class TMIMPLEXPORT DataRetrieverStaticHTTP: public DataRetrieverHTTP
    {
      public:
        /*!
         * \brief DataRetrieverHTTP Constructor
         *
         * Initializes the Curl and check the URL to download.
         * Initializes scheme information. Ex. "file://".
         * Initializes temporaFolder Folder information where the files will be saved. Ex. "/tmp/".
         * Create the directory where you will download the files. Ex. "/tmp/terrama2-download/".
         * \param dataprovider dataprovider Dataprovider information.
         * \exception DataRetrieverException when HTTP address is invalid.
         * \exception DataRetreiverHTTPException when unknown Error, HTTP address is invalid.
        */
        explicit DataRetrieverStaticHTTP(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperHttp>&& curlwrapper);

        /*!
         * \brief DataRetrieverHTTP Default Destructor.
         *
         */
        virtual ~DataRetrieverStaticHTTP() = default;

        /*!
         * \brief retrieveData Retrieving remote data from HTTP servers.
         * \param mask Mask to the data files.
         * \param filter Filter to the data files.
         * \return Returns the absolute path of the folder that contains the files that have been made the download.
         * \exception DataRetrieverException when could not perform the download files.
         * \exception DataRetrieverException when Unknown error, Could not perform the download files.
        */
        virtual std::string retrieveData(const std::string& mask,
                                         const Filter& filter,
                                         const std::string& timezone,
                                         std::shared_ptr<terrama2::core::FileRemover> remover,
                                         const std::string& temporaryFolder = "",
                                         const std::string& foldersMask = "") const override;

      virtual void retrieveDataCallback(const std::string& mask,
                                        const Filter& filter,
                                        const std::string& timezone,
                                        std::shared_ptr<terrama2::core::FileRemover> remover,
                                        const std::string& temporaryFolderUri,
                                        const std::string& foldersMask,
                                        std::function<void(const std::string& /*uri*/, const std::string& /*filename*/)> processFile) const override;

        static DataRetrieverPtr make(DataProviderPtr dataProvider);
        static DataRetrieverType dataRetrieverType() { return "STATIC-HTTP"; }

      private:
        using DataRetrieverHTTP::listFiles;
    };
  }
}

#endif //__TERRAMA2_IMPL_DATARETRIEVER_STATIC_HTTP_HPP__
