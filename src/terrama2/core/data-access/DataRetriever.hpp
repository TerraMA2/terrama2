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
#include "../Config.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
    \brief Base class to download data from a remote server.

    Derived classes should access a remote server and download files to a temporary storage
    and return a te::core::URI to this file.

    */
    class DataRetriever
    {
    public:
      DataRetriever(DataProvider) {}
      /*!

        \return Uri to the termporary file
      */
      virtual te::core::URI retrieveData(const std::string& query, Filter) = 0;
      virtual te::dt::TimeInstantTZ lastDateTime() = 0;

    protected:
      DataProvider dataProvider_;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_RETRIEVER_HPP__
