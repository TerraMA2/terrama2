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

//terralib
#include <terralib/datatype/TimeInstantTZ.h>

namespace terrama2
{
  namespace core
  {
    /*!
    \brief Base class to download data from a remote server.

    Derived classes should access a remote server and download files to a temporary storage
    and return a uri to this file.

    */
    class DataRetriever
    {
      public:
        //!< Default constructor.
        DataRetriever(DataProviderPtr dataProvider);
        //!< Default destructor.
        virtual ~DataRetriever() {}

        //!< Utility method to construct a DataRetriever, used as a callback in the DataRetreiverFactory.
        static DataRetriever* make(DataProviderPtr dataProvider);

        /*!
          \brief Downloads the remote file to a temporary location.

          This method is overloaded by derived classes, the default behavior is to raise an exception.

          \warning This method depends the data to be downloadable. see DataRetriever::isRetrivable()

          \exception NotRetrivableException Raised when the DataRetriever doesn't allow the download of the data toa file.

          \return Uri to the termporary file
        */
        virtual std::string retrieveData(const std::string& query, const Filter& filter);

        //! Returns the last data timestamp found on last access.
        virtual te::dt::TimeInstantTZ lastDateTime() const;

        /*!
          \brief Returns true if the data should be downloaded to a file or false if should be access directly.

          \exception NotRetrivableException Raised when the DataRetriever doesn't allow the download of the data toa file.
        */
        virtual bool isRetrivable() const;

      protected:
        DataProviderPtr dataProvider_;//!< Information of the remote server.
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_RETRIEVER_HPP__
