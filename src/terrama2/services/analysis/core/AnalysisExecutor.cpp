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
  \file terrama2/services/analysis/core/AnalysisExecutor.hpp

  \brief Prepare context for an analysis execution.

  \author Paulo R. M. Oliveira
*/

// TerraMA2

#include "AnalysisExecutor.hpp"
#include "python/PythonInterpreter.hpp"
#include "DataManager.hpp"
#include "ContextManager.hpp"
#include "../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/StoragerManager.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "GridContext.hpp"
#include "MonitoredObjectContext.hpp"

// STL
#include <thread>
#include <future>

//ThreadPool
#include <ThreadPool.h>

// Python
#include <Python.h>

// TerraLib
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/DateTimeInstant.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/dataaccess/dataset/UniqueKey.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Raster.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

terrama2::services::analysis::core::AnalysisExecutor::AnalysisExecutor()
{
  qRegisterMetaType<std::shared_ptr<te::dt::TimeInstantTZ>>("std::shared_ptr<te::dt::TimeInstantTZ>");
  qRegisterMetaType<uint32_t>("uint32_t");
}


void terrama2::services::analysis::core::AnalysisExecutor::runAnalysis(DataManagerPtr dataManager,
                                                     terrama2::core::StoragerManagerPtr storagerManager,
                                                     std::shared_ptr<terrama2::services::analysis::core::AnalysisLogger> logger,
                                                     std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                                                     AnalysisPtr analysis, ThreadPoolPtr threadPool,
                                                     PyThreadState* mainThreadState)
{
  RegisterId logId = 0;
  AnalysisHashCode analysisHashCode = analysis->hashCode(startTime);

  if(!logger.get())
  {
    QString errMsg = QObject::tr("Unable to access Logger class in analysis");
    TERRAMA2_LOG_ERROR() << errMsg;
    ContextManager::getInstance().addError(analysisHashCode,  errMsg.toStdString());
    return;
  }

  if(!startTime)
  {
    ContextManager::getInstance().addError(analysisHashCode, QObject::tr("Invalid start time").toStdString());
    return;
  }

  try
  {
    TERRAMA2_LOG_INFO() << QObject::tr("Starting analysis %1 execution: %2").arg(analysis->id).arg(startTime->toString().c_str());

    logId = logger->start(analysis->id);

    verifyInactiveDataSeries(dataManager, analysis, logger);

    switch(analysis->type)
    {
      case AnalysisType::MONITORED_OBJECT_TYPE:
      {
        runMonitoredObjectAnalysis(dataManager, storagerManager, analysis, startTime, threadPool, mainThreadState);
        break;
      }
      case AnalysisType::DCP_TYPE:
      {
        runDCPAnalysis(dataManager, storagerManager, analysis, startTime, threadPool, mainThreadState);
        break;
      }
      case AnalysisType::GRID_TYPE:
      {
        runGridAnalysis(dataManager, storagerManager, analysis, startTime, threadPool, mainThreadState);
        break;
      }
    }
  }
  catch(const terrama2::Exception& e)
  {
    ContextManager::getInstance().addError(analysisHashCode,  boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(const std::exception& e)
  {
    ContextManager::getInstance().addError(analysisHashCode, e.what());
  }
  catch(const boost::python::error_already_set&)
  {
    std::string errMsg = terrama2::services::analysis::core::python::extractException();
    ContextManager::getInstance().addError(analysisHashCode, errMsg);
  }
  catch(const boost::exception& e)
  {
    ContextManager::getInstance().addError(analysisHashCode, boost::diagnostic_information(e));
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    ContextManager::getInstance().addError(analysisHashCode, errMsg.toStdString());
  }

  try
  {
    auto warnings = ContextManager::getInstance().getErrors(analysisHashCode);
    if(!warnings.empty())
    {
      for (auto warning: warnings)
      {
        logger->log(AnalysisLogger::WARNING_MESSAGE, warning, logId);
      }
    }

    auto errors = ContextManager::getInstance().getErrors(analysisHashCode);
    if(!errors.empty())
    {

      std::string errorStr;
      for(auto error : errors)
      {
        errorStr += error + "\n";
        logger->log(AnalysisLogger::ERROR_MESSAGE, error, logId);
      }

      QString errMsg = QObject::tr("Analysis %1 (%2) finished with the following error(s):\n%3").arg(analysis->id).arg(startTime->toString().c_str()).arg(QString::fromStdString(errorStr));
      TERRAMA2_LOG_INFO() << errMsg;

      logger->result(AnalysisLogger::ERROR, startTime, logId);

      emit analysisFinished(analysis->id, false);
    }
    else
    {
      logger->result(AnalysisLogger::DONE, startTime, logId);

      QString errMsg = QObject::tr("Analysis %1 finished successfully: %2").arg(analysis->id).arg(startTime->toString().c_str());
      TERRAMA2_LOG_INFO() << errMsg;

      emit analysisFinished(analysis->id, true);
    }
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    emit analysisFinished(analysis->id, false);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    emit analysisFinished(analysis->id, false);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    emit analysisFinished(analysis->id, false);
  }

  // Clears context
  ContextManager::getInstance().clearContext(analysisHashCode);


}

void terrama2::services::analysis::core::AnalysisExecutor::runMonitoredObjectAnalysis(DataManagerPtr dataManager,
                                                                    terrama2::core::StoragerManagerPtr storagerManager,
                                                                    AnalysisPtr analysis,
                                                                    std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                                                                    ThreadPoolPtr threadPool,
                                                                    PyThreadState* mainThreadState)
{
  auto context = std::make_shared<terrama2::services::analysis::core::MonitoredObjectContext>(dataManager, analysis, startTime);
  ContextManager::getInstance().addMonitoredObjectContext(analysis->hashCode(startTime), context);

  std::vector<std::future<void> > futures;
  std::vector<PyThreadState*> states;
  try
  {
    context->loadMonitoredObject();

    size_t size = 0;
    for(auto analysisDataSeries : analysis->analysisDataSeriesList)
    {
      if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
      {
        auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
        auto datasets = dataSeries->datasetList;
        assert(datasets.size() == 1);
        auto dataset = datasets[0];

        auto contextDataset = context->getContextDataset(dataset->id);
        if(!contextDataset->series.syncDataSet->dataset())
        {
          QString errMsg = QObject::tr("Could not recover monitored object dataset.");
          throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
        }
        size = contextDataset->series.syncDataSet->size();

        break;
      }
    }

    if(size == 0)
    {
      QString errMsg = QObject::tr("Could not recover monitored object dataset.");
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    if(mainThreadState == nullptr)
    {
      QString errMsg = QObject::tr("Could not recover python interpreter main thread state");
      throw PythonInterpreterException() << ErrorDescription(errMsg);
    }

    // get a reference to the PyInterpreterState
    PyInterpreterState* mainInterpreterState = mainThreadState->interp;
    if(mainInterpreterState == nullptr)
    {
      QString errMsg = QObject::tr("Could not recover python interpreter thread state");
      throw PythonInterpreterException() << ErrorDescription(errMsg);
    }

    size_t threadNumber = std::min(threadPool->numberOfThreads(), size);

    // Calculates the number of geometries that each thread will contain.
    size_t packageSize = 1;
    if(size >= threadNumber)
    {
      packageSize = size / threadNumber;
    }

    // if it's different than 0, the last package will be bigger.
    uint32_t mod = size % threadNumber;

    uint32_t begin = 0;


    //Starts collection threads
    for(size_t i = 0; i < threadNumber; ++i)
    {

      std::vector<uint32_t> indexes;
      // The last package takes the rest of the division.
      if(i == threadNumber - 1)
        packageSize += mod;

      for(size_t j = begin; j < begin + packageSize; ++j)
      {
        indexes.push_back(j);
      }


      // create a thread state object for this thread
      PyThreadState* myThreadState = PyThreadState_New(mainInterpreterState);
      myThreadState->dict = PyDict_New();

      states.push_back(myThreadState);
      futures.push_back(threadPool->enqueue(&terrama2::services::analysis::core::python::runMonitoredObjectScript, myThreadState, context, indexes));

      begin += packageSize;
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });

    storeMonitoredObjectAnalysisResult(dataManager, storagerManager, context);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }


  // grab the lock
  terrama2::services::analysis::core::python::GILLock lock;
  PyThreadState_Swap(mainThreadState);

  for(auto state : states)
  {
    Py_DecRef(state->dict);
    // clear out any cruft from thread state object
    PyThreadState_Clear(state);
    // delete my thread state object
    PyThreadState_Delete(state);
  }
}


void terrama2::services::analysis::core::AnalysisExecutor::runDCPAnalysis(DataManagerPtr dataManager, terrama2::core::StoragerManagerPtr storagerManager, AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime, ThreadPoolPtr threadPool, PyThreadState* mainThreadState)
{
  // TODO: Ticket #433
  QString errMsg = QObject::tr("NOT IMPLEMENTED YET.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw Exception() << ErrorDescription(errMsg);
}

void terrama2::services::analysis::core::AnalysisExecutor::storeMonitoredObjectAnalysisResult(DataManagerPtr dataManager, terrama2::core::StoragerManagerPtr storagerManager, MonitoredObjectContextPtr context)
{

  if(context->hasError())
    return;

  auto resultMap = context->analysisResult();

  if(resultMap.empty())
  {
    QString errMsg = QObject::tr("Empty result.");
    throw EmptyResultException() << ErrorDescription(errMsg);
  }

  // In case an error occurred in the analysis execution there is nothing to do.
  if(context->hasError())
    return;

  auto analysis = context->getAnalysis();
  if(!analysis)
  {
    QString errMsg = QObject::tr("Could not recover the analysis configuration.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }


  if(analysis->type != AnalysisType::MONITORED_OBJECT_TYPE)
  {
    QString errMsg = QObject::tr("Invalid analysis type.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataSeries = dataManager->findDataSeries(analysis->outputDataSeriesId);

  if(!dataSeries)
  {
    QString errMsg = QObject::tr("Could not find the output data series.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);
  if(!dataProvider)
  {
    QString errMsg = QObject::tr("Could not find the output data provider.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  std::string datasetName;

  if(dataSeries->semantics.dataFormat == "POSTGIS")
  {
    auto dataSet = dataSeries->datasetList[0];
    datasetName = terrama2::core::getProperty(dataSet, dataSeries, "table_name");
  }
  else
  {
    //TODO Paulo: Implement storager file
    throw terrama2::Exception() << ErrorDescription("NOT IMPLEMENTED YET");
  }

  auto date = context->getStartTime();

  std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> moDsContext;

  // Reads the object monitored
  te::dt::Property* identifierProperty = nullptr;

  bool found = false;
  auto analysisDataSeriesList = analysis->analysisDataSeriesList;
  for(auto analysisDataSeries : analysisDataSeriesList)
  {
    if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
    {
      found = true;


      auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
      assert(dataSeries->datasetList.size() == 1);
      auto datasetMO = dataSeries->datasetList[0];

      if(!context->exists(datasetMO->id))
      {
        QString errMsg(QObject::tr("Could not recover monitored object dataset."));
        context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
        return;
      }

      moDsContext = context->getContextDataset(datasetMO->id);

      if(moDsContext->identifier.empty())
      {
        QString errMsg(QObject::tr("Monitored object identifier is empty."));
        context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
        return;
      }

      if(!moDsContext->series.teDataSetType)
      {
        QString errMsg(QObject::tr("Invalid dataset type."));
        context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
        return;
      }

      auto property = moDsContext->series.teDataSetType->getProperty(moDsContext->identifier);
      if(property != nullptr)
      {
        identifierProperty = property->clone();
      }

    }
  }

  if(!found)
  {
    QString errMsg(QObject::tr("Could not find a monitored object dataseries."));
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }

  if(!identifierProperty)
  {
    QString errMsg(QObject::tr("Invalid monitored object attribute identifier."));
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }


  auto attributes = context->getAttributes();

  assert(dataSeries->datasetList.size() == 1);


  std::shared_ptr<te::da::DataSetType> dt = std::make_shared<te::da::DataSetType>(datasetName);

  // first property is the geomId
  identifierProperty->setName("geom_id");
  dt->add(identifierProperty);


  //second property: analysis execution date
  te::dt::DateTimeProperty* dateProp = new te::dt::DateTimeProperty("execution_date", te::dt::TIME_INSTANT_TZ, true);
  dt->add(dateProp);

  // the unique key is composed by the geomId and the execution date.
  std::string nameuk = datasetName+ "_uk";
  te::da::UniqueKey* uk = new te::da::UniqueKey(nameuk);
  uk->add(identifierProperty);
  uk->add(dateProp);

  dt->add(uk);

  //create index on date column
  te::da::Index* indexDate = new te::da::Index(datasetName+ "_idx", te::da::B_TREE_TYPE);
  indexDate->add(dateProp);

  dt->add(indexDate);

  for(std::string attribute : attributes)
  {
    te::dt::SimpleProperty* prop = new te::dt::SimpleProperty(attribute, te::dt::DOUBLE_TYPE, false);
    dt->add(prop);
  }


  // Creates memory dataset and add the items.
  std::shared_ptr<te::mem::DataSet> ds = std::make_shared<te::mem::DataSet>(static_cast<te::da::DataSetType*>(dt->clone()));
  for(auto it = resultMap.begin(); it != resultMap.end(); ++it)
  {
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(ds.get());

    auto geomId = moDsContext->series.syncDataSet->getValue(it->first, moDsContext->identifier)->clone();
    dsItem->setValue("geom_id", geomId);
    dsItem->setDateTime("execution_date",  dynamic_cast<te::dt::DateTimeInstant*>(date.get()->clone()));
    for(auto itAttribute = it->second.begin(); itAttribute != it->second.end(); ++itAttribute)
    {
      dsItem->setDouble(itAttribute->first, itAttribute->second);
    }

    ds->add(dsItem);
  }

  assert(dataSeries->datasetList.size() == 1);
  auto dataset = dataSeries->datasetList[0];

  std::shared_ptr<terrama2::core::SynchronizedDataSet> syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(ds);

  terrama2::core::DataSetSeries series;
  series.teDataSetType = dt;
  series.syncDataSet.swap(syncDataSet);

  try
  {
    storagerManager->store(series, dataset);
  }
  catch(const terrama2::Exception /*e*/)
  {
    QString errMsg = QObject::tr("Could not store the result of the analysis.");
    throw Exception() << ErrorDescription(errMsg);
  }

}

void terrama2::services::analysis::core::AnalysisExecutor::runGridAnalysis(DataManagerPtr dataManager,
                                                         terrama2::core::StoragerManagerPtr storagerManager,
                                                         AnalysisPtr analysis,
                                                         std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                                                         ThreadPoolPtr threadPool,
                                                         PyThreadState* mainThreadState)
{
  auto context = std::make_shared<terrama2::services::analysis::core::GridContext>(dataManager, analysis, startTime);

  if(!analysis->outputGridPtr)
  {
    QString errMsg = QObject::tr("Invalid output grid configuration.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }


  ContextManager::getInstance().addGridContext(analysis->hashCode(startTime), context);

  std::vector<std::future<void> > futures;
  std::vector<PyThreadState*> states;

  try
  {
    auto outputRaster = context->getOutputRaster();

    if(!outputRaster)
    {
      QString errMsg = QObject::tr("Invalid output raster.");
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    size_t size = outputRaster->getNumberOfRows();
    if(size == 0)
    {
      QString errMsg = QObject::tr("Could not recover resolution X for the output grid.");
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    if(mainThreadState == nullptr)
    {
      QString errMsg = QObject::tr("Could not recover python interpreter main thread state");
      throw PythonInterpreterException() << ErrorDescription(errMsg);
    }

    // get a reference to the PyInterpreterState
    PyInterpreterState* mainInterpreterState = mainThreadState->interp;
    if(mainInterpreterState == nullptr)
    {
      QString errMsg = QObject::tr("Could not recover python interpreter thread state");
      throw PythonInterpreterException() << ErrorDescription(errMsg);
    }

    size_t threadNumber = std::min(threadPool->numberOfThreads(), size);


    // Calculates the number of geometries that each thread will contain.
    size_t packageSize = 1;
    if(size >= threadNumber)
    {
      packageSize = size / threadNumber;
    }

    // if it's different than 0, the last package will be bigger.
    uint32_t mod = size % threadNumber;

    uint32_t begin = 0;

    //Starts collection threads
    for(size_t i = 0; i < threadNumber; ++i)
    {

      std::vector<uint32_t> indexes;
      // The last package takes the rest of the division.
      if(i == threadNumber - 1)
        packageSize += mod;

      for(size_t j = begin; j < begin + packageSize; ++j)
      {
        indexes.push_back(j);
      }


      // create a thread state object for this thread
      PyThreadState* myThreadState = PyThreadState_New(mainInterpreterState);
      myThreadState->dict = PyDict_New();
      states.push_back(myThreadState);
      futures.push_back(threadPool->enqueue(&terrama2::services::analysis::core::python::runScriptGridAnalysis, myThreadState, context, indexes));

      begin += packageSize;
    }

    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });

    if(!context->hasError())
      storeGridAnalysisResult(storagerManager, context);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const boost::python::error_already_set&)
  {
    std::string errMsg = terrama2::services::analysis::core::python::extractException();
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg);
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const boost::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::diagnostic_information(e));
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f){ f.get(); });
  }

  // grab the lock
  terrama2::services::analysis::core::python::GILLock lock;
  PyThreadState_Swap(mainThreadState);

  for(auto state : states)
  {
    // clear out any cruft from thread state object
    PyThreadState_Clear(state);
    // delete my thread state object
    PyThreadState_Delete(state);
  }
}

void terrama2::services::analysis::core::AnalysisExecutor::storeGridAnalysisResult(terrama2::core::StoragerManagerPtr storagerManager,
                                                                 terrama2::services::analysis::core::GridContextPtr context)
{
  auto analysis = context->getAnalysis();
  if(!analysis)
  {
    QString errMsg = QObject::tr("Could not recover the analysis configuration.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  if(analysis->type != AnalysisType::GRID_TYPE)
  {
    QString errMsg = QObject::tr("Invalid analysis type.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }
  auto dataManager = context->getDataManager().lock();
  if(!dataManager.get())
  {
    QString errMsg = QObject::tr("Unable to access DataManager.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataSeries = dataManager->findDataSeries(analysis->outputDataSeriesId);
  if(!dataSeries)
  {
    QString errMsg = QObject::tr("Could not find the output data series.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);
  if(!dataProvider)
  {
    QString errMsg = QObject::tr("Could not find the output data provider.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }


  std::map<std::string, std::string> rinfo;

  auto raster = context->getOutputRaster();
  if(!raster)
  {
    QString errMsg = QObject::tr("Empty result.");
    throw EmptyResultException() << ErrorDescription(errMsg);
  }

  auto grid = raster->getGrid();
  if(grid == nullptr)
  {
    QString errMsg = QObject::tr("Empty result.");
    throw EmptyResultException() << ErrorDescription(errMsg);
  }

  std::vector<te::rst::BandProperty*> bprops;
  bprops.push_back(new te::rst::BandProperty(0, te::dt::DOUBLE_TYPE));

  te::rst::RasterProperty* rstp = new te::rst::RasterProperty(new te::rst::Grid(*grid), bprops, rinfo);
  te::dt::Property* timestamp = new te::dt::DateTimeProperty("analysis_timestamp", te::dt::TIME_INSTANT_TZ);
  te::da::DataSetType* dt = new te::da::DataSetType("raster dataset");

  dt->add(rstp);
  dt->add(timestamp);

  assert(dataSeries->datasetList.size() == 1);
  auto dataset = dataSeries->datasetList[0];

  std::shared_ptr<te::mem::DataSet> ds = std::make_shared<te::mem::DataSet>(dt);

  te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(ds.get());
  std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);
  std::size_t datePos = te::da::GetFirstPropertyPos(ds.get(), te::dt::DATETIME_TYPE);

  //REVIEW: should clone be used? why not the self raster?
  dsItem->setRaster(rpos, dynamic_cast<te::rst::Raster*>(raster->clone()));
  dsItem->setDateTime(datePos, dynamic_cast<te::dt::TimeInstantTZ*>(context->getStartTime()->clone()));
  ds->add(dsItem);

  std::shared_ptr<terrama2::core::SynchronizedDataSet> syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(ds);

  terrama2::core::DataSetSeries series;
  series.teDataSetType.reset(dt);
  series.syncDataSet.swap(syncDataSet);

  try
  {
    storagerManager->store(series, dataset);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return;
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    return;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred trying to store the results.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }
}

void terrama2::services::analysis::core::AnalysisExecutor::verifyInactiveDataSeries(DataManagerPtr dataManager, AnalysisPtr analysis, std::shared_ptr<terrama2::services::analysis::core::AnalysisLogger> logger)
{
  for(auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
    if(!dataSeries->active)
    {
      QString errMsg = QObject::tr("Analysis is using an inactive data series (%1).").arg(dataSeries->id);
      logger->log(AnalysisLogger::WARNING_MESSAGE, errMsg.toStdString(), analysis->id);
      TERRAMA2_LOG_WARNING() << errMsg;
    }
  }
}