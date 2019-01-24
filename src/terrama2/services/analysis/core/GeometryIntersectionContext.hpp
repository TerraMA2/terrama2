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
  \file terrama2/services/analysis/core/GeometryIntersectionContext.hpp

  \brief Contains Geometric Intersection analysis context object
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GEOMETRY_INTERSECTION_CONTEXT_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GEOMETRY_INTERSECTION_CONTEXT_HPP__

// TerraMa2
#include "../../Config.hpp"
#include "Analysis.hpp"
#include "BaseContext.hpp"

// Boost
#include <boost/any.hpp>

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
        struct GeoIntersectionDataSeries
        {
          terrama2::core::DataSetSeries series; //!< Dataset information.
          std::string identifier; //!< Identifier column.
          std::size_t geometryPos = std::numeric_limits<std::size_t>::max(); //!< Geometry column position.
          te::sam::rtree::Index<uint32_t, 8> rtree; //!< Spatial index in memory
        };

        class TMANALYSISEXPORT GeometryIntersectionContext : public BaseContext
        {
          public:
            GeometryIntersectionContext(DataManagerPtr dataManager,  AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

            ~GeometryIntersectionContext() = default;
            GeometryIntersectionContext(const GeometryIntersectionContext& other) = default;
            GeometryIntersectionContext(GeometryIntersectionContext&& other) = default;
            GeometryIntersectionContext& operator=(const GeometryIntersectionContext& other) = default;
            GeometryIntersectionContext& operator=(GeometryIntersectionContext&& other) = default;

            void load();

            /*!
              \brief Returns the set of attributes that compose the analysis result.

              \param analysisHashCode Hash code of the analysis.
            */
            std::set<std::pair<std::string, int>> getAttributes() const { return attributes_; }
            /*!
            \brief Reads the DataSeries that fits the date filter and adds it to the context.

            \param dataSeries A smart pointer to the DataSeries to be loaded.
            \param filter The filter to be applied and used as key.
            \param createSpatialIndex Defines if a spatial index should be created to optimize data access.
            */
            void addDataSeries(terrama2::core::DataSeriesPtr dataSeries,
                               bool createSpatialIndex);
            /*!
              \brief Returns a smart pointer that contains the TerraLib DataSet for the given DataSetId.

              \param datasetId The DataSet identifier.
              \param filter The filter to be used as key.

              \return A smart pointer to the context data series.
            */
            std::shared_ptr<GeoIntersectionDataSeries> getContextDataset(const DataSetId datasetId) const;

            /*!
              \brief Returns true if the given dataset has already been loaded into the context.

              \param datasetId The DataSet identifier.
              \param filter The filter to be applied and used as key.
              \return True if the given dataset has already been loaded into the context.
            */
            bool exists(const DataSetId datasetId) const;

            /*!
              \brief Returns the map with the result for the given analysis.

              \return The map with the analysis result.
            */
            inline std::unordered_map<int, std::map<std::string, boost::any> > analysisResult() const { return analysisResult_; }

            /*!
            \brief Sets the analysis result for a geometry and a given attribute.

            \param index Geometry index.
            \param attribute Name of the attribute.
            \result The result value.
            */
            void setAnalysisResult(const int index, const std::string& attribute,
                                   boost::any result);

            /*!
              \brief Adds an attribute to the result list of the given analysis.

              \param attribute The name of the attribute.
            */
            void addAttribute(const std::string& attribute, int dataType);

            /*!
              \brief Returns the ContextDataSeries of the monitored object for the given analysis.
              \param analysisHashCode Analysis hashcode.
              \param dataManagerPtr Smart pointer to the data manager.
              \return The ContextDataSeries of the monitored object.
            */
            std::shared_ptr<GeoIntersectionDataSeries> getStaticDataSeries();

            /*!
              \brief
              \return
            */
            std::shared_ptr<GeoIntersectionDataSeries> getDynamicDataSeries() { return dynamicDataSeries_; }

          protected:
            std::shared_ptr<terrama2::services::analysis::core::GeoIntersectionDataSeries> staticDataSeries_;
            std::shared_ptr<terrama2::services::analysis::core::GeoIntersectionDataSeries> dynamicDataSeries_;
            std::set<std::pair<std::string, int> > attributes_; //!< Set of attributes and datatypes
            std::unordered_map<int, std::map<std::string, boost::any> >  analysisResult_;
            std::unordered_map<ObjectKey, std::shared_ptr<GeoIntersectionDataSeries>, ObjectKeyHash, EqualKeyComparator > datasetMap_; //!< Map containing all loaded datasets.
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_GEOMETRY_INTERSECTION_CONTEXT_HPP__
