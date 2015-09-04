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
  \file terrama2/ws/collector/core/Data.hpp

  \brief Represents each data of a dataset

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_DATA_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_DATA_HPP__

#include <string>


namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {

        /*!
         * \brief Enum that contains all data possible data types
         */
        enum DataType
        {
          PCD-INPE, PCD-TOA5, FIRE_POINTS, DISEASE_OCCURRENCE
        }

        /*!
         * \brief Class that represents each data of a dataset
         */
        class Data
        {
        public:

          /*!
          * \brief Constructor
          */
          Data(const std::string& mask);

          /*!
          * \brief Destructor
          */
          ~Data();

          /*!
            \brief It returns the mask.

            \return The mask.
          */
          std::string getMask() const;

          /*!
            \brief It sets the mask.

            \param The mask.
          */
          void setMask(const std::string& mask);

          /*!
          * \brief Method to read and store the collected data
          */
          void import(const std::string& uri);

        protected:
          std::string mask_;
        };
      } // core
    } // collector
  } // ws
} // terrama2



#endif // __TERRAMA2_WS_COLLECTOR_CORE_DATA_HPP__
