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
  \file terrama2/services/analysis/core/PythonInterpreter.cpp

  \brief Manages the communication of Python and C.

  \author Paulo R. M. Oliveira
*/



#include "PythonInterpreter.hpp"

#include <boost/python.hpp>

#include <QObject>
#include <QTextStream>
#include <QFile>

#include "BufferMemory.hpp"
#include "Context.hpp"
#include "Analysis.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../core/Shared.hpp"


// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>
#include <terralib/common/UnitOfMeasure.h>

#include <math.h>

using namespace boost::python;

int terrama2::services::analysis::core::occurrenceCount(const std::string& dataSeriesName, double radius, Buffer bufferType, std::string dateFilter, std::string restriction)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);

  uint64_t count = 0;
  bool found = false;

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  std::shared_ptr<ContextDataset> moDsContext;

  // Reads the object monitored
  for(AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      terrama2::core::DataSeriesPtr& dataSeries = analysisDataSeries.dataSeries;
      assert(dataSeries->datasetList.size() == 1);
      auto datasetMO = dataSeries->datasetList[0];

      if(datasetMO->id == 0)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Invalid dataset for monitored object."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return 0;
      }

      if(!Context::getInstance().exists(analysis.id, datasetMO->id))
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return 0;
      }

      moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

      if(!moDsContext)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return 0;
      }
    }
  }


  auto moGeom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!moGeom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return 0;
  }



  std::shared_ptr<ContextDataset> contextDataset;

  for(auto& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;

      Context::getInstance().addDataset(analysisId, analysisDataSeries.dataSeries, dateFilter, true);

      auto datasets = analysisDataSeries.dataSeries->datasetList;

      for(auto dataset : datasets)
      {

        contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilter);
        if(!contextDataset)
        {
          continue;
        }


        // Save thread state before entering multi-thread zone

        Py_BEGIN_ALLOW_THREADS

        std::vector<uint64_t> indexes;
        terrama2::core::SyncronizedDataSetPtr syncDs = contextDataset->series.syncDataSet;

        // Converts the monitored object to the same srid of the occurrences
        auto sampleGeom = syncDs->getGeometry(0, contextDataset->geometryPos);

        if(contextDataset->series.syncDataSet->size() > 0)
        {
          auto spatialRefSystem = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(moGeom->getSRID());
          std::string unit = spatialRefSystem->getName();

          if(bufferType != NONE && unit != "meter")
          {
            // Needs to convert to a srs in meters
            // if the unit of the occurrence is already in meters transform to that srs.
            auto spatialRefSystem = te::srs::SpatialReferenceSystemManager::getInstance().getUnit(sampleGeom->getSRID());
            std::string unit = spatialRefSystem->getName();
            if(unit == "meter")
            {
              moGeom->transform(sampleGeom->getSRID());
            }
            else
            {
              // UTM SAD 69  zone 23
              moGeom->transform(29193);
            }
          }



          switch (bufferType)
          {
            case NONE:
              break;
            case EXTERN:
            {
              assert(false);
              break;
            }
            case INTERN:
            {
              assert(false);
              break;
            }
            case INTERN_PLUS_EXTERN:
            {
              assert(false);
              break;
            }
            case OBJECT_PLUS_EXTERN:
            {
              assert(false);
              break;
            }
            case OBJECT_WITHOUT_INTERN:
            {
              assert(false);
              break;
            }
          }


          // Searchs in the spatial index for the occurrences that intersects the monitored object box
          contextDataset->rtree.search(*moGeom->getMBR(), indexes);


          std::vector<std::shared_ptr<te::gm::Geometry> > geometries;
          for(uint64_t i : indexes)
          {
            // Verifies if the occurrence intersects the monitored object
            auto occurenceGeom = syncDs->getGeometry(i, contextDataset->geometryPos);
            if(occurenceGeom->intersects(moGeom.get()))
            {
              geometries.push_back(occurenceGeom);
            }
          }

          count = geometries.size();

          // TODO Paulo: Use this code in the occurrence operator with aggregation buffer
          /*if(!geometries.empty())
          {
            // Creates aggregation buffer
            std::shared_ptr<te::gm::Envelope> box(syncDs->dataset()->getExtent(contextDataset->geometryPos));
            if(radius != 0.)
            {
              auto bufferDs = createBuffer(geometries, box, radius);
              count = bufferDs->size();
            }
            else
            {
              count = geometries.size();
            }
          }*/
        }

        // All operations are done, acquires the GIL and set the return value
        Py_END_ALLOW_THREADS
      }
    }
  }

  return count;
}

