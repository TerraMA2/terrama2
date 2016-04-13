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

using namespace boost::python;

int terrama2::services::analysis::core::occurrenceCount(std::string dataSeriesName, double radius, int bufferType, std::string dateFilter, std::string restriction)
{
  object module(handle<>(borrowed(PyImport_AddModule("__main__"))));
  object dictionary = module.attr("__dict__");

  object analysisObj = dictionary["analysis"];
  int analysisId = extract<int>(analysisObj);

  object indexObj = dictionary["index"];
  int index = extract<int>(indexObj);

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


  auto geom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
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

        if(contextDataset->series.syncDataSet->size() > 0)
        {
          auto sampleGeom = syncDs->getGeometry(0, contextDataset->geometryPos);
          geom->transform(sampleGeom->getSRID());

          contextDataset->rtree.search(*geom->getMBR(), indexes);

          std::vector<std::shared_ptr<te::gm::Geometry> > geometries;

          for(uint64_t i : indexes)
          {
            auto occurenceGeom = syncDs->getGeometry(i, contextDataset->geometryPos);
            if(occurenceGeom->intersects(geom.get()))
            {
              geometries.push_back(occurenceGeom);
            }
          }

          if(!geometries.empty())
          {
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
          }
        }

        // All operations are done, acquires the GIL and set the return value
        Py_END_ALLOW_THREADS
      }
    }
  }

  return count;
}

void terrama2::services::analysis::core::addValue(std::string attribute, double value)
{
  object module(handle<>(borrowed(PyImport_AddModule("__main__"))));
  object dictionary = module.attr("__dict__");

  object analysisObj = dictionary["analysis"];
  int analysisId = extract<int>(analysisObj);

  object indexObj = dictionary["index"];
  int index = extract<int>(indexObj);
  std::cerr << index << ": " << value << std::endl;

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
        std::string geomId = moDsContext->series.syncDataSet->getString(index, moDsContext->identifier);
        Context::getInstance().setAnalysisResult(analysisId, geomId, value);
      }
    }
  }
}


int terrama2::services::analysis::core::dcpCount(std::string dataSeriesName, double radius, Buffer bufferType)
{
  return dcpOperator(COUNT, dataSeriesName, radius, bufferType);
}

int terrama2::services::analysis::core::dcpMin(std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MIN, dataSeriesName, radius, bufferType, ids);
}

int terrama2::services::analysis::core::dcpMax(std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MAX, dataSeriesName, radius, bufferType, ids);
}

int terrama2::services::analysis::core::dcpMean(std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MEAN, dataSeriesName, radius, bufferType, ids);
}

int terrama2::services::analysis::core::dcpMedian(std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(MEDIAN, dataSeriesName, radius, bufferType, ids);
}

int terrama2::services::analysis::core::dcpSum(std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(SUM, dataSeriesName, radius, bufferType, ids);
}

int terrama2::services::analysis::core::dcpStandardDeviation(std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return dcpOperator(STANDARD_DEVIATION, dataSeriesName, radius, bufferType, ids);
}

int terrama2::services::analysis::core::dcpOperator(StatisticOperation statisticOperation, std::string dataSeriesName, double radius, Buffer bufferType, boost::python::list ids)
{
  return 0;
}

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
  def("dcp::min", terrama2::services::analysis::core::dcpMin);
  def("dcp::max", terrama2::services::analysis::core::dcpMax);
  def("dcp::mean", terrama2::services::analysis::core::dcpMean);
  def("dcp::median", terrama2::services::analysis::core::dcpMedian);
  def("dcp::sum", terrama2::services::analysis::core::dcpSum);
  def("dcp::standardDeviation", terrama2::services::analysis::core::dcpStandardDeviation);
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


