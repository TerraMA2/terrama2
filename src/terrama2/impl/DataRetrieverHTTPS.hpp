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
  \file terrama2/impl/DataRetrieverHTTPS.hpp

  \brief Data Retriever HTTPS.

 \author Jean Souza
*/

#ifndef __TERRAMA2_IMPL_DATARETRIEVERHTTPS_HPP__
#define __TERRAMA2_IMPL_DATARETRIEVERHTTPS_HPP__

// TerraMA2
#include "./DataRetrieverHTTP.hpp"
#include "../core/utility/CurlWrapperHttp.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
       * \brief The DataRetrieverHTTPS class performs the download of
       * occurrences of files, DCP-TOA5, DCP-INPE, GRADES ETA15km.
       *
       * \warning The DataRetrieverHTTPS class only performs the download of files, but does not
       * perform the removal of the files after downloading.
    */
    class TMIMPLEXPORT DataRetrieverHTTPS: public DataRetrieverHTTP
    {
      public:
        /*!
         * \brief DataRetrieverHTTPS Constructor:
         * \param dataprovider dataprovider Dataprovider information.
         * \param curlwrapper curlwrapper CurlWrapperHttp.
         * \exception DataRetrieverException when HTTPS address is invalid.
         * \exception DataRetreiverHTTPSException when unknown Error, HTTPS address is invalid.
        */
        explicit DataRetrieverHTTPS(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperHttp>&& curlwrapper);

        /*!
         * \brief DataRetrieverHTTPS Default Destructor.
         *
         */
        virtual ~DataRetrieverHTTPS();

        static DataRetrieverPtr make(DataProviderPtr dataProvider);
        static DataRetrieverType dataRetrieverType() { return "HTTPS"; }
      private:
        std::unique_ptr<CurlWrapperHttp> curlwrapper_; //!< Curl handler.
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;//!< Shared pointer to a DataRetriever.
  }
}

#endif //__TERRAMA2_IMPL_DATARETRIEVERHTTPS_HPP__
