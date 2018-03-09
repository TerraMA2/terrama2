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
  \file terrama2/services/analysis/core/dcp/Operator.hpp

  \brief Contains DCP analysis operators.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_DCP_ZONAL_OPERATOR_HPP__
#define __TERRAMA2_ANALYSIS_CORE_DCP_ZONAL_OPERATOR_HPP__

// TerraMA2
#include "../../python/PythonInterpreter.hpp"
#include "../../BufferMemory.hpp"
#include "../../Analysis.hpp"
#include "../../Shared.hpp"

// STL
#include <string>

// Boost
#include <boost/python.hpp>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace dcp
        {
          namespace zonal
          {
            /*!
              \brief Implementation of operator for latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param statisticOperation The statistic operation chosen by the user.
              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double value with the result.
            */
            TMANALYSISEXPORT double operatorImpl(terrama2::services::analysis::core::StatisticOperation statisticOperation,
                                const std::string& dataSeriesName, const std::string& attribute,
                                boost::python::list pcds, const std::string& dateFilterBegin = "",
                                const std::string& dateFilterEnd = "");
                              
            /*!
              \brief It returns the number of DCPs that have influence over the current monitored object.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param buffer Buffer to be used in the monitored object.

              \return The number of DCP that have influence over the current monitored object.
            */
            TMANALYSISEXPORT int count(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer);

            /*!
              \brief Calculates the minimum value of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param statisticOperation The statistic operation chosen by the user.
              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the minimum value.
            */
            TMANALYSISEXPORT double min(const std::string& dataSeriesName, const std::string& attribute,
                       boost::python::list ids);

            /*!
              \brief Calculates the maximum value of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the maximum value.
            */
            TMANALYSISEXPORT double max(const std::string& dataSeriesName, const std::string& attribute,
                       boost::python::list ids);

            /*!
              \brief Calculates the mean of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the mean.
            */
            TMANALYSISEXPORT double mean(const std::string& dataSeriesName, const std::string& attribute,
                        boost::python::list ids);

            /*!
              \brief Calculates the median value of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the median.
            */
            TMANALYSISEXPORT double median(const std::string& dataSeriesName, const std::string& attribute,
                          boost::python::list ids);

            /*!
              \brief Calculates the sum of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the sum.
             */
            TMANALYSISEXPORT double sum(const std::string& dataSeriesName, const std::string& attribute,
                       boost::python::list ids);

            /*!
              \brief Calculates the standard deviation of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the standard deviation.
            */
            TMANALYSISEXPORT double standardDeviation(const std::string& dataSeriesName, const std::string& attribute,
                                     boost::python::list ids);

            /*!
              \brief Calculates the variance of the latest DCP series data.

              In case an empty set of identifiers is given, it will use the influence
              configured for the analysis to determine which DCP dataset will be used.

              In case of an error or no data available it will return NAN(Not A Number).

              \param dataSeriesName DataSeries name.
              \param attribute Which DCP attribute will be used.
              \param ids A set of identifiers of DataSet.

              \return A double with the variance between DCP values.
            */
            TMANALYSISEXPORT double variance(const std::string& dataSeriesName, const std::string& attribute,
                            boost::python::list ids);


            /*!
             \brief Returns the influence type of an analysis.
             \param analysis Analysis configuration.
             \return The influence type.
            */
            TMANALYSISEXPORT terrama2::services::analysis::core::InfluenceType getInfluenceType(const terrama2::services::analysis::core::AnalysisPtr analysis);

            /*!
             \brief Creates the influence buffer.
             \param analysis Analysis configuration.
             \param geometry DCP position.
             \param monitoredObjectSrid SRID of the monitored object.
             \param influenceType Influence type of the analysis.
             \return The buffer geometry.
            */
            TMANALYSISEXPORT std::shared_ptr<te::gm::Geometry> createDCPInfluenceBuffer(const terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::gm::Geometry> position, int monitoredObjectSrid, InfluenceType influenceType);

            /*!
             \brief Verify if the DCP influences the monitored object.
             \param influenceType Influence type of the analysis.
             \param geom Monitored object geometry.
             \param dcpInfluenceBuffer  DCP influence buffer.
             \return True if the DCP influences the monitored object
            */
            TMANALYSISEXPORT bool verifyDCPInfluence(terrama2::services::analysis::core::InfluenceType influenceType, std::shared_ptr<te::gm::Geometry> moGeom, std::shared_ptr<te::gm::Geometry> dcpInfluenceBuffer);

          } // end namespace zonal
        } // end namespace dcp
      }   // end namespace core
    }     // end namespace analysis
  }       // end namespace services
}         // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_DCP_OPERATOR_HPP__
