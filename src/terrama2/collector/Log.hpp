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
  \file terrama2/collector/Log.cpp

  \brief Manage the log of data handled by collector service

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_COLLECTOR_LOG_HPP__
#define __TERRAMA2_COLLECTOR_LOG_HPP__

// STL
#include <iostream>

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>

// TerraMA2
#include "../core/ApplicationController.hpp"

namespace terrama2
{
  namespace collector
  {
    /*!
      \class Log

      \brief Register the manipulation of data from their source until avaible to analisys.

      The data manipulation log will be stored in a table on database.

      It will register the Data Set Item reponsible by the data, the data source URI,
      the URI where the data is after downloaded or imported, the timestamp that data contains,
      the timestamp that the data was manipulated by TerraMA2 and the status of data
      for each data in data source.

     */
    class Log
    {
      public:

        //! Possible status to collect data.
        enum Status
        {
          //! Data was downloaded but is not avaible to analysis
          DOWNLOADED,
          //! Data is avaible to analysis
          IMPORTED,
          //! The source don't contains data to analysis
          NODATA,
          //! Was not possible to download or import data
          FAILED,
          //! Is not possible to know de data status
          UNKNOW
        };

        /*!
           \brief  Constructor Log
           \param transactor The instance transactor.
         */
        Log(std::shared_ptr < te::da::DataSourceTransactor > transactor);

        /*!
          \brief Register in log table that the Dataset Item has a new data and the these data status

          \param dataSetItemId The DataSetItem ID that has new data
          \param orign_uri The URI from data
          \param s The data status

          \exception LogException

          \return The log ID generated

        */
        virtual uint64_t log(const uint64_t dataSetItemId, const std::string& origin_uri, const Status s) const;

        /*!
          \brief Register in log table that the Dataset Item has new data and the these data status

          \param dataSetItemId The DataSetItem ID that has new data
          \param orign_uris A vector with each URI from each data
          \param s The status of all the data

          \exception LogException

        */
        virtual void log( const uint64_t dataSetItemId, const std::vector< std::string >& origin_uris, const Status s) const;

        /*!
          \brief Update a register in log table

          \param id The register ID to be changed
          \param uri The data uri
          \param s The updated status of the data
          \param data_timestamp The data timestamp

          \exception LogException

        */
        virtual void updateLog(const uint64_t id, const std::string& uri, const Status s, const std::string& data_timestamp) const;

        /*!
          \brief Update registers in log table

          \param origin_uris A vector with URI from each data to be updated
          \param uri The URI where the data is
          \param s The updated status of the data
          \param data_timestamp The data timestamp

          \exception LogException

        */
        virtual void updateLog(const std::vector<std::string>& origin_uris, const std::string& uri, Status s, const std::string& data_timestamp) const;

        /*!
          \brief Get in log table the last data timestamp from a DataSetItem

          \param id The DataSetItem ID
        */
        virtual std::shared_ptr<te::dt::TimeInstantTZ> getDataSetItemLastDateTime(uint64_t id) const;


      private:
        std::shared_ptr< te::da::DataSourceTransactor > transactor_; //!< Contains the instance transactor
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_LOG_HPP__
