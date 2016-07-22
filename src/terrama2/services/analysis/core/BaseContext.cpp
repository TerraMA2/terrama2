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
  \file terrama2/services/analysis/core/BaseContext.cpp

  \brief Base class for analysis context

  \author Jano Simas
*/

#include "BaseContext.hpp"
#include "PythonInterpreter.hpp"

terrama2::services::analysis::core::BaseContext::BaseContext(terrama2::services::analysis::core::DataManagerPtr dataManager, terrama2::services::analysis::core::AnalysisPtr analysis, std::shared_ptr<te::dt::TimeInstantTZ> startTime)
  : dataManager_(dataManager),
    analysis_(analysis),
    startTime_(startTime)
{
  GILLock lock;
  auto oldState = PyThreadState_Get();
  mainThreadState_ = Py_NewInterpreter();
  PyThreadState_Swap(oldState);
}

terrama2::services::analysis::core::BaseContext::~BaseContext()
{
  GILLock lock;
  auto oldState = PyThreadState_Swap(mainThreadState_);
  Py_EndInterpreter(mainThreadState_);
  PyThreadState_Swap(oldState);
}

void terrama2::services::analysis::core::BaseContext::addError(const std::string& errorMessage)
{
  errosSet_.insert(errorMessage);
}

terrama2::core::DataSeriesPtr terrama2::services::analysis::core::BaseContext::findDataSeries(const std::string& dataSeriesName)
{
  auto it = dataSeriesMap_.find(dataSeriesName);
  if(it == dataSeriesMap_.end())
  {
    auto dataManagerPtr = getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    auto dataSeries = dataManagerPtr->findDataSeries(analysis_->id, dataSeriesName);

    dataSeriesMap_.emplace(dataSeriesName, dataSeries);
    return dataSeries;
  }

  return it->second;
}
