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
  \file terrama2/core/data-access/DataAccessorDcp.hpp

  \brief Base class to access data from a DCP DataSeries.

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_HPP__

#include <algorithm>
#include <memory>

#include "../Config.hpp"
//TerraMA2
#include "../Shared.hpp"
#include "../Config.hpp"
#include "DataAccessor.hpp"
#include "DcpSeries.hpp"

namespace terrama2 {
namespace core {
class FileRemover;
struct Filter;
}  // namespace core
}  // namespace terrama2

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorDcp
      \brief Base class to access data from a DCP DataSeries.

      DCP are Data Collecting Plataforms, here we consider that:
       - It's fixed, one static geographical position associated
       - The data has a timestamp attribute

      A weather station is an example of DCP.

    */
    class TMCOREEXPORT DataAccessorDcp : public virtual DataAccessor
    {
      public:
        //! Default constructor
        DataAccessorDcp(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
         : DataAccessor(dataProvider, dataSeries) {}
        //! Default destructor.
        virtual ~DataAccessorDcp() = default;
        //! Default copy constructor
        DataAccessorDcp(const DataAccessorDcp& other) = default;
        //! Default move constructor
        DataAccessorDcp(DataAccessorDcp&& other) : DataAccessor(std::move(other)){}
        //! Default const assignment operator
        DataAccessorDcp& operator=(const DataAccessorDcp& other) = default;
        //! Default assignment operator
        DataAccessorDcp& operator=(DataAccessorDcp&& other) { DataAccessor::operator=(std::move(other)); return *this; }

        //! Returns a DcpSeries filtered by Filter
        virtual DcpSeriesPtr getDcpSeries(const Filter& filter, std::shared_ptr<terrama2::core::FileRemover> remover);

      protected:
        // Doc in base class
        virtual bool intersects(DataSetPtr dataset, const Filter& filter) const override;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_DCP_HPP__
