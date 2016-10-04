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
  \file terrama2/services/analysis/core/Analysis.hpp

  \brief Model for the analysis configuration.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__
#define __TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__

#include "Exception.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/Schedule.hpp"
#include "../../../core/utility/Verify.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"

// QT
#include <QObject>

// STL
#include <string>
#include <vector>

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/geometry/Geometry.h>


namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \brief Defines the type of the analysis.
        */
        enum class AnalysisType
        {
          DCP_TYPE = 1, //!< Analysis for DCP.
          MONITORED_OBJECT_TYPE = 2, //!< Analysis for monitored objects.
          GRID_TYPE = 3, //!< Analysis for grids.
        };

        /*!
          \brief Defines the type of influence of a DCP over the monitored object.
        */
        enum class InfluenceType
        {
          RADIUS_TOUCHES = 1, //!< The DCP will be considered if monitored object intercepts the influence radius.
          RADIUS_CENTER = 2, //!< The DCP will be considered if monitored object centroid intercepts the influence radius.
          REGION = 3, //!< A DataSeries will be used to identify the region of influence of each DCP.
        };

        /*!
          \brief Defines the language of the script.
        */
        enum class ScriptLanguage
        {
          PYTHON = 1, //!< Scripts in Python.
          LUA = 2 //!< Scripts in LUA.
        };

        /*!
          \brief Defines the type of use of a DataSeries in the analysis.
          Analysis of type DCP_TYPE requires an DATASERIES_PCD_TYPE in the analysis DataSeries list.
          Analysis of type MONITORED_OBJECT_TYPE requires an DATASERIES_MONITORED_OBJECT_TYPE in the analysis DataSeries list.
          Analysis of type GRID_TYPE requires an DATASERIES_GRID_TYPE in the analysis DataSeries list.

          Any additional DataSeries that will be used in the analysis must exist in the DataSeries list with the type ADDITIONAL_DATA_TYPE.

        */
        enum class AnalysisDataSeriesType
        {
          DATASERIES_MONITORED_OBJECT_TYPE = 1, //!< Identifies a DataSeries used as monitored object.
          DATASERIES_GRID_TYPE = 2, //!< Identifies a DataSeries used as grid.
          DATASERIES_PCD_TYPE = 3, //!< Identifies a DataSeries used as DCP.
          ADDITIONAL_DATA_TYPE = 4 //!< Identifies a DataSeries used as an additional data.
        };

        /*!
          \struct AnalysisDataSeries
          \brief Contains the configuration of an DataSeries used in an analysis.
        */
        struct AnalysisDataSeries
        {
          AnalysisDataSeriesId id = 0; //!< AnalysisDataSeries identifier.
          DataSeriesId dataSeriesId; //!< Identifier of the DataSeries.
          AnalysisDataSeriesType type; //!< Type of use of the DataSeries in the analysis.
          std::string alias; //!< Map containing the alias for the columns of a DataSeries.
          std::map<std::string, std::string> metadata; //!< Metadata of the AnalysisDataSeries.
        };

        /*!
          \brief Allowed interpolation methods.
        */
        enum class InterpolationMethod
        {
          //REVIEW: Why is this redefined? should te::rst::InterpolationMethod be used instead?
          NEARESTNEIGHBOR = 1,    //!< Near neighborhood interpolation method.
          BILINEAR = 2,           //!< Bilinear interpolation method.
          BICUBIC = 3             //!< Bicubic interpolation method.
        };

        /*!
          \brief Defines the resolution type to be used in the output grid
        */
        enum class ResolutionType
        {
          SMALLEST_GRID = 1, //!< Use the resolution from the smallest grid.
          BIGGEST_GRID = 2, //!< Use the resolution from the biggest grid.
          SAME_FROM_DATASERIES = 3, //!< Use the same resolution of a given grid.
          CUSTOM = 4 //!< Use a custom resolution.
        };

        /*!
          \brief Defines the interest area type to be used in the output grid.
        */
        enum class InterestAreaType
        {
          UNION = 1, //!< Use the union of the areas from the DataSeries in the analysis.
          SAME_FROM_DATASERIES = 2, //!< Use the same box of a given grid.
          CUSTOM = 3 //!< Use a custom box.
        };

        /*!
          \brief Defines the date filter for reprocessing of historical data
        */
        struct ReprocessingHistoricalData
        {
          std::shared_ptr<te::dt::TimeInstantTZ> startDate = nullptr; //!< Initial date of interest.
          std::shared_ptr<te::dt::TimeInstantTZ> endDate = nullptr; //!< Final date of interest.
        };

        /*!
          \brief Defines the parameters used to construct the output grid of an analysis.
        */
        struct AnalysisOutputGrid
        {
          AnalysisId analysisId = 0; //!< Identifier of the analysis.
          InterpolationMethod interpolationMethod = InterpolationMethod::NEARESTNEIGHBOR; //!< Interpolation method.
          double interpolationDummy = 0; //!< Dummy value.
          ResolutionType resolutionType; //!< Resolution type to be used in the output grid.
          DataSeriesId resolutionDataSeriesId; //!< Identifier of the DataSeries to copy the grid resolution.
          double resolutionX = 0; //!< Resolution to be used in X.
          double resolutionY = 0; //!< Resolution to be used in Y.
          Srid srid = 0; //!< SRID of the output grid.
          InterestAreaType interestAreaType; //!< Type of interest area.
          DataSeriesId interestAreaDataSeriesId = 0; //!< Identifier of the DataSeries to copy the box resolution.
          std::shared_ptr<te::gm::Geometry> interestAreaBox; //!< Custom box.
        };

        /*!
          \struct Analysis
          \brief Model for the configuration of an analysis execution.
        */
        struct Analysis
        {
          AnalysisId id = 0; //!< Analysis identifier.
          ProjectId projectId = 0; //!< Project identifier.
          ScriptLanguage scriptLanguage; //!< Language of the script.
          std::string script; //!< Content of the script.
          AnalysisType type; //!< Type of the analysis.
          std::string name; //!< Name of the analysis.
          std::string description; //!< Short description of the purpose of the analysis.
          bool active = true; //!< Defines if the analysis is active, if true it will be executed according to the schedule.
          DataSeriesId outputDataSeriesId; //!< The dataset that stores the result of the analysis.
          std::map<std::string, std::string> metadata; //!< Metadata of the analysis.
          std::vector<AnalysisDataSeries> analysisDataSeriesList; //!< DataSeries that are used in this analysis.
          terrama2::core::Schedule schedule; //!< Time schedule for the analysis execution.
          ServiceInstanceId serviceInstanceId; //!< Identifier of the service instance that should run the analysis.
          AnalysisOutputGridPtr outputGridPtr; //!< Output grid configuration.
          ReprocessingHistoricalDataPtr reprocessingHistoricalData; //!< Date filter for reprocessing of historical data.

          /*!
           \brief Hash code is formed from the hash of the string AnalysisId + startDate.
          */
          AnalysisHashCode hashCode(std::shared_ptr<te::dt::TimeInstantTZ> startDate) const
          {
            try
            {
              terrama2::core::verify::date(startDate);
            }
            catch (const terrama2::core::VerifyException&)
            {
              throw InvalidParameterException() << ErrorDescription(QObject::tr("Analysis %1 : Start date not set.").arg(id));
            }

            std::string str = std::to_string(id) + startDate->toString();
            std::hash<std::string> hash_fn;
            return hash_fn(str);
          }

          friend bool operator==(const Analysis& lhs, const Analysis& rhs){ return lhs.id == rhs.id; }
        };


      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__
