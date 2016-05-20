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
#include "../../../core/data-model/DataSetDcp.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../core/Shared.hpp"


// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/common/UnitOfMeasure.h>
#include <terralib/common/UnitsOfMeasureManager.h>

#include <math.h>

using namespace boost::python;




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

double terrama2::services::analysis::core::dcpHistoryOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
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
  bool hasData = false;

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);


  std::shared_ptr<ContextDataSeries> moDsContext;
  terrama2::core::DataSetPtr datasetMO;

  // Reads the object monitored
  for(AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(dataSeries->datasetList.size() == 1);
      datasetMO = dataSeries->datasetList[0];

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


  bool dataSeriesFound = false;

  // Frees the GIL, from now on can not use the interpreter
  Py_BEGIN_ALLOW_THREADS

  std::shared_ptr<ContextDataSeries> contextDataset;

  try
  {
    for(auto analysisDataSeries : analysis.analysisDataSeriesList)
    {
      terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
      if(dataSeries->name == dataSeriesName)
      {
        dataSeriesFound = true;

        if(dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesSemantics::DCP)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
          errMsg = errMsg.arg(analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }

        Context::getInstance().addDCP(analysisId, dataSeries, dateFilter, false);

        for(auto dataset : dataSeries->datasetList)
        {
          if(dataset->id != dcpId)
            continue;
          contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilter);

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


          try
          {

            // Reads influence type
            std::string typeStr = analysis.metadata["INFLUENCE_TYPE"];
            int type = atoi(typeStr.c_str());
            if(type == 0 || type > 3)
            {
              QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence type for DCP analysis."));
              errMsg = errMsg.arg(analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }
            InfluenceType influenceType = (InfluenceType)type;


            // For influence based on radius, creates a buffer for the DCP location
            if(influenceType == RADIUS_CENTER || influenceType == RADIUS_TOUCHES)
            {

              if(analysis.metadata["INFLUENCE_RADIUS"].empty())
              {
                QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence radius."));
                errMsg = errMsg.arg(analysisId);
                TERRAMA2_LOG_ERROR() << errMsg;
                return NAN;
              }

              double radius = 0.;
              try
              {
                std::string radiusStr = analysis.metadata["INFLUENCE_RADIUS"];
                std::string radiusUnit = analysis.metadata["INFLUENCE_RADIUS_UNIT"];

                if(radiusStr.empty())
                  radiusStr = "0";
                if(radiusUnit.empty())
                  radiusUnit = "km";

                radius = atof(radiusStr.c_str());

                radius = te::common::UnitsOfMeasureManager::getInstance().getConversion(radiusUnit, "METER") * radius;
              }
              catch(...)
              {
                QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence radius."));
                errMsg = errMsg.arg(analysisId);
                TERRAMA2_LOG_ERROR() << errMsg;
                return NAN;
              }

              try
              {
                auto dcpBuffer = dcpDataset->position->buffer(radius, 16, te::gm::CapButtType);

                int srid  = dcpDataset->position->getSRID();
                dcpBuffer->setSRID(srid);



                if(geom->getSRID() == 0)
                {
                  QString errMsg(QObject::tr("Analysis: %1 -> Invalid monitored object SRID."));
                  errMsg = errMsg.arg(analysisId);
                  TERRAMA2_LOG_ERROR() << errMsg;
                  return NAN;
                }

                // Converts the buffer to monitored object SRID
                dcpBuffer->transform(geom->getSRID());

                auto resultGeom = createBuffer(buffer, geom);


                std::vector<double> values;

                bool intersects = false;
                if(influenceType == RADIUS_TOUCHES)
                {
                  intersects = resultGeom->intersects(dcpBuffer);
                }
                else if(influenceType == RADIUS_CENTER)
                {
                  //TODO: use method from terralib_mod_sa_core
                  std::string geomType = resultGeom->getGeometryType();
                  if(geomType == "MultiPolygon")
                  {
                    auto polygon = dynamic_cast<te::gm::MultiPolygon*>(resultGeom.get());
                    if(polygon)
                    {
                      auto centroid = polygon->getCentroid();
                      intersects = centroid->within(dcpBuffer);
                    }
                  }
                  else
                  {
                    intersects = resultGeom->intersects(dcpBuffer);
                  }
                }

                if(intersects)
                {

                  uint64_t countValues = 0;
                  if(contextDataset->series.syncDataSet->size() == 0)
                    continue;
                  for(unsigned int i = 0; i < contextDataset->series.syncDataSet->size(); ++i)
                  {
                    try
                    {
                      if(!attribute.empty() && !contextDataset->series.syncDataSet->isNull(i, attribute))
                      {
                        hasData = true;
                        countValues++;
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

                  mean = sum / countValues;
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

                  double sumVariance = 0.;
                  for(unsigned int i = 0; i < contextDataset->series.syncDataSet->size(); ++i)
                  {
                    if(!contextDataset->series.syncDataSet->isNull(i, attribute))
                    {
                      try
                      {
                        double value = contextDataset->series.syncDataSet->getDouble(i, attribute);
                        sumVariance += (value - mean) * (value - mean);
                      }
                      catch(...)
                      {
                        // In case the DCP doesn't have the specified column
                        continue;
                      }
                    }
                  }

                  standardDeviation = sumVariance / countValues;

                }

              }
              catch(std::exception& e)
              {
                QString errMsg(QObject::tr("Analysis: %1 -> %2").arg(e.what()));
                errMsg = errMsg.arg(analysisId);
                TERRAMA2_LOG_ERROR() << errMsg;
                return NAN;
              }
            }
          }
          catch(terrama2::Exception /*e*/)
          {
            return NAN;
          }

        }

        break;
      }
    }
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    return NAN;
  }


  // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

  if(!dataSeriesFound)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not find a data series with the name: %2.").arg(analysisId).arg(dataSeriesName.c_str()));
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }

  if(!hasData && statisticOperation != COUNT)
  {
    return NAN;
  }

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

  return NAN;

}


double terrama2::services::analysis::core::dcpHistorySum(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(SUM, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMean(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MEAN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMin(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MIN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMax(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MAX, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMedian(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MEDIAN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, uint64_t dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(STANDARD_DEVIATION, dataSeriesName, attribute, dcpId,  buffer, dateFilter);
}

int terrama2::services::analysis::core::occurrenceCount(const std::string& dataSeriesName, Buffer buffer, std::string dateFilter, std::string restriction)
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

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  std::shared_ptr<ContextDataSeries> moDsContext;

  // Reads the object monitored
  for(AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
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


  // Save thread state before entering multi-thread zone

  Py_BEGIN_ALLOW_THREADS

  std::shared_ptr<ContextDataSeries> contextDataset;

  try
  {
    for(auto& analysisDataSeries : analysis.analysisDataSeriesList)
    {
      auto dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
      if(dataSeries->name == dataSeriesName)
      {
        found = true;

        Context::getInstance().addDataset(analysisId, dataSeries, dateFilter, true);

        auto datasets = dataSeries->datasetList;

        for(auto dataset : datasets)
        {

          contextDataset = Context::getInstance().getContextDataset(analysisId, dataset->id, dateFilter);
          if(!contextDataset)
          {
            continue;
          }


          std::vector<uint64_t> indexes;
          terrama2::core::SyncronizedDataSetPtr syncDs = contextDataset->series.syncDataSet;

          if(syncDs->size() == 0)
          {
            continue;
          }


          int size = contextDataset->series.syncDataSet->size();
          if(size > 0)
          {
            auto geomResult = createBuffer(buffer, moGeom);

            // Converts the monitored object to the same srid of the occurrences
            auto firstOccurrence = contextDataset->series.syncDataSet->getGeometry(0, contextDataset->geometryPos);
            geomResult->transform(firstOccurrence->getSRID());

            // Searchs in the spatial index for the occurrences that intersects the monitored object box
            contextDataset->rtree.search(*geomResult->getMBR(), indexes);



            std::vector<std::shared_ptr<te::gm::Geometry> > geometries;
            for(uint64_t i : indexes)
            {
              // Verifies if the occurrence intersects the monitored object
              auto occurrenceGeom = syncDs->getGeometry(i, contextDataset->geometryPos);

              if(occurrenceGeom->intersects(geomResult.get()))
              {
                geometries.push_back(occurrenceGeom);
              }
            }

            count = geometries.size();

            /*if(!geometries.empty())
            {
              // Creates aggregation buffer
              std::shared_ptr<te::gm::Envelope> box(syncDs->dataset()->getExtent(contextDataset->geometryPos));
              if(distance != 0.)
              {
                auto bufferDs = createAggregationBuffer(geometries, box, distance, bufferType);
                count = bufferDs->size();
              }
              else
              {
                count = geometries.size();
              }
            }*/
          }

        }
      }
    }
  }
  catch(terrama2::Exception e)
  {
    std::cout << e.what() << std::endl;
  }
  catch(std::exception e)
  {
    std::cout << e.what() << std::endl;
  }


  // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

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

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);
  if(analysis.type == MONITORED_OBJECT_TYPE)
  {
    std::shared_ptr<ContextDataSeries> moDsContext;
    terrama2::core::DataSetPtr datasetMO;

    // Reads the object monitored
    auto analysisDataSeriesList = analysis.analysisDataSeriesList;
    for(auto analysisDataSeries : analysisDataSeriesList)
    {
      if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
      {
        auto dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
        assert(dataSeries->datasetList.size() == 1);
        datasetMO = dataSeries->datasetList[0];

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

        Context::getInstance().addAttribute(analysisId, attribute);
        Context::getInstance().setAnalysisResult(analysisId, geomId, attribute, value);
      }
    }
  }
}


int terrama2::services::analysis::core::dcpCount(const std::string& dataSeriesName, Buffer buffer)
{
  return dcpOperator(COUNT, dataSeriesName, "", buffer);
}

double terrama2::services::analysis::core::dcpMin(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids)
{
  return dcpOperator(MIN, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcpMax(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids)
{
  return dcpOperator(MAX, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcpMean(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids)
{
  return dcpOperator(MEAN, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcpMedian(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids)
{
  return dcpOperator(MEDIAN, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcpSum(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids)
{
  return dcpOperator(SUM, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcpStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, Buffer buffer, boost::python::list ids)
{
  return dcpOperator(STANDARD_DEVIATION, dataSeriesName, attribute, buffer, ids);
}

double terrama2::services::analysis::core::dcpOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName,  const std::string& attribute, Buffer buffer, boost::python::list ids)
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

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(msg);
  }

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);


  std::shared_ptr<ContextDataSeries> moDsContext;
  terrama2::core::DataSetPtr datasetMO;

  // Reads the object monitored
  for(AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
      assert(dataSeries->datasetList.size() == 1);
      datasetMO = dataSeries->datasetList[0];

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


  std::shared_ptr<ContextDataSeries> contextDataset;



  // Frees the GIL, from now on can not use the interpreter
  Py_BEGIN_ALLOW_THREADS

  for(auto analysisDataSeries : analysis.analysisDataSeriesList)
  {
    terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

    if(dataSeries->name == dataSeriesName)
    {
      found = true;

      if(dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesSemantics::DCP)
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
        errMsg = errMsg.arg(analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NAN;
      }


      Context::getInstance().addDCP(analysisId, dataSeries, "", true);

      // For DCP count returns the number of datasets
      count = dataSeries->datasetList.size();

      for(auto dataset : dataSeries->datasetList)
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


        try
        {

          // Reads influence type
          std::string typeStr = analysis.metadata["INFLUENCE_TYPE"];
          int type = atoi(typeStr.c_str());
          if(type == 0 || type > 3)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence type for DCP analysis."));
            errMsg = errMsg.arg(analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }
          InfluenceType influenceType = (InfluenceType)type;


          // For influence based on radius, creates a buffer for the DCP location
          if(influenceType == RADIUS_CENTER || influenceType == RADIUS_TOUCHES)
          {

            if(analysis.metadata["INFLUENCE_RADIUS"].empty())
            {
              QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence radius."));
              errMsg = errMsg.arg(analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }

            double influenceRadius = 0.;
            try
            {
              std::string radiusStr = analysis.metadata["INFLUENCE_RADIUS"];
              std::string radiusUnit = analysis.metadata["INFLUENCE_RADIUS_UNIT"];

              if(radiusStr.empty())
                radiusStr = "0";
              if(radiusUnit.empty())
                radiusUnit = "km";

              influenceRadius = atof(radiusStr.c_str());

              influenceRadius = te::common::UnitsOfMeasureManager::getInstance().getConversion(radiusUnit, "METER") * influenceRadius;
            }
            catch(...)
            {
              QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence radius."));
              errMsg = errMsg.arg(analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }

            try
            {
              auto buffer = dcpDataset->position->buffer(influenceRadius, 16, te::gm::CapButtType);

              int srid  = dcpDataset->position->getSRID();
              buffer->setSRID(srid);


              auto polygon = dynamic_cast<te::gm::MultiPolygon*>(geom.get());

              if(polygon->getSRID() == 0)
              {
                QString errMsg(QObject::tr("Analysis: %1 -> Invalid monitored object SRID."));
                errMsg = errMsg.arg(analysisId);
                TERRAMA2_LOG_ERROR() << errMsg;
                return NAN;
              }

              // Converts the buffer to monitored object SRID
              buffer->transform(polygon->getSRID());


              if(polygon != nullptr)
              {
                std::vector<double> values;

                bool intersects = false;
                if(influenceType == RADIUS_TOUCHES)
                {
                  intersects = polygon->touches(buffer);
                }
                else if(influenceType == RADIUS_CENTER)
                {
                  auto centroid = polygon->getCentroid();
                  intersects = centroid->within(buffer);
                }

                if(intersects)
                {
                  uint64_t countValues = 0;
                  if(contextDataset->series.syncDataSet->size() == 0)
                    continue;
                  for(unsigned int i = 0; i < contextDataset->series.syncDataSet->size(); ++i)
                  {
                    try
                    {
                      if(!attribute.empty() && !contextDataset->series.syncDataSet->isNull(i, attribute))
                      {
                        hasData = true;
                        countValues++;
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

                  mean = sum / countValues;
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

                  double sumVariance = 0.;
                  for(unsigned int i = 0; i < contextDataset->series.syncDataSet->size(); ++i)
                  {
                    if(!contextDataset->series.syncDataSet->isNull(i, attribute))
                    {
                      try
                      {
                        double value = contextDataset->series.syncDataSet->getDouble(i, attribute);
                        sumVariance += (value - mean) * (value - mean);
                      }
                      catch(...)
                      {
                        // In case the DCP doesn't have the specified column
                        continue;
                      }
                    }
                  }

                  standardDeviation = sumVariance / countValues;

                }
              }
              else
              {
                // TODO: Monitored object is not a multi polygon.
                assert(false);
              }
            }
            catch(std::exception& e)
            {
              QString errMsg(QObject::tr("Analysis: %1 -> %2").arg(e.what()));
              errMsg = errMsg.arg(analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }
          }
        }
        catch(terrama2::Exception /*e*/)
        {
          return NAN;
        }


      }


      break;
    }
  }


  // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

  if(!hasData && statisticOperation != COUNT)
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
      default:
        return NAN;
    }
  }

  return NAN;
}

// Declaration needed for default parameters
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMin_overloads, terrama2::services::analysis::core::dcpMin, 3, 4);
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMax_overloads, terrama2::services::analysis::core::dcpMax, 3, 4);
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMean_overloads, terrama2::services::analysis::core::dcpMean, 3, 4);
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpMedian_overloads, terrama2::services::analysis::core::dcpMedian, 3, 4);
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpSum_overloads, terrama2::services::analysis::core::dcpSum, 3, 4);
BOOST_PYTHON_FUNCTION_OVERLOADS(dcpStandardDeviation_overloads, terrama2::services::analysis::core::dcpStandardDeviation, 3, 4);


void terrama2::services::analysis::core::registerDCPFunctions()
{
  // map the dcp namespace to a sub-module
  // make "from terrama2.dcp import <function>" work
  object dcpModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp"))));
  // make "from terrama2 import dcp" work
  scope().attr("dcp") = dcpModule;
  // set the current scope to the new sub-module
  scope dcpScope = dcpModule;
  // export functions inside dcp namespace
  def("min", terrama2::services::analysis::core::dcpMin, dcpMin_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Minimum operator for DCP"));
  def("max", terrama2::services::analysis::core::dcpMax, dcpMax_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Maximum operator for DCP"));
  def("mean", terrama2::services::analysis::core::dcpMean, dcpMean_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Mean operator for DCP"));
  def("median", terrama2::services::analysis::core::dcpMedian, dcpMedian_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Median operator for DCP"));
  def("sum", terrama2::services::analysis::core::dcpSum, dcpSum_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Sum operator for DCP"));
  def("standard_deviation", terrama2::services::analysis::core::dcpStandardDeviation, dcpStandardDeviation_overloads(args("dataSeriesName", "attribute", "buffer", "ids"), "Standard deviation operator for DCP"));
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
  def("standard_deviation", terrama2::services::analysis::core::dcpHistoryStandardDeviation);

}

void terrama2::services::analysis::core::registerOccurrenceFunctions()
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

  enum_<terrama2::services::analysis::core::BufferType>("BufferType")
    .value("none", terrama2::services::analysis::core::NONE)
    .value("only_buffer", terrama2::services::analysis::core::ONLY_BUFFER)
    .value("intern_plus_extern", terrama2::services::analysis::core::INTERN_PLUS_EXTERN)
    .value("object_plus_buffer", terrama2::services::analysis::core::OBJECT_PLUS_BUFFER)
    .value("object_minus_buffer", terrama2::services::analysis::core::OBJECT_MINUS_BUFFER)
    .value("distance_zone", terrama2::services::analysis::core::DISTANCE_ZONE);

  class_<terrama2::services::analysis::core::Buffer>("Buffer", init<>())
    .def(init<terrama2::services::analysis::core::BufferType, double, std::string>())
    .def(init<terrama2::services::analysis::core::BufferType, double, std::string, double, std::string>())
    .def_readwrite("buffer_type", &terrama2::services::analysis::core::Buffer::bufferType)
    .def_readwrite("distance", &terrama2::services::analysis::core::Buffer::distance)
    .def_readwrite("distance2", &terrama2::services::analysis::core::Buffer::distance2)
    .def_readwrite("unit", &terrama2::services::analysis::core::Buffer::unit)
    .def_readwrite("unit2", &terrama2::services::analysis::core::Buffer::unit2);

  terrama2::services::analysis::core::registerDCPFunctions();
  terrama2::services::analysis::core::registerOccurrenceFunctions();

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
  PyEval_InitThreads();
  Py_Initialize();
  INIT_MODULE();
  PyEval_ReleaseLock();
}

void terrama2::services::analysis::core::finalizeInterpreter()
{
  // shut down the interpreter
  PyEval_AcquireLock();
  Py_Finalize();
}
