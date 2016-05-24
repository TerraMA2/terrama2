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

double terrama2::services::analysis::core::dcpHistoryOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  OperatorCache cache;
  readInfoFromDict(cache);

  bool hasData = false;

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    return NAN;
  }

  Analysis analysis = Context::getInstance().getAnalysis(cache.analysisId);

  auto moDsContext = getMonitoredObjectContextDataset(analysis, dataManagerPtr);
  if(!moDsContext)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }

  auto geom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(cache.analysisId);
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
          errMsg = errMsg.arg(cache.analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }

        Context::getInstance().addDCPDataSeries(cache.analysisId, dataSeries, dateFilter, false);

        for(auto dataset : dataSeries->datasetList)
        {
          if(dataset->id != dcpId)
            continue;
          contextDataset = Context::getInstance().getContextDataset(cache.analysisId, dataset->id, dateFilter);

          terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
          if(!dcpDataset)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> Could not recover DCP dataset."));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }


          if(dcpDataset->position == nullptr)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }


          try
          {

            auto influenceType = getInfluenceType(analysis);
            auto dcpInfluenceBuffer = createDCPInfluenceBuffer(analysis, dcpDataset->position, geom->getSRID(), influenceType);

            auto resultGeom = createBuffer(buffer, geom);


            bool intersects = verifyDCPInfluence(influenceType, resultGeom, dcpInfluenceBuffer);

            if(intersects)
            {

              auto syncDs = contextDataset->series.syncDataSet;

              int attributeType = 0;
              if(!attribute.empty())
              {
                auto property = contextDataset->series.teDataSetType->getProperty(attribute);

                // only operation COUNT can be done without attribute.
                if(!property && statisticOperation != COUNT)
                {
                  QString errMsg(QObject::tr("Analysis: %1 -> Invalid attribute name"));
                  errMsg = errMsg.arg(cache.analysisId);
                  TERRAMA2_LOG_ERROR() << errMsg;
                  return NAN;
                }
                attributeType = property->getType();
              }

              uint64_t countValues = 0;

              if(syncDs->size() == 0)
                continue;


              std::vector<double> values;
              for(unsigned int i = 0; i < syncDs->size(); ++i)
              {
                try
                {
                  if(!attribute.empty() && !syncDs->isNull(i, attribute))
                  {
                    hasData = true;
                    countValues++;
                    double value = getValue(syncDs, attribute, i, attributeType);
                    values.push_back(value);
                    cache.sum += value;
                    if(value > cache.max)
                      cache.max = value;
                    if(value < cache.min)
                      cache.min = value;
                  }
                }
                catch(...)
                {
                  // In case the DCP doesn't have the specified column
                  continue;
                }
              }

              if(countValues == 0)
                continue;

              calculateStatistics(values, cache);

            }

          }
          catch(std::exception& e)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> %2").arg(e.what()));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
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
    QString errMsg(QObject::tr("Analysis: %1 -> Could not find a data series with the name: %2.").arg(cache.analysisId).arg(dataSeriesName.c_str()));
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }

  if(!hasData && statisticOperation != COUNT)
  {
    return NAN;
  }

  return getOperationResult(cache, statisticOperation);

}


