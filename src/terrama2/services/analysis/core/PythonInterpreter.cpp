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
#include "BufferMemory.hpp"
#include "Analysis.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../core/Shared.hpp"

#include <QObject>


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

  std::string dateFilterStr(dateFilter);
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

      Context::getInstance().addDataset(analysisId, analysisDataSeries.dataSeries, dateFilterStr, true);

      auto datasets = analysisDataSeries.dataSeries->datasetList;

      for(auto dataset : datasets)
      {

        contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilterStr);
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


PyObject* terrama2::services::analysis::core::sumHistoryPCD(PyObject* self, PyObject* args)
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
  int pcdId;
  const char* dateFilter;

  if (!PyArg_ParseTuple(args, "ssis", &dataSeriesName, &attribute, &pcdId, &dateFilter))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read function parameters.");
    return NULL;
  }

  std::string dateFilterStr(dateFilter);
  double sum = 0;
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

      Context::getInstance().addDCP(analysisId, analysisDataSeries.dataSeries, dateFilterStr);

      for(auto dataset : analysisDataSeries.dataSeries->datasetList)
      {
        contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilterStr);

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
                    sum += value;
                  }
                  catch(...)
                  {
                    // In case the DCP doesn't the specified column
                    continue;
                  }
                }
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
    return Py_BuildValue("d", sum);

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

static PyMethodDef module_methods[] = {
  { "countPoints", terrama2::services::analysis::core::countPoints, METH_VARARGS, "Count points operator"},
  { "sumHistoryPCD", terrama2::services::analysis::core::sumHistoryPCD, METH_VARARGS, "Sum history PCD"},
  { "result", terrama2::services::analysis::core::result, METH_VARARGS, "Set the result value"},
  { NULL, NULL, 0, NULL }
};

static PyObject * terrama2Error;
void terrama2::services::analysis::core::initInterpreter()
{
  Py_Initialize();

  PyEval_InitThreads();

  PyObject *m = Py_InitModule("terrama2", module_methods);
  if (m == NULL)
    return;

  terrama2Error = PyErr_NewException("terrama2.error", NULL, NULL);
  Py_INCREF(terrama2Error);
  PyModule_AddObject(m, "error", terrama2Error);

  // release our hold on the global interpreter
  PyEval_ReleaseLock();
}


void terrama2::services::analysis::core::runMonitoredObjAnalysis(PyThreadState* state, uint64_t analysisId, std::vector<uint64_t> indexes)
{


  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  for(uint64_t index : indexes)
  {
    // grab the global interpreter lock
    PyEval_AcquireLock();
    // swap in my thread state
    PyThreadState_Swap(state);


    PyThreadState_Clear(state);

    PyObject *indexValue = PyInt_FromLong(index);
    PyObject *analysisValue = PyInt_FromLong(analysisId);

    PyObject* poDict = PyDict_New();
    PyDict_SetItemString(poDict, "index", indexValue);
    PyDict_SetItemString(poDict, "analysis", analysisValue);
    state->dict = poDict;

    PyRun_SimpleString("from terrama2 import *");
    PyRun_SimpleString(analysis.script.c_str());


    // release our hold on the global interpreter
    PyEval_ReleaseLock();
  }


}


void terrama2::services::analysis::core::finalizeInterpreter()
{
  // shut down the interpreter
  PyEval_AcquireLock();
  Py_Finalize();
}
