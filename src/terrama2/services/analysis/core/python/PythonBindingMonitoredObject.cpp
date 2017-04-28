
#include "PythonBindingMonitoredObject.hpp"
#include "../dcp/zonal/Operator.hpp"
#include "../dcp/zonal/history/Operator.hpp"
#include "../dcp/zonal/history/interval/Operator.hpp"
#include "../dcp/zonal/influence/PythonOperator.hpp"
#include "../occurrence/zonal/Operator.hpp"
#include "../occurrence/zonal/interval/Operator.hpp"
#include "../occurrence/zonal/aggregation/Operator.hpp"

// pragma to silence python macros warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedef"

// // Declaration needed for default parameter restriction
BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceCount_overloads, terrama2::services::analysis::core::occurrence::zonal::count, 2, 4)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMin_overloads, terrama2::services::analysis::core::occurrence::zonal::min, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMax_overloads, terrama2::services::analysis::core::occurrence::zonal::max, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMean_overloads, terrama2::services::analysis::core::occurrence::zonal::mean, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceMedian_overloads, terrama2::services::analysis::core::occurrence::zonal::median, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceSum_overloads, terrama2::services::analysis::core::occurrence::zonal::sum, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::zonal::standardDeviation, 4, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceVariance_overloads, terrama2::services::analysis::core::occurrence::zonal::variance, 3, 5)

// Occurence interval


BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalCount_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::count, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalMin_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::min, 4, 6)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalMax_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::max, 4, 6)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalMean_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::mean, 4, 6)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalMedian_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::median, 4, 6)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalSum_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::sum, 4, 6)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::standardDeviation, 4, 6)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceIntervalVariance_overloads, terrama2::services::analysis::core::occurrence::zonal::interval::variance, 4, 6)

// Occurence aggreagation

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationCount_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::count, 3, 5)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMin_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::min, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMax_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::max, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMean_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::mean, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationMedian_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::median, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationSum_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::sum, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationStandardDeviation_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::standardDeviation, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(occurrenceAggregationVariance_overloads, terrama2::services::analysis::core::occurrence::zonal::aggregation::variance, 5, 7)

BOOST_PYTHON_FUNCTION_OVERLOADS(dcpInfluenceByRule_overloads, terrama2::services::analysis::core::dcp::zonal::influence::python::byRule, 1, 2)

// closing "-Wunused-local-typedef" pragma
#pragma GCC diagnostic pop

void terrama2::services::analysis::core::python::MonitoredObject::registerFunctions()
{
  registerOccurrenceFunctions();
  registerOccurrenceIntervalFunctions();
  registerOccurrenceAggregationFunctions();
  registerDCPFunctions();
  registerDCPZonalFunctions();
  registerDCPZonalHistoryFunctions();
  registerDCPZonalHistoryIntervalFunctions();
  registerDCPZonalInfluenceFunctions();

}

