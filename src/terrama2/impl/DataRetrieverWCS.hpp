/*
  Copyright (C) 2017 National Institute For Space Research (INPE) - Brazil.

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
  \file terrama2/impl/DataRetrieverWCS.hpp

  \brief Data Retriever WCS.

 \author Ricardo Pontes Bonfiglioli
*/


#ifndef __TERRAMA2_IMPL_DATARETRIEVERWCS_HPP__
#define __TERRAMA2_IMPL_DATARETRIEVERWCS_HPP__

// STL
#include <memory>
#include <cassert>

// Terralib
#include <terralib/core/uri.h>
#include <terralib/ws/ogc/wcs/client/WCSClient.h>


// TerraMA2
#include "../core/data-access/DataRetriever.hpp"
#include "../core/Shared.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
       * \brief The DataRetrieverWCS class performs WCS client retriever
    */
    class DataRetrieverWCS: public DataRetriever
    {
      public:
        /*!
         * \brief DataRetrieverWCS Constructor:
        */
        explicit DataRetrieverWCS(DataProviderPtr dataprovider);

        /*!
         * \brief DataRetrieverWCS Default Destructor.
         *
         */
        virtual ~DataRetrieverWCS();

        te::core::URI getConnectionInfo(std::string& downloadFolder) const;

        void verifyConnectionInfo(const te::core::URI& uri) const;

        //comments on parent
        virtual bool isRetrivable() const noexcept override;

        /*!
         * \brief retrieveData Retrieving remote data from WCS servers.
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
                                         const std::string& foldersMask = "") override;

        static DataRetrieverPtr make(DataProviderPtr dataProvider);
        static DataRetrieverType dataRetrieverType() { return "WCS"; }

      private:
        std::unique_ptr<te::ws::ogc::WCSClient> wcs_; // WCS handle client
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;//!< Shared pointer to a DataRetriever.
  }
}

#endif //__TERRAMA2_IMPL_DATARETRIEVERWCS_HPP__
