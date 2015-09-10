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
  \file terrama2/core/DataSet.hpp

  \brief Metadata about a given dataset.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATASET_HPP__
#define __TERRAMA2_CORE_DATASET_HPP__



// STL
#include <memory>
#include <string>
#include <cstdint>



namespace terrama2
{
  namespace core
  {

    class DataProvider;
    typedef std::shared_ptr<DataProvider> DataProviderPtr;

    /*!
      \class DataSet

      \brief Contains metadata about data servers.

      A data provider can be a remote server that provides data through
      FTP protocol or an OGC Web Service, such as WFS, WCS or SOS.

      It can also be an URI for a folder into the file system...
     */
    class DataSet
    {
      public:

      //! Dataset type.
      enum Kind
      {
        UNKNOWN_TYPE,
        PCD_TYPE,
        OCCURENCE_TYPE,
        GRID_TYPE
      };

      //! Dataset status.
      enum Status
      {
        ACTIVE,
        INACTIVE
      };

      DataSet(DataProviderPtr provider);

      ~DataSet();

      uint64_t        id()              const {return id_;}
      DataProviderPtr getDataProvider() const { return provider_; }

      private:

        /*!
          \brief

          \param id
         */
        void setId(uint64_t id);

      private:

        uint64_t id_;
        std::string name_;
        std::string description_;
        Status status_;
        DataProviderPtr provider_;
        Kind kind_;
        double dataFrequency_;
        //UOM dataFrequencyUnit_;    //TODO: is this needed?
        //Time schedule_;            //TODO: is this needed?
        double scheduleRetry_;
        //UOM scheduleRetryUnit_;    //TODO: is this needed?
        double scheduleTimeout_;
        //UOM scheduleTimeoutUnit_;  //TODO: is this needed?

      friend class DataSetDAO; //review
    };

    typedef std::shared_ptr<DataSet> DataSetPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASET_HPP__

