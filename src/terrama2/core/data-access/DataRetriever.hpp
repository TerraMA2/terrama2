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
  \file terrama2/core/data-access/DataRetriever.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_RETRIEVER_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_RETRIEVER_HPP__

//TerraMA2
#include "../../Config.hpp"
#include "../data-model/DataProvider.hpp"
#include "../data-model/Filter.hpp"
#include "../utility/FileRemover.hpp"

#include <functional>

//terralib
#include <terralib/datatype/TimeInstantTZ.h>

namespace terrama2
{
  namespace core
  {
    typedef std::string DataRetrieverType;
    /*!
    \brief DataRetriever provides an interface to download files from, usualy, remote servers.

    ## Retrieving files ##

    After you have a DataRetriever object, calling retrieveData() should be enough
    to get the files. The \e query parameter is defined by the driver, it can be a FTP uri or a WCS query for example, details
    of the parameters can be found in the appropriate driver class.

    The return value is the uri where the downloaded files are stored.

    \note The best way to get a DataRetriever is from a DataRetrieverFactory,
    the DataRetrieverFactory::make will return a DataRetriever from the right type.

    \note A DataRetriever should be used from inside DataAccessor::getSeries

    ## Derived classes ##

    Derived classes should access a remote server and download files to a temporary storage
    and return a uri to this file.

    */
    class DataRetriever
    {
      public:
        /*!
         \brief Default constructor.
         \exception DataRetrieverException Reaised if the DataProvider is NULL.
         */
        DataRetriever(DataProviderPtr dataProvider);
        //! Default destructor.
        virtual ~DataRetriever() = default;
        //! Default copy constructor
        DataRetriever(const DataRetriever& other) = default;
        //! Default move constructor
        DataRetriever(DataRetriever&& other) = default;
        //! Default const assignment operator
        DataRetriever& operator=(const DataRetriever& other) = default;
        //! Default assignment operator
        DataRetriever& operator=(DataRetriever&& other) = default;

        //! Utility method to construct a DataRetriever, used as a callback in the DataRetreiverFactory.
        static DataRetriever* make(DataProviderPtr dataProvider);

        /*!
          \brief Downloads the remote file to a temporary location.

          This method is overloaded by derived classes, the default behavior is to raise an exception.

          \warning This method depends the data to be downloadable. see DataRetriever::isRetrivable()

          \exception NotRetrivableException Raised when this DataRetriever doesn't allow the download of the data.
          This will happen based on the DataProviderType.

          \return Uri to the termporary file
        */
        virtual std::string retrieveData(const std::string& query,
                                         const Filter& filter,
                                         const std::string& timezone,
                                         std::shared_ptr<terrama2::core::FileRemover> remover,
                                         const std::string& temporaryFolder = "",
                                         const std::string& folderPath = "") const;

         /*!
           \brief Downloads the remote file to a temporary location.

           This method is overloaded by derived classes, the default behavior is to raise an exception.

           \warning This method depends the data to be downloadable. see DataRetriever::isRetrivable()

           \exception NotRetrivableException Raised when this DataRetriever doesn't allow the download of the data.
           This will happen based on the DataProviderType.

           The processFile callback takes the uri of the temporary folder as a parameter
         */
        void retrieveDataCallback(const std::string& mask,
                                  const Filter& filter,
                                  const std::string& timezone,
                                  std::shared_ptr<terrama2::core::FileRemover> remover,
                                  const std::string& temporaryFolderUri,
                                  const std::string& foldersMask,
                                  std::function<void(const std::string& /*uri*/)> processFile) const;

      /*!
        \brief Downloads the remote file to a temporary location.

        This method is overloaded by derived classes, the default behavior is to raise an exception.

        \warning This method depends the data to be downloadable. see DataRetriever::isRetrivable()

        \exception NotRetrivableException Raised when this DataRetriever doesn't allow the download of the data.
        This will happen based on the DataProviderType.

        The processFile callback takes the uri of the temporary folder and the downloaded filename as a parameter
      */
        virtual void retrieveDataCallback(const std::string& /*mask*/,
                                          const Filter& /*filter*/,
                                          const std::string& /*timezone*/,
                                          std::shared_ptr<terrama2::core::FileRemover> /*remover*/,
                                          const std::string& /*temporaryFolderUri*/,
                                          const std::string& /*foldersMask*/,
                                          std::function<void(const std::string& /*uri*/, const std::string& /*filename*/)> /*processFile*/) const;

        //! Returns the last data timestamp found on last access.
        virtual te::dt::TimeInstantTZ lastDateTime() const;

        /*!
          \brief Returns true if the data should be downloaded to a file or false if should be accessed directly.

          \exception NotRetrivableException Raised when this DataRetriever doesn't allow the download of the data.
          This will happen based on the DataProviderType
        */
        virtual bool isRetrivable() const;

      protected:
        std::string getTemporaryFolder(std::shared_ptr<terrama2::core::FileRemover> remover, const std::string& oldTempTerraMAFolder = "") const;

        DataProviderPtr dataProvider_;//!< Information of the remote server.
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_RETRIEVER_HPP__
