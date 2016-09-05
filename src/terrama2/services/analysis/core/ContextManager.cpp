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
  \file terrama2/services/analysis/core/occurrence/ContextManager.cpp

  \brief Contains occurrence analysis context.

  \author Jano Simas
*/

#include "ContextManager.hpp"
#include "MonitoredObjectContext.hpp"
#include "GridContext.hpp"

#include "../../../core/utility/Logger.hpp"

void terrama2::services::analysis::core::ContextManager::addMonitoredObjectContext(const AnalysisHashCode analysisHashCode, MonitoredObjectContextPtr context)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto it = monitoredObjectContextMap_.find(analysisHashCode);
  if(it == monitoredObjectContextMap_.cend())
  {
    monitoredObjectContextMap_.emplace(analysisHashCode, context);
    analysisMap_.emplace(analysisHashCode, context->getAnalysis());
  }
  else
  {
    QString errMsg = QObject::tr("Monitored Object Context already registered for hascode: %1.").arg(analysisHashCode);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ContextManagerException() << ErrorDescription(errMsg);
  }
}

void terrama2::services::analysis::core::ContextManager::addGridContext(const AnalysisHashCode analysisHashCode, GridContextPtr context)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto it = gridContextMap_.find(analysisHashCode);
  if(it == gridContextMap_.cend())
  {
    gridContextMap_.emplace(analysisHashCode, context);
    analysisMap_.emplace(analysisHashCode, context->getAnalysis());
  }
  else
  {
    QString errMsg = QObject::tr("Grid Context already registered for hascode: %1.").arg(analysisHashCode);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ContextManagerException() << ErrorDescription(errMsg);
  }
}

terrama2::services::analysis::core::MonitoredObjectContextPtr terrama2::services::analysis::core::ContextManager::getMonitoredObjectContext(const AnalysisHashCode analysisHashCode) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto it = monitoredObjectContextMap_.find(analysisHashCode);
  if(it != monitoredObjectContextMap_.cend())
    return it->second;
  else
  {
    QString errMsg = QObject::tr("Unable to locate Monitored Object Context for hashcode %1.").arg(analysisHashCode);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ContextManagerException() << ErrorDescription(errMsg);
  }
}

terrama2::services::analysis::core::GridContextPtr terrama2::services::analysis::core::ContextManager::getGridContext(const AnalysisHashCode analysisHashCode) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto it = gridContextMap_.find(analysisHashCode);
  if(it != gridContextMap_.cend())
    return it->second;
  else
  {
    QString errMsg = QObject::tr("Unable to locate Grid Context for hashcode: %1.").arg(analysisHashCode);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ContextManagerException() << ErrorDescription(errMsg);
  }
}

terrama2::services::analysis::core::AnalysisPtr terrama2::services::analysis::core::ContextManager::getAnalysis(const AnalysisHashCode analysisHashCode) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto it = analysisMap_.find(analysisHashCode);
  if(it != analysisMap_.cend())
    return it->second;
  else
  {
    QString errMsg = QObject::tr("Unable to locate Analysis hashcode: %1.").arg(analysisHashCode);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ContextManagerException() << ErrorDescription(errMsg);
  }
}

void terrama2::services::analysis::core::ContextManager::clearContext(const AnalysisHashCode analysisHashCode)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto itm = monitoredObjectContextMap_.find(analysisHashCode);
  if(itm != monitoredObjectContextMap_.cend())
  {
    monitoredObjectContextMap_.erase(itm);
  }

  auto itg = gridContextMap_.find(analysisHashCode);
  if(itg != gridContextMap_.cend())
  {
    gridContextMap_.erase(itg);
  }

  auto ita = analysisMap_.find(analysisHashCode);
  if(ita != analysisMap_.cend())
    analysisMap_.erase(ita);
}

void terrama2::services::analysis::core::ContextManager::addError(const AnalysisHashCode analysisHashCode, const std::string& error)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  auto& errorList = analysisErrorMap_[analysisHashCode];
  errorList.insert(error);
}

void terrama2::services::analysis::core::ContextManager::addError(const std::string& error)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  contextError_.insert(error);
}

std::set<std::string> terrama2::services::analysis::core::ContextManager::getErrors(const AnalysisHashCode analysisHashCode) const
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  std::set<std::string> errors;
  auto it = analysisErrorMap_.find(analysisHashCode);
  if(it != analysisErrorMap_.cend())
    errors.insert(it->second.cbegin(), it->second.cend());

  auto itm = monitoredObjectContextMap_.find(analysisHashCode);
  if(itm != monitoredObjectContextMap_.cend())
  {
    auto errorList = itm->second->getErrors();
    errors.insert(errorList.cbegin(), errorList.cend());
  }

  auto itg = gridContextMap_.find(analysisHashCode);
  if(itg != gridContextMap_.cend())
  {
    auto errorList = itg->second->getErrors();
    errors.insert(errorList.cbegin(), errorList.cend());
  }

  return errors;
}
