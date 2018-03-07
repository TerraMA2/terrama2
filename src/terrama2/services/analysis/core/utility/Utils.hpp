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
  \file terrama2/services/analysis/core/Utils.hpp

  \brief Utility functions for TerraMA2 Analysis module.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_UTILS_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_UTILS_HPP__

// TerraMA2

#include "../Analysis.hpp"
#include "../OperatorCache.hpp"
#include "../../../../core/Shared.hpp"
#include "../../../../core/utility/FileRemover.hpp"

// TerraLib
#include <terralib/raster/Raster.h>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \brief Defines the statistic operation to be used in the analysis.
        */
        enum class StatisticOperation
        {
          INVALID = 0, //!< Invalid operation.
          MIN = 1, //!< Minimum value.
          MAX = 2, //!< Maximum value.
          SUM = 3, //!< Sum.
          MEAN = 4, //!< Mean.
          MEDIAN = 5, //!< Median.
          STANDARD_DEVIATION = 6, //!< Standard deviation.
          COUNT = 7, //!< Count.
          VARIANCE = 8//!< Variance
        };

        /*!
          \brief Returns a enum with the type of the Analysis based on the given parameter.

          \param type Type of the Analysis.

          \return Enum with the type of the Analysis.
         */
        AnalysisType ToAnalysisType(uint32_t type);

        /*!
          \brief Returns a enum with the type of use of the DataSeries in the analysis based on the given parameter.

          \param type Type of use of the DataSeries in the Analysis.

          \return Enum with the type of use of the DataSeries in the Analysis.
         */
        AnalysisDataSeriesType ToAnalysisDataSeriesType(uint32_t type);


        /*!
          \brief Returns a enum with the script language of the Analysis based on the given parameter.

          \param scriptLanguage Script language of the Analysis.

          \return Enum with the script language of the Analysis.
         */
        ScriptLanguage ToScriptLanguage(uint32_t scriptLanguage);

        /*!
          \brief Returns a enum with the interpolation method used for the output grid.

          \param interpolationMethod Integer containing the interpolation method.

          \return The interpolation method used for the output grid.
         */
        InterpolationMethod ToInterpolationMethod(uint32_t interpolationMethod);

        /*!
          \brief Returns a enum with the strategy used to determine the resolution of the output grid.

          \param resolutionType Integer containing the strategy used to determine the resolution of the output grid.

          \return The strategy used to determine the resolution of the output grid.
         */
        ResolutionType ToResolutionType(uint32_t resolutionType);

        /*!
          \brief Returns a enum with the interest area strategy for the box of the output grid.

          \param resolutionType Integer containing the interest area strategy for the box of the output grid.

          \return The interest area strategy for the box of the output grid.
         */
        InterestAreaType ToInterestAreaType(uint32_t interestAreaType);



        /*!
          \brief Returns a map with the raster for each dataset from the given data series.

          \param dataManager Smart pointer to the data manager.
          \param dataSeriesId Identifier of the data series.

          \return The interest area strategy for the box of the output grid.
         */
        std::unordered_multimap<terrama2::core::DataSetGridPtr, std::shared_ptr<te::rst::Raster> > getGridMap(DataManagerPtr dataManager, DataSeriesId dataSeriesId, std::shared_ptr<terrama2::core::FileRemover> remover);


        /*
          \brief Returns a map with the parameters to create the output raster.

          \return The map with the parameters to create the output raster.
         */
        std::tuple<te::rst::Grid*, const std::vector<te::rst::BandProperty*> > getOutputRasterInfo(std::map<std::string, std::string> rinfo);

        /*!
          \brief Returns the attribute value for the given position, it tries a lexical cast to double in case the attribute has a different type.
          \param syncDs Smart pointer to the dataset.
          \param attribute Attribute name.
          \param i The position.
          \param attributeType The attribute type.
          \return The attribute value for the given position
        */
        double getValue(terrama2::core::SynchronizedDataSetPtr syncDs, const std::string& attribute, uint32_t i, int attributeType);

        /*!
         \brief Calculates the statistics based on the given values.
         \param values The list of values.
         \param cache The OperatorCache to store the results.
        */
        void calculateStatistics(std::vector<double>& values, OperatorCache& cache);


        /*!
          \brief Returns the result of the operation for the given statistic.
          \param cache Cache with the calculated statistics.
          \param statisticOperation The statistic operation called by the script.
        */
        double getOperationResult(OperatorCache& cache, StatisticOperation statisticOperation);

        std::pair<size_t, size_t> getBandInterval(terrama2::core::DataSetPtr dataset, double secondsPassed, std::string dateDiscardBefore, std::string dateDiscardAfter);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_UTILS_HPP__
