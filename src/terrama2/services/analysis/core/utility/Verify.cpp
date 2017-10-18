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
  \file terrama2/services/analysis/core/utility/Verify.cpp

  \brief Utility functions for easy consistency check

  \author Jano Simas
*/

#include "Verify.hpp"
#include "../DataManager.hpp"
#include "../Exception.hpp"
#include "../Analysis.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/DataAccessorFactory.hpp"
#include "../../../../core/data-access/DataAccessor.hpp"

#include <QObject>
#include <QString>

void terrama2::services::analysis::core::verify::analysisType(terrama2::services::analysis::core::AnalysisPtr analysis, int analysisType)
{
  // Use bitwise comparison
  // DCP_TYPE is a special case of MONITORED_OBJECT_TYPE, thus testing MONITORED_OBJECT_TYPE & DCP_TYPE returns true.
  if((static_cast<int>(analysis->type) & analysisType) == 0)
  {
    QString errMsg = QObject::tr("Wrong analysis type.");
    throw terrama2::core::VerifyException() << terrama2::ErrorDescription(errMsg);
  }
}

void terrama2::services::analysis::core::verify::analysisGrid(const terrama2::services::analysis::core::AnalysisPtr analysis)
{
  analysisType(analysis, static_cast<int>(terrama2::services::analysis::core::AnalysisType::GRID_TYPE));
}

void terrama2::services::analysis::core::verify::analysisMonitoredObject(const terrama2::services::analysis::core::AnalysisPtr analysis)
{
  // DCP_TYPE is a special case of MONITORED_OBJECT_TYPE, thus testing MONITORED_OBJECT_TYPE & DCP_TYPE returns true.
  analysisType(analysis, static_cast<int>(terrama2::services::analysis::core::AnalysisType::DCP_TYPE) | static_cast<int>(terrama2::services::analysis::core::AnalysisType::MONITORED_OBJECT_TYPE));
}

void terrama2::services::analysis::core::verify::analysisDCP(const terrama2::services::analysis::core::AnalysisPtr analysis)
{
  analysisType(analysis, static_cast<int>(terrama2::services::analysis::core::AnalysisType::DCP_TYPE));
}

std::set<std::string> terrama2::services::analysis::core::verify::inactiveDataSeries(DataManagerPtr dataManager, AnalysisPtr analysis)
{
  std::set<std::string> vecMessages;
  for(auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {
    auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
    if(!dataSeries->active)
    {
      QString errMsg = QObject::tr("Data series '%1' is inactive.").arg(dataSeries->name.c_str());
      TERRAMA2_LOG_WARNING() << errMsg;
      vecMessages.insert(errMsg.toStdString());
    }
  }

  return vecMessages;
}

std::set<std::string> terrama2::services::analysis::core::verify::dataAvailable(DataManagerPtr dataManager, AnalysisPtr analysis)
{
  std::set<std::string> vecMessages;
  for(auto& analysisDataSeries : analysis->analysisDataSeriesList)
  {

    auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
    auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

    auto dataAccesor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries);

    try
    {
      terrama2::core::Filter filter;
      auto series = dataAccesor->getSeries(filter, nullptr);

      if(series.empty())
      {
        QString errMsg = QObject::tr("No data available for data series '%1'.").arg(dataSeries->name.c_str());
        TERRAMA2_LOG_WARNING() << errMsg;
        vecMessages.insert(vecMessages.end(), errMsg.toStdString());
      }
    }
    catch(const terrama2::core::NoDataException&)
    {
      QString errMsg = QObject::tr("No data available for data series '%1'.").arg(dataSeries->name.c_str());
      TERRAMA2_LOG_WARNING() << errMsg;
      vecMessages.insert(vecMessages.end(), errMsg.toStdString());
    }
    catch(const terrama2::Exception& e)
    {
      std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
      vecMessages.insert(vecMessages.end(), errMsg);
      TERRAMA2_LOG_ERROR() << errMsg;
    }
  }

  return vecMessages;
}

std::set<std::string> terrama2::services::analysis::core::verify::attributeIdentifier(DataManagerPtr dataManager, AnalysisPtr analysis)
{
  std::set<std::string> vecMessages;
  if(analysis->type == AnalysisType::MONITORED_OBJECT_TYPE)
  {
    for(auto analysisDataSeries : analysis->analysisDataSeriesList)
    {
      if(analysisDataSeries.type == AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE)
      {

        auto dataSeries = dataManager->findDataSeries(analysisDataSeries.dataSeriesId);
        auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

        std::string identifier = analysisDataSeries.metadata["identifier"];

        auto dataAccesor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries);
        try
        {
          terrama2::core::Filter filter;
          auto series = dataAccesor->getSeries(filter, nullptr);

          if(!series.empty())
          {
            auto it = series.begin();
            if(it != series.end())
            {
              if(!it->second.teDataSetType)
              {
                QString errMsg = QObject::tr("Could not read dataset type for data series '%1'.").arg(dataSeries->name.c_str());
                TERRAMA2_LOG_WARNING() << errMsg;
                vecMessages.insert(vecMessages.end(), errMsg.toStdString());
              }
              auto property = it->second.teDataSetType->getProperty(identifier);
              if(property == nullptr)
              {
                QString errMsg = QObject::tr("Could not find the attribute identifier '%1' in data series '%2'.").arg(QString::fromStdString(identifier), QString::fromStdString(dataSeries->name));
                TERRAMA2_LOG_WARNING() << errMsg;
                vecMessages.insert(vecMessages.end(), errMsg.toStdString());
              }
            }
            else
            {
              throw new terrama2::core::NoDataException();
            }
          }
        }
        catch(const terrama2::core::NoDataException&)
        {
          QString errMsg = QObject::tr("No data available for data series '%1'.").arg(dataSeries->name.c_str());
          TERRAMA2_LOG_WARNING() << errMsg;
          vecMessages.insert(vecMessages.end(), errMsg.toStdString());
        }
        catch(const terrama2::Exception& e)
        {
          std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
          vecMessages.insert(vecMessages.end(), errMsg);
          TERRAMA2_LOG_ERROR() << errMsg;
        }

      }
    }
  }

  return vecMessages;
}

void terrama2::services::analysis::core::verify::validateAnalysis(DataManagerPtr dataManager, AnalysisPtr analysis, ValidateResult& validateResult)
{
  auto messages = inactiveDataSeries(dataManager, analysis);
  for(const auto& message : messages)
    validateResult.messages.insert(validateResult.messages.end(), message);

  messages = dataAvailable(dataManager, analysis);
  for(const auto& message : messages)
    validateResult.messages.insert(validateResult.messages.end(), message);

  messages = attributeIdentifier(dataManager, analysis);
  for(const auto& message : messages)
    validateResult.messages.insert(validateResult.messages.end(), message);

}
