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
#include "../../../core/shared.hpp"
#include "../../../impl/DataAccessorOccurrenceMvf.hpp"
#include "../../../impl/DataAccessorDcpInpe.hpp"


#include <ctime>
#include <iomanip>

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


  auto geom = moDsContext->dataset->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }

  time_t ts = 0;
  struct tm t;
  char buf[16];
  ::localtime_r(&ts, &t);
  ::strftime(buf, sizeof(buf), "%Z", &t);


  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(buf));
  boost::local_time::local_date_time ldt = boost::local_time::local_microsec_clock::local_time(zone);


  char format = dateFilterStr.at(dateFilterStr.size() - 1);
  if(format == 'h')
  {
    std::string hoursStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int hours = atoi(hoursStr.c_str());
      ldt -= boost::posix_time::hours(hours);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }
  else if(format == 'm')
  {
    std::string minutesStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int minutes = atoi(minutesStr.c_str());
      ldt -= boost::posix_time::minutes(minutes);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }
  else if(format == 's')
  {
    std::string secondsStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int seconds = atoi(secondsStr.c_str());
      ldt -= boost::posix_time::seconds(seconds);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }
  else if(format == 'd')
  {
    std::string daysStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int days = atoi(daysStr.c_str());
      //FIXME: subtrair dias
      ldt -= boost::posix_time::hours(days);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }


  std::shared_ptr<ContextDataset> contextDataset;

  for(auto& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;
      auto datasets = analysisDataSeries.dataSeries->datasetList;

      for(auto dataset : datasets)
      {
        if(Context::getInstance().exists(analysisId, dataset->id, dateFilterStr))
        {
          contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilterStr);
        }
        else
        {

          auto dataProvider = terrama2::core::DataManager::getInstance().findDataProvider(analysisDataSeries.dataSeries->dataProviderId);
          terrama2::core::Filter filter;

          std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
          filter.discardBefore_ = std::move(titz);

          //accessing data
          terrama2::core::DataAccessorOccurrenceMvf accessor(dataProvider, analysisDataSeries.dataSeries);


          auto teDataset = accessor.getDataSet(dataProvider->uri, filter, dataset);

          teDataset->moveFirst();
          auto format = dataset->format;
          std::string identifier = format["identifier"];

          contextDataset = Context::getInstance().addDataset(analysisId, dataset->id, dateFilterStr, teDataset, identifier, true);

          if(!contextDataset)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> Could not recover dataset."));
            errMsg = errMsg.arg(analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NULL;
          }

        }


        // Save thread state before entering multi-thread zone

        Py_BEGIN_ALLOW_THREADS

        std::vector<uint64_t> indexes;
        std::shared_ptr<SyncronizedDataSet> syncDs = contextDataset->dataset;

        if(contextDataset->dataset->size() > 0)
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


  auto geom = moDsContext->dataset->getGeometry(index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }

  time_t ts = 0;
  struct tm t;
  char buf[16];
  ::localtime_r(&ts, &t);
  ::strftime(buf, sizeof(buf), "%Z", &t);

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(buf));
  boost::local_time::local_date_time ldt = boost::local_time::local_microsec_clock::local_time(zone);

  char format = dateFilterStr.at(dateFilterStr.size() - 1);
  if(format == 'h')
  {
    std::string hoursStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int hours = atoi(hoursStr.c_str());
      ldt -= boost::posix_time::hours(hours);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }
  else if(format == 'm')
  {
    std::string minutesStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int minutes = atoi(minutesStr.c_str());
      ldt -= boost::posix_time::minutes(minutes);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }
  else if(format == 's')
  {
    std::string secondsStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int seconds = atoi(secondsStr.c_str());
      ldt -= boost::posix_time::seconds(seconds);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }
  else if(format == 'd')
  {
    std::string daysStr = dateFilterStr.substr(0, dateFilterStr.size() - 1);
    try
    {
      int days = atoi(daysStr.c_str());
      //FIXME: subtrair dias
      assert(false);
    }
    catch(...)
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid date filter."));
      errMsg = errMsg.arg(analysisId);
      TERRAMA2_LOG_ERROR() << errMsg;
      return NULL;
    }
  }


  std::shared_ptr<ContextDataset> contextDataset;

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.dataSeries->name == dataSeriesName)
    {
      found = true;

      auto dataProvider = terrama2::core::DataManager::getInstance().findDataProvider(analysisDataSeries.dataSeries->dataProviderId);

      terrama2::core::Filter filter;
      std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
      filter.discardBefore_ = std::move(titz);

      terrama2::core::DataAccessorDcpInpe accessor(dataProvider, analysisDataSeries.dataSeries);
      terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

      for(auto pairDatasetDCP : dcpSeries->dcpList())
      {
        terrama2::core::DataSetDcpPtr datasetDCP = pairDatasetDCP.first;
        std::shared_ptr<te::mem::DataSet> teDataset = pairDatasetDCP.second;
        contextDataset = Context::getInstance().addDCP(analysisId, datasetDCP, dateFilterStr, teDataset);


        // Frees the GIL, from now on can not use the interpreter
        Py_BEGIN_ALLOW_THREADS

        auto positionDCP = datasetDCP->position;

        for(auto analysisDataSeries : analysis.analysisDataSeriesList)
        {
          for(auto dataset : analysisDataSeries.dataSeries->datasetList)
          {
            if(dataset->id == datasetDCP->id)
            {
              auto metadata = analysisDataSeries.metadata;

              if(metadata["INFLUENCE_TYPE"] != "REGION")
              {

                auto buffer = positionDCP->buffer(atof(metadata["RADIUS"].c_str()), 16, te::gm::CapButtType);

                int srid  = positionDCP->getSRID();
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
                    uint64_t size = contextDataset->dataset->size();
                    for(unsigned int i = 0; i < size; ++i)
                    {
                      if(!contextDataset->dataset->isNull(i, attribute))
                      {
                        try
                        {
                          double value = contextDataset->dataset->getDouble(i, attribute);
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
            }
          }

        }

        // All operations are done, acquires the GIL and set the return value
        Py_END_ALLOW_THREADS

        break;
      }
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
        std::string geomId = moDsContext->dataset->getString(index, moDsContext->identifier);
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
void terrama2::services::analysis::core::init()
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


void terrama2::services::analysis::core::finalize()
{
  // shut down the interpreter
  PyEval_AcquireLock();
  Py_Finalize();
}
