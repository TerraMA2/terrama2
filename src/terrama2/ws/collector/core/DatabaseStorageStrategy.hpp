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
  \file terrama2/ws/collector/core/DatabaseStorageStrategy.hpp

  \brief Defines the storage strategy for database

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_DATABASE_STORAGE_STRATEGY_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_DATABASE_STORAGE_STRATEGY_HPP__

#include "StorageStrategy.hpp"
#include "Collector.hpp"


// STL
#include <memory>
#include <string>

// QT
#include <QJsonObject>

// terralib
#include <terralib/dataaccess.h>


namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {

        class DatabaseStorageStrategy : public StorageStrategy
        {
        public:

          /*!
          * \brief Constructor.
          */
          DatabaseStorageStrategy(const std::string& storageName, const bool& uniqueStorage);

          /*!
          * \brief Destructor
          */
          virtual ~DatabaseStorageStrategy();

          /*!
            \brief It returns the storage name.
          
            \return The storage name.
          */
          std::string getStorageName() const;
           
          /*!
            \brief It sets the storage name.
          
            \param The storage name.
          */
          void setStorageName(const std::string& storageName);

          /*!
            \brief It returns ff all collected data of this collector should be stored in the same table..
          
            \return If all collected data of this collector should be stored in the same table.
          */
          bool getUniqueStorage() const;
           
          /*!
            \brief It sets the attribute unique storage .
          
            \param If all collected data of this collector should be stored in the same table.
          */
          void setUniqueStorage(const bool& uniqueStorage);

          /*!
          * \brief Method to store the collected file, should be implemented by each subclass.
          */
          void store(std::shared_ptr<te::da::DataSet> dataSet, const Collector& collector);

        protected:          
          std::string storageName_;
          bool uniqueStorage_;          

        };      
      } // core    
    } // collector
  } // ws
} // terrama2

#endif // __TERRAMA2_WS_COLLECTOR_CORE_DATABASE_STORAGE_STRATEGY_HPP__

