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
  \file terrama2/services/analysis/core/dcp/influence/PythonOperator.hpp

  \brief Contains python wrapper for DCP influence operators.

  \author Paulo R. M. Oliveira
*/

#include "../../../BufferMemory.hpp"
#include "../../../../../../core/Typedef.hpp"

#include <vector>
#include <string>

#ifndef __TERRAMA2_ANALYSIS_CORE_DCP_INFLUENCE_PYTHON_OPERATOR_HPP__
#define __TERRAMA2_ANALYSIS_CORE_DCP_INFLUENCE_PYTHON_OPERATOR_HPP__


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
            namespace influence
            {
              namespace python
              {

                /*
                 \brief Python wrapper for influence by attribute operator
                 */
                TMANALYSISEXPORT boost::python::list byAttribute(const std::string& dataSeriesName, boost::python::list attributeList);

                /*
                 \brief Python wrapper for influence by rule operator
                 */
                TMANALYSISEXPORT boost::python::list byRule(const std::string& dataSeriesName, const terrama2::services::analysis::core::Buffer& buffer = terrama2::services::analysis::core::Buffer(), bool isActive = false);

              } // end namespace python
            }   // end namespace influence
          }     // end namespace zonal
        }     // end namespace dcp
      }       // end namespace core
    }         // end namespace analysis
  }           // end namespace services
}             // end namespace terrama2


#endif //__TERRAMA2_ANALYSIS_CORE_DCP_INFLUENCE_PYTHON_OPERATOR_HPP__