double terrama2::services::analysis::core::dcpHistorySum(const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(SUM, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMean(const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MEAN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMin(const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MIN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMax(const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MAX, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryMedian(const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(MEDIAN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcpHistoryStandardDeviation(const std::string& dataSeriesName, const std::string& attribute, DataSetId dcpId, Buffer buffer, const std::string& dateFilter)
{
  return dcpHistoryOperator(STANDARD_DEVIATION, dataSeriesName, attribute, dcpId,  buffer, dateFilter);
}

void terrama2::services::analysis::core::addValue(const std::string& attribute, double value)
{
  OperatorCache cache;
  readInfoFromDict(cache);

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    return;
  }

  Analysis analysis = Context::getInstance().getAnalysis(cache.analysisId);
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
          errMsg = errMsg.arg(cache.analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
        }

        moDsContext = Context::getInstance().getContextDataset(analysis.id, datasetMO->id);

        if(moDsContext->identifier.empty())
          assert(false);

        // Stores the result in the context
        std::string geomId = moDsContext->series.syncDataSet->getString(cache.index, moDsContext->identifier);

        Context::getInstance().addAttribute(cache.analysisId, attribute);
        Context::getInstance().setAnalysisResult(cache.analysisId, geomId, attribute, value);
      }
    }
  }
}


int terrama2::services::analysis::core::dcpCount(const std::string& dataSeriesName, Buffer buffer)
{
  return (int)dcpOperator(COUNT, dataSeriesName, "", buffer);
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
  OperatorCache cache;
  readInfoFromDict(cache);

  bool found = false;
  bool hasData = false;

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    return NAN;
  }

  Analysis analysis = Context::getInstance().getAnalysis(cache.analysisId);

  auto moDsContext = getMonitoredObjectContextDataset(analysis, dataManagerPtr);
  if(!moDsContext)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }


  auto geom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(cache.analysisId);
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
        errMsg = errMsg.arg(cache.analysisId);
        TERRAMA2_LOG_ERROR() << errMsg;
        return NAN;
      }


      Context::getInstance().addDCPDataSeries(cache.analysisId, dataSeries, "", true);

      // For DCP operator count returns the number of DCP that influence the monitored object
      uint64_t influenceCount = 0;

      for(auto dataset : dataSeries->datasetList)
      {
        contextDataset = Context::getInstance().getContextDataset(cache.analysisId, dataset->id);

        terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
        if(!dcpDataset)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> Could not recover DCP dataset."));
          errMsg = errMsg.arg(cache.analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }


        if(dcpDataset->position == nullptr)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
          errMsg = errMsg.arg(cache.analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }


        try
        {
          auto influenceType = getInfluenceType(analysis);

          auto dcpInfluenceBuffer = createDCPInfluenceBuffer(analysis, dcpDataset->position, geom->getSRID(), influenceType);

          bool intersects = verifyDCPInfluence(influenceType, geom, dcpInfluenceBuffer);

          if(intersects)
          {
            ++influenceCount;

            auto syncDs = contextDataset->series.syncDataSet;

            int attributeType = 0;
            if(!attribute.empty())
            {
              auto property = contextDataset->series.teDataSetType->getProperty(attribute);

              // only operation COUNT can be done without attribute.
              if(!property && statisticOperation != COUNT)
              {
                QString errMsg(QObject::tr("Analysis: %1 -> Invalid attribute name"));
                errMsg = errMsg.arg(cache.analysisId);
                TERRAMA2_LOG_ERROR() << errMsg;
                return NAN;
              }
              attributeType = property->getType();
            }

            uint64_t countValues = 0;

            if(syncDs->size() == 0)
              continue;

            std::vector<double> values;
            for(unsigned int i = 0; i < syncDs->size(); ++i)
            {
              try
              {
                if(!attribute.empty() && !syncDs->isNull(i, attribute))
                {
                  hasData = true;
                  countValues++;
                  double value = getValue(syncDs, attribute, i, attributeType);
                  values.push_back(value);
                  cache.sum += value;
                  if(value > cache.max)
                    cache.max = value;
                  if(value < cache.min)
                    cache.min = value;
                }
              }
              catch(...)
              {
                // In case the DCP doesn't have the specified column
                continue;
              }
            }

            if(countValues == 0)
              continue;

            // Statitics are calculated based on the number of values
            // but the operator count for DCP returns the number of DCPs that influence the monitored object

            cache.count = countValues;

            calculateStatistics(values, cache);
          }
        }
        catch(std::exception& e)
        {
          QString errMsg(QObject::tr("Analysis: %1 -> %2").arg(e.what()));
          errMsg = errMsg.arg(cache.analysisId);
          TERRAMA2_LOG_ERROR() << errMsg;
          return NAN;
        }


      }


      // Set the number of DCPs that influence the monitored object
      cache.count  = influenceCount;
      break;
    }
  }



  // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

  if(!found)
    return NAN;

  if(!hasData && statisticOperation != COUNT)
  {
    return NAN;
  }

  return getOperationResult(cache, statisticOperation);
}

double terrama2::services::analysis::core::occurrenceOperator(StatisticOperation statisticOperation, const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute, Buffer aggregationBuffer)
{
  OperatorCache cache;
  readInfoFromDict(cache);


  std::cout << statisticOperation << " "  << cache.index << std::endl;

  bool found = false;
  bool hasData = false;

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    return NAN;
  }

  Analysis analysis = Context::getInstance().getAnalysis(cache.analysisId);

  std::shared_ptr<ContextDataSeries> moDsContext = getMonitoredObjectContextDataset(analysis, dataManagerPtr);
  if(!moDsContext)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }


  auto moGeom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
  if(!moGeom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
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

        Context::getInstance().addDataSeries(cache.analysisId, dataSeries, dateFilter, true);

        auto datasets = dataSeries->datasetList;

        for(auto dataset : datasets)
        {

          contextDataset = Context::getInstance().getContextDataset(cache.analysisId, dataset->id, dateFilter);
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
          else
          {
            auto geomResult = createBuffer(buffer, moGeom);

            // Converts the monitored object to the same srid of the occurrences
            auto firstOccurrence = syncDs->getGeometry(0, contextDataset->geometryPos);
            geomResult->transform(firstOccurrence->getSRID());

            // Searchs in the spatial index for the occurrences that intersects the monitored object box
            contextDataset->rtree.search(*geomResult->getMBR(), indexes);


            std::vector<double> values;

            std::vector<std::shared_ptr<te::gm::Geometry> > geometries;

            int attributeType = 0;
            if(!attribute.empty())
            {
              auto property = contextDataset->series.teDataSetType->getProperty(attribute);

              // only operation COUNT can be done without attribute.
              if(!property && statisticOperation != COUNT)
              {
                QString errMsg(QObject::tr("Analysis: %1 -> Invalid attribute name"));
                errMsg = errMsg.arg(cache.analysisId);
                TERRAMA2_LOG_ERROR() << errMsg;
                return NAN;
              }
              attributeType = property->getType();
            }

            for(uint64_t i : indexes)
            {
              // Verifies if the occurrence intersects the monitored object
              auto occurrenceGeom = syncDs->getGeometry(i, contextDataset->geometryPos);

              if(occurrenceGeom->intersects(geomResult.get()))
              {
                geometries.push_back(occurrenceGeom);

                try
                {
                  if(!attribute.empty() && !syncDs->isNull(i, attribute))
                  {
                    hasData = true;
                    cache.count++;
                    double value = getValue(syncDs, attribute, i, attributeType);

                    values.push_back(value);
                    cache.sum += value;
                    if(value > cache.max)
                      cache.max = value;
                    if(value < cache.min)
                      cache.min = value;
                  }
                }
                catch(...)
                {
                  // In case the dataset doesn't have the specified attribute
                  continue;
                }
              }
            }


            calculateStatistics(values, cache);

            cache.count = geometries.size();

            if(aggregationBuffer.bufferType != NONE && !geometries.empty())
            {
              // Creates aggregation buffer
              std::shared_ptr<te::gm::Envelope> box(syncDs->dataset()->getExtent(contextDataset->geometryPos));

              auto bufferDs = createAggregationBuffer(geometries, box, aggregationBuffer);

              if(bufferDs)
                cache.count = bufferDs->size();

            }
          }

        }
      }
    }
  }
  catch(terrama2::Exception e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    return NAN;
  }
  catch(std::exception e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    return NAN;
  }
  catch(...)
  {
    return NAN;
  }


  // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

  if(!found)
    return NAN;

  if(!hasData && statisticOperation != COUNT)
  {
    return NAN;
  }

  return getOperationResult(cache, statisticOperation);
}

int terrama2::services::analysis::core::occurrenceCount(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction)
{
  return (int)occurrenceOperator(COUNT, dataSeriesName, buffer, dateFilter, restriction, "");
}

double terrama2::services::analysis::core::occurrenceMin(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute)
{
  return occurrenceOperator(MIN, dataSeriesName, buffer, dateFilter, restriction, attribute);
}

double terrama2::services::analysis::core::occurrenceMax(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute)
{
  return occurrenceOperator(MAX, dataSeriesName, buffer, dateFilter, restriction, attribute);
}

double terrama2::services::analysis::core::occurrenceMean(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute)
{
  return occurrenceOperator(MEAN, dataSeriesName, buffer, dateFilter, restriction, attribute);
}

double terrama2::services::analysis::core::occurrenceMedian(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute)
{
  return occurrenceOperator(MEDIAN, dataSeriesName, buffer, dateFilter, restriction, attribute);
}

double terrama2::services::analysis::core::occurrenceStandardDeviation(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute)
{
  return occurrenceOperator(STANDARD_DEVIATION, dataSeriesName, buffer, dateFilter, restriction, attribute);
}

double terrama2::services::analysis::core::occurrenceSum(const std::string& dataSeriesName, Buffer buffer, const std::string& dateFilter, const std::string& restriction, const std::string& attribute)
{
  return occurrenceOperator(SUM, dataSeriesName, buffer, dateFilter, restriction, attribute);
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

  // Register operations for dcp.history
  object dcpHistoryModule(handle<>(borrowed(PyImport_AddModule("terrama2.dcp.history"))));
  // make "from terrama2.dcp import history" work
  scope().attr("history") = dcpHistoryModule;
  // set the current scope to the new sub-module
  scope dcpHistoryScope = dcpHistoryModule;

  // export functions inside history namespace
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
  def("min", terrama2::services::analysis::core::occurrenceMin);
  def("max", terrama2::services::analysis::core::occurrenceMax);
  def("mean", terrama2::services::analysis::core::occurrenceMean);
  def("median", terrama2::services::analysis::core::occurrenceMedian);
  def("sum", terrama2::services::analysis::core::occurrenceSum);
  def("standard_deviation", terrama2::services::analysis::core::occurrenceStandardDeviation);
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
    .value("extern_plus_intern", terrama2::services::analysis::core::EXTERN_PLUS_INTERN)
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

void terrama2::services::analysis::core::readInfoFromDict(OperatorCache& cache)
{
  PyThreadState* state = PyThreadState_Get();
  PyObject* pDict = state->dict;

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  cache.index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  cache.analysisId = PyInt_AsLong(analysisPy);
}

double terrama2::services::analysis::core::getOperationResult(OperatorCache& cache, StatisticOperation statisticOperation)
{
  switch (statisticOperation)
  {
    case SUM:
      return cache.sum;
    case MEAN:
      return cache.mean;
    case MIN:
      return cache.min;
    case MAX:
      return cache.max;
    case STANDARD_DEVIATION:
      return cache.standardDeviation;
    case MEDIAN:
      return cache.median;
    case COUNT:
      return cache.count;
  }

  return NAN;
}


std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> terrama2::services::analysis::core::getMonitoredObjectContextDataset(const Analysis& analysis, std::shared_ptr<DataManager>& dataManagerPtr)
{
  std::shared_ptr<ContextDataSeries> contextDataSeries;

  for(const AnalysisDataSeries& analysisDataSeries : analysis.analysisDataSeriesList)
  {
    terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);

    if(analysisDataSeries.type == DATASERIES_MONITORED_OBJECT_TYPE)
    {
      assert(dataSeries->datasetList.size() == 1);
      auto datasetMO = dataSeries->datasetList[0];

      if(!Context::getInstance().exists(analysis.id, datasetMO->id))
      {
        QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
        errMsg = errMsg.arg(analysis.id);
        TERRAMA2_LOG_ERROR() << errMsg;
        return contextDataSeries;
      }

      return Context::getInstance().getContextDataset(analysis.id, datasetMO->id);
    }
  }

  return contextDataSeries;
}

double terrama2::services::analysis::core::getValue(terrama2::core::SyncronizedDataSetPtr syncDs, const std::string& attribute, uint64_t i, int attributeType)
{
  if(attribute.empty())
    return NAN;

  double value = NAN;
  switch (attributeType)
  {
    case te::dt::INT16_TYPE:
    {
      value = syncDs->getInt16(i, attribute);
    }
      break;
    case te::dt::INT32_TYPE:
    {
      value = syncDs->getInt32(i, attribute);
    }
      break;
    case te::dt::INT64_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getInt64(i, attribute));
    }
      break;
    case te::dt::DOUBLE_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getDouble(i, attribute));
    }
      break;
    case te::dt::NUMERIC_TYPE:
    {
      value = boost::lexical_cast<double>(syncDs->getNumeric(i, attribute));
    }
      break;
    default:
      break;
  }

  return value;
}

