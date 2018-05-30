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
#include "Config.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/CurlWrapperFtp.hpp"
#include "../core/data-access/DataRetriever.hpp"
#include "../core/Shared.hpp"

// LibCurl
#include <curl/curl.h>

// Qt
#include <QTemporaryDir>

namespace terrama2
{
  namespace core
  {
    /*!
       * \brief The DataRetrieverFTP class performs the download of
       * occurrences of files, DCP-TOA5, DCP-INPE, GRADES ETA15km.
       *
       * \warning The DataRetrieverFTP class only performs the download of files, but does not
       * perform the removal of the files after downloading.
    */
    class TMIMPLEXPORT DataRetrieverFTP: public DataRetriever
    {
      public:
        /*!
         * \brief DataRetrieverFTP Constructor:
         * Initializes the Curl and check the URL to download.
         * Initializes scheme information. Ex. "file://".
         * Initializes temporaFolder Folder information where the files will be saved. Ex. "/tmp/".
         * Create the directory where you will download the files. Ex. "/tmp/terrama2-download/".
         * \param dataprovider dataprovider Dataprovider information.
         * \exception DataRetrieverException when FTP address is invalid.
         * \exception DataRetreiverFTPException when unknown Error, FTP address is invalid.
        */
        explicit DataRetrieverFTP(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperFtp>&& curlwrapper);

        /*!
         * \brief DataRetrieverFTP Default Destructor.
         *
         */
        virtual ~DataRetrieverFTP();

        //comments on parent
        virtual bool isRetrivable() const noexcept override;

        /*!
         * \brief retrieveData Retrieving remote data from FTP servers.
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

        /*!
         * \brief Check if the URIs and their subfolders matches the folders mask.
         * \param uris The list of URIs to check
         * \param foldersMask The folders mask
         * \return A list with the full path of the URIs that matched the folders mask.
         */
        virtual std::vector<std::string> getFoldersList(const std::vector<std::string>& uris,
                                                        const std::string& foldersMask) const;

        /*!
         * \brief Receives a list of URIs, check if theirs subfolders match with the mask and returns
         * the full path of the ones that match.
         * \param baseURIs List of URIs to check the subfolders
         * \param mask The mask
         * \return Returns the full path of the subfolders that match the mask.
         */
        virtual std::vector<std::string> checkSubfolders(const std::vector<std::string> baseURIs,
                                                         const std::string mask) const;

        static DataRetrieverPtr make(DataProviderPtr dataProvider);
        static DataRetrieverType dataRetrieverType() { return "FTP"; }

      private:
        std::unique_ptr<CurlWrapperFtp> curlwrapper_; //!< Curl handler.
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;//!< Shared pointer to a DataRetriever.
  }
}

#endif //__TERRAMA2_IMPL_DATARETRIEVERFTP_HPP__