static PyObject * terrama2Error;
void terrama2::services::analysis::core::initInterpreter()
{
  Py_Initialize();

  PyEval_InitThreads();

  PyImport_AppendInittab((char*)"terrama2", INIT_MODULE);

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


    PyThreadState_Clear(state);

    object main_module = import("__main__");
    dict main_namespace = extract<dict>(main_module.attr("__dict__"));
    object mymodule = import("terrama2");
    main_namespace["analysis"] = analysis.id;
    main_namespace["index"] = index;

    std::string initFile = terrama2::core::FindInTerraMA2Path("share/terrama2/python/init.py");
    if(initFile.empty())
    {
      QString errMsg = QObject::tr("Could not read python init file.");
      TERRAMA2_LOG_WARNING() << errMsg;
      throw PythonInterpreterException() << ErrorDescription(errMsg);
    }

    QFile file(initFile.c_str());

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
      QString errMsg = QObject::tr("Could not read python init file: %1").arg(initFile.c_str());
      TERRAMA2_LOG_ERROR() << errMsg.toStdString();
      throw PythonInterpreterException() << ErrorDescription(errMsg);
    }

    QTextStream in(&file);

    exec(in.readAll().toStdString().c_str(), main_namespace, main_namespace);
    exec(analysis.script.c_str(), main_namespace, main_namespace);

    // release our hold on the global interpreter
    PyEval_ReleaseLock();
  }
}


void terrama2::services::analysis::core::runScriptDCPAnalysis(PyThreadState* state, uint64_t analysisId)
{
  /*
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

  PyRun_SimpleString("import history");
  PyRun_SimpleString("from import history");
  PyRun_SimpleString(analysis.script.c_str());


  // release our hold on the global interpreter
  PyEval_ReleaseLock();
  */
}


