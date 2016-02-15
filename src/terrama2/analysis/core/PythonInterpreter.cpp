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


#include "../../collector/TransferenceData.hpp"
#include "../../collector/DataFilter.hpp"
#include "../../collector/Parser.hpp"
#include "../../collector/Factory.hpp"

#include "../../core/Logger.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/geometry/GeometryProperty.h>

PyObject* terrama2::analysis::core::max(PyObject* self, PyObject* args)
{
  return NULL;
}

static PyMethodDef module_methods[] = {
   { "max", (PyCFunction)terrama2::analysis::core::max, METH_NOARGS, NULL },
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
}


void terrama2::analysis::core::runMonitoredObjectAnalysis(terrama2::analysis::core::Analysis analysis)
{
  std::string fullScript = "from terrama2 import *\n";
  fullScript += analysis.script();

  terrama2::core::DataSet monitoredObjectDs = analysis.monitoredObject();
  auto dsItems = monitoredObjectDs.dataSetItems();

  assert(monitoredObjectDs.dataSetItems().size() == 1);
  auto item = monitoredObjectDs.dataSetItems()[0];


  std::vector<terrama2::collector::TransferenceData> transferenceDataVec;

  std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime;
   /*terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(dataSetItem, lastLogTime);

  terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(dataSetItem);

  //read data and create a terralib dataset
  parser->read(filter, transferenceDataVec);

  assert(transferenceDataVec.size() == 1);
  std::shared_ptr<te::da::DataSet> teMonitoredOobjectDs = transferenceDataVec[0].teDataSet;
  std::shared_ptr<te::da::DataSetType> teMonitoredOobjectDst = transferenceDataVec[0].teDataSetType;

  te::gm::GeometryProperty* geomProperty = te::da::GetFirstGeomProperty(teMonitoredOobjectDst);
  while(teMonitoredOobjectDs->moveNext())
  {
    te::gm::Geometry* geom = teMonitoredOobjectDs->getGeometry(geomProperty->name());

    PyObject * main = PyImport_AddModule("__main__"); // borrowed
    if (main == NULL)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException << QObject::tr("Could not add operation context");
    }
    PyObject *globals = PyModule_GetDict(main); // borrowed
    PyObject *value = PyInt_FromLong(geom->);
    if (value == NULL)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException << QObject::tr("Could not add operation context");
    }

    if (PyDict_SetItemString(globals, "n", value) < 0)
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Could not add operation context");
      throw terrama2::InitializationException << QObject::tr("Could not add operation context");
    }
    Py_DECREF(value);

    PyRun_SimpleString(fullScript.c_str());
  }
*/
}

void terrama2::analysis::core::finalize()
{
  Py_Finalize();
}

