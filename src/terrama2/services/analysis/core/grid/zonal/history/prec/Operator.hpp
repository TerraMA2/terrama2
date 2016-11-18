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
  \file terrama2/services/analysis/core/grid/zonal/history/prec/Operator.hpp

  \brief Contains grid zonal analysis operators.

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_HISTORY_PREC_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_HISTORY_PREC_OPERATOR_HPP__

// TerraMA2
#include "../../../../BufferMemory.hpp"

// STL
#include <string>

//Terralib
#include <terralib/raster/PositionIterator.h>

namespace te
{
  namespace gm
  {
    class Geometry;
    class Polygon;
    class Point;
    class Line;
  } /* gm */
} /* te */

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace grid
        {
          namespace zonal
          {
            namespace history
            {
              namespace prec
              {
                //Cantor pairing function for hashing the pair (column X row) https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
                struct PairHash
                {
                  std::size_t operator()(const std::pair<int, int>& pair) const
                  {
                    return 0.5*(pair.first+pair.second)*(pair.first+pair.second+1)+pair.second;;
                  }
                };

                /*!
                  \brief Populates a map of coordinates to the accumulated value and number of occurrences, intesecting the geometry.

                  This method will call the geometry type especific method.
                */
                template<class T>
                void appendValues(const std::vector< std::shared_ptr<te::rst::Raster> >& rasterList,
                                  const size_t band,
                                  te::gm::Geometry* geom,
                                  std::unordered_map<std::pair<int, int>, std::pair<T, int>, PairHash>& valuesMap);
                //!\brief Populates a map of coordinates to the accumulated value and number of occurrences, intesecting the polygon.
                template<class T>
                void appendValues(const std::vector< std::shared_ptr<te::rst::Raster> >& rasterList,
                                  const size_t band,
                                  te::gm::Polygon* polygon,
                                  std::unordered_map<std::pair<int, int>, std::pair<T, int>, PairHash>& values);
                //!\brief Populates a map of coordinates to the accumulated value and number of occurrences, intesecting the line.
                template<class T>
                void appendValues(const std::vector< std::shared_ptr<te::rst::Raster> >& rasterList,
                                  const size_t band,
                                  te::gm::Line* line,
                                  std::unordered_map<std::pair<int, int>, std::pair<T, int>, PairHash>& valuesMap);
                //!\brief Populates a map of coordinates to the accumulated value and number of occurrences, intesecting the point set.
                template<class T>
                void appendValues(const std::vector< std::shared_ptr<te::rst::Raster> >& rasterList,
                                  const size_t band,
                                  std::vector<te::gm::Point*> pointSet,
                                  std::unordered_map<std::pair<int, int>, std::pair<T, int>, PairHash>& valuesMap);

              template<template<class> class U, class T>
              void appendValues(const std::vector< std::shared_ptr<te::rst::Raster> >& rasterList,
                                  const size_t band,
                                  U<T>& rasterIt,
                                  U<T>& end,
                                  std::unordered_map<std::pair<int, int>, std::pair<T, int>, PairHash>& valuesMap);
                /*!
                  \brief Implementation of grid zonal operator.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param statisticOperation The statistic operation chosen by the user.
                  \param dataSeriesName DataSeries name.
                  \param dateFilter Time filter for the data.

                  \return A double value with the result.
                */
                double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                    const std::string& dataSeriesName,
                                    const std::string& dateDiscardBefore,
                                    const std::string& dateDiscardAfter,
                                    const size_t band,
                                    terrama2::services::analysis::core::Buffer buffer = Buffer());

                double count(const std::string& dataSeriesName, const std::string& dateDiscardBefore, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the sum of zonal grid data.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.

                  \return A double value with the result.
                */
                double sum(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the mean of zonal grid data.


                In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.
                  \param dateFilter Time filter for the data.

                  \return A double value with the result.
                */
                double mean(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the min of zonal grid data.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.


                  \return A double value with the result.
                */
                double min(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the max of zonal grid data.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.
                  \param attribute Which DCP attribute will be used.
                  \param dcpId Identifier of DCP dataset.
                  \param buffer = Buffer() buffer = Buffer() to be used in the monitored object.


                  \return A double value with the result.
                */
                double max(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the median of zonal grid data.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.


                  \return A double value with the result.
                */
                double median(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the standard deviation of zonal grid data.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.

                  \return A double value with the result.
                */
                double standardDeviation(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                /*!
                  \brief Calculates the variance of zonal grid data.

                  In case of an error or no data available it will return NAN(Not A Number).

                  \param dataSeriesName DataSeries name.

                  \return A double value with the result.
                */
                double variance(const std::string& dataSeriesName, const std::string& dateDiscardBefore, const size_t band = 0, terrama2::services::analysis::core::Buffer buffer = Buffer());

                void appendValues(te::rst::Raster* raster, te::gm::Polygon* polygon, std::vector<double>& values);
              } /* prec */
            }
          } /* zonal */
        }   // end namespace grid
      }     // end namespace core
    }       // end namespace analysis
  }         // end namespace services
}           // end namespace terrama2

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_GRID_ZONAL_HISTORY_PREC_OPERATOR_HPP__
