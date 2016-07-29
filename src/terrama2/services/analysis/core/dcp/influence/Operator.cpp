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
  \file terrama2/services/analysis/core/dcp/influence/Operator.cpp

  \brief Contains DCP influence operators.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Operator.hpp"
#include "../Operator.hpp"
#include "../../ContextManager.hpp"
#include "../../MonitoredObjectContext.hpp"
#include "../../Analysis.hpp"
#include "../../Typedef.hpp"
#include "../../Shared.hpp"
#include "../../DataManager.hpp"
#include "../../PythonInterpreter.hpp"
#include "../../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../../core/data-access/DataSetSeries.hpp"
#include "../../Exception.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSetType.h>

// QT
#include <QString>

std::vector<DataSetId> terrama2::services::analysis::core::dcp::influence::byAttribute(const std::string& dataSeriesName, std::vector<std::string> attributeList)
{

  std::vector<DataSetId> vecIds;

  OperatorCache cache;
  readInfoFromDict(cache);
  auto context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);

  if(dataSeriesName.empty())
  {
    QString errMsg(QObject::tr("Invalid data series name"));
    context->addError(errMsg.toStdString());
    return vecIds;
  }

  if(attributeList.empty())
  {
    QString errMsg(QObject::tr("Empty attribute list"));
    context->addError(errMsg.toStdString());
    return vecIds;
  }


  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return vecIds;
    }

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    AnalysisPtr analysis = context->getAnalysis();

    auto moDsContext = getMonitoredObjectContextDataSeries(context, dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      errMsg = errMsg.arg(analysis->id);
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    for(std::string attribute : attributeList)
    {
      if(!moDsContext->series.teDataSetType)
      {
        QString errMsg(QObject::tr("Invalid dataset type for monitored object data series."));
        context->addError(errMsg.toStdString());
        throw terrama2::services::analysis::core::InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
      }

      if(moDsContext->series.teDataSetType->getProperty(attribute) == nullptr)
      {
        QString errMsg(QObject::tr("Invalid monitored object attribute: %1.").arg(attribute.c_str()));
        context->addError(errMsg.toStdString());
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(!moDsContext->series.syncDataSet->isNull(cache.index, attribute))
      {
        // Stores the result in the context
        auto syncDs = moDsContext->series.syncDataSet;
        DataSetId dcpId = syncDs->getInt32(cache.index, attribute);
        vecIds.push_back(dcpId);
      }


    }

  }
  catch(const terrama2::Exception& e)
  {
    context->addError( boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    vecIds.clear();
    return vecIds;
  }
  catch(const std::exception& e)
  {
    context->addError(e.what());
    vecIds.clear();
    return vecIds;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addError(errMsg.toStdString());
    vecIds.clear();
    return vecIds;
  }

  return vecIds;

}

std::vector<DataSetId> terrama2::services::analysis::core::dcp::influence::byRule(const std::string& dataSeriesName, const terrama2::services::analysis::core::Buffer& buffer)
{
  std::vector<DataSetId> vecIds;

  return vecIds;
}