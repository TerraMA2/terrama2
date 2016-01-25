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
  \file terrama2/collector/DataRetrieverWCS.hpp

  \brief Data Retriever WCS.

 \author Vinicius Campanha
*/


#ifndef __TERRAMA2_COLLECTOR_DATARETRIEVERWCS_HPP__
#define __TERRAMA2_COLLECTOR_DATARETRIEVERWCS_HPP__

// STL


// TerraMA2
#include "DataRetriever.hpp"



namespace terrama2
{
  namespace collector
  {
    class DataFilter;
    typedef std::shared_ptr<DataFilter> DataFilterPtr;

    /*!
       * \brief
    */
    class DataRetrieverWCS: public DataRetriever
    {
    public:
      /*!
       * \brief  Constructor DataRetrieverWCS
       * \param DataProvider dataprovider information.
       */
      explicit DataRetrieverWCS(const core::DataProvider& dataprovider);

      /*! \brief Returns if the data should be retrieved or not.

          Local files and wms data don't need to be retrieved,
          data from WCS server need to be retrieved.
      */
      virtual bool isRetrivable() const noexcept;

      //! Does nothing. In derived classes opens the connectin to the server.
      virtual void open() override;
      //! Initializes the Curl and check if the FTP address is valid.
      virtual bool isOpen() override;
      //! Does nothing. In derived classes closes the connection to the server.
      virtual void close() override;
        /*!
         * \brief Retrieving remote data from WCS servers.
         * \param Filter to the data files.
         * \param Datasetitem datasetitem information.
         * \param Log_uris log information.
         * \return Returns the absolute path of the folder that contains the files that have been made the download.
         * \exception DataRetrieverError when could not perform the download files.
         * \exception DataRetrieverError when Unknown error, Could not perform the download files.
         */
      virtual std::string retrieveData(const terrama2::core::DataSetItem& datasetitem, DataFilterPtr filter, std::vector<terrama2::collector::TransferenceData>& transferenceDataVec) override;

    private:
      std::string folder_;


    };


  }
}

#endif //__TERRAMA2_COLLECTOR_DATARETRIEVERWCS_HPP__
