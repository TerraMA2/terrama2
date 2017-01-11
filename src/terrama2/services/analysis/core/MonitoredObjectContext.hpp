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
  \file terrama2/services/analysis/core/occurrence/Context.hpp

  \brief Contains occurrence analysis context.

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_MONITORED_OBJECT_CONTEXT_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_MONITORED_OBJECT_CONTEXT_HPP__

#include "Analysis.hpp"
#include "BaseContext.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/data-model/Filter.hpp"

#include <terralib/geometry/Coord2D.h>
#include <terralib/sam/kdtree.h>

// Forward declaration
namespace te
{
  namespace rst
  {
    class Raster;
  }
}

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \brief Contains additional information about a DataSeries to be used in an analysis.
        */
        struct ContextDataSeries
        {
          terrama2::core::DataSetSeries series; //!< Dataset information.
          std::string identifier; //!< Identifier column.
          int32_t geometryPos = -1; //!< Geometry column position.
          te::sam::rtree::Index<uint32_t, 8> rtree; //!< Spatial index in memory

        };

        class MonitoredObjectContext : public BaseContext
        {
          public:
            MonitoredObjectContext(DataManagerPtr dataManager,  AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

            ~MonitoredObjectContext() = default;
            MonitoredObjectContext(const MonitoredObjectContext& other) = default;
            MonitoredObjectContext(MonitoredObjectContext&& other) = default;
            MonitoredObjectContext& operator=(const MonitoredObjectContext& other) = default;
            MonitoredObjectContext& operator=(MonitoredObjectContext&& other) = default;

            void loadMonitoredObject();

            void addDCPDataSeries(terrama2::core::DataSeriesPtr dataSeries,
                                  const terrama2::core::Filter& filter);
            /*!
              \brief Returns the set of attributes that compose the analysis result.

              \param analysisHashCode Hash code of the analysis.
            */
            std::set<std::string> getAttributes() const { return attributes_;}
            /*!
            \brief Reads the DataSeries that fits the date filter and adds it to the context.

            \param dataSeries A smart pointer to the DataSeries to be loaded.
            \param filter The filter to be applied and used as key.
            \param createSpatialIndex Defines if a spatial index should be created to optimize data access.
            */
            void addDataSeries(terrama2::core::DataSeriesPtr dataSeries,
                               const terrama2::core::Filter& filter,
                               bool createSpatialIndex);
            /*!
              \brief Returns a smart pointer that contains the TerraLib DataSet for the given DataSetId.

              \param datasetId The DataSet identifier.
              \param filter The filter to be used as key.

              \return A smart pointer to the context data series.
            */
            std::shared_ptr<ContextDataSeries> getContextDataset(const DataSetId datasetId,
                                                                 const terrama2::core::Filter& filter) const;

            /*!
              \brief Returns true if the given dataset has already been loaded into the context.

              \param datasetId The DataSet identifier.
              \param filter The filter to be applied and used as key.
              \return True if the given dataset has already been loaded into the context.
            */
            bool exists(const DataSetId datasetId, const terrama2::core::Filter& filter) const;

            /*!
              \brief Returns the map with the result for the given analysis.

              \return The map with the analysis result.
            */
            inline std::unordered_map<int, std::map<std::string, double> > analysisResult() const { return analysisResult_; }

            /*!
            \brief Sets the analysis result for a geometry and a given attribute.

            \param index Geometry index.
            \param attribute Name of the attribute.
            \result The result value.
            */
            void setAnalysisResult(const int index, const std::string& attribute,
                                   double result);

            /*!
              \brief Adds an attribute to the result list of the given analysis.

              \param attribute The name of the attribute.
            */
            void addAttribute(const std::string& attribute);

            /*!
              \brief Returns the ContextDataSeries of the monitored object for the given analysis.
              \param analysisHashCode Analysis hashcode.
              \param dataManagerPtr Smart pointer to the data manager.
              \return The ContextDataSeries of the monitored object.
            */
            std::shared_ptr<ContextDataSeries> getMonitoredObjectContextDataSeries(std::shared_ptr<DataManager>& dataManagerPtr);

            /*!
              \brief Returns the DCP buffer for the given dataset identifier.
              \note It will return an empty smart pointer if none buffer is found.

              \param datasetId The DataSet identifier.
              \param filter The filter to be used as key.
              \return The DCP buffer.
            */
            std::shared_ptr<te::gm::Geometry> getDCPBuffer(const DataSetId datasetId, const terrama2::core::Filter& filter);

            /*!
              \brief Adds the given DCP buffer to the context.

              \param buffer The DCP buffer to be added.
              \param datasetId The DataSet identifier.
              \param filter The filter to be used as key.

            */
            void addDCPBuffer(const DataSetId datasetId, std::shared_ptr<te::gm::Geometry> buffer, const terrama2::core::Filter& filter);

            /*!
              \brief Returns a time instant object from the date filter using as base date the start date set in the context.

              \param dateFilter The date restriction to be used in the DataSet.
              \return The time instant object created from date filter.

            */
            std::unique_ptr<te::dt::TimeInstantTZ> getTimeInstantFromDateFilter(const std::string& dateFilter) const;

          protected:
            std::set<std::string> attributes_;
            std::unordered_map<int, std::map<std::string, double> >  analysisResult_;
            std::unordered_map<ObjectKey, std::shared_ptr<ContextDataSeries>, ObjectKeyHash, EqualKeyComparator > datasetMap_; //!< Map containing all loaded datasets.
            std::unordered_map<ObjectKey, std::shared_ptr<te::gm::Geometry>, ObjectKeyHash, EqualKeyComparator > bufferDcpMap_; //!< Map containing DCP buffers.
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_MONITORED_OBJECT_CONTEXT_HPP__
