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
  \file terrama2/core/PCD.hpp

  \brief Model for the PCD information.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_PCD_HPP__
#define __TERRAMA2_CORE_PCD_HPP__

// Forward declaration
namespace te
{
  namespace gm
  {
    class Geometry;
  }
}

// TerraMA2
#include "DataSetItem.hpp"

// STL
#include <memory>
#include <inttypes.h>

namespace terrama2
{
  namespace core
  {

    /*!
      \class PCD

      \brief Model for the PCD information.

     */
    class PCD : public terrama2::core::DataSetItem
    {
      public:

        /*! \brief Constructor. */
        PCD(Kind kind = UNKNOWN_TYPE, uint64_t id = 0, uint64_t datasetId = 0);

        /*! \brief Returns the location of the PCD. */
        te::gm::Geometry* location() const;

        /*! \brief Sets the location of the PCD. */
        void setLocation(std::unique_ptr<te::gm::Geometry> location);

      private:
        std::unique_ptr<te::gm::Geometry> location_; //!< Location of the PCD.

    };
  }
}

#endif  // __TERRAMA2_CORE_PCD_HPP__
