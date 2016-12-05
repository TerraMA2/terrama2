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
#include "../../utility/Verify.hpp"
#include "../../python/PythonInterpreter.hpp"
#include "../../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../../core/data-access/DataSetSeries.hpp"
#include "../../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../../core/utility/Logger.hpp"
#include "../../Exception.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSetType.h>

// QT
#include <QString>


std::vector<DataSetId> terrama2::services::analysis::core::dcp::zonal::influence::byAttribute(const std::string& dataSeriesName, std::vector<std::string> attributeList)
{

  std::vector<DataSetId> vecIds;

  OperatorCache cache;
  python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::core::verify::analysisMonitoredObject(analysis);
  }
  catch (const terrama2::core::VerifyException&)
  {
    contextManager.addError(cache.analysisHashCode, QObject::tr("Use of invalid operator for analysis %1.").arg(analysis->id).toStdString());
    return {};
  }

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return vecIds;
  }


  // In case an error has already occurred, there is nothing to do.
  if(context->hasError())
    return vecIds;

  if(dataSeriesName.empty())
  {
    QString errMsg(QObject::tr("Invalid data series name"));
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return vecIds;
  }

  if(attributeList.empty())
  {
    QString errMsg(QObject::tr("Empty attribute list"));
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return vecIds;
  }

  try
  {
    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }


    auto dcpDataSeries = dataManagerPtr->findDataSeries(dataSeriesName);
    if(!dcpDataSeries)
    {
      QString errMsg(QObject::tr("Could not recover a data series with name: %1.").arg(dataSeriesName.c_str()));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    for(std::string attribute : attributeList)
    {
      if(!moDsContext->series.teDataSetType)
      {
        QString errMsg(QObject::tr("Invalid dataset type for monitored object data series."));
        throw terrama2::services::analysis::core::InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
      }

      if(moDsContext->series.teDataSetType->getProperty(attribute) == nullptr)
      {
        QString errMsg(QObject::tr("Invalid monitored object attribute: %1.").arg(attribute.c_str()));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(!moDsContext->series.syncDataSet->isNull(cache.index, attribute))
      {
        // Stores the result in the context
        auto syncDs = moDsContext->series.syncDataSet;
        DataSetId dcpId = syncDs->getInt32(cache.index, attribute);

        bool found = false;
        for(auto dataSet : dcpDataSeries->datasetList)
        {
          if(dcpId == dataSet->id)
          {
            vecIds.push_back(dcpId);
            found = true;
            break;
          }
        }

        if(!found)
        {
          QString errMsg(QObject::tr("Could not find DCP identifier (%1) in dataseries %2.").arg(dcpId).arg(dataSeriesName.c_str()));
          throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
        }
      }
    }

  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    vecIds.clear();
    return vecIds;
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    vecIds.clear();
    return vecIds;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    vecIds.clear();
    return vecIds;
  }

  return vecIds;

}

std::vector<DataSetId> terrama2::services::analysis::core::dcp::zonal::influence::byRule(const std::string& dataSeriesName, const terrama2::services::analysis::core::Buffer& buffer)
{
  std::vector<DataSetId> vecIds;


  OperatorCache cache;
  python::readInfoFromDict(cache);

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return vecIds;
  }


  // In case an error has already occurred, there is nothing to do.
  if(context->hasError())
  {
    return vecIds;
  }

  if(dataSeriesName.empty())
  {
    QString errMsg(QObject::tr("Invalid data series name"));
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return vecIds;
  }

  try
  {
    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    AnalysisPtr analysis = context->getAnalysis();

    auto moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      errMsg = errMsg.arg(analysis->id);
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto geom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!geom.get())
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      errMsg = errMsg.arg(analysis->id);
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    std::string geomId = moDsContext->series.syncDataSet->getString(cache.index, moDsContext->identifier);

    auto dcpDataSeries = dataManagerPtr->findDataSeries(dataSeriesName);
    if(!dcpDataSeries)
    {
      QString errMsg(QObject::tr("Could not recover a data series with name: %1.").arg(dataSeriesName.c_str()));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto influenceType = zonal::getInfluenceType(analysis);

    for(auto dataset : dcpDataSeries->datasetList)
    {
      auto dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
      if(!dcpDataset)
      {
        QString errMsg(QObject::tr("Invalid dataset for data series: ").arg(dataSeriesName.c_str()));
        throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
      }

      auto dcpInfluenceBuffer = context->getDCPBuffer(dcpDataset->id);
      if(!dcpInfluenceBuffer)
      {
        dcpInfluenceBuffer = zonal::createDCPInfluenceBuffer(analysis, dcpDataset->position, geom->getSRID(),
                                                      influenceType);
        context->addDCPBuffer(dcpDataset->id, dcpInfluenceBuffer);
      }

      if(zonal::verifyDCPInfluence(influenceType, geom, dcpInfluenceBuffer))
      {
        vecIds.push_back(dcpDataset->id);
      }
    }
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    vecIds.clear();
    return vecIds;
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    vecIds.clear();
    return vecIds;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    vecIds.clear();
    return vecIds;
  }


  return vecIds;
}