terrama2::services::analysis::core::InfluenceType terrama2::services::analysis::core::getInfluenceType(const Analysis& analysis)
{
  // Reads influence type
  std::string typeStr = analysis.metadata.at("INFLUENCE_TYPE");
  int type = std::atoi(typeStr.c_str());
  if(type == 0 || type > 3)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence type for DCP analysis."));
    errMsg = errMsg.arg(analysis.id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }
  InfluenceType influenceType = (InfluenceType) type;
  return influenceType;
}

std::shared_ptr<te::gm::Geometry> terrama2::services::analysis::core::createDCPInfluenceBuffer(const Analysis& analysis, std::shared_ptr<te::gm::Geometry> position, int monitoredObjectSrid, InfluenceType influenceType)
{
  std::shared_ptr<te::gm::Geometry> buffer;

  // For influence based on radius, creates a buffer for the DCP location
  if(influenceType == RADIUS_CENTER || influenceType == RADIUS_TOUCHES)
  {

    if(analysis.metadata.at("INFLUENCE_RADIUS").empty())
    {
      QString errMsg(QObject::tr("Analysis: %1 -> Invalid influence radius."));
      errMsg = errMsg.arg(analysis.id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
    }


    std::string radiusStr = analysis.metadata.at("INFLUENCE_RADIUS");
    std::string radiusUnit = analysis.metadata.at("INFLUENCE_RADIUS_UNIT");

    if(radiusStr.empty())
      radiusStr = "0";
    if(radiusUnit.empty())
      radiusUnit = "km";

    double influenceRadius = std::atof(radiusStr.c_str());

    influenceRadius = te::common::UnitsOfMeasureManager::getInstance().getConversion(radiusUnit, "METER") * influenceRadius;

    buffer.reset(position->buffer(influenceRadius, 16, te::gm::CapButtType));

    int srid = position->getSRID();
    buffer->setSRID(srid);

    // Converts the buffer to monitored object SRID
    buffer->transform(monitoredObjectSrid);
  }
  return buffer;
}

void terrama2::services::analysis::core::calculateStatistics(std::vector<double>& values, OperatorCache& cache)
{
  if(values.size() == 0)
    return;

  cache.mean = cache.sum / cache.count;
  std::sort (values.begin(), values.end());
  double half = values.size() / 2;
  if(values.size() > 1 && values.size() % 2 == 0)
  {
    cache.median = (values[(int)half] + values[(int)half - 1]) / 2;
  }
  else
  {
    cache.median = values.size() == 1 ? values[0] : 0.;
  }

  // calculates the variance
  double sumVariance = 0.;
  for(unsigned int i = 0; i < values.size(); ++i)
  {
    double value = values[i];
    sumVariance += (value - cache.mean) * (value - cache.mean);
  }

  cache.standardDeviation = sumVariance / cache.count;
}

bool terrama2::services::analysis::core::verifyDCPInfluence(InfluenceType influenceType, std::shared_ptr<te::gm::Geometry> geom, std::shared_ptr<te::gm::Geometry> dcpInfluenceBuffer)
{
  bool intersects = false;
  if(influenceType == RADIUS_TOUCHES)
  {
    intersects = geom->intersects(dcpInfluenceBuffer.get());
  }
  else if(influenceType == RADIUS_CENTER)
  {
    //TODO: use method from terralib_mod_sa_core
    std::string geomType = geom->getGeometryType();
    if(geomType == "MultiPolygon")
    {
      auto polygon = dynamic_cast<te::gm::MultiPolygon*>(geom.get());
      if(polygon)
      {
        auto centroid = polygon->getCentroid();
        intersects = centroid->within(dcpInfluenceBuffer.get());
      }
    }
    else
    {
      intersects = geom->intersects(dcpInfluenceBuffer.get());
    }
  }

  return intersects;

}