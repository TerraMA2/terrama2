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
#include "utility/Verify.hpp"
#include "../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/StoragerManager.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "GridContext.hpp"

// STL
#include <thread>
#include <future>

//ThreadPool
#include <ThreadPool.h>

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
  qRegisterMetaType<uint32_t>("size_t");
}


void terrama2::services::analysis::core::AnalysisExecutor::runAnalysis(DataManagerPtr dataManager,
                                                                       terrama2::core::StoragerManagerPtr storagerManager,
                                                                       std::shared_ptr<AnalysisLogger> logger,
                                                                       const terrama2::core::ExecutionPackage& executionPackage,
                                                                       AnalysisPtr analysis, ThreadPoolPtr threadPool,
                                                                       PyThreadState* mainThreadState)
{
  AnalysisHashCode analysisHashCode = analysis->hashCode(executionPackage.executionDate);

  auto logId = executionPackage.registerId;
  auto startTime = executionPackage.executionDate;

  if(!executionPackage.executionDate)
  {
    ContextManager::getInstance().addError(analysisHashCode, QObject::tr("Invalid start time").toStdString());
    return;
  }

  auto processingStartTime = terrama2::core::TimeUtils::nowUTC();

  try
  {
    TERRAMA2_LOG_INFO() << QObject::tr("Starting analysis %1 execution: %2").arg(analysis->id).arg(startTime->toString().c_str());

    verify::inactiveDataSeries(dataManager, analysis);
    std::set<std::string> messages = verify::inactiveDataSeries(dataManager, analysis);
    if(!messages.empty())
    {
      for(const std::string& message : messages)
      {
        logger->log(AnalysisLogger::WARNING_MESSAGE, message, analysis->id);
      }
    }

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
  catch(const boost::python::error_already_set&)
  {
    std::string errMsg = terrama2::services::analysis::core::python::extractException();
    ContextManager::getInstance().addError(analysisHashCode, errMsg);
  }
  catch(const boost::exception& e)
  {
    ContextManager::getInstance().addError(analysisHashCode, boost::diagnostic_information(e));
  }
  catch(const std::exception& e)
  {
    ContextManager::getInstance().addError(analysisHashCode, e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    ContextManager::getInstance().addError(analysisHashCode, errMsg.toStdString());
  }

  try
  {
    auto warnings = ContextManager::getInstance().getMessages(analysisHashCode, BaseContext::MessageType::WARNING_MESSAGE);
    if(!warnings.empty())
    {
      for (const auto& warning: warnings)
      {
        logger->log(AnalysisLogger::WARNING_MESSAGE, warning, logId);
      }
    }

    auto errors = ContextManager::getInstance().getMessages(analysisHashCode, BaseContext::MessageType::ERROR_MESSAGE);
    if(!errors.empty())
    {
      // Analysis finished with errors

      std::string errorStr;
      for(const auto& error : errors)
      {
        errorStr += error + "\n";
        logger->log(AnalysisLogger::ERROR_MESSAGE, error, logId);
      }

      QString errMsg = QObject::tr("Analysis %1 (%2) finished with the following error(s):\n%3").arg(analysis->id).arg(QString::fromStdString(startTime->toString()), QString::fromStdString(errorStr));
      TERRAMA2_LOG_INFO() << errMsg;

      logger->result(AnalysisLogger::ERROR, startTime, logId);

      emit analysisFinished(analysis->id, startTime, false);
    }
    else
    {
      // Analysis finished successfully

      auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

      // log star and end processing time
      logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);
      logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

      logger->result(AnalysisLogger::DONE, startTime, logId);

      QString errMsg = QObject::tr("Analysis %1 finished successfully: %2").arg(analysis->id).arg(startTime->toString().c_str());
      TERRAMA2_LOG_INFO() << errMsg;

      emit analysisFinished(analysis->id, startTime, true);
    }
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    emit analysisFinished(analysis->id, startTime, false);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    emit analysisFinished(analysis->id, startTime, false);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
    emit analysisFinished(analysis->id, startTime, false);
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
    for(const auto& analysisDataSeries : analysis->analysisDataSeriesList)
    {
      if(analysis->type == AnalysisType::MONITORED_OBJECT_TYPE && analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
      {
        auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
        auto datasets = dataSeries->datasetList;
        assert(datasets.size() == 1);
        auto dataset = datasets[0];

        terrama2::core::Filter filter;
        auto contextDataset = context->getContextDataset(dataset->id, filter);
        if(!contextDataset->series.syncDataSet->dataset())
        {
          QString errMsg = QObject::tr("Could not recover monitored object dataset.");
          throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
        }
        size = contextDataset->series.syncDataSet->size();

        break;
      }

      if(analysis->type == AnalysisType::DCP_TYPE && analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
      {
        auto moDataSeries = context->getMonitoredObjectContextDataSeries(dataManager);
        size = moDataSeries->series.syncDataSet->size();
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

      std::for_each(futures.begin(), futures.end(), [](std::future<void>& f)
      {
        if(f.valid())
          f.get();
      });



    storeMonitoredObjectAnalysisResult(dataManager, storagerManager, context);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f)
    {
      if(f.valid())
        f.get();
    });
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f)
    {
      if(f.valid())
        f.get();
    });
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f)
    {
      if(f.valid())
        f.get();
    });
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

void terrama2::services::analysis::core::AnalysisExecutor::runDCPAnalysis(DataManagerPtr dataManager,
                                                                          terrama2::core::StoragerManagerPtr storagerManager,
                                                                          AnalysisPtr analysis,
                                                                          std::shared_ptr<te::dt::TimeInstantTZ> startTime,
                                                                          ThreadPoolPtr threadPool,
                                                                          PyThreadState* mainThreadState)
{
  runMonitoredObjectAnalysis(dataManager,
                             storagerManager,
                             analysis,
                             startTime,
                             threadPool,
                             mainThreadState);
}

std::shared_ptr<te::mem::DataSet>
terrama2::services::analysis::core::AnalysisExecutor::addDataToDataSet(std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> moDsContext,
                                                                       std::shared_ptr<te::da::DataSetType> dt,
                                                                       te::da::PrimaryKey* pkMonitoredObject,
                                                                       te::dt::Property* identifierProperty,
                                                                       std::unordered_map<int, std::map<std::string, boost::any> > resultMap,
                                                                       std::shared_ptr<te::dt::TimeInstantTZ>  date)
{
  std::shared_ptr<te::mem::DataSet> ds = std::make_shared<te::mem::DataSet>(static_cast<te::da::DataSetType*>(dt->clone()));
  for(auto it = resultMap.begin(); it != resultMap.end(); ++it)
  {
    te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(ds.get());

    if(pkMonitoredObject != nullptr)
    {
      for(auto property : pkMonitoredObject->getProperties())
      {
        // In case there is a column called 'execution_date' use the alternate name, because this name is used in analysis result table
        auto monitoredObjectId = moDsContext->series.syncDataSet->getValue(it->first, property->getName())->clone();
        if(property->getName() == EXECUTION_DATE_PROPERTY)
          dsItem->setValue(ALTERNATE_EXECUTION_DATE_PROPERTY, monitoredObjectId);
        else
          dsItem->setValue(property->getName(), monitoredObjectId);
      }
    }
    else
    {
      auto geomId = moDsContext->series.syncDataSet->getValue(it->first, moDsContext->identifier)->clone();
      dsItem->setValue(identifierProperty->getName(), geomId);
    }


    dsItem->setDateTime(EXECUTION_DATE_PROPERTY,  dynamic_cast<te::dt::DateTimeInstant*>(date.get()->clone()));
    for(auto itAttribute = it->second.begin(); itAttribute != it->second.end(); ++itAttribute)
    {
      if(itAttribute->second.empty())
      {
        dsItem->setValue(itAttribute->first, nullptr);
        continue;
      }

      try
      {
        auto value = boost::any_cast<double>(itAttribute->second);
        dsItem->setDouble(itAttribute->first, value);
        continue;
      }
      catch (...)
      {
        /* code */
      }

      try
      {
        auto value = boost::any_cast<std::string>(itAttribute->second);
        dsItem->setString(itAttribute->first, value);
        continue;
      }
      catch (...)
      {
        /* code */
      }

      QString errMsg = QObject::tr("Unknown data type in analysis result.");
      TERRAMA2_LOG_ERROR() << errMsg;
    }

    ds->add(dsItem);
  }

  return ds;
}

std::shared_ptr<te::da::DataSetType> terrama2::services::analysis::core::AnalysisExecutor::createDatasetType(std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> moDsContext,
                                                                                                             te::da::PrimaryKey* pkMonitoredObject,
                                                                                                             te::dt::Property* identifierProperty,
                                                                                                             std::set<std::pair<std::string, int> > attributes,
                                                                                                             std::string outputDatasetName)
{
  std::shared_ptr<te::da::DataSetType> dt = std::make_shared<te::da::DataSetType>(outputDatasetName);


  // the unique key is composed by primary key columns and the execution date.
  std::string nameuk = outputDatasetName+ "_uk";
  std::unique_ptr<te::da::UniqueKey> uk(new te::da::UniqueKey(nameuk));
  std::unique_ptr<te::da::ForeignKey> fk(new te::da::ForeignKey("monitored_object_fk"));
  fk->setReferencedDataSetType(moDsContext->series.teDataSetType.get());

  if(pkMonitoredObject != nullptr)
  {
    // the monitored object dataset has a primary key
    // use this the create a unique key based on the primary key and the execution date
    // also use it as a foreign key
    for(auto property : pkMonitoredObject->getProperties())
    {
      auto pkProperty = property->clone();

      // In case there is a column called 'execution_date' rename it because this name is used in analysis result table
      if(pkProperty->getName() == EXECUTION_DATE_PROPERTY)
        pkProperty->setName(ALTERNATE_EXECUTION_DATE_PROPERTY);

      dt->add(pkProperty);
      uk->add(pkProperty);
      fk->add(pkProperty);

      std::vector<te::dt::Property *> prop;
      prop.push_back(property);
      fk->setReferencedProperties(prop);
    }
  }
  else
  {
    // the monitored object dataset doesn't have a primary key
    // but has a identifier property
    // use this the create a unique key based on the primary key and the execution date
    // also use it as a foreign key

    dt->add(identifierProperty);
    uk->add(identifierProperty);
    fk->add(identifierProperty);

    std::vector<te::dt::Property *> prop;
    prop.push_back(moDsContext->series.teDataSetType->getProperty(moDsContext->identifier));
    fk->setReferencedProperties(prop);
  }
  dt->add(fk.release());

  //second property: analysis execution date
  std::unique_ptr<te::dt::DateTimeProperty> dateProp(new te::dt::DateTimeProperty(EXECUTION_DATE_PROPERTY, te::dt::TIME_INSTANT_TZ, true));
  dateProp->setPrecision(3);
  dt->add(dateProp.get());
  uk->add(dateProp.get());

  dt->add(uk.release());

  //create index on date column
  std::unique_ptr<te::da::Index> indexDate(new te::da::Index(outputDatasetName+ "_idx", te::da::B_TREE_TYPE));
  indexDate->add(dateProp.release());

  dt->add(indexDate.release());

  for(const auto& attribute : attributes)
  {
    te::dt::SimpleProperty* prop = nullptr;
    if(attribute.second == te::dt::STRING_TYPE)
      prop = new te::dt::StringProperty(attribute.first);
    else
      prop = new te::dt::SimpleProperty(attribute.first, attribute.second, false);
    dt->add(prop);
  }

  return dt;
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


  verify::analysisMonitoredObject(analysis);

  auto outputDataSeries = dataManager->findDataSeries(analysis->outputDataSeriesId);

  std::string outputDatasetName;

  if(outputDataSeries->semantics.dataFormat == "POSTGIS")
  {
    terrama2::core::DataSetPtr outputDataSet;

    for(auto& dataset : outputDataSeries->datasetList)
    {
      if(dataset->id == analysis->outputDataSetId)
      {
        outputDataSet = dataset;
        break;
      }
    }

    if(!outputDataSet.get())
      throw terrama2::Exception() << ErrorDescription("Output dataSet not found!");

    outputDatasetName = terrama2::core::getProperty(outputDataSet, outputDataSeries, "table_name");
  }
  else
  {
    //TODO Paulo: Implement storager file
    throw terrama2::Exception() << ErrorDescription("NOT IMPLEMENTED YET");
  }

  auto date = context->getStartTime();

  std::shared_ptr<terrama2::services::analysis::core::ContextDataSeries> moDsContext;

  // Reads the object monitored
  te::da::PrimaryKey* pkMonitoredObject = nullptr;
  te::dt::Property* identifierProperty = nullptr;

  moDsContext = context->getMonitoredObjectContextDataSeries(dataManager);
  if(moDsContext->identifier.empty())
  {
    QString errMsg(QObject::tr("Monitored object identifier is empty."));
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }

  if(!moDsContext->series.teDataSetType)
  {
    QString errMsg(QObject::tr("Invalid dataset type."));
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }

  pkMonitoredObject = moDsContext->series.teDataSetType->getPrimaryKey();

  // we need a unique identifier attribute,
  // if there is no primary key nor an identifier, return
  if(pkMonitoredObject == nullptr)
  {
    auto property = moDsContext->series.teDataSetType->getProperty(moDsContext->identifier);
    if(property != nullptr)
    {
      identifierProperty = property->clone();
    }

    if(!identifierProperty)
    {
      QString errMsg(QObject::tr("Invalid monitored object attribute identifier."));
      context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
      return;
    }
  }


  auto attributes = context->getAttributes();

  assert(outputDataSeries->datasetList.size() == 1);

  ////////////////////////////////////
  /// create datasetType
  std::shared_ptr<te::da::DataSetType> dt = createDatasetType(moDsContext, pkMonitoredObject, identifierProperty, attributes, outputDatasetName);

  // Creates memory dataset and add the items.
  std::shared_ptr<te::mem::DataSet> ds = addDataToDataSet(moDsContext, dt, pkMonitoredObject, identifierProperty, resultMap, date);
  /////////////////////////////////

  assert(outputDataSeries->datasetList.size() == 1);

  terrama2::core::DataSetPtr outputDataSet;

  for(auto& dataset : outputDataSeries->datasetList)
  {
    if(dataset->id == analysis->outputDataSetId)
    {
      outputDataSet = dataset;
      break;
    }
  }

  if(!outputDataSet.get())
    throw terrama2::Exception() << ErrorDescription("Output dataSet not found!");

  std::shared_ptr<terrama2::core::SynchronizedDataSet> syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(ds);

  terrama2::core::DataSetSeries series;
  series.teDataSetType = dt;
  series.syncDataSet.swap(syncDataSet);

  try
  {
    storagerManager->store(series, outputDataSet);
  }
  catch(const terrama2::Exception& e)
  {
    QString errMsg = QObject::tr("Could not store the result of the analysis: %1").arg(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString().c_str());
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
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
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
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const boost::python::error_already_set&)
  {
    std::string errMsg = terrama2::services::analysis::core::python::extractException();
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg);
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const boost::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::diagnostic_information(e));
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.get(); });
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
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

  verify::analysisGrid(analysis);

  auto dataManager = context->getDataManager().lock();
  if(!dataManager.get())
  {
    QString errMsg = QObject::tr("Unable to access DataManager.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto outputDataSeries = dataManager->findDataSeries(analysis->outputDataSeriesId);
  if(!outputDataSeries)
  {
    QString errMsg = QObject::tr("Could not find the output data series.");
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager->findDataProvider(outputDataSeries->dataProviderId);
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

  std::unique_ptr<te::rst::RasterProperty> rstp(new te::rst::RasterProperty(new te::rst::Grid(*grid), bprops, rinfo));
  std::unique_ptr<te::dt::Property> timestamp(new te::dt::DateTimeProperty("analysis_timestamp", te::dt::TIME_INSTANT_TZ));
  std::shared_ptr<te::da::DataSetType> dataSetType(new te::da::DataSetType("raster dataset"));

  dataSetType->add(rstp.release());
  dataSetType->add(timestamp.release());

  assert(outputDataSeries->datasetList.size() == 1);

  terrama2::core::DataSetPtr outputDataSet;

  for(auto& dataset : outputDataSeries->datasetList)
  {
    if(dataset->id == analysis->outputDataSetId)
    {
      outputDataSet = dataset;
      break;
    }
  }

  if(!outputDataSet.get())
    throw terrama2::Exception() << ErrorDescription("Output dataSet not found!");

  std::shared_ptr<te::mem::DataSet> ds = std::make_shared<te::mem::DataSet>(dataSetType.get());

  te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(ds.get());
  std::size_t rpos = te::da::GetFirstPropertyPos(ds.get(), te::dt::RASTER_TYPE);
  std::size_t datePos = te::da::GetFirstPropertyPos(ds.get(), te::dt::DATETIME_TYPE);

  //REVIEW: should clone be used? why not the self raster?
  dsItem->setRaster(rpos, dynamic_cast<te::rst::Raster*>(raster->clone()));
  dsItem->setDateTime(datePos, dynamic_cast<te::dt::TimeInstantTZ*>(context->getStartTime()->clone()));
  ds->add(dsItem);

  std::shared_ptr<terrama2::core::SynchronizedDataSet> syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(ds);

  terrama2::core::DataSetSeries series;
  series.teDataSetType = dataSetType;
  series.syncDataSet.swap(syncDataSet);

  try
  {
    storagerManager->store(series, outputDataSet);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return;
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred trying to store the results.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return;
  }
}


terrama2::services::analysis::core::ValidateResult terrama2::services::analysis::core::AnalysisExecutor::validateAnalysis(DataManagerPtr dataManager,
                                                                           AnalysisPtr analysis)
{
  ValidateResult validateResult;
  validateResult.analysisId = analysis->id;

  verify::validateAnalysis(dataManager, analysis, validateResult);

  if(analysis->scriptLanguage == ScriptLanguage::PYTHON)
  {
    try
    {
      python::validateAnalysisScript(analysis, validateResult);
    }
    catch(const terrama2::Exception& e)
    {
      validateResult.messages.insert(boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    }
    catch(const boost::python::error_already_set&)
    {
      std::string errMsg = python::extractException();
      validateResult.messages.insert(errMsg);
    }
    catch(const std::exception& e)
    {
      validateResult.messages.insert(e.what());
    }
    catch(...)
    {
      QString errMsg = QObject::tr("An unknown exception occurred.");
      validateResult.messages.insert(errMsg.toStdString());
    }
  }
  else
  {
    QString errMsg = QObject::tr("VALIDATION FOR LUA SCRIPT NOT IMPLEMENTED YET.");
    TERRAMA2_LOG_WARNING() << errMsg;
    validateResult.messages.insert(errMsg.toStdString());
  }

  validateResult.valid = validateResult.messages.empty();

  return validateResult;
}
