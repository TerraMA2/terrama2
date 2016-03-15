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
  \file terrama2/analysis/core/PythonInterpreter.cpp

  \brief Manages the communication of Python and C.

  \author Paulo R. M. Oliveira
*/



#include "PythonInterpreter.hpp"

#include "../../analysis/core/Context.hpp"
#include "../../analysis/core/Analysis.hpp"
#include "../../collector/TransferenceData.hpp"
#include "../../collector/Parser.hpp"
#include "../../collector/Factory.hpp"
#include "../../core/Logger.hpp"
#include "../../core/DataSetItem.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/Utils.hpp"

#include <ctime>

// Boost
#include "boost/date_time/local_time/local_time.hpp"

// QT
#include <QObject>
#include <QFile>
#include <QTextStream>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/vp/BufferMemory.h>


PyObject* terrama2::analysis::core::countPoints(PyObject* self, PyObject* args)
{

  PyObject * main = PyImport_AddModule("__main__"); // borrowed
  if (main == NULL)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
    throw terrama2::InitializationException() << ErrorDescription(QObject::tr("Could not add operation context"));
  }
  PyObject *pDict = PyModule_GetDict(main); // borrowed

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);

  // Reads parameters
  const char * datasetName;
  double radius = 0.;
  const char * dateFilter;
  const char * restriction;

  if (!PyArg_ParseTuple(args, "sdss", &datasetName, &radius, &dateFilter, &restriction))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read function parameters.");
    return NULL;
  }

  std::string dateFilterStr(dateFilter);
  uint64_t count = 0;
  bool found = false;

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);


  // Reads the object monitored
  auto moItems = analysis.monitoredObject().dataSetItems();
  assert(moItems.size() == 1);
  auto moItem = moItems[0];
  if(!Context::getInstance().exists(analysis.id(), moItem.id(), ""))
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }

  auto datasetContext = Context::getInstance().getContextDataset(analysis.id(), moItem.id(), "");

  auto geom = datasetContext->dataset->getGeometry(index, datasetContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }

  std::time_t t = std::time(NULL);
  std::stringstream ss;
  ss << std::put_time(std::gmtime(&t), "%Z");

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(ss.str()));
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

  for(auto selectedDataset : analysis.additionalDataList())
  {
    if(selectedDataset.name() == datasetName)
    {
      found = true;
      auto items = selectedDataset.dataSetItems();

      for(auto item : items)
      {
        if(Context::getInstance().exists(analysisId, item.id(), dateFilterStr))
        {
          contextDataset = Context::getInstance().getContextDataset(analysisId, item.id(), dateFilterStr);
        }
        else
        {
          std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime;
          terrama2::collector::TransferenceData transferenceData;
          transferenceData.dataSet = selectedDataset;

          terrama2::core::Filter filter;
          std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
          filter.setDiscardBefore(std::move(titz));

          item.setFilter(filter);

          terrama2::collector::DataFilterPtr dataFilter = std::make_shared<terrama2::collector::DataFilter>(item, lastLogTime);
          terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(item);

          auto provider = terrama2::core::DataManager::getInstance().findDataProvider(selectedDataset.provider());

          transferenceData.uriTemporary = provider.uri() + item.path() + item.mask();
          std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
          transferenceDataVec.push_back(transferenceData);
          parser->read(dataFilter, transferenceDataVec);

          if(transferenceDataVec.size() == 0)
          {
            QString errMsg(QObject::tr("Analysis: \%1\ -> No data found for dataset: \"%2\".").arg(analysisId).arg(datasetName));
            TERRAMA2_LOG_WARNING() << errMsg;
            return NULL;
          }

          assert(transferenceDataVec.size() == 1);

          auto& tranfDataFiltered = transferenceDataVec[0];
          dataFilter->filterDataSet(tranfDataFiltered);
          std::shared_ptr<te::da::DataSet> teSelectedDataset = tranfDataFiltered.teDataSet;

          teSelectedDataset->moveBeforeFirst();


          teSelectedDataset->moveFirst();
          auto metadata = item.metadata();
          std::string identifier = metadata["identifier"];

          contextDataset = Context::getInstance().addDataset(analysisId, item.id(), dateFilterStr, teSelectedDataset, identifier, true);

        }

        // Frees the GIL, from now on can not use the interpreter
        Py_BEGIN_ALLOW_THREADS

        std::vector<uint64_t> indexes;

        auto sampleGeom = contextDataset->dataset->getGeometry(0, contextDataset->geometryPos);
        geom->transform(sampleGeom->getSRID());

        contextDataset->rtree.search(*geom->getMBR(), indexes);

        for(uint64_t index : indexes)
        {
          auto occurenceGeom = contextDataset->dataset->getGeometry(index, contextDataset->geometryPos);
          if(occurenceGeom->intersects(geom.get()))
          {
            ++count;

            // TODO: Paulo: Criar buffer e remover pontos dentro do buffer.
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


PyObject* terrama2::analysis::core::sumHistoryPCD(PyObject* self, PyObject* args)
{
  PyObject * main = PyImport_AddModule("__main__"); // borrowed
  if (main == NULL)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
    throw terrama2::InitializationException() << ErrorDescription(QObject::tr("Could not add operation context"));
  }
  PyObject *pDict = PyModule_GetDict(main); // borrowed

  // Geom index
  PyObject *geomKey = PyString_FromString("index");
  PyObject* geomIdPy = PyDict_GetItem(pDict, geomKey);
  uint64_t index = PyInt_AsLong(geomIdPy);

  // Analysis ID
  PyObject *analysisKey = PyString_FromString("analysis");
  PyObject* analysisPy = PyDict_GetItem(pDict, analysisKey);
  uint64_t analysisId = PyInt_AsLong(analysisPy);

  // Reads parameters
  const char* datasetName;
  const char* attribute;
  int pcdId;
  const char* dateFilter;

  if (!PyArg_ParseTuple(args, "ssis", &datasetName, &attribute, &pcdId, &dateFilter))
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read function parameters.");
    return NULL;
  }

  std::string dateFilterStr(dateFilter);
  double sum = 0;
  bool found = false;

  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  // Reads the dataset
  auto moItems = analysis.monitoredObject().dataSetItems();
  assert(moItems.size() == 1);
  auto moItem = moItems[0];
  if(!Context::getInstance().exists(analysis.id(), moItem.id(), ""))
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NULL;
  }

  auto datasetContext = Context::getInstance().getContextDataset(analysis.id(), moItem.id(), "");

  auto geom = datasetContext->dataset->getGeometry(index, datasetContext->geometryPos);


  std::time_t t = std::time(NULL);
  std::stringstream ss;
  ss << std::put_time(std::gmtime(&t), "%Z");

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(ss.str()));
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

  for(auto selectedDataset : analysis.additionalDataList())
  {
    if(selectedDataset.name() == datasetName)
    {
      found = true;

      auto items = selectedDataset.dataSetItems();
      for(auto selectedItem : items)
      {
        if(selectedItem.id() == pcdId)
        {
          terrama2::core::PCD dcp(selectedItem);
          if(Context::getInstance().exists(analysisId, selectedItem.id(), dateFilterStr))
          {
            contextDataset = Context::getInstance().getContextDataset(analysisId, selectedItem.id(), dateFilterStr);
          }
          else
          {
            std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime;
            terrama2::collector::TransferenceData transferenceData;
            transferenceData.dataSet = selectedDataset;

            terrama2::core::Filter filter;
            std::unique_ptr<te::dt::TimeInstantTZ> titz(new te::dt::TimeInstantTZ(ldt));
            filter.setDiscardBefore(std::move(titz));

            selectedItem.setFilter(filter);

            terrama2::collector::DataFilterPtr dataFilter = std::make_shared<terrama2::collector::DataFilter>(selectedItem, lastLogTime);
            terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(selectedItem);

            auto provider = terrama2::core::DataManager::getInstance().findDataProvider(selectedDataset.provider());

            transferenceData.uriTemporary = provider.uri() + selectedItem.path() + selectedItem.mask();
            std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
            transferenceDataVec.push_back(transferenceData);
            parser->read(dataFilter, transferenceDataVec);

            if(transferenceDataVec.size() == 0)
            {
              QString errMsg(QObject::tr("Analysis: \%1\ -> No data found for dataset: \"%2\".").arg(analysisId).arg(datasetName));
              TERRAMA2_LOG_WARNING() << errMsg;
              return NULL;
            }

            assert(transferenceDataVec.size() == 1);

            auto& tranfDataFiltered = transferenceDataVec[0];
            dataFilter->filterDataSet(tranfDataFiltered);
            std::shared_ptr<te::da::DataSet> teSelectedDataset = tranfDataFiltered.teDataSet;

            contextDataset = Context::getInstance().addDCP(analysisId, selectedItem.id(), dcp, dateFilterStr, teSelectedDataset, tranfDataFiltered.teDataSetType);

          }



          // Frees the GIL, from now on can not use the interpreter
          Py_BEGIN_ALLOW_THREADS

          auto dcpLocation = dcp.location();

          bool hasAttribute = false;
          for(unsigned int j = 0; j < contextDataset->datasetType->size(); ++j)
          {
            std::string name = contextDataset->datasetType->getProperty(j)->getName();
            if(name == attribute)
            {
              hasAttribute = true;
            }
          }

          // In case of the DCP doesn't have the selected attribute, moves to the next DCP.
          if(!hasAttribute)
            continue;


          auto influence = analysis.influence(selectedItem.id());
          if(influence.type == Analysis::RADIUS_CENTER)
          {
            auto buffer = dcpLocation->buffer(5, 16, te::gm::CapButtType);

            auto polygon = dynamic_cast<te::gm::MultiPolygon*>(geom.get());

            if(polygon != nullptr)
            {
              auto centroid = polygon->getCentroid();
              if(centroid->getSRID() == 0)
                centroid->setSRID(selectedItem.srid());
              else if(centroid->getSRID() != selectedItem.srid() && selectedItem.srid() != 0)
                centroid->transform(selectedItem.srid());

              if(centroid->within(buffer))
              {
                uint64_t size = contextDataset->dataset->size();
                for(unsigned int i = 0; i < size; ++i)
                {
                  if(!contextDataset->dataset->isNull(i, attribute))
                  {
                    double value = contextDataset->dataset->getDouble(i, attribute);
                    sum += value;
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

          break;
        }
      }
    }
  }




  if(found)
    return Py_BuildValue("d", sum);

  return NULL;


}

PyObject* terrama2::analysis::core::result(PyObject* self, PyObject* args)
{

  PyObject * main = PyImport_AddModule("__main__"); // borrowed
  if (main == NULL)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
    throw terrama2::InitializationException() << ErrorDescription(QObject::tr("Could not add operation context"));
  }
  PyObject *pDict = PyModule_GetDict(main); // borrowed

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
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not read the parameters from function countPoints.");
    return NULL;
  }


  Analysis analysis = Context::getInstance().getAnalysis(analysisId);
  if(analysis.type() == Analysis::MONITORED_OBJECT_TYPE)
  {
    auto moDs = analysis.monitoredObject();
    auto moItems = moDs.dataSetItems();
    assert(moItems.size() == 1);
    auto moItem = moItems[0];
    if(Context::getInstance().exists(analysisId, moItem.id(), ""))
    {
      auto contextDs = Context::getInstance().getContextDataset(analysisId, moItem.id(), "");
      std::string geomId = contextDs->dataset->getString(index, contextDs->identifier);
      Context::getInstance().setAnalysisResult(analysisId, geomId, result);
    }
  }

  return new PyObject();
}

static PyMethodDef module_methods[] = {
  { "countPoints", terrama2::analysis::core::countPoints, METH_VARARGS, "Count points operator"},
  { "sumHistoryPCD", terrama2::analysis::core::sumHistoryPCD, METH_VARARGS, "Sum history PCD"},
  { "result", terrama2::analysis::core::result, METH_VARARGS, "Set the result value"},
  { NULL, NULL, 0, NULL }
};

static PyObject * terrama2Error;
void terrama2::analysis::core::init()
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


void terrama2::analysis::core::runMonitoredObjAnalysis(PyThreadState* state, uint64_t analysisId, std::vector<uint64_t> indexes)
{
  // grab the global interpreter lock
  PyEval_AcquireLock();
  // swap in my thread state
  PyThreadState_Swap(state);


  PyObject * main = PyImport_AddModule("__main__"); // borrowed
  if (main == NULL)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
    throw terrama2::InitializationException() << terrama2::ErrorDescription(QObject::tr("Could not add operation context"));
  }

  PyObject *pDict = PyModule_GetDict(main); // borrowed

  PyObject *geomIdKey = PyString_FromString("index");
  PyObject *analysisKey = PyString_FromString("analysis");


  Analysis analysis = Context::getInstance().getAnalysis(analysisId);

  for(uint64_t index : indexes)
  {

    std::cout << "Running script for index: " << index << std::endl;
    PyThreadState_Clear(state);

    PyObject *indexValue = PyInt_FromLong(index);
    if (indexValue == NULL)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException() << terrama2::ErrorDescription(QObject::tr("Could not add geom index to the context"));
    }

    if (PyDict_SetItem(pDict, geomIdKey, indexValue) < 0)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException() << terrama2::ErrorDescription(QObject::tr("Could not add geom index to the context"));
    }

    PyObject *analysisValue = PyInt_FromLong(analysisId);
    if (analysisValue == NULL)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException() << terrama2::ErrorDescription(QObject::tr("Could not add analysis ID to the context"));
    }

    if (PyDict_SetItem(pDict, analysisKey, analysisValue) < 0)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException() << terrama2::ErrorDescription(QObject::tr("Could not add analysis ID to the context"));
    }


    PyRun_SimpleString("from terrama2 import *");
    PyRun_SimpleString(analysis.script().c_str());

  }

  // clear the thread state
  PyThreadState_Swap(NULL);

  // release our hold on the global interpreter
  PyEval_ReleaseLock();

}


void terrama2::analysis::core::finalize()
{
  // shut down the interpreter
  PyEval_AcquireLock();
  Py_Finalize();
}
