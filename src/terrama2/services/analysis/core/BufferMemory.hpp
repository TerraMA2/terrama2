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
  \file terrama2/services/analysis/core/BufferMemory.hpp

  \brief Utility function to create a buffer for a set of geometries.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_ANALYSIS_CORE_BUFFERMEMORY_HPP__
#define __TERRAMA2_ANALYSIS_CORE_BUFFERMEMORY_HPP__

// STL
#include <memory>
#include <vector>

// Forward declaration
namespace te
{
  namespace mem
  {
    class DataSet;
  }
  namespace gm
  {
    class Geometry;
    class Envelope;
  }
}

// Forward declaration
namespace terrama2
{
  namespace core
  {
    class SyncronizedDataSet;
  }

  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \brief Defines the buffer type.
        */
        enum BufferType
        {
          NONE = 0, //!< No buffer
          EXTERN = 1, //!< External buffer.
          INTERN = 2, //!< Internal buffer.
          INTERN_PLUS_EXTERN = 3, //!< Internal buffer plus external external buffer.
          OBJECT_PLUS_EXTERN = 4, //!< Geometry plus external buffer.
          OBJECT_WITHOUT_INTERN = 5 //!< Geometry minus internal buffer.
        };

        std::unique_ptr<te::gm::Geometry> createBuffer(BufferType bufferType, std::shared_ptr<te::gm::Geometry> geometry, double distance);

        /*!
          \brief Creates a buffer for each given geometry with the given distance.

          \param geometries Vector with the geometries.
          \param box A envelope that covers all given geometries.
          \param distance The distance of the buffer.
          \param bufferType The type of the buffer.
          \return A smart pointer to a memory dataset with the buffers created from the given geometries.
        */
        std::shared_ptr<te::mem::DataSet> createAggregationBuffer(std::vector<std::shared_ptr<te::gm::Geometry> >& geometries, std::shared_ptr<te::gm::Envelope>& box, double distance, BufferType bufferType);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_ANALYSIS_CORE_BUFFERMEMORY_HPP__
