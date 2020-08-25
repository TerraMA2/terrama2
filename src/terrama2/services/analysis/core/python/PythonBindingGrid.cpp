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
#include "../grid/Operator.hpp"
#include "../grid/history/Operator.hpp"
#include "../grid/history/interval/Operator.hpp"
#include "../grid/forecast/Operator.hpp"
#include "../grid/forecast/interval/Operator.hpp"
#include "../grid/zonal/Operator.hpp"
#include "../grid/zonal/forecast/Operator.hpp"
#include "../grid/zonal/forecast/accum/Operator.hpp"
#include "../grid/zonal/forecast/interval/Operator.hpp"
#include "../grid/zonal/history/Operator.hpp"
#include "../grid/zonal/history/accum/Operator.hpp"
#include "../grid/zonal/history/prec/Operator.hpp"
#include "../grid/zonal/history/interval/Operator.hpp"

void terrama2::services::analysis::core::python::Grid::registerFunctions()
{
  registerGridFunctions();
  registerGridHistoryFunctions();
  registerGridHistoryIntervalFunctions();
  registerGridForecastFunctions();
  registerGridForecastIntervalFunctions();
  registerGridZonalFunctions();
  registerGridZonalForecastFunctions();
  registerGridZonalForecastAccumFunctions();
  registerGridZonalForecastIntervalFunctions();
  registerGridZonalHistoryFunctions();
  registerGridZonalHistoryAccumFunctions();
  registerGridZonalHistoryPrecFunctions();
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridSample_overloads, terrama2::services::analysis::core::grid::sample, 1, 3)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

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
  def("sample", terrama2::services::analysis::core::grid::sample, gridSample_overloads(args("dataSeriesName", "band", "date"), "Grid sample operator."));
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistorySum_overloads, terrama2::services::analysis::core::grid::history::sum, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryMin_overloads, terrama2::services::analysis::core::grid::history::min, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryMax_overloads, terrama2::services::analysis::core::grid::history::max, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryMean_overloads, terrama2::services::analysis::core::grid::history::mean, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryMedian_overloads, terrama2::services::analysis::core::grid::history::median, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryStandardDeviation_overloads, terrama2::services::analysis::core::grid::history::standardDeviation, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryVariance_overloads, terrama2::services::analysis::core::grid::history::variance, 2, 3)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridHistoryFunctions()
{
  using namespace boost::python;

  // Register operations for grid.history
  object gridHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.history"))));
  // make "from terrama2.grid import history" work
  import("terrama2.grid").attr("history") = gridHistoryModule;
  // set the current scope to the new sub-module
  scope gridHistoryScope = gridHistoryModule;

  def("sum", terrama2::services::analysis::core::grid::history::sum, gridHistorySum_overloads(args("dataSeriesName", "DateBegin", "band"), "History sum operator."));
  def("min", terrama2::services::analysis::core::grid::history::min, gridHistoryMin_overloads(args("dataSeriesName", "DateBegin", "band"), "History min operator."));
  def("max", terrama2::services::analysis::core::grid::history::max, gridHistoryMax_overloads(args("dataSeriesName", "DateBegin", "band"), "History max operator."));
  def("mean", terrama2::services::analysis::core::grid::history::mean, gridHistoryMean_overloads(args("dataSeriesName", "DateBegin", "band"), "History mean operator."));
  def("median", terrama2::services::analysis::core::grid::history::median, gridHistoryMedian_overloads(args("dataSeriesName", "DateBegin", "band"), "History median operator."));
  def("standard_deviation", terrama2::services::analysis::core::grid::history::standardDeviation, gridHistoryStandardDeviation_overloads(args("dataSeriesName", "DateBegin", "band"), "History standard deviation operator."));
  def("variance", terrama2::services::analysis::core::grid::history::variance, gridHistoryVariance_overloads(args("dataSeriesName", "DateBegin", "band"), "History variance operator."));
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalSum_overloads, terrama2::services::analysis::core::grid::history::interval::sum, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalMin_overloads, terrama2::services::analysis::core::grid::history::interval::min, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalMax_overloads, terrama2::services::analysis::core::grid::history::interval::max, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalMean_overloads, terrama2::services::analysis::core::grid::history::interval::mean, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalMedian_overloads, terrama2::services::analysis::core::grid::history::interval::median, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalStandardDeviation_overloads, terrama2::services::analysis::core::grid::history::interval::standardDeviation, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridHistoryIntervalVariance_overloads, terrama2::services::analysis::core::grid::history::interval::variance, 3, 4)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridHistoryIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.history.interval
  object gridHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.history.interval"))));
  // make "from terrama2.grid.history import interval" work
  import("terrama2.grid.history").attr("interval") = gridHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope gridHistoryIntervalScope = gridHistoryIntervalModule;

  def("sum", terrama2::services::analysis::core::grid::history::interval::sum, gridHistoryIntervalSum_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval sum operator."));
  def("min", terrama2::services::analysis::core::grid::history::interval::min, gridHistoryIntervalMin_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval min operator."));
  def("max", terrama2::services::analysis::core::grid::history::interval::max, gridHistoryIntervalMax_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval max operator."));
  def("mean", terrama2::services::analysis::core::grid::history::interval::mean, gridHistoryIntervalMean_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval mean operator."));
  def("median", terrama2::services::analysis::core::grid::history::interval::median, gridHistoryIntervalMedian_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval median operator."));
  def("standard_deviation", terrama2::services::analysis::core::grid::history::interval::standardDeviation, gridHistoryIntervalStandardDeviation_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval standard deviation operator."));
  def("variance", terrama2::services::analysis::core::grid::history::interval::variance, gridHistoryIntervalVariance_overloads(args("dataSeriesName", "DateBegin", "DateEnd", "band"), "History interval variance operator."));
}

void terrama2::services::analysis::core::python::Grid::registerGridForecastFunctions()
{
  using namespace boost::python;

  // Register operations for grid.forecast
  object gridForecastModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.forecast"))));
  // make "from terrama2.grid import forecast" work
  import("terrama2.grid").attr("forecast") = gridForecastModule;
  // set the current scope to the new sub-module
  scope gridHistoryIntervalScope = gridForecastModule;

  def("min", terrama2::services::analysis::core::grid::forecast::min);
  def("max", terrama2::services::analysis::core::grid::forecast::max);
  def("mean", terrama2::services::analysis::core::grid::forecast::mean);
  def("median", terrama2::services::analysis::core::grid::forecast::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::forecast::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::forecast::variance);
  def("sum", terrama2::services::analysis::core::grid::forecast::sum);
}

void terrama2::services::analysis::core::python::Grid::registerGridForecastIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.forecast.interval
  object gridHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.forecast.interval"))));
  // make "from terrama2.grid.forecast import interval" work
  import("terrama2.grid.forecast").attr("interval") = gridHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope gridHistoryIntervalScope = gridHistoryIntervalModule;

  def("min", terrama2::services::analysis::core::grid::forecast::interval::min);
  def("max", terrama2::services::analysis::core::grid::forecast::interval::max);
  def("mean", terrama2::services::analysis::core::grid::forecast::interval::mean);
  def("median", terrama2::services::analysis::core::grid::forecast::interval::median);
  def("standard_deviation", terrama2::services::analysis::core::grid::forecast::interval::standardDeviation);
  def("variance", terrama2::services::analysis::core::grid::forecast::interval::variance);
  def("sum", terrama2::services::analysis::core::grid::forecast::interval::sum);
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalCount_overloads, terrama2::services::analysis::core::grid::zonal::count, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalCountByValue_overloads, terrama2::services::analysis::core::grid::zonal::countByValue, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalCountByRange_overloads, terrama2::services::analysis::core::grid::zonal::countByRange, 3, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMin_overloads, terrama2::services::analysis::core::grid::zonal::min, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMax_overloads, terrama2::services::analysis::core::grid::zonal::max, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMean_overloads, terrama2::services::analysis::core::grid::zonal::mean, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalMedian_overloads, terrama2::services::analysis::core::grid::zonal::median, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::standardDeviation, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalVariance_overloads, terrama2::services::analysis::core::grid::zonal::variance, 1, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalSum_overloads, terrama2::services::analysis::core::grid::zonal::sum, 1, 3)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal
  object gridZonalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal"))));
  // make "from terrama2.grid import zonal" work
  import("terrama2.grid").attr("zonal") = gridZonalModule;
  // set the current scope to the new sub-module
  scope gridZonalScope = gridZonalModule;

  def("count", terrama2::services::analysis::core::grid::zonal::count,
      gridZonalCount_overloads(args("dataSeriesName", "band", "buffer"),
                                "Count operator for grid zonal"));
  def("count_by_value", terrama2::services::analysis::core::grid::zonal::countByValue,
      gridZonalCountByValue_overloads(args("dataSeriesName", "value", "band", "buffer"),
                                "Count by value operator for grid zonal"));
  def("count_by_range", terrama2::services::analysis::core::grid::zonal::countByRange,
      gridZonalCountByRange_overloads(args("dataSeriesName", "begin", "end",  "band", "buffer"),
                                "Count by range operator for grid zonal"));
  def("min", terrama2::services::analysis::core::grid::zonal::min,
      gridZonalMin_overloads(args("dataSeriesName", "band",  "buffer"),
                             "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::max,
      gridZonalMax_overloads(args("dataSeriesName", "band", "buffer"),
                             "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::mean,
      gridZonalMean_overloads(args("dataSeriesName", "band", "buffer"),
                              "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::median,
      gridZonalMedian_overloads(args("dataSeriesName", "band", "buffer"),
                                "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::standardDeviation,
      gridZonalStandardDeviation_overloads(args("dataSeriesName", "band", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::variance,
      gridZonalVariance_overloads(args("dataSeriesName", "band", "buffer"),
                                  "Variance operator for grid zonal"));
  def("sum", terrama2::services::analysis::core::grid::zonal::sum,
      gridZonalSum_overloads(args("dataSeriesName", "band", "buffer"),
                                  "Sum operator for grid zonal"));
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMin_overloads, terrama2::services::analysis::core::grid::zonal::history::min, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMax_overloads, terrama2::services::analysis::core::grid::zonal::history::max, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMean_overloads, terrama2::services::analysis::core::grid::zonal::history::mean, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryMedian_overloads, terrama2::services::analysis::core::grid::zonal::history::median, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::history::standardDeviation, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryVariance_overloads, terrama2::services::analysis::core::grid::zonal::history::variance, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistorySum_overloads, terrama2::services::analysis::core::grid::zonal::history::sum, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryNum_overloads, terrama2::services::analysis::core::grid::zonal::history::num, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryList_overloads, terrama2::services::analysis::core::grid::zonal::history::list, 2, 3)
// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalHistoryFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.history
  object gridZonalHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.history"))));
  // make "from terrama2.grid.zonal import history" work
  import("terrama2.grid.zonal").attr("history") = gridZonalHistoryModule;
  // set the current scope to the new sub-module
  scope gridZonalHistoryScope = gridZonalHistoryModule;

  def("min", terrama2::services::analysis::core::grid::zonal::history::min,
      gridZonalHistoryMin_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                    "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::history::max,
      gridZonalHistoryMax_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                    "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::history::mean,
      gridZonalHistoryMean_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                     "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::history::median,
      gridZonalHistoryMedian_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                       "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::history::standardDeviation,
      gridZonalHistoryStandardDeviation_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::history::variance,
      gridZonalHistoryVariance_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                         "Variance operator for grid zonal"));
  def("sum", terrama2::services::analysis::core::grid::zonal::history::sum,
      gridZonalHistorySum_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                         "Sum operator for grid zonal"));
  def("num", terrama2::services::analysis::core::grid::zonal::history::num,
      gridZonalHistoryNum_overloads(args("dataSeriesName", "discardBefore", "buffer"),
                                    "Num operator for grid zonal"));
  def("list", terrama2::services::analysis::core::grid::zonal::history::list,
      gridZonalHistoryList_overloads(args("dataSeriesName", "discardBefore", "buffer"),
                                     "List operator for grid zonal"));
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumMin_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::min, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumMax_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::max, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumMean_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::mean, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumMedian_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::median, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::standardDeviation, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumVariance_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::variance, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryAccumSum_overloads, terrama2::services::analysis::core::grid::zonal::history::accum::sum, 2, 4)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalHistoryAccumFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.history.accum
  object gridZonalHistoryAccumModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.history.accum"))));
  // make "from terrama2.grid.zonal.history import accum" work
  import("terrama2.grid.zonal.history").attr("accum") = gridZonalHistoryAccumModule;
  // set the current scope to the new sub-module
  scope gridZonalHistoryAccumScope = gridZonalHistoryAccumModule;

  def("min", terrama2::services::analysis::core::grid::zonal::history::accum::min,
      gridZonalHistoryAccumMin_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
                                         "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::history::accum::max,
      gridZonalHistoryAccumMax_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
                                         "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::history::accum::mean,
      gridZonalHistoryAccumMean_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
                                          "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::history::accum::median,
      gridZonalHistoryAccumMedian_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
          "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::history::accum::standardDeviation,
      gridZonalHistoryAccumStandardDeviation_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::history::accum::variance,
      gridZonalHistoryAccumVariance_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
          "Variance operator for grid zonal"));
  def("sum", terrama2::services::analysis::core::grid::zonal::history::accum::sum,
      gridZonalHistoryAccumSum_overloads(args("dataSeriesName", "discardBefore",  "band", "buffer"),
                                              "Sum operator for grid zonal"));
}


// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecCount_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::count, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecMin_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::min, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecMax_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::max, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecMean_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::mean, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecMedian_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::median, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::standardDeviation, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecVariance_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::variance, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryPrecSum_overloads, terrama2::services::analysis::core::grid::zonal::history::prec::sum, 2, 4)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalHistoryPrecFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.history.prec
  object gridZonalHistoryPrecModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.history.prec"))));
  // make "from terrama2.grid.zonal.history import prec" work
  import("terrama2.grid.zonal.history").attr("prec") = gridZonalHistoryPrecModule;
  // set the current scope to the new sub-module
  scope gridZonalHistoryPrecScope = gridZonalHistoryPrecModule;

  def("count", terrama2::services::analysis::core::grid::zonal::history::prec::count,
      gridZonalHistoryPrecCount_overloads(args("dataSeriesName", "discardBefore", "buffer"),
                                          "Count operator for grid zonal"));
  def("min", terrama2::services::analysis::core::grid::zonal::history::prec::min,
      gridZonalHistoryPrecMin_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                        "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::history::prec::max,
      gridZonalHistoryPrecMax_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                        "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::history::prec::mean,
      gridZonalHistoryPrecMean_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                         "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::history::prec::median,
      gridZonalHistoryPrecMedian_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
          "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::history::prec::standardDeviation,
      gridZonalHistoryPrecStandardDeviation_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::history::prec::variance,
      gridZonalHistoryPrecVariance_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
          "Variance operator for grid zonal"));
  def("sum", terrama2::services::analysis::core::grid::zonal::history::prec::sum,
      gridZonalHistoryPrecSum_overloads(args("dataSeriesName", "discardBefore", "band", "buffer"),
                                             "Sum operator for grid zonal"));
}

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastMin_overloads, terrama2::services::analysis::core::grid::zonal::forecast::min, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastMax_overloads, terrama2::services::analysis::core::grid::zonal::forecast::max, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastMean_overloads, terrama2::services::analysis::core::grid::zonal::forecast::mean, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastMedian_overloads, terrama2::services::analysis::core::grid::zonal::forecast::median, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::forecast::standardDeviation, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastVariance_overloads, terrama2::services::analysis::core::grid::zonal::forecast::variance, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastSum_overloads, terrama2::services::analysis::core::grid::zonal::forecast::sum, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastNum_overloads, terrama2::services::analysis::core::grid::zonal::forecast::num, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastList_overloads, terrama2::services::analysis::core::grid::zonal::forecast::list, 2, 3)
// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalForecastFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.forecast
  object gridZonalForecastModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.forecast"))));
  // make "from terrama2.grid.zonal import forecast" work
  import("terrama2.grid.zonal").attr("forecast") = gridZonalForecastModule;
  // set the current scope to the new sub-module
  scope gridZonalForecastScope = gridZonalForecastModule;

  def("min", terrama2::services::analysis::core::grid::zonal::forecast::min,
      gridZonalForecastMin_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                    "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::forecast::max,
      gridZonalForecastMax_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                    "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::forecast::mean,
      gridZonalForecastMean_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                     "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::forecast::median,
      gridZonalForecastMedian_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                       "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::forecast::standardDeviation,
      gridZonalForecastStandardDeviation_overloads(args("dataSeriesName", "discardAfter", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::forecast::variance,
      gridZonalForecastVariance_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                         "Variance operator for grid zonal"));
  def("sum", terrama2::services::analysis::core::grid::zonal::forecast::sum,
      gridZonalForecastSum_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                          "Sum operator for grid zonal"));
  def("num", terrama2::services::analysis::core::grid::zonal::forecast::num,
      gridZonalForecastNum_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                          "Num operator for grid zonal"));
  def("list", terrama2::services::analysis::core::grid::zonal::forecast::list,
      gridZonalForecastList_overloads(args("dataSeriesName", "discardAfter", "buffer"),
                                          "List operator for grid zonal"));
}


// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalMin_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::min, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalMax_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::max, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalMean_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::mean, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalMedian_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::median, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::standardDeviation, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalVariance_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::variance, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalSum_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::sum, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalNum_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::num, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastIntervalList_overloads, terrama2::services::analysis::core::grid::zonal::forecast::interval::list, 3, 4)
// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalForecastIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.forecast
  object gridZonalForecastIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.forecast.interval"))));
  // make "from terrama2.grid.zonal.forecast import interval" work
  import("terrama2.grid.zonal.forecast").attr("interval") = gridZonalForecastIntervalModule;
  // set the current scope to the new sub-module
  scope gridZonalForecastIntervalScope = gridZonalForecastIntervalModule;

  def("min", terrama2::services::analysis::core::grid::zonal::forecast::interval::min,
      gridZonalForecastIntervalMin_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                    "Min operator for grid zonal"));
  def("max", terrama2::services::analysis::core::grid::zonal::forecast::interval::max,
      gridZonalForecastIntervalMax_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                    "Max operator for grid zonal"));
  def("mean", terrama2::services::analysis::core::grid::zonal::forecast::interval::mean,
      gridZonalForecastIntervalMean_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                     "Mean operator for grid zonal"));
  def("median", terrama2::services::analysis::core::grid::zonal::forecast::interval::median,
      gridZonalForecastIntervalMedian_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                       "Median operator for grid zonal"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::forecast::interval::standardDeviation,
      gridZonalForecastIntervalStandardDeviation_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
          "Standard deviation operator for grid zonal"));
  def("variance", terrama2::services::analysis::core::grid::zonal::forecast::interval::variance,
      gridZonalForecastIntervalVariance_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                         "Variance operator for grid zonal"));
  def("sum", terrama2::services::analysis::core::grid::zonal::forecast::interval::sum,
     gridZonalForecastIntervalSum_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                         "Sum operator for grid zonal"));
  def("num", terrama2::services::analysis::core::grid::zonal::forecast::interval::num,
    gridZonalForecastIntervalNum_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                        "Num operator for grid zonal"));
  def("list", terrama2::services::analysis::core::grid::zonal::forecast::interval::list,
     gridZonalForecastIntervalList_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                         "List operator for grid zonal"));
}


// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumCount_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::count, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumMin_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::min, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumMax_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::max, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumMean_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::mean, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumMedian_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::median, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::standardDeviation, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumVariance_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::variance, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalForecastAccumSum_overloads, terrama2::services::analysis::core::grid::zonal::forecast::accum::sum, 2, 3)
// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalForecastAccumFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.forecast
  object gridZonalForecastAccumModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.forecast.accum"))));
  // make "from terrama2.grid.zonal.forecast import accum" work
  import("terrama2.grid.zonal.forecast").attr("accum") = gridZonalForecastAccumModule;
  // set the current scope to the new sub-module
  scope gridZonalForecastAccumScope = gridZonalForecastAccumModule;

  def("count", terrama2::services::analysis::core::grid::zonal::forecast::accum::count,
      gridZonalForecastAccumCount_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                          "Count operator for grid zonal forecast accumulate"));
  def("min", terrama2::services::analysis::core::grid::zonal::forecast::accum::min,
      gridZonalForecastAccumMin_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                             "Min operator for grid zonal forecast accumulate"));
  def("max", terrama2::services::analysis::core::grid::zonal::forecast::accum::max,
      gridZonalForecastAccumMax_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                             "Max operator for grid zonal forecast accumulate"));
  def("mean", terrama2::services::analysis::core::grid::zonal::forecast::accum::mean,
      gridZonalForecastAccumMean_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                              "Mean operator for grid zonal forecast accumulate"));
  def("median", terrama2::services::analysis::core::grid::zonal::forecast::accum::median,
      gridZonalForecastAccumMedian_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                                "Median operator for grid zonal forecast accumulate"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::forecast::accum::standardDeviation,
      gridZonalForecastAccumStandardDeviation_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                                           "Standard deviation operator for grid zonal forecast accumulate"));
  def("variance", terrama2::services::analysis::core::grid::zonal::forecast::accum::variance,
      gridZonalForecastAccumVariance_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                                  "Variance operator for grid zonal forecast accumulate"));
  def("sum", terrama2::services::analysis::core::grid::zonal::forecast::accum::sum,
      gridZonalForecastAccumSum_overloads(args("dataSeriesName",  "discardAfter", "buffer"),
                                             "Sum operator for grid zonal forecast accumulate"));
}


// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalMin_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::min, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalMax_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::max, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalMean_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::mean, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalMedian_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::median, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalStandardDeviation_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::standardDeviation, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalVariance_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::variance, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalSum_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::sum, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalNum_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::num, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(gridZonalHistoryIntervalList_overloads, terrama2::services::analysis::core::grid::zonal::history::interval::list, 3, 4)
// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::Grid::registerGridZonalHistoryIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for grid.zonal.history
  object gridZonalHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.grid.zonal.history.interval"))));
  // make "from terrama2.grid.zonal.history import interval" work
  import("terrama2.grid.zonal.history").attr("interval") = gridZonalHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope gridZonalHistoryIntervalScope = gridZonalHistoryIntervalModule;

  def("min", terrama2::services::analysis::core::grid::zonal::history::interval::min,
      gridZonalHistoryIntervalMin_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                             "Min operator for grid zonal history with interval"));
  def("max", terrama2::services::analysis::core::grid::zonal::history::interval::max,
      gridZonalHistoryIntervalMax_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                             "Max operator for grid zonal history with interval"));
  def("mean", terrama2::services::analysis::core::grid::zonal::history::interval::mean,
      gridZonalHistoryIntervalMean_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                              "Mean operator for grid zonal history with interval"));
  def("median", terrama2::services::analysis::core::grid::zonal::history::interval::median,
      gridZonalHistoryIntervalMedian_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                                "Median operator for grid zonal history with interval"));
  def("standard_deviation", terrama2::services::analysis::core::grid::zonal::history::interval::standardDeviation,
      gridZonalHistoryIntervalStandardDeviation_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                                           "Standard deviation operator for grid zonal history with interval"));
  def("variance", terrama2::services::analysis::core::grid::zonal::history::interval::variance,
      gridZonalHistoryIntervalVariance_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                                  "Variance operator for grid zonal history with interval"));
  def("sum", terrama2::services::analysis::core::grid::zonal::history::interval::sum,
      gridZonalHistoryIntervalSum_overloads(args("dataSeriesName", "discardBefore", "discardAfter", "buffer"),
                                             "Sum operator for grid zonal history with interval"));
  def("num", terrama2::services::analysis::core::grid::zonal::history::interval::num,
      gridZonalHistoryIntervalNum_overloads(args("dataSeriesName", "discardBefore", "buffer"),
                                    "Num operator for grid zonal history with interval"));
  def("list", terrama2::services::analysis::core::grid::zonal::history::interval::list,
      gridZonalHistoryIntervalList_overloads(args("dataSeriesName", "discardBefore", "buffer"),
                                     "List operator for grid zonal history with interval"));
}