/*
#include "Context.hpp"
#include "BufferMemory.hpp"
#include "Analysis.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../core/Shared.hpp"

#include <QObject>
#include <QFile>
#include <QString>
#include <QTextStream>


// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>


PyObject* terrama2::services::analysis::core::countPoints(PyObject* self, PyObject* args)
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

  // Reads parameters
  const char * dataSeriesName;
  double radius = 0.;
  const char * dateFilter;
  const char * restriction;

  if (!PyArg_ParseTuple(args, "sdss", &dataSeriesName, &radius, &dateFilter, &restriction))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read function parameters.");
    return NULL;
  }

  std::string dateFilter(dateFilter);
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
        return NULL;
      }

      if(!Context::getInstance().exists(analysis.id, datasetMO->id))
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NULL;
      }

      moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

      if(!moDsContext)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NULL;
      }
    }

  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
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

        if(contextDataset->series.syncDataSet->size() > 0)
        {
          auto sampleGeom = syncDs->getGeometry(0, contextDataset->geometryPos);
          geom->transform(sampleGeom->getSRID());

          contextDataset->rtree.search(*geom->getMBR(), indexes);

          std::vector<std::shared_ptr<te::gm::Geometry> > geometries;

          for(uint64_t i : indexes)
          {
            auto occurenceGeom = syncDs->getGeometry(i, contextDataset->geometryPos);
            if(occurenceGeom->intersects(geom.get()))
            {
              geometries.push_back(occurenceGeom);
            }
          }

          if(!geometries.empty())
          {
            std::shared_ptr<te::gm::Envelope> box(syncDs->dataset()->getExtent(contextDataset->geometryPos));
            auto bufferDs = createBuffer(geometries, box, radius);

            count = bufferDs->size();
          }
        }

        // All operations are done, acquires the GIL and set the return value
        Py_END_ALLOW_THREADS
      }
    }
  }

  if(found)
    return Py_BuildValue("i", count);

  return NULL;
}


PyObject* terrama2::services::analysis::core::operatorHistoryDCP(PyObject* args, StatisticOperation statisticOperation)
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

  // Reads parameters
  const char* dataSeriesName;
  const char* attribute;
  int DCPId;
  const char* dateFilter;

  if (!PyArg_ParseTuple(args, "ssis", &dataSeriesName, &attribute, &DCPId, &dateFilter))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read function parameters.");
    return NULL;
  }

  std::string dateFilter(dateFilter);
  double sum = 0;
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double median = 0;
  double mean = 0;
  double standardDeviation = 0;
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
        return NULL;
      }

      moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

      if(!moDsContext)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NULL;
      }
    }
  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }


  std::shared_ptr<ContextDataset> contextDataset;

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;

      if(analysisDataSeries.dataSeries->semantics.macroType != terrama2::core::DataSeriesSemantics::DCP)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NULL;
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
          return NULL;
        }


        if(dcpDataset->position == nullptr)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NULL;
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
              uint64_t size = contextDataset->series.syncDataSet->size();
              for(unsigned int i = 0; i < size; ++i)
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

              mean = sum / size;
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
        return Py_BuildValue("d", sum);
      case MEAN:
        return Py_BuildValue("d", mean);
      case MIN:
        return Py_BuildValue("d", min);
      case MAX:
        return Py_BuildValue("d", max);
      case STANDARD_DEVIATION:
        return Py_BuildValue("d", standardDeviation);
      case MEDIAN:
        return Py_BuildValue("d", median);
    }
  }

  return Py_BuildValue("d", 0.);

}


PyObject* terrama2::services::analysis::core::sumHistoryDCP(PyObject* self, PyObject* args)
{
  return operatorHistoryDCP(args, SUM);
}

PyObject* terrama2::services::analysis::core::meanHistoryDCP(PyObject* self, PyObject* args)
{
  return operatorHistoryDCP(args, MEAN);
}

PyObject* terrama2::services::analysis::core::minHistoryDCP(PyObject* self, PyObject* args)
{
  return operatorHistoryDCP(args, MIN);
}

PyObject* terrama2::services::analysis::core::maxHistoryDCP(PyObject* self, PyObject* args)
{
  return operatorHistoryDCP(args, MAX);
}

PyObject* terrama2::services::analysis::core::medianHistoryDCP(PyObject* self, PyObject* args)
{
  return operatorHistoryDCP(args, MEDIAN);
}

PyObject* terrama2::services::analysis::core::standardDeviationHistoryDCP(PyObject* self, PyObject* args)
{
  return operatorHistoryDCP(args, STANDARD_DEVIATION);
}

PyObject* terrama2::services::analysis::core::sumDCP(PyObject* self, PyObject* args)
{
  return operatorDCP(args, SUM);
}

PyObject* terrama2::services::analysis::core::meanDCP(PyObject* self, PyObject* args)
{
  return operatorDCP(args, MEAN);
}

PyObject* terrama2::services::analysis::core::minDCP(PyObject* self, PyObject* args)
{
  return operatorDCP(args, MIN);
}

PyObject* terrama2::services::analysis::core::maxDCP(PyObject* self, PyObject* args)
{
  return operatorDCP(args, MAX);
}

PyObject* terrama2::services::analysis::core::standardDeviationDCP(PyObject* self, PyObject* args)
{
  return operatorDCP(args, STANDARD_DEVIATION);
}

PyObject* terrama2::services::analysis::core::medianDCP(PyObject* self, PyObject* args)
{
  return operatorDCP(args, MEDIAN);
}

PyObject* terrama2::services::analysis::core::operatorDCP(PyObject* args, StatisticOperation statisticOperation)
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

  // Reads parameters
  const char* dataSeriesName;
  const char* attribute;
  double bufferDistance = 0;
  int bufferType = 0;
  std::vector<std::string> ids;

  if (!PyArg_ParseTuple(args, "ssis|[items]", &dataSeriesName, &attribute, bufferDistance, &bufferType, &ids))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read function parameters.");
    return NULL;
  }

  double sum = 0;
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double median = 0;
  double mean = 0;
  double standardDeviation = 0;
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
        return NULL;
      }

      moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

      if(!moDsContext)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NULL;
      }
    }
  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }


  std::shared_ptr<ContextDataset> contextDataset;

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;

      if(analysisDataSeries.dataSeries->semantics.macroType != terrama2::core::DataSeriesSemantics::DCP)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NULL;
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
          return NULL;
        }


        if(dcpDataset->position == nullptr)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NULL;
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
              uint64_t size = contextDataset->series.syncDataSet->size();
              for(unsigned int i = 0; i < size; ++i)
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

              mean = sum / size;
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
        return Py_BuildValue("d", sum);
      case MEAN:
        return Py_BuildValue("d", mean);
      case MIN:
        return Py_BuildValue("d", min);
      case MAX:
        return Py_BuildValue("d", max);
      case STANDARD_DEVIATION:
        return Py_BuildValue("d", standardDeviation);
      case MEDIAN:
        return Py_BuildValue("d", median);
    }
  }

  return Py_BuildValue("d", 0.);

}

PyObject* terrama2::services::analysis::core::result(PyObject* self, PyObject* args)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;


  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* indexPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyLong_AsLong(indexPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);

  // Reads parameters
  double result = 0;

  if (!PyArg_ParseTuple(args, "d", &result))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read the parameters from function result.");
    return NULL;
  }

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
          return NULL;
        }

        moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

        if(moDsContext->identifier.empty())
          assert(false);
        std::string geomId = moDsContext->series.syncDataSet->getString(index, moDsContext->identifier);
        Context::getInstance().setAnalysisResult(analysisId, geomId, result);
      }
    }
  }

  return new PyObject();
}

static PyMethodDef history_methods[] = {
  { "sum", terrama2::services::analysis::core::sumHistoryDCP, METH_VARARGS, "Sum history DCP"},
  { "mean", terrama2::services::analysis::core::meanHistoryDCP, METH_VARARGS, "Mean history DCP"},
  { "min", terrama2::services::analysis::core::minHistoryDCP, METH_VARARGS, "Min history DCP"},
  { "max", terrama2::services::analysis::core::maxHistoryDCP, METH_VARARGS, "Max history DCP"},
  { "standard_deviation", terrama2::services::analysis::core::standardDeviationHistoryDCP, METH_VARARGS, "Standard deviation history DCP"},
  { "mean", terrama2::services::analysis::core::meanHistoryDCP, METH_VARARGS, "Median history DCP"},
  { NULL, NULL, 0, NULL }
};

static PyMethodDef dcp_methods[] = {
  { "sum", terrama2::services::analysis::core::sumDCP, METH_VARARGS, "Sum DCP"},
  { "mean", terrama2::services::analysis::core::meanDCP, METH_VARARGS, "Mean DCP"},
  { "min", terrama2::services::analysis::core::minDCP, METH_VARARGS, "Min DCP"},
  { "max", terrama2::services::analysis::core::maxDCP, METH_VARARGS, "Max DCP"},
  { "standard_deviation", terrama2::services::analysis::core::standardDeviationDCP, METH_VARARGS, "Standard deviation DCP"},
  { "median", terrama2::services::analysis::core::meanDCP, METH_VARARGS, "Median DCP"},
  { NULL, NULL, 0, NULL }
};

static PyMethodDef occurrence_methods[] = {
  { "count", terrama2::services::analysis::core::countPoints, METH_VARARGS, "Count points operator"},
  { NULL, NULL, 0, NULL }
};


static PyMethodDef terrama2_methods[] = {
  { "add_value", terrama2::services::analysis::core::result, METH_VARARGS, "Set the result value"},
  { NULL, NULL, 0, NULL }
};

static PyObject * terrama2Error;
void terrama2::services::analysis::core::initInterpreter()
{
  Py_Initialize();

  PyEval_InitThreads();

  PyObject *m = Py_InitModule("history", history_methods);
  if (m == NULL)
  {
    return;
  }

  m = Py_InitModule("dcp", dcp_methods);
  if (m == NULL)
  {
    return;
  }

  m = Py_InitModule("occurrence", occurrence_methods);
  if (m == NULL)
  {
    return;
  }

  m = Py_InitModule("terrama2", terrama2_methods);
  if (m == NULL)
  {
    return;
  }

  std::string initFile = terrama2::core::FindInTerraMA2Path("share/terrama2/python/init.py");
  QFile file(initFile.c_str());

  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QString errMsg = QObject::tr("Could not read python init file: %1").arg(initFile.c_str());
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    throw PythonInterpreterException() << ErrorDescription(errMsg);
  }

  QTextStream in(&file);

  PyRun_SimpleString(in.readAll().toStdString().c_str());

  terrama2Error = PyErr_NewException("terrama2.error", NULL, NULL);
  Py_INCREF(terrama2Error);
  PyModule_AddObject(m, "error", terrama2Error);




  // release our hold on the global interpreter
  PyEval_ReleaseLock();
}

*/
