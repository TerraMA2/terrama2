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
#include "../../../ContextManager.hpp"
#include "../../../MonitoredObjectContext.hpp"
#include "../../../Analysis.hpp"
#include "../../../Typedef.hpp"
#include "../../../Shared.hpp"
#include "../../../DataManager.hpp"
#include "../../../utility/Verify.hpp"
#include "../../../python/PythonInterpreter.hpp"
#include "../../../../../../core/data-access/SynchronizedDataSet.hpp"
#include "../../../../../../core/data-access/DataSetSeries.hpp"
#include "../../../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../../../core/utility/Logger.hpp"
#include "../../../Exception.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSetType.h>

// QT
#include <QString>

#include <boost/algorithm/string.hpp>

std::vector< std::string > terrama2::services::analysis::core::dcp::zonal::influence::byAttribute(const std::string& dataSeriesName, std::vector<std::string> attributeList)
{

  /////////////////////////////////////////
  // verify input data
  OperatorCache cache;
  python::readInfoFromDict(cache);

  auto& contextManager = ContextManager::getInstance();
  auto analysis = cache.analysisPtr;
  try
  {
    terrama2::services::analysis::core::verify::analysisMonitoredObject(analysis);
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
    return {};
  }

  // In case an error has already occurred, there is nothing to do.
  if(context->hasError())
    return {};

    if(dataSeriesName.empty())
    {
      QString errMsg(QObject::tr("Invalid data series name"));
      context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
      return {};
    }

    if(attributeList.empty())
    {
      QString errMsg(QObject::tr("Empty attribute list"));
      context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
      return {};
    }

  // end verify input data
  ////////////////////////////////////////////////////////


  try
  {
    /////////////////////////////////////////////////////////////
    // recover operation data

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    auto dcpDataSeries = dataManagerPtr->findDataSeries(analysis->id, dataSeriesName);
    if(!dcpDataSeries)
    {
      QString errMsg(QObject::tr("Could not recover a data series with name: %1.").arg(dataSeriesName.c_str()));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext || !moDsContext->series.teDataSetType)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    // end recover operation data
    ////////////////////////////////

    ////////////////////////////////
    // find influence

    // set of unique dcp alias
    std::set< std::string > vecDCP;
    terrama2::core::SynchronizedDataSetPtr syncDs = moDsContext->series.syncDataSet;
    for(const std::string& attribute : attributeList)
    {
      if(moDsContext->series.teDataSetType->getProperty(attribute) == nullptr)
      {
        QString errMsg(QObject::tr("Invalid monitored object attribute: %1.").arg(attribute.c_str()));
        throw terrama2::InvalidArgumentException() << terrama2::ErrorDescription(errMsg);
      }

      if(syncDs->isNull(cache.index, attribute))
        continue;

      auto dataSetAliasListStr = syncDs->getString(cache.index, attribute);
      if(dataSetAliasListStr.empty())
        continue;


      std::vector<std::string> aliasVec;
      boost::split(aliasVec, dataSetAliasListStr, [](char c){return c == ';';});

      vecDCP.insert(aliasVec.begin(), aliasVec.end());
    }
    // end find influence
    ////////////////////////////////

    return std::vector<std::string>(vecDCP.begin(), vecDCP.end());
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return {};
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return {};
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred in dcp::zonal::influence::byAttribute.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return {};
  }
}

std::vector< std::string > terrama2::services::analysis::core::dcp::zonal::influence::byRule(const std::string& dataSeriesName,
                                                                                             const terrama2::services::analysis::core::Buffer& buffer,
                                                                                             bool isActive)
{
  ///////////////////////////////////////
  // verify input data
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
    return {};
  }

  // In case an error has already occurred, there is nothing to do.
  if(context->hasError())
    return {};

  if(dataSeriesName.empty())
  {
    QString errMsg(QObject::tr("Invalid data series name"));
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return {};
  }
  // end verify input data
  ///////////////////////////////////////


  try
  {
    /////////////////////////////////////////////////////////////
    // recover operation data
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

    auto moGeom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!moGeom)
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      errMsg = errMsg.arg(analysis->id);
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto dcpDataSeries = dataManagerPtr->findDataSeries(analysis->id, dataSeriesName);
    if(!dcpDataSeries)
    {
      QString errMsg(QObject::tr("Could not recover a data series with name: %1.").arg(dataSeriesName.c_str()));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }
    // end recover operation data
    /////////////////////////////////////////////////////////////

    std::vector< std::string > vecDCP;

    // create monitored object buffer
    auto geomResult = createBuffer(buffer, moGeom);

    auto influenceType = zonal::getInfluenceType(analysis);
    for(const auto& dataset : dcpDataSeries->datasetList)
    {
      auto dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(dataset);
      if(!dcpDataset)
      {
        QString errMsg(QObject::tr("Invalid dataset for data series: ").arg(dataSeriesName.c_str()));
        throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
      }

      terrama2::core::Filter filter;
      auto dcpInfluenceBuffer = context->getDCPBuffer(dcpDataset->id);
      if(!dcpInfluenceBuffer)
      {
        dcpInfluenceBuffer = zonal::createDCPInfluenceBuffer(analysis, dcpDataset->position, geomResult->getSRID(), influenceType);
        context->addDCPBuffer(dcpDataset->id, dcpInfluenceBuffer);
      }

      if(zonal::verifyDCPInfluence(influenceType, geomResult, dcpInfluenceBuffer))
      {
        if(isActive && dcpDataset->active)//only add active DCP
          vecDCP.push_back(dcpDataset->alias());
        else
          vecDCP.push_back(dcpDataset->alias());
      }
    }
    
    return vecDCP;
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return {};
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, e.what());
    return {};
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::MessageType::ERROR_MESSAGE, errMsg.toStdString());
    return {};
  }
}
