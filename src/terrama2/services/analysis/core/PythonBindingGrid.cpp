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
  \file terrama2/services/analysis/core/PythonBindindGrid.cpp

  \brief

  \author Jano Simas
*/

#include "PythonBindingGrid.hpp"
#include "grid/Operator.hpp"
#include "grid/history/Operator.hpp"
#include "grid/history/interval/Operator.hpp"
#include "grid/forecast/Operator.hpp"
#include "grid/forecast/interval/Operator.hpp"

void terrama2::services::analysis::core::python::Grid::registerFunctions()
{
  registerGridFunctions();
}

void terrama2::services::analysis::core::python::Grid::registerGridFunctions()
{
  using namespace boost::python;

  // map the grid namespace to a sub-module
  // make "from terrama2.grid import <function>" work
  boost::python::object gridModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid"))));
  // make "from terrama2 import grid" work
  import("terrama2").attr("grid") = gridModule;
  // set the current scope to the new sub-module
  scope gridScope = gridModule;

  // export functions inside grid namespace
  def("sample", terrama2::services::analysis::core::grid::sample);

  // Register operations for grid.history
  object gridHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.history"))));
  // make "from terrama2.grid import history" work
  scope().attr("history") = gridHistoryModule;
  // set the current scope to the new sub-module
  scope gridHistoryScope = gridHistoryModule;

  def("min", terrama2::services::analysis::core::grid::history::min);
  def("max", terrama2::services::analysis::core::grid::history::max);
  def("mean", terrama2::services::analysis::core::grid::history::mean);
  def("median", terrama2::services::analysis::core::grid::history::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::history::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::history::variance);
}
