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
  \file terrama2/services/analysis/core/python/PythonBindingDCP.hpp

  \brief register dcp analysis binding

  \author Jano Simas
*/

#include "PythonBindingDCP.hpp"
#include "../dcp/Operator.hpp"

void terrama2::services::analysis::core::python::DCP::registerFunctions()
{
  registerDCPFunctions();
}

void terrama2::services::analysis::core::python::DCP::registerDCPFunctions()
{
  using namespace boost::python;

  // set the current scope to the new sub-module
  scope scpScope = import("terrama2.dcp");

  // export functions inside dcp namespace
  def("sample", terrama2::services::analysis::core::dcp::sample);
  def("min", terrama2::services::analysis::core::dcp::min);
  def("max", terrama2::services::analysis::core::dcp::max);
  def("mean", terrama2::services::analysis::core::dcp::mean);
  def("median", terrama2::services::analysis::core::dcp::median);
  def("sum", terrama2::services::analysis::core::dcp::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::standardDeviation);
  def("variance", terrama2::services::analysis::core::dcp::variance);
  def("count", terrama2::services::analysis::core::dcp::count);
}