void terrama2::services::analysis::core::python::MonitoredObject::registerOccurrenceFunctions()
{
  using namespace boost::python;

  // map the occurence namespace to a sub-module
  // make "from terrama2.occurence import <function>" work
  boost::python::object occurrenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence"))));
  // make "from terrama2 import occurrence" work
  import("terrama2").attr("occurrence") = occurrenceModule;
  // set the current scope to the new sub-module
  scope occurrenceScope = occurrenceModule;

  boost::python::object occurrenceZonalModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence.zonal"))));
  import("terrama2.occurrence").attr("zonal") = occurrenceZonalModule;

  scope occurrenceZonalScope = occurrenceZonalModule;

  // export functions inside occurrence namespace
  def("count", terrama2::services::analysis::core::occurrence::zonal::count,
      occurrenceCount_overloads(args("dataSeriesName", "dateFilter", "buffer", "restriction"),
                                "Count operator for occurrence"));
  def("min", terrama2::services::analysis::core::occurrence::zonal::min,
      occurrenceMin_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Minimum operator for occurrence"));
  def("max", terrama2::services::analysis::core::occurrence::zonal::max,
      occurrenceMax_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Maximum operator for occurrence"));
  def("mean", terrama2::services::analysis::core::occurrence::zonal::mean,
      occurrenceMean_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Mean operator for occurrence"));
  def("median", terrama2::services::analysis::core::occurrence::zonal::median,
      occurrenceMedian_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Median operator for occurrence"));
  def("sum", terrama2::services::analysis::core::occurrence::zonal::sum,
      occurrenceSum_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Sum operator for occurrence"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::zonal::standardDeviation,
      occurrenceStandardDeviation_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"),
          "Standard deviation operator for occurrence"));
  def("variance", terrama2::services::analysis::core::occurrence::zonal::variance,
      occurrenceVariance_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"),
                                   "Variance operator for occurrence"));

}


void terrama2::services::analysis::core::python::MonitoredObject::registerOccurrenceIntervalFunctions()
{
  using namespace boost::python;
  // Register operations for occurrence.aggregation
  object occurrenceIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence.zonal.interval"))));
  // make "from terrama2.occurrence import aggregation" work
  import("terrama2.occurrence.zonal").attr("interval") = occurrenceIntervalModule;
  // set the current scope to the new sub-module
  scope occurrenceIntervalScope = occurrenceIntervalModule;

  // export functions inside occurrence namespace
  def("count", terrama2::services::analysis::core::occurrence::zonal::interval::count,
      occurrenceIntervalCount_overloads(args("dataSeriesName", "dateFilter", "buffer", "restriction"),
                                "Count operator for occurrence using time interval"));
  def("min", terrama2::services::analysis::core::occurrence::zonal::interval::min,
      occurrenceIntervalMin_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Minimum operator for occurrence using time interval"));
  def("max", terrama2::services::analysis::core::occurrence::zonal::interval::max,
      occurrenceIntervalMax_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Maximum operator for occurrence using time interval"));
  def("mean", terrama2::services::analysis::core::occurrence::zonal::interval::mean,
      occurrenceIntervalMean_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Mean operator for occurrence using time interval"));
  def("median", terrama2::services::analysis::core::occurrence::zonal::interval::median,
      occurrenceIntervalMedian_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Median operator for occurrence using time interval"));
  def("sum", terrama2::services::analysis::core::occurrence::zonal::interval::sum,
      occurrenceIntervalSum_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"), "Sum operator for occurrence using time interval"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::zonal::interval::standardDeviation,
      occurrenceIntervalStandardDeviation_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"),
                                            "Standard deviation operator for occurrence using time interval"));
  def("variance", terrama2::services::analysis::core::occurrence::zonal::interval::variance,
      occurrenceIntervalVariance_overloads(args("dataSeriesName", "attribute", "dateFilter", "buffer", "restriction"),
                                   "Variance operator for occurrence using time interval"));

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

}

void terrama2::services::analysis::core::python::MonitoredObject::registerDCPZonalFunctions()
{
  using namespace boost::python;

  // map the dcp namespace to a sub-module
  // make "from terrama2.dcp import <function>" work
  boost::python::object dcpModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.zonal"))));
  // make "from terrama2 import dcp" work
  import("terrama2.dcp").attr("zonal") = dcpModule;
  // set the current scope to the new sub-module
  scope scpScope = dcpModule;

  // export functions inside dcp namespace
  def("min", terrama2::services::analysis::core::dcp::zonal::min);
  def("max", terrama2::services::analysis::core::dcp::zonal::max);
  def("mean", terrama2::services::analysis::core::dcp::zonal::mean);
  def("median", terrama2::services::analysis::core::dcp::zonal::median);
  def("sum", terrama2::services::analysis::core::dcp::zonal::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::zonal::standardDeviation);
  def("variance", terrama2::services::analysis::core::dcp::zonal::variance);
  def("count", terrama2::services::analysis::core::dcp::zonal::count);
}

void terrama2::services::analysis::core::python::MonitoredObject::registerDCPZonalHistoryFunctions()
{
  using namespace boost::python;

  // Register operations for dcp.history
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.zonal.history"))));
  // make "from terrama2.dcp import history" work
  import("terrama2.dcp.zonal").attr("history") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::zonal::history::min);
  def("max", terrama2::services::analysis::core::dcp::zonal::history::max);
  def("mean", terrama2::services::analysis::core::dcp::zonal::history::mean);
  def("median", terrama2::services::analysis::core::dcp::zonal::history::median);
  def("sum", terrama2::services::analysis::core::dcp::zonal::history::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::zonal::history::standardDeviation);
  def("variance", terrama2::services::analysis::core::dcp::zonal::history::variance);
}

void terrama2::services::analysis::core::python::MonitoredObject::registerDCPZonalHistoryIntervalFunctions()
{
  using namespace boost::python;

  // Register operations for dcp.history
  object dcpHistoryIntervalModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.zonal.history.interval"))));
  // make "from terrama2.dcp import history" work
  import("terrama2.dcp.zonal.history").attr("interval") = dcpHistoryIntervalModule;
  // set the current scope to the new sub-module
  scope dcpHistoryIntervalScope = dcpHistoryIntervalModule;

  // export functions inside history namespace
  def("min", terrama2::services::analysis::core::dcp::zonal::history::interval::min);
  def("max", terrama2::services::analysis::core::dcp::zonal::history::interval::max);
  def("mean", terrama2::services::analysis::core::dcp::zonal::history::interval::mean);
  def("median", terrama2::services::analysis::core::dcp::zonal::history::interval::median);
  def("sum", terrama2::services::analysis::core::dcp::zonal::history::interval::sum);
  def("standard_deviation", terrama2::services::analysis::core::dcp::zonal::history::interval::standardDeviation);
  def("variance", terrama2::services::analysis::core::dcp::zonal::history::interval::variance);
}

void terrama2::services::analysis::core::python::MonitoredObject::registerDCPZonalInfluenceFunctions()
{
  using namespace boost::python;

  // Register operations for dcp.history
  object dcpInfluenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.zonal.influence"))));

  import("terrama2.dcp.zonal").attr("influence") = dcpInfluenceModule;
  // set the current scope to the new sub-module
  scope dcpInfluenceScope = dcpInfluenceModule;


  // export functions inside history namespace
  def("by_attribute", terrama2::services::analysis::core::dcp::zonal::influence::python::byAttribute);

  def("by_rule", terrama2::services::analysis::core::dcp::zonal::influence::python::byRule,
      dcpInfluenceByRule_overloads(args("dataSeriesName", "buffer"), "Influence by rule operator"));
}

void terrama2::services::analysis::core::python::MonitoredObject::registerOccurrenceAggregationFunctions()
{
  using namespace boost::python;
  // Register operations for occurrence.aggregation
  object occurrenceAggregationModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence.zonal.aggregation"))));
  // make "from terrama2.occurrence import aggregation" work
  import("terrama2.occurrence.zonal").attr("aggregation") = occurrenceAggregationModule;
  // set the current scope to the new sub-module
  scope occurrenceAggregationScope = occurrenceAggregationModule;

  def("count", terrama2::services::analysis::core::occurrence::zonal::aggregation::count,
      occurrenceAggregationCount_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"),
                                           "Count operator for occurrence aggregation"));
  def("min", terrama2::services::analysis::core::occurrence::zonal::aggregation::min,
      occurrenceAggregationMin_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"), "Minimum operator for occurrence aggregation"));
  def("max", terrama2::services::analysis::core::occurrence::zonal::aggregation::max,
      occurrenceAggregationMax_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"), "Maximum operator for occurrence aggregation"));
  def("mean", terrama2::services::analysis::core::occurrence::zonal::aggregation::mean,
      occurrenceAggregationMean_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"), "Mean operator for occurrence aggregation"));
  def("median", terrama2::services::analysis::core::occurrence::zonal::aggregation::median,
      occurrenceAggregationMedian_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"), "Median operator for occurrence aggregation"));
  def("sum", terrama2::services::analysis::core::occurrence::zonal::aggregation::sum,
      occurrenceAggregationSum_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"), "Sum operator for occurrence aggregation"));
  def("standard_deviation", terrama2::services::analysis::core::occurrence::zonal::aggregation::standardDeviation,
      occurrenceAggregationStandardDeviation_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"),
                                                       "Standard deviation operator for occurrence aggregation"));
  def("variance", terrama2::services::analysis::core::occurrence::zonal::aggregation::variance,
      occurrenceAggregationVariance_overloads(args("dataSeriesName", "dateFilter", "aggregationBuffer", "buffer", "restriction"),
                                              "Variance operator for occurrence aggregation"));
}
