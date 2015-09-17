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
  \file terrama2/core/Data.hpp

  \brief Metadata about a given data.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATA_HPP__
#define __TERRAMA2_CORE_DATA_HPP__

// STL
#include <memory>
#include <string>


namespace terrama2
{
  namespace core
  {

    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    /*!
      \class Data

      \brief Contains metadata about a data.

      A data can be a INPE Format PCD, TOA5 PCD, an occurrence of fire or an occurrence of diseases.

     */
    class Data
    {
      public:

        //! Data type.
        enum Kind
        {
          UNKNOWN_TYPE,
          PCD_INPE_TYPE,
          PCD_TOA5_TYPE,
          FIRE_POINTS_TYPE,
          DISEASE_OCCURRENCE_TYPE
        };

        //! Data status.
        enum Status
        {
          ACTIVE,
          INACTIVE
        };


      public:

        /*!
          \brief Constructor.
        */
        Data(DataSetPtr dataSet, Kind kind);

        /*!
          \brief Destructor.
        */
        virtual ~Data();

        /*!
          \brief It returns the identifier of the data.

          \return The identifier of the data.
        */
        uint64_t id() const;

        /*!
          \brief It returns the the kind of the data.

          \return The kind of the data.
        */
        Kind kind() const;

        /*!
          \brief It sets the the kind of the data.

          \param The kind of the data.
        */
        void setKind(const Kind& kind);

        /*!
          \brief It returns the the status of the data.

          \return The status of the data.
        */
        Status status() const;

        /*!
          \brief It sets the the status of the data.

          \param The status of the data.
        */
        void setStatus(const Status& status);

        /*!
          \brief It returns the mask of the data.

          \return The mask of the data.
        */
        std::string mask() const;

        /*!
          \brief It sets the mask of the data.

          \param The mask of the data.
        */
        void setMask(const std::string& mask);

        /*!
          \brief It returns the timezone of the data.

          \return The timezone of the data.
        */
        std::string timezone() const;

        /*!
          \brief It sets the timezone of the data.

          \param The timezone of the data.
        */
        void setTimezone(const std::string& timezone);

        /*!
          \brief It returns the the dataset.

          \return The the dataset.
        */
        DataSetPtr dataSet() const;


      protected:

        /*!
          \brief It sets the identifier of the data.

          \param The identifier of the data.
        */
        void setId(uint64_t id);


      private:

        uint64_t id_;
        Status status_;
        DataSetPtr dataSet_;
        Kind kind_;
        std::string mask_;
        std::string timezone_;

      friend class DataSetDAO;
    };

    typedef std::shared_ptr<Data> DataPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATA_HPP__

