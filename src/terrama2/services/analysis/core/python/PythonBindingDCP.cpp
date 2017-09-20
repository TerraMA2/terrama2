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
#include "../dcp/history/Operator.hpp"
#include "../dcp/history/interval/Operator.hpp"

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMin_overloads, terrama2::services::analysis::core::dcp::min, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMax_overloads, terrama2::services::analysis::core::dcp::max, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMean_overloads, terrama2::services::analysis::core::dcp::mean, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMedian_overloads, terrama2::services::analysis::core::dcp::median, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpSum_overloads, terrama2::services::analysis::core::dcp::sum, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpStandardDeviation_overloads, terrama2::services::analysis::core::dcp::standardDeviation, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpVariance_overloads, terrama2::services::analysis::core::dcp::variance, 1, 2)

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryMin_overloads, terrama2::services::analysis::core::dcp::history::min, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryMax_overloads, terrama2::services::analysis::core::dcp::history::max, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryMean_overloads, terrama2::services::analysis::core::dcp::history::mean, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryMedian_overloads, terrama2::services::analysis::core::dcp::history::median, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistorySum_overloads, terrama2::services::analysis::core::dcp::history::sum, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryStandardDeviation_overloads, terrama2::services::analysis::core::dcp::history::standardDeviation, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryVariance_overloads, terrama2::services::analysis::core::dcp::history::variance, 2, 3)

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalMin_overloads, terrama2::services::analysis::core::dcp::history::interval::min, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalMax_overloads, terrama2::services::analysis::core::dcp::history::interval::max, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalMean_overloads, terrama2::services::analysis::core::dcp::history::interval::mean, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalMedian_overloads, terrama2::services::analysis::core::dcp::history::interval::median, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalSum_overloads, terrama2::services::analysis::core::dcp::history::interval::sum, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalStandardDeviation_overloads, terrama2::services::analysis::core::dcp::history::interval::standardDeviation, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpHistoryIntervalVariance_overloads, terrama2::services::analysis::core::dcp::history::interval::variance, 3, 4)

void terrama2::services::analysis::core::python::DCP::registerFunctions()
{
  registerDCPFunctions();
  registerDCPHistoryFunctions();
  registerDCPHistoryIntervalFunctions();
  registerDCPInfluenceFunctions();
}

void terrama2::services::analysis::core::python::DCP::registerDCPFunctions()
{
  using namespace boost::python;

  // set the current scope to the new sub-module
  scope scpScope = import("terrama2.dcp");

  // export functions inside dcp namespace
  def("value", terrama2::services::analysis::core::dcp::value);
  def("min", terrama2::services::analysis::core::dcp::min,
    dcpMin_overloads(args("attribute", "ids"),
                          "Min operator for dcp"));
  def("max", terrama2::services::analysis::core::dcp::max,
    dcpMax_overloads(args("attribute", "ids"),
                          "Max operator for dcp"));
  def("mean", terrama2::services::analysis::core::dcp::mean,
    dcpMean_overloads(args("attribute", "ids"),
                          "Mean operator for dcp"));
  def("median", terrama2::services::analysis::core::dcp::median,
    dcpMedian_overloads(args("attribute", "ids"),
                          "Median operator for dcp"));
  def("sum", terrama2::services::analysis::core::dcp::sum,
    dcpSum_overloads(args("attribute", "ids"),
                          "Sum operator for dcp"));
  def("Standard_deviation", terrama2::services::analysis::core::dcp::standardDeviation,
    dcpStandardDeviation_overloads(args("attribute", "ids"),
                          "Standard deviation operator for dcp"));
  def("variance", terrama2::services::analysis::core::dcp::variance,
    dcpVariance_overloads(args("attribute", "ids"),
                          "Variance operator for dcp"));
  def("count", terrama2::services::analysis::core::dcp::count);
}

void terrama2::services::analysis::core::python::DCP::registerDCPHistoryFunctions()
{
  using namespace boost::python;

  // Register operations for dcp.history
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history"))));
  // make "from terrama2.dcp import history" work
  import("terrama2.dcp").attr("history") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::history::min,
    dcpHistoryMin_overloads(args("attribute", "dateFilter", "ids"),
                              "Min history operator for dcp"));
  def("max", terrama2::services::analysis::core::dcp::history::max,
    dcpHistoryMax_overloads(args("attribute", "dateFilter", "ids"),
                              "Max history operator for dcp"));
  def("mean", terrama2::services::analysis::core::dcp::history::mean,
    dcpHistoryMean_overloads(args("attribute", "dateFilter", "ids"),
                              "Mean history operator for dcp"));
  def("median", terrama2::services::analysis::core::dcp::history::median,
    dcpHistoryMedian_overloads(args("attribute", "dateFilter", "ids"),
                              "Median history operator for dcp"));
  def("sum", terrama2::services::analysis::core::dcp::history::sum,
    dcpHistorySum_overloads(args("attribute", "dateFilter", "ids"),
                              "Sum history operator for dcp"));
  def("standard_deviation", terrama2::services::analysis::core::dcp::history::standardDeviation,
    dcpHistoryStandardDeviation_overloads(args("attribute", "dateFilter", "ids"),
                              "Standard deviation history operator for dcp"));
  def("variance", terrama2::services::analysis::core::dcp::history::variance,
    dcpHistoryVariance_overloads(args("attribute", "dateFilter", "ids"),
                              "Variance history operator for dcp"));
}

void terrama2::services::analysis::core::python::DCP::registerDCPHistoryIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for dcp.history
  object dcpHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history.interval"))));
  // make "from terrama2.dcp import history" work
  import("terrama2.dcp.history").attr("interval") = dcpHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope dcpHistoryIntervalScope = dcpHistoryIntervalModule;


  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::history::interval::min,
    dcpHistoryIntervalMin_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Min interval history operator for dcp"));
  def("max", terrama2::services::analysis::core::dcp::history::interval::max,
    dcpHistoryIntervalMax_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Max interval history operator for dcp"));
  def("mean", terrama2::services::analysis::core::dcp::history::interval::mean,
    dcpHistoryIntervalMean_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Mean interval history operator for dcp"));
  def("median", terrama2::services::analysis::core::dcp::history::interval::median,
    dcpHistoryIntervalMedian_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Median interval history operator for dcp"));
  def("sum", terrama2::services::analysis::core::dcp::history::interval::sum,
    dcpHistoryIntervalSum_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Sum interval history operator for dcp"));
  def("standard_deviation", terrama2::services::analysis::core::dcp::history::interval::standardDeviation,
    dcpHistoryIntervalStandardDeviation_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Standard deviation interval history operator for dcp"));
  def("variance", terrama2::services::analysis::core::dcp::history::interval::variance,
    dcpHistoryIntervalVariance_overloads(args("attribute", "dateFilterBegin", "dateFilterEnd", "ids"),
                              "Variance interval history operator for dcp"));
}

void terrama2::services::analysis::core::python::DCP::registerDCPInfluenceFunctions()
{
  using namespace boost::python;

  // Register operations for dcp.influence
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.influence"))));
  // make "from terrama2.dcp import influence" work
  import("terrama2.dcp").attr("influence") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  def("by_rule", terrama2::services::analysis::core::dcp::influence::python::byRule);
}
