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

#include "AnalysisExecutor.hpp"


// Python
#include <Python.h>

#include "PythonInterpreter.hpp"
#include "Context.hpp"
#include "DataManager.hpp"
#include "../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/data-access/DataStorager.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "../../../impl/DataStoragerPostGis.hpp"

// TerraLib
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/DateTimeInstant.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/memory/DataSet.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/memory/DataSetItem.h>


void terrama2::services::analysis::core::joinThread(std::thread& t)
{
  if(t.joinable())
    t.join();
}

void terrama2::services::analysis::core::joinAllThreads(std::vector<std::thread>& threads)
{
  std::for_each(threads.begin(), threads.end(), joinThread);
}

void terrama2::services::analysis::core::runAnalysis(DataManagerPtr dataManager, std::shared_ptr<terrama2::services::analysis::core::AnalysisLogger> logger, const Analysis& analysis, unsigned int threadNumber)
{
  RegisterId logId = 0;

  try
  {

    TERRAMA2_LOG_INFO() << QObject::tr("Starting analysis %1 execution").arg(analysis.id);
    if(logger.get())
      logId = logger->start(analysis.id);

    // If it's the first analysis to be run, it needs to set the main thread state in the context.
    if(Context::getInstance().getMainThreadState() == nullptr)
    {
      Context::getInstance().setMainThreadState(PyThreadState_Get());
    }

    Context::getInstance().addAnalysis(analysis);

    switch(analysis.type)
    {
      case AnalysisType::MONITORED_OBJECT_TYPE:
      {
        runMonitoredObjectAnalysis(dataManager, analysis, threadNumber);
        break;
      }
      case AnalysisType::PCD_TYPE:
      {
        runDCPAnalysis(dataManager, analysis, threadNumber);
        break;
      }
      case AnalysisType::GRID_TYPE:
      {
        runGridAnalysis(dataManager, analysis, threadNumber);
        break;
      }
      default:
      {
        QString errMsg = QObject::tr("Invalid analysis type.");
        Context::getInstance().addError(analysis.hashCode(), errMsg.toStdString());
        break;
      }
    }
  }
  catch(terrama2::Exception e)
  {
    Context::getInstance().addError(analysis.hashCode(),  boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(std::exception e)
  {
    Context::getInstance().addError(analysis.hashCode(), e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    Context::getInstance().addError(analysis.hashCode(), errMsg.toStdString());
  }

  try
  {
    auto errors = Context::getInstance().getErrors(analysis.hashCode());
    if(!errors.empty())
    {

      std::string errorStr;
      for(auto error : errors)
      {
        errorStr += error + "\n";
      }

      if(logger.get())
        logger->error(errorStr, logId);

      QString errMsg = QObject::tr("Analysis %1 finished with the following error(s):\n%2").arg(analysis.id).arg(QString::fromStdString(errorStr));
      TERRAMA2_LOG_INFO() << errMsg;
    }
    else
    {
      if(logger.get())
        logger->done(analysis.startDate, logId);

      QString errMsg = QObject::tr("Analysis %1 finished successfully.").arg(analysis.id);
      TERRAMA2_LOG_INFO() << errMsg;
    }


    // Clears context
    Context::getInstance().clearAnalysisContext(analysis.hashCode());
  }
  catch(terrama2::Exception e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(std::exception e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    TERRAMA2_LOG_ERROR() << errMsg.toStdString();
  }
}

void terrama2::services::analysis::core::runMonitoredObjectAnalysis(DataManagerPtr dataManager, const Analysis& analysis, unsigned int threadNumber)
{
  try
  {
    terrama2::services::analysis::core::Context::getInstance().loadMonitoredObject(analysis);

    int size = 0;
    for(auto analysisDataSeries : analysis.analysisDataSeriesList)
    {
      if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
      {
        auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
        auto datasets = dataSeries->datasetList;
        assert(datasets.size() == 1);
        auto dataset = datasets[0];

        auto contextDataset = terrama2::services::analysis::core::Context::getInstance().getContextDataset(analysis.hashCode(), dataset->id);
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

    // Recovers the main thread state
    PyThreadState * mainThreadState = Context::getInstance().getMainThreadState();
    assert(mainThreadState != nullptr);

    // get a reference to the PyInterpreterState
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;
    assert(mainInterpreterState != nullptr);

    if(threadNumber > size)
      threadNumber = size;


    // Calculates the number of geometries that each thread will contain.
    int packageSize = 1;
    if(size >= threadNumber)
    {
      packageSize = (int)(size / threadNumber);
    }

    // if it's different than 0, the last package will be bigger.
    int mod = size % threadNumber;

    unsigned int begin = 0;

    std::vector<std::thread> threads(threadNumber);
    std::vector<PyThreadState*> states;

    //Starts collection threads
    for (uint i = 0; i < threadNumber; ++i)
    {

      std::vector<uint64_t> indexes;
       // The last package takes the rest of the division.
      if(i == threadNumber - 1)
        packageSize += mod;

      for(unsigned int j = begin; j < begin + packageSize; ++j)
      {
        indexes.push_back(j);
      }


      // create a thread state object for this thread
      PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
      states.push_back(myThreadState);
      threads[i] = std::thread(&terrama2::services::analysis::core::runScriptMonitoredObjectAnalysis, myThreadState, analysis.hashCode(), indexes);

      begin += packageSize;
    }

    joinAllThreads(threads);


    // grab the lock
    PyEval_AcquireLock();
    for(auto state : states)
    {
      // swap my thread state out of the interpreter
      PyThreadState_Swap(NULL);
      // clear out any cruft from thread state object
      PyThreadState_Clear(state);
      // delete my thread state object
      PyThreadState_Delete(state);
    }

    PyThreadState_Swap(mainThreadState);

    // release the lock
    PyEval_ReleaseLock();

    storeAnalysisResult(dataManager, analysis);

  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}


void terrama2::services::analysis::core::runDCPAnalysis(DataManagerPtr dataManager, const Analysis& analysis, unsigned int threadNumber)
{
  // TODO: Ticket #433
  QString errMsg = QObject::tr("NOT IMPLEMENTED YET.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw Exception() << ErrorDescription(errMsg);

  /*
  try
  {

    int size = 0;
    for(auto analysisDataSeries : analysis.analysisDataSeriesList)
    {
      if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_PCD_TYPE)
      {
        auto dataSeriesPtr = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
        size =  dataSeriesPtr->datasetList.size();

        Context::getInstance().addDCPDataSeries(analysis.hashCode(), dataSeriesPtr);
        break;
      }
    }

    // Recovers the main thread state
    PyThreadState * mainThreadState = Context::getInstance().getMainThreadState();
    assert(mainThreadState != nullptr);

    // get a reference to the PyInterpreterState
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;
    assert(mainInterpreterState != nullptr);

    // create a thread state object for this thread
    PyThreadState* myThreadState = PyThreadState_New(mainInterpreterState);
    std::thread thread(&terrama2::services::analysis::core::runScriptDCPAnalysis, myThreadState, analysis.id);

    thread.join();


    // grab the lock
    PyEval_AcquireLock();

    // swap my thread state out of the interpreter
    PyThreadState_Swap(NULL);
    // clear out any cruft from thread state object
    PyThreadState_Clear(myThreadState);
    // delete my thread state object
    PyThreadState_Delete(myThreadState);

    PyThreadState_Swap(mainThreadState);

    // release the lock
    PyEval_ReleaseLock();

  }
  catch(terrama2::Exception e)
  {
    Context::getInstance().addError(analysis.hashCode(),  boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
  }
  catch(std::exception e)
  {
    Context::getInstance().addError(analysis.hashCode(), e.what());
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    Context::getInstance().addError(analysis.hashCode(), errMsg.toStdString());
  }*/
}

void terrama2::services::analysis::core::storeAnalysisResult(DataManagerPtr dataManager, const Analysis& analysis)
{
  auto resultMap = Context::getInstance().analysisResult(analysis.hashCode());

  if(resultMap.empty())
  {
    QString errMsg = QObject::tr("Empty result.");
    throw EmptyResultException() << ErrorDescription(errMsg);
  }

  // In case an error occurred in the analysis execution there is nothing to do.
  if(!Context::getInstance().getErrors(analysis.hashCode()).empty())
  {
    return;
  }

  auto attributes = Context::getInstance().getAttributes(analysis.hashCode());

  auto dataSeries = dataManager->findDataSeries(analysis.outputDataSeriesId);

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

  if(analysis.type == AnalysisType::MONITORED_OBJECT_TYPE || analysis.type == AnalysisType::PCD_TYPE)
  {
    assert(dataSeries->datasetList.size() == 1);

    std::string datasetName;
    std::unique_ptr<terrama2::core::DataStorager> storager;
    if(dataProvider->dataProviderType == "POSTGIS")
    {
      auto datasSet = dataSeries->datasetList[0];
      datasetName = datasSet->format.at("table_name");
      storager.reset(new terrama2::core::DataStoragerPostGis(dataProvider));
    }
    else
    {
      //TODO Paulo: Implement storager file
      throw terrama2::Exception() << ErrorDescription("NOT IMPLEMENTED YET");
    }

    te::da::DataSetType* dt = new te::da::DataSetType(datasetName);

    // first property is the geomId
    te::dt::StringProperty* geomIdProp = new te::dt::StringProperty("geom_id", te::dt::VAR_STRING, 255, true);
    dt->add(geomIdProp);


    //second property: analysis execution date
    te::dt::DateTimeProperty* dateProp = new te::dt::DateTimeProperty( "execution_date", te::dt::TIME_INSTANT_TZ, true);
    dt->add(dateProp);

    // the primary key is composed by the geomId and the execution date.
    std::string namepk = datasetName+ "_pk";
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(namepk, dt);
    pk->add(geomIdProp);
    pk->add(dateProp);



    //create index on date column
    te::da::Index* indexDate = new te::da::Index(datasetName+ "_idx", te::da::B_TREE_TYPE, dt);
    indexDate->add(dateProp);

    for(std::string attribute : attributes)
    {
      te::dt::SimpleProperty* prop = new te::dt::SimpleProperty(attribute, te::dt::DOUBLE_TYPE, false);
      dt->add(prop);
    }

    auto date = terrama2::core::TimeUtils::nowUTC();

    // Creates memory dataset and add the items.
    std::shared_ptr<te::mem::DataSet> ds = std::make_shared<te::mem::DataSet>(dt);
    for(auto it = resultMap.begin(); it != resultMap.end(); ++it)
    {
      te::mem::DataSetItem* dsItem = new te::mem::DataSetItem(ds.get());
      dsItem->setString("geom_id", it->first);
      dsItem->setDateTime("execution_date",  dynamic_cast<te::dt::DateTimeInstant*>(date.get()->clone()));
      for(auto itAttribute = it->second.begin(); itAttribute != it->second.end(); ++itAttribute)
      {
        dsItem->setDouble(itAttribute->first, itAttribute->second);
      }

      ds->add(dsItem);
    }


    assert(dataSeries->datasetList.size() == 1);
    auto dataset = dataSeries->datasetList[0];


    if(!storager)
    {
      QString errMsg = QObject::tr("Could not find storager support for the output data provider.");
      throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
    }

    std::shared_ptr<terrama2::core::SynchronizedDataSet> syncDataSet = std::make_shared<terrama2::core::SynchronizedDataSet>(ds);

    terrama2::core::DataSetSeries series;
    series.teDataSetType.reset(dt);
    series.syncDataSet.swap(syncDataSet);

    try
    {
      storager->store(series, dataset);
    }
    catch(terrama2::Exception /*e*/)
    {
      QString errMsg = QObject::tr("Could not store the result of the analysis: %1.").arg(analysis.id);
    }
  }
}

void ::terrama2::services::analysis::core::runGridAnalysis(DataManagerPtr shared_ptr, const Analysis& analysis, unsigned int number)
{
  QString errMsg = QObject::tr("NOT IMPLEMENTED YET.");
  throw Exception()  << ErrorDescription(errMsg);
}


