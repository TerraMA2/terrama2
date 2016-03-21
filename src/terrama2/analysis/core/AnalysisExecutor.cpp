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
  \file terrama2/analysis/core/AnalysisExecutor.hpp

  \brief Prepare context for an analysis execution.

  \author Paulo R. M. Oliveira
*/

#include "AnalysisExecutor.hpp"

#include "Context.hpp"
#include "PythonInterpreter.hpp"
#include "../../Exception.hpp"
#include "../../core/Logger.hpp"
#include "../../core/PCD.hpp"
#include "../../core/DataManager.hpp"
#include "../../collector/TransferenceData.hpp"
#include "../../collector/Parser.hpp"
#include "../../collector/Factory.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSet.h>

//STL
#include <memory>

// Python
#include <Python.h>

void terrama2::analysis::core::joinThread(std::thread& t)
{
    t.join();
}

void terrama2::analysis::core::joinAllThreads(std::vector<std::thread>& threads)
{
    std::for_each(threads.begin(), threads.end(), joinThread);
}

void terrama2::analysis::core::runAnalysis(const Analysis& analysis)
{

  terrama2::analysis::core::Context::getInstance().addAnalysis(analysis);
  switch(analysis.type())
  {
    case Analysis::MONITORED_OBJECT_TYPE:
    {
      runMonitoredObjectAnalysis(analysis);
      break;
    }
    case Analysis::PCD_TYPE:
    {
      runDCPAnalysis(analysis);
      break;
    }
  }
}

void terrama2::analysis::core::runMonitoredObjectAnalysis(const Analysis& analysis)
{
  try
  {

    terrama2::core::DataSet monitoredObjectDs = analysis.monitoredObject();

    auto dsItems = monitoredObjectDs.dataSetItems();

    assert(dsItems.size() == 1);
    auto dataSetItem = dsItems[0];


    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;

    std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime;
    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(dataSetItem, lastLogTime);

    terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(dataSetItem);

    terrama2::collector::TransferenceData transferenceData;
    transferenceData.dataSet = monitoredObjectDs;

    auto provider = terrama2::core::DataManager::getInstance().findDataProvider(monitoredObjectDs.provider());

    transferenceData.uriTemporary = provider.uri() + dataSetItem.path() + dataSetItem.mask();
    transferenceDataVec.push_back(transferenceData);

    //read data and create a terralib dataset
    parser->read(filter, transferenceDataVec);

    assert(transferenceDataVec.size() == 1);
    std::shared_ptr<te::da::DataSet> teMonitoredOobjectDs = transferenceDataVec[0].teDataSet;

    auto metadata = dataSetItem.metadata();
    std::string identifier = metadata["identifier"];

    terrama2::analysis::core::Context::getInstance().addDataset(analysis.id(), dataSetItem.id(), "", teMonitoredOobjectDs, identifier, true);

    int size = teMonitoredOobjectDs->size();

     //check for the number o threads to create
    int threadNumber = std::thread::hardware_concurrency();

    PyThreadState * mainThreadState = NULL;
    // save a pointer to the main PyThreadState object
    mainThreadState = PyThreadState_Get();

    // get a reference to the PyInterpreterState
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;



    // Calculates the number of geometries that each thread will contain.
    int packageSize = 1;
    if(size >= threadNumber)
    {
      packageSize = (int)(size / threadNumber);
    }

    // if it's different than 0, the last package will be bigger.
    int mod = size % threadNumber;

    int begin = 0;

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
      threads[i] = std::thread(&terrama2::analysis::core::runMonitoredObjAnalysis, myThreadState, analysis.id(), indexes);

      begin += packageSize;
    }

    // release the lock
    PyEval_ReleaseLock();

    joinAllThreads(threads);

    auto result = Context::getInstance().analysisResult(analysis.id());

    if(result.empty())
    {
      QString errMsg(QObject::tr("Analysis: \%1\ -> Empty result.").arg(analysis.id()));
      TERRAMA2_LOG_WARNING() << errMsg;
    }
    for(auto it : result)
    {
      QString errMsg(QObject::tr("Analysis: \%1\ -> Geometry: \%2\ Result: %3.").arg(analysis.id()).arg(it.first.c_str()).arg(it.second));
      TERRAMA2_LOG_INFO() << errMsg;
    }

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
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}


void terrama2::analysis::core::runDCPAnalysis(const Analysis& analysis)
{
  /*try
  {

    terrama2::core::DataSet pcdDs = analysis.DCP();

    auto dcpItems = pcdDs.dataSetItems();

    for(auto dcpItem : dcpItems)
    {
      terrama2::core::PCD dcp(dcpItem);
      std::vector<terrama2::collector::TransferenceData> transferenceDataVec;

      std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime;
      terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(dcpItem, lastLogTime);

      terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(dcpItem);

      terrama2::collector::TransferenceData transferenceData;
      transferenceData.dataSet = pcdDs;

      auto provider = terrama2::core::DataManager::getInstance().findDataProvider(pcdDs.provider());

      transferenceData.uriTemporary = provider.uri() + dcpItem.path() + dcpItem.mask();
      transferenceDataVec.push_back(transferenceData);

      //read data and create a terralib dataset
      parser->read(filter, transferenceDataVec);

      assert(transferenceDataVec.size() == 1);
      std::shared_ptr<te::da::DataSet> teMonitoredOobjectDs = transferenceDataVec[0].teDataSet;
      std::shared_ptr<te::da::DataSetType> teMonitoredOobjectDst = transferenceDataVec[0].teDataSetType;

      auto metadata = dcpItem.metadata();
      std::string identifier = metadata["identifier"];

      uint64_t identifierPos = 0;
      for(int i = 0; i < teMonitoredOobjectDst->size(); i++)
      {
        if(identifier == teMonitoredOobjectDst->getProperty(i)->getName())
        {
          identifierPos = i;
          break;
        }
      }

      terrama2::analysis::core::Context::getInstance().addDCP(analysis.id(), dcp, teMonitoredOobjectDs, identifierPos, true);
    }


    int size = teMonitoredOobjectDs->size();

     //check for the number o threads to create
    int threadNumber = std::thread::hardware_concurrency();

    PyThreadState * mainThreadState = NULL;
    // save a pointer to the main PyThreadState object
    mainThreadState = PyThreadState_Get();

    // get a reference to the PyInterpreterState
    PyInterpreterState * mainInterpreterState = mainThreadState->interp;



    // Calculates the number of geometries that each thread will contain.
    int packageSize = 1;
    if(size >= threadNumber)
    {
      packageSize = (int)(size / threadNumber);
    }

    // if it's different than 0, the last package will be bigger.
    int mod = size % threadNumber;

    int begin = 0;

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
      threads[i] = std::thread(&terrama2::analysis::core::runMonitoredObjAnalysis, myThreadState, analysis.id(), indexes);

      begin += packageSize;
    }

    // release the lock
    PyEval_ReleaseLock();

    joinAllThreads(threads);

    auto result = Context::getInstance().analysisResult(analysis.id());

    if(result.empty())
    {
      QString errMsg(QObject::tr("Analysis: \%1\ -> Empty result."));
      TERRAMA2_LOG_WARNING() << errMsg;
    }
    for(auto it : result)
    {
      QString errMsg(QObject::tr("Analysis: \%1\ -> Geometry: \%2\ Result: %3.").arg(analysis.id()).arg(it.first.c_str()).arg(it.second));
      TERRAMA2_LOG_INFO() << errMsg;
    }

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

    // release the lock
    PyEval_ReleaseLock();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  */
}
