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


#include "python/PythonInterpreter.hpp"

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
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \brief Defines the buffer type.
          \note For inside buffer the value must be negative.
        */
        enum BufferType
        {
          NONE = 0, //!< No buffer
          IN = 1, //!< Only inside buffer.
          OUT = 2, //!< Only outside buffer.
          IN_OUT = 3, //!< Result buffer is the union between outside buffer and inside buffer.
          OUT_UNION = 4, //!< Geometry plus buffer, must be a positive value because it's an outside buffer.
          IN_DIFF = 5, //!< Geometry minus buffer, must be a negative value because it's an inside buffer.
          LEVEL = 6 //! Result buffer is the difference between buffer 1 and buffer 2.
        };

        /*!
          \brief Contains information about the occurrences that were aggregated because they were in the same area.
        */
        struct OccurrenceAggregation
        {
          std::shared_ptr<te::gm::Geometry> buffer; //!< The geometry of created by the aggregation of the occurrences buffers.
          std::vector<unsigned int> indexes; //!< The indexes that were aggregated.
        };

        struct Buffer
        {
          //! Default constructor
          Buffer(BufferType type = NONE)
            : bufferType(type),
            distance(0),
            unit("m"),
            distance2(0),
            unit2("m")
          { }

          /*!
            \brief Constructor for composed buffers such as OUTSIDE_PLUS_INSIDE and DISTANCE_ZONE.
            \param type The buffer type.
            \param d Distance of the buffer, use negative values for an inside buffer.
            \param u Unit of the distance.
          */
          Buffer(BufferType type, double d, std::string u) : bufferType(type), distance(d), unit(u), distance2(0), unit2("m")
          { }

          /*!
            \brief Constructor for composed buffers such as OUTSIDE_PLUS_INSIDE and DISTANCE_ZONE.
            \param type The buffer type, must be OUTSIDE_PLUS_INSIDE or DISTANCE_ZONE.
            \param d1 Distance for the first buffer, for OUTSIDE_PLUS_INSIDE type this is outside buffer.
            \param u1 Unit of the distance for the first buffer,  for OUTSIDE_PLUS_INSIDE type this is outside buffer.
            \param d2 Distance for the second buffer, for OUTSIDE_PLUS_INSIDE type this is inside buffer.
            \param u2 Unit of the distance for the second buffer,  for OUTSIDE_PLUS_INSIDE type this is inside buffer.
          */
          Buffer(BufferType type, double d1, std::string u1, double d2, std::string u2)
                  : bufferType(type), distance(d1), unit(u1), distance2(d2), unit2(u2)
          { }

          BufferType bufferType; //!< The type of the buffer.
          double distance; //!< The distance of the buffer, positive value for outside buffer and negative for inside buffer.
          std::string unit; //!< The distance unit.
          double distance2; //!< The distance of the second buffer, this attribute is only used for composed buffer such as OUTSIDE_PLUS_INSIDE and DISTANCE_ZONE.
          std::string unit2; //!< The distance unit of the second buffer, this attribute is only used for composed buffer such as OUTSIDE_PLUS_INSIDE and DISTANCE_ZONE.
        };

        /*!
          \brief Creates a buffer based on the given configuration.
          \param buffer The buffer configuration.
          \param geometry The geometry.
          \return A smart pointer to a memory dataset with the buffers created from the given geometries.
        */
        TMANALYSISEXPORT std::shared_ptr<te::gm::Geometry> createBuffer(Buffer buffer, std::shared_ptr<te::gm::Geometry> geometry);

        /*!
          \brief Creates a buffer for each given geometry with the given distance.

          \param indexes Vector with the geometries indexes.
          \param contextDataSeries Smart pointer to the ContextDataSeries of the occurrences.
          \param buffer Aggregation buffer configuration.
          \return A smart pointer to a memory dataset with the buffers created from the given geometries.
        */
        TMANALYSISEXPORT std::shared_ptr<te::mem::DataSet> createAggregationBuffer( std::shared_ptr<ContextDataSeries> contextDataSeries,
                                                                                    Buffer buffer,
                                                                                    StatisticOperation aggregationStatisticOperation,
                                                                                    const std::string& attribute);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_ANALYSIS_CORE_BUFFERMEMORY_HPP__