void terrama2::services::analysis::core::addValue(const std::string& attribute, double value)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);
  if(analysis.type == MONITORED_OBJECT_TYPE)
  {
    std::shared_ptr<ContextDataset> moDsContext;
    terrama2::core::DataSetPtr datasetMO;

    // Reads the object monitored
    auto analysisDataSeriesList = analysis.analysisDataSeriesList;
    for(auto analysisDataSeries : analysisDataSeriesList)
    {
      if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
      {
        assert(analysisDataSeries.dataSeries->datasetList.size() == 1);
        datasetMO = analysisDataSeries.dataSeries->datasetList[0];

        if(!Context::getInstance().exists(analysis.id, datasetMO->id))
        {
          QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
        }

        moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

        if(moDsContext->identifier.empty())
          assert(false);

        // Stores the result in the context
        std::string geomId = moDsContext->series.syncDataSet->getString(index, moDsContext->identifier);
        Context::getInstance().setAnalysisResult(analysisId, geomId, attribute, value);

        QString errMsg(QObject::tr("Analysis: %1 -> Geometry: %2 \nAttribute: %3 \nResult: %4").arg(analysis.id).arg(geomId.c_str()).arg(attribute.c_str()).arg(value));
        TERRAMA2_LOG_INFO() << errMsg;

      }
    }
  }
}


int terrama2::services::analysis::core::dcpCount(const std::string& dataSeriesName, double radius, Buffer bufferType)
{
  return dcpOperator(COUNT, dataSeriesName, "", radius, bufferType);
}

double terrama2::services::analysis::core::dcpMin(const std::string& dataSeriesName, const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MIN, dataSeriesName, attribute, radius, bufferType, ids);
}

double terrama2::services::analysis::core::dcpMax(const std::string& dataSeriesName, const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MAX, dataSeriesName, attribute, radius, bufferType, ids);
}

double terrama2::services::analysis::core::dcpMean(const std::string& dataSeriesName, const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MEAN, dataSeriesName, attribute, radius, bufferType, ids);
}

double terrama2::services::analysis::core::dcpMedian(const std::string& dataSeriesName, const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MEDIAN, dataSeriesName, attribute, radius, bufferType, ids);
}

double terrama2::services::analysis::core::dcpSum(const std::string& dataSeriesName, const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(SUM, dataSeriesName, attribute, radius, bufferType, ids);
}

double terrama2::services::analysis::core::dcpStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(STANDARD_DEVIATION, dataSeriesName, attribute, radius, bufferType, ids);
}

double terrama2::services::analysis::core::dcpOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName,  const std::string& attribute, double radius, Buffer bufferType, boost::python::list ids)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);

  uint64_t count = 0;
  double sum = 0;
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double median = 0;
  double mean = 0;
  double standardDeviation = 0;
  bool found = false;
  bool hasData = false;

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);


  std::shared_ptr<ContextDataset> moDsContext;
  terrama2::core::DataSetPtr datasetMO;

  // Reads the object monitored
  for(AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(analysisDataSeries.dataSeries->datasetList.size() == 1);
      datasetMO = analysisDataSeries.dataSeries->datasetList[0];

      if(!Context::getInstance().exists(analysis.id, datasetMO->id))
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NAN;
      }

      moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

      if(!moDsContext)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NAN;
      }
    }
  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }


  std::shared_ptr<ContextDataset> contextDataset;

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;

      if(analysisDataSeries.dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesSemantics::DCP)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NAN;
      }

      Context::getInstance().addDCP(analysisId, analysisDataSeries.dataSeries, "", true);

      for(auto dataset : analysisDataSeries.dataSeries->datasetList)
      {
        contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id);

        terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
        if(!dcpDataset)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> Could not recover DCP dataset."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }


        if(dcpDataset->position == nullptr)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }

        // Frees the GIL, from now on can not use the interpreter
        Py_BEGIN_ALLOW_THREADS

        auto metadata = analysisDataSeries.metadata;

        if(metadata["INFLUENCE_TYPE"] != "REGION")
        {

          auto buffer = dcpDataset->position->buffer(atof(metadata["RADIUS"].c_str()), 16, te::gm::CapButtType);

          int srid  = dcpDataset->position->getSRID();
          if(srid == 0)
          {
            auto format = datasetMO->format;
            if(format.find("srid") != format.end())
            {
              srid = std::stoi(format["srid"]);
            }
          }

          auto polygon = dynamic_cast<te::gm::MultiPolygon*>(geom.get());

          if(polygon != nullptr)
          {
            auto centroid = polygon->getCentroid();
            if(centroid->getSRID() == 0)
              centroid->setSRID(srid);
            else if(centroid->getSRID() != srid && srid != 0)
              centroid->transform(srid);

            std::vector<double> values;
            if((metadata["INFLUENCE_TYPE"] == "RADIUS_CENTER" && centroid->within(buffer)) || (metadata["INFLUENCE_TYPE"] == "RADIUS_TOUCHES" && polygon->touches(buffer)))
            {
              count = contextDataset->series.syncDataSet->size();

              if(count == 0)
                continue;
              for(unsigned int i = 0; i < count; ++i)
              {
                try
                {
                  if(!attribute.empty() && !contextDataset->series.syncDataSet->isNull(i, attribute))
                  {
                    hasData = true;

                    double value = contextDataset->series.syncDataSet->getDouble(i, attribute);
                    values.push_back(value);
                    sum += value;
                    if(value > max)
                      max = value;
                    if(value < min)
                      min = value;
                  }
                }
                catch(...)
                {
                  // In case the DCP doesn't have the specified column
                  continue;
                }
              }

              mean = sum / count;
              std::sort (values.begin(), values.end());
              double half = values.size() / 2;
              if(values.size() > 1 && values.size() % 2 == 0)
              {
                median = values[(int)half] + values[(int)half - 1] / 2;
              }
              else
              {
                median = values.size() == 1 ? values[0] : 0.;
              }

            }
          }
          else
          {
            // TODO: Monitored object is not a multi polygon.
            assert(false);
          }
        }

        // All operations are done, acquires the GIL and set the return value
        Py_END_ALLOW_THREADS

      }


      break;
    }
  }

  if(!hasData)
  {
    return NAN;
  }


  if(found)
  {
    switch (statisticOperation)
    {
      case SUM:
        return sum;
      case MEAN:
        return mean;
      case MIN:
        return min;
      case MAX:
        return max;
      case STANDARD_DEVIATION:
        return standardDeviation;
      case MEDIAN:
        return median;
      case COUNT:
        return count;
    }
  }

  return NAN;
}


BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMin_overloads, terrama2::services::analysis::core::dcpMin, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMax_overloads, terrama2::services::analysis::core::dcpMax, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMean_overloads, terrama2::services::analysis::core::dcpMean, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMedian_overloads, terrama2::services::analysis::core::dcpMedian, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpSum_overloads, terrama2::services::analysis::core::dcpSum, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpStandardDeviation_overloads, terrama2::services::analysis::core::dcpStandardDeviation, 4, 5)


void terrama2::services::analysis::core::exportDCP()
{
  // map the dcp namespace to a sub-module
  // make "from terrama2.dcp import <function>" work
  object dcpModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp"))));
  // make "from terrama2 import dcp" work
  scope().attr("dcp") = dcpModule;
  // set the current scope to the new sub-module
  scope dcpScope = dcpModule;
  // export functions inside dcp namespace
  def("min", terrama2::services::analysis::core::dcpMin, dcpMin_overloads(args("dataSeriesName", "attribute", "radius", "bufferType", "ids"), "Minimum operator for DCP"));
  def("max", terrama2::services::analysis::core::dcpMax, dcpMax_overloads(args("dataSeriesName", "attribute", "radius", "bufferType", "ids"), "Maximum operator for DCP"));
  def("mean", terrama2::services::analysis::core::dcpMean, dcpMean_overloads(args("dataSeriesName", "attribute", "radius", "bufferType", "ids"), "Mean operator for DCP"));
  def("median", terrama2::services::analysis::core::dcpMedian, dcpMedian_overloads(args("dataSeriesName", "attribute", "radius", "bufferType", "ids"), "Median operator for DCP"));
  def("sum", terrama2::services::analysis::core::dcpSum, dcpSum_overloads(args("dataSeriesName", "attribute", "radius", "bufferType", "ids"), "Sum operator for DCP"));
  def("standardDeviation", terrama2::services::analysis::core::dcpStandardDeviation, dcpStandardDeviation_overloads(args("dataSeriesName", "attribute", "radius", "bufferType", "ids"), "Standard deviation operator for DCP"));

  def("count", terrama2::services::analysis::core::dcpCount);

  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history"))));
  // make "from terrama2 import dcp" work
  scope().attr("history") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;
  // export functions inside dcp namespace

  def("min", terrama2::services::analysis::core::dcpHistoryMin);
  def("max", terrama2::services::analysis::core::dcpHistoryMax);
  def("mean", terrama2::services::analysis::core::dcpHistoryMean);
  def("median", terrama2::services::analysis::core::dcpHistoryMedian);
  def("sum", terrama2::services::analysis::core::dcpHistorySum);
  def("standardDeviation", terrama2::services::analysis::core::dcpHistoryStandardDeviation);

}

void terrama2::services::analysis::core::exportOccurrence()
{
  // map the occurrence namespace to a sub-module
  // make "from terrama2.occurrence import <function>" work
  object occurrenceModule(handle<>(borrowed(PyImport_AddModule("terrama2.occurrence"))));
  // make "from terrama2 import occurrence" work
  scope().attr("occurrence") = occurrenceModule;
  // set the current scope to the new sub-module
  scope occurrenceScope = occurrenceModule;
  // export functions inside occurrence namespace
  def("count", terrama2::services::analysis::core::occurrenceCount);
}


BOOST_PYTHON_MODULE(terrama2)
{
  // specify that this module is actually a package
  object package = scope();
  package.attr("__path__") = "terrama2";


  def("add_value", terrama2::services::analysis::core::addValue);

  enum_<terrama2::services::analysis::core::Buffer>("Buffer")
          .value("EXTERN", terrama2::services::analysis::core::EXTERN)
          .value("INTERN", terrama2::services::analysis::core::INTERN)
          .value("INTERN_PLUS_EXTERN", terrama2::services::analysis::core::INTERN_PLUS_EXTERN)
          .value("OBJECT_PLUS_EXTERN", terrama2::services::analysis::core::OBJECT_PLUS_EXTERN)
          .value("OBJECT_WITHOUT_INTERN", terrama2::services::analysis::core::OBJECT_WITHOUT_INTERN);

  terrama2::services::analysis::core::exportDCP();
  terrama2::services::analysis::core::exportOccurrence();

}

#if PY_MAJOR_VERSION >= 3
#   define INIT_MODULE PyInit_terrama2
    extern "C" PyObject* INIT_MODULE();
#else
#   define INIT_MODULE initterrama2
    extern "C" void INIT_MODULE();
#endif


void terrama2::services::analysis::core::initInterpreter()
{
  Py_Initialize();

  PyEval_InitThreads();

  initterrama2();

  // release our hold on the global interpreter
  PyEval_ReleaseLock();
}

void terrama2::services::analysis::core::finalizeInterpreter()
{
  // shut down the interpreter
  PyEval_AcquireLock();
  Py_Finalize();
}



void terrama2::services::analysis::core::runScriptMonitoredObjectAnalysis(PyThreadState* state, uint64_t analysisId, std::vector<uint64_t> indexes)
{
  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  for(uint64_t index : indexes)
  {
    // grab the global interpreter lock
    PyEval_AcquireLock();
    // swap in my thread state
    PyThreadState_Swap(state);


    PyObject *indexValue = PyInt_FromLong(index);
    PyObject *analysisValue = PyInt_FromLong(analysisId);

    PyObject* poDict = PyDict_New();
    PyDict_SetItemString(poDict, "index", indexValue);
    PyDict_SetItemString(poDict, "analysis", analysisValue);
    state->dict = poDict;

    try
    {
      PyRun_SimpleString("from terrama2 import *");
      PyRun_SimpleString(analysis.script.c_str());
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Error running the python script."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
    }


    // release our hold on the global interpreter
    PyEval_ReleaseLock();
  }
}

void terrama2::services::analysis::core::runScriptDCPAnalysis(PyThreadState* state, uint64_t analysisId)
{

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  // grab the global interpreter lock
  PyEval_AcquireLock();
  // swap in my thread state
  PyThreadState_Swap(state);

  PyThreadState_Clear(state);

  PyObject *analysisValue = PyInt_FromLong(analysisId);

  PyObject* poDict = PyDict_New();
  PyDict_SetItemString(poDict, "analysis", analysisValue);
  state->dict = poDict;

  PyRun_SimpleString("from terrama2 import *");
  PyRun_SimpleString(analysis.script.c_str());


  // release our hold on the global interpreter
  PyEval_ReleaseLock();

}


double terrama2::services::analysis::core::dcpHistoryOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);


  double sum = 0;
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double median = 0;
  double mean = 0;
  double standardDeviation = 0;
  uint64_t count = 0;
  bool found = false;

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);


  std::shared_ptr<ContextDataset> moDsContext;
  terrama2::core::DataSetPtr datasetMO;

  // Reads the object monitored
  for(AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(analysisDataSeries.dataSeries->datasetList.size() == 1);
      datasetMO = analysisDataSeries.dataSeries->datasetList[0];

      if(!Context::getInstance().exists(analysis.id, datasetMO->id))
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return 0.;
      }

      moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

      if(!moDsContext)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return 0.;
      }
    }
  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return 0.;
  }


  std::shared_ptr<ContextDataset> contextDataset;

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;

      if(analysisDataSeries.dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesSemantics::DCP)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return 0.;
      }

      Context::getInstance().addDCP(analysisId, analysisDataSeries.dataSeries, dateFilter, false);

      for(auto dataset : analysisDataSeries.dataSeries->datasetList)
      {
        contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilter);

        terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
        if(!dcpDataset)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> Could not recover DCP dataset."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return 0.;
        }


        if(dcpDataset->position == nullptr)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return 0.;
        }

        // Frees the GIL, from now on can not use the interpreter
        Py_BEGIN_ALLOW_THREADS

        auto metadata = analysisDataSeries.metadata;

        if(metadata["INFLUENCE_TYPE"] != "REGION")
        {

          auto buffer = dcpDataset->position->buffer(atof(metadata["RADIUS"].c_str()), 16, te::gm::CapButtType);

          int srid  = dcpDataset->position->getSRID();
          if(srid == 0)
          {
            auto format = datasetMO->format;
            if(format.find("srid") != format.end())
            {
              srid = std::stoi(format["srid"]);
            }
          }

          auto polygon = dynamic_cast<te::gm::MultiPolygon*>(geom.get());

          if(polygon != nullptr)
          {
            auto centroid = polygon->getCentroid();
            if(centroid->getSRID() == 0)
              centroid->setSRID(srid);
            else if(centroid->getSRID() != srid && srid != 0)
              centroid->transform(srid);

            std::vector<double> values;
            if((metadata["INFLUENCE_TYPE"] == "RADIUS_CENTER" && centroid->within(buffer)) || (metadata["INFLUENCE_TYPE"] == "RADIUS_TOUCHES" && polygon->touches(buffer)))
            {
              count = contextDataset->series.syncDataSet->size();
              for(unsigned int i = 0; i < count; ++i)
              {
                if(!contextDataset->series.syncDataSet->isNull(i, attribute))
                {
                  try
                  {
                    double value = contextDataset->series.syncDataSet->getDouble(i, attribute);
                    values.push_back(value);
                    sum += value;
                    if(value > max)
                      max = value;
                    if(value < min)
                      min = value;
                  }
                  catch(...)
                  {
                    // In case the DCP doesn't have the specified column
                    continue;
                  }
                }
              }

              mean = sum / count;
              std::sort (values.begin(), values.end());
              double half = values.size() / 2;
              if(values.size() % 2 == 0)
              {
                median = values[(int)half] + values[(int)half - 1] / 2;
              }

            }
          }
          else
          {
            // TODO: Monitored object is not a multi polygon.
            assert(false);
          }
        }

        // All operations are done, acquires the GIL and set the return value
        Py_END_ALLOW_THREADS
      }

      break;
    }
  }


  if(found)
  {
    switch (statisticOperation)
    {
      case SUM:
        return sum;
      case MEAN:
        return mean;
      case MIN:
        return min;
      case MAX:
        return max;
      case STANDARD_DEVIATION:
        return standardDeviation;
      case MEDIAN:
        return median;
      case COUNT:
        return count;
    }
  }

  return 0.;

}


double terrama2::services::analysis::core::dcpHistorySum(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  return dcpHistoryOperator(SUM, dataSeriesName, attribute, dcpId, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMean(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  return dcpHistoryOperator(MEAN, dataSeriesName, attribute, dcpId, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMin(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  return dcpHistoryOperator(MIN, dataSeriesName, attribute, dcpId, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMax(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  return dcpHistoryOperator(MAX, dataSeriesName, attribute, dcpId, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMedian(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  return dcpHistoryOperator(MEDIAN, dataSeriesName, attribute, dcpId, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, const std::string& dateFilter)
{
  return dcpHistoryOperator(STANDARD_DEVIATION, dataSeriesName, attribute, dcpId, dateFilter);
}
