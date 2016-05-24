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
#include <string>

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
          \note For internal buffer the value must be negative.
        */
        enum BufferType
        {
          NONE = 0, //!< No buffer
          ONLY_BUFFER = 1, //!< Only buffer, can be external or internal.
          EXTERN_PLUS_INTERN = 2, //!< Result buffer is the union between external buffer and internal buffer.
          OBJECT_PLUS_BUFFER = 3, //!< Geometry plus buffer, must be a positive value because it's an external buffer.
          OBJECT_MINUS_BUFFER = 4, //!< Geometry minus buffer, must be a negative value because it's an internal buffer.
          DISTANCE_ZONE = 5 //! Result buffer is the difference between buffer 1 and buffer 2.
        };

        struct Buffer
        {
          //! Default constructor
          Buffer() : bufferType(NONE) {}

          /*!
            \brief Constructor for composed buffers such as EXTERN_PLUS_INTERN and DISTANCE_ZONE.
            \param type The buffer type.
            \param d Distance of the buffer, use negative values for an internal buffer.
            \param u Unit of the distance.
          */
          Buffer(BufferType type, double d, std::string u) : bufferType(type), distance(d), unit(u) {}

          /*!
            \brief Constructor for composed buffers such as EXTERN_PLUS_INTERN and DISTANCE_ZONE.
            \param type The buffer type, must be EXTERN_PLUS_INTERN or DISTANCE_ZONE.
            \param d1 Distance for the first buffer, for EXTERN_PLUS_INTERN type this is external buffer.
            \param u1 Unit of the distance for the first buffer,  for EXTERN_PLUS_INTERN type this is external buffer.
            \param d2 Distance for the second buffer, for EXTERN_PLUS_INTERN type this is internal buffer.
            \param u2 Unit of the distance for the second buffer,  for EXTERN_PLUS_INTERN type this is internal buffer.
          */
          Buffer(BufferType type, double d1, std::string u1, double d2, std::string u2)
            : bufferType(type), distance(d1), unit(u1), distance2(d2), unit2(u2) {}

          BufferType bufferType; //!< The type of the buffer.
          double distance; //!< The distance of the buffer, positive value for external buffer and negative for internal buffer.
          std::string unit; //!< The distance unit.
          double distance2; //!< The distance of the second buffer, this attribute is only used for composed buffer such as EXTERN_PLUS_INTERN and DISTANCE_ZONE.
          std::string unit2; //!< The distance unit of the second buffer, this attribute is only used for composed buffer such as EXTERN_PLUS_INTERN and DISTANCE_ZONE.
        };

        /*!
          \brief Creates a buffer based on the given configuration.
          \param buffer The buffer configuration.
          \param geometry The geometry.
          \return A smart pointer to a memory dataset with the buffers created from the given geometries.
        */
        std::shared_ptr<te::gm::Geometry> createBuffer(Buffer buffer, std::shared_ptr<te::gm::Geometry> geometry);

        /*!
          \brief Creates a buffer for each given geometry with the given distance.

          \param geometries Vector with the geometries.
          \param box A envelope that covers all given geometries.
          \param distance The distance of the buffer.
          \param bufferType The type of the buffer.
          \return A smart pointer to a memory dataset with the buffers created from the given geometries.
        */
        std::shared_ptr<te::mem::DataSet> createAggregationBuffer(std::vector<std::shared_ptr<te::gm::Geometry> >& geometries, std::shared_ptr<te::gm::Envelope>& box, Buffer buffer);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_ANALYSIS_CORE_BUFFERMEMORY_HPP__
