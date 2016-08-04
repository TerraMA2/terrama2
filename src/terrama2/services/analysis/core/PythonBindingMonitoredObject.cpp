
#include "PythonBindingMonitoredObject.hpp"
#include "dcp/Operator.hpp"
#include "dcp/history/Operator.hpp"
#include "dcp/influence/PythonOperator.hpp"
#include "occurrence/Operator.hpp"
#include "occurrence/aggregation/Operator.hpp"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceCount_overloads, terrama2::services::analysis::core::occurrence::count, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMin_overloads, terrama2::services::analysis::core::occurrence::min, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMax_overloads, terrama2::services::analysis::core::occurrence::max, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMean_overloads, terrama2::services::analysis::core::occurrence::mean, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMedian_overloads, terrama2::services::analysis::core::occurrence::median, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceSum_overloads, terrama2::services::analysis::core::occurrence::sum, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::standardDeviation, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceVariance_overloads, terrama2::services::analysis::core::occurrence::variance, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationCount_overloads, terrama2::services::analysis::core::occurrence::aggregation::count, 4, 5);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMin_overloads, terrama2::services::analysis::core::occurrence::aggregation::min, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMax_overloads, terrama2::services::analysis::core::occurrence::aggregation::max, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMean_overloads, terrama2::services::analysis::core::occurrence::aggregation::mean, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMedian_overloads, terrama2::services::analysis::core::occurrence::aggregation::median, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationSum_overloads, terrama2::services::analysis::core::occurrence::aggregation::sum, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::aggregation::standardDeviation, 6, 7);

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationVariance_overloads, terrama2::services::analysis::core::occurrence::aggregation::variance, 6, 7);

void terrama2::services::analysis::core::python::MonitoredObject::registerFunctions()
{
  registerOccurrenceFunctions();
  registerDCPFunctions();
}

void terrama2::services::analysis::core::python::MonitoredObject::registerOccurrenceFunctions()
{
  using namespace boost::python;

  // map the occurence namespace to a sub-module
  // make "from terrama2.occurence import <function>" work
  boost::python::object occurrenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence"))));
  // make "from terrama2 import occurence" work
  import("terrama2").attr("occurrence") = occurrenceModule;
  // set the current scope to the new sub-module
  scope occurrenceScope = occurrenceModule;

  // export functions inside occurrence namespace
  def("count", terrama2::services::analysis::core::occurrence::count,
      occurrenceCount_overloads(args("dataSeriesName", "buffer", "dateFilter", "restriction"),
                                "Count operator for occurrence"));
  def("min", terrama2::services::analysis::core::occurrence::min,
      occurrenceMin_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Minimum operator for occurrence"));
  def("max", terrama2::services::analysis::core::occurrence::max,
      occurrenceMax_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Maximum operator for occurrence"));
  def("mean", terrama2::services::analysis::core::occurrence::mean,
      occurrenceMean_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Mean operator for occurrence"));
  def("median", terrama2::services::analysis::core::occurrence::median,
      occurrenceMedian_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Median operator for occurrence"));
  def("sum", terrama2::services::analysis::core::occurrence::sum,
      occurrenceSum_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"), "Sum operator for occurrence"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::standardDeviation,
      occurrenceStandardDeviation_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"),
          "Standard deviation operator for occurrence"));
  def("variance", terrama2::services::analysis::core::occurrence::variance,
      occurrenceVariance_overloads(args("dataSeriesName", "buffer", "dateFilter", "attribute", "restriction"),
                                   "Variance operator for occurrence"));

  // Register operations for occurrence.aggregation
  object occurrenceAggregationModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence.aggregation"))));
  // make "from terrama2.occurrence import aggregation" work
  scope().attr("aggregation") = occurrenceAggregationModule;
  // set the current scope to the new sub-module
  scope occurrenceAggregationScope = occurrenceAggregationModule;

  def("count", terrama2::services::analysis::core::occurrence::aggregation::count,
      occurrenceAggregationCount_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
          "Count operator for occurrence aggregation"));
  def("min", terrama2::services::analysis::core::occurrence::aggregation::min,
      occurrenceAggregationMin_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Minimum operator for occurrence aggregation"));
  def("max", terrama2::services::analysis::core::occurrence::aggregation::max,
      occurrenceAggregationMax_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Maximum operator for occurrence aggregation"));
  def("mean", terrama2::services::analysis::core::occurrence::aggregation::mean,
      occurrenceAggregationMean_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Mean operator for occurrence aggregation"));
  def("median", terrama2::services::analysis::core::occurrence::aggregation::median,
      occurrenceAggregationMedian_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Median operator for occurrence aggregation"));
  def("sum", terrama2::services::analysis::core::occurrence::aggregation::sum,
      occurrenceAggregationSum_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"), "Sum operator for occurrence aggregation"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::aggregation::standardDeviation,
      occurrenceAggregationStandardDeviation_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
          "Standard deviation operator for occurrence aggregation"));
  def("variance", terrama2::services::analysis::core::occurrence::aggregation::variance,
      occurrenceAggregationVariance_overloads(args("dataSeriesName", "buffer", "dateFilter", "aggregationBuffer", "restriction"),
          "Variance operator for occurrence aggregation"));
}


void terrama2::services::analysis::core::python::MonitoredObject::registerDCPFunctions()
{
  using namespace boost::python;

  // map the dcp namespace to a sub-module
  // make "from terrama2.dcp import <function>" work
  boost::python::object dcpModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp"))));
  // make "from terrama2 import dcp" work
  import("terrama2").attr("dcp") = dcpModule;
  // set the current scope to the new sub-module
  scope scpScope = dcpModule;

  // export functions inside dcp namespace
  def("min", terrama2::services::analysis::core::dcp::min);
  def("max", terrama2::services::analysis::core::dcp::max);
  def("mean", terrama2::services::analysis::core::dcp::mean);
  def("median", terrama2::services::analysis::core::dcp::median);
  def("sum", terrama2::services::analysis::core::dcp::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::standardDeviation);
  def("variance", terrama2::services::analysis::core::dcp::variance);
  def("count", terrama2::services::analysis::core::dcp::count);

  // Register operations for dcp.history
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history"))));
  // make "from terrama2.dcp import history" work
  scope().attr("history") = dcpHistoryModule;


  // Register operations for dcp.history
  object dcpInfluenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.influence"))));
  // make "from terrama2.dcp import history" work
  scope().attr("influence") = dcpInfluenceModule;


  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::history::min);
  def("max", terrama2::services::analysis::core::dcp::history::max);
  def("mean", terrama2::services::analysis::core::dcp::history::mean);
  def("median", terrama2::services::analysis::core::dcp::history::median);
  def("sum", terrama2::services::analysis::core::dcp::history::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::history::standardDeviation);
  def("variance", terrama2::services::analysis::core::dcp::history::variance);


  // set the current scope to the new sub-module
  scope dcpInfluenceScope = dcpInfluenceModule;

  // export functions inside history namespace
  def("by_attribute", terrama2::services::analysis::core::dcp::influence::python::byAttribute);
  def("by_rule", terrama2::services::analysis::core::dcp::influence::python::byRule);

}
