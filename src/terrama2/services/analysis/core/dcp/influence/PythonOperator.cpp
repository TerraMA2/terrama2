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
  \file terrama2/services/analysis/core/dcp/influence/PythonOperator.cpp

  \brief Contains python wrapper for DCP influence operators.

  \author Paulo R. M. Oliveira
*/

#include "PythonOperator.hpp"
#include "Operator.hpp"
#include "../../python/PythonUtils.hpp"

boost::python::list terrama2::services::analysis::core::dcp::zonal::influence::python::byAttribute(const std::string& dataSeriesName, boost::python::list attributeList)
{
  std::vector<std::string> vecAttr;
  terrama2::services::analysis::core::python::pythonToVector<std::string>(attributeList, vecAttr);

  auto vecDCP = terrama2::services::analysis::core::dcp::zonal::influence::byAttribute(dataSeriesName, vecAttr);

  boost::python::list pyList;
  for(auto& dcp : vecDCP)
  {
    pyList.append(boost::python::object(dcp));
  }
  return pyList;
}

boost::python::list terrama2::services::analysis::core::dcp::zonal::influence::python::byRule(const std::string& dataSeriesName, const terrama2::services::analysis::core::Buffer& buffer)
{
  auto vecDCP = terrama2::services::analysis::core::dcp::zonal::influence::byRule(dataSeriesName, buffer);

  boost::python::list pyList;
  for(auto& dcp : vecDCP)
  {
    pyList.append(boost::python::object(dcp));
  }
  return pyList;
}
