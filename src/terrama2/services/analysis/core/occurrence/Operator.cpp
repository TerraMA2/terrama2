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
  \file terrama2/services/analysis/core/occurrence/Operator.cpp

  \brief Contains occurrence analysis operators.

  \author Paulo R. M. Oliveira
*/


// TerraMA2
#include "Operator.hpp"
#include "../utility/Utils.hpp"
#include "../utility/Verify.hpp"
#include "../ContextManager.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/data-model/Filter.hpp"

#include <QTextStream>

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>
#include <terralib/geometry/Utils.h>

double terrama2::services::analysis::core::occurrence::operatorImpl(StatisticOperation statisticOperation,
    const std::string& dataSeriesName,
    Buffer buffer,
    const std::string& dateFilter,
    Buffer aggregationBuffer,
    const std::string& attribute,
    StatisticOperation aggregationStatisticOperation,
    const std::string& restriction)
{

  OperatorCache cache;
  terrama2::services::analysis::core::python::readInfoFromDict(cache);

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

  // After the operator lock is released it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;

  terrama2::services::analysis::core::MonitoredObjectContextPtr context;
  try
  {
    context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);
  }
  catch(const terrama2::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    return std::nan("");
  }

  try
  {
    // In case an error has already occurred, there is nothing to do.
    if(context->hasError())
    {
      return std::nan("");
    }


    bool hasData = false;

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    AnalysisPtr analysis = context->getAnalysis();



    std::shared_ptr<ContextDataSeries> moDsContext = context->getMonitoredObjectContextDataSeries(dataManagerPtr);
    if(!moDsContext)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    if(moDsContext->series.syncDataSet->size() == 0)
    {
      QString errMsg(QObject::tr("Could not recover monitored object data series."));
      throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
    }

    auto moEnvelope = moDsContext->series.syncDataSet->getExtent(moDsContext->geometryPos);
    auto firstMO = moDsContext->series.syncDataSet->getGeometry(0, moDsContext->geometryPos);
    int moSrid = firstMO->getSRID();

    std::shared_ptr<te::gm::Geometry> geomEnvelope(te::gm::GetGeomFromEnvelope(moEnvelope.get(), moSrid));

    auto moGeom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!moGeom.get())
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Save thread state and unlock python interpreter before entering multi-thread zone
    {
      terrama2::services::analysis::core::python::OperatorLock operatorLock;

      std::shared_ptr<ContextDataSeries> contextDataSeries;

      try
      {
        auto dataSeries = dataManagerPtr->findDataSeries(analysis->id, dataSeriesName);

        if(!dataSeries)
        {
          QString errMsg(QObject::tr("Could not find a data series with the given name: %1"));
          errMsg = errMsg.arg(QString::fromStdString(dataSeriesName));
          throw InvalidDataSeriesException() << terrama2::ErrorDescription(errMsg);
        }


        context->addDataSeries(dataSeries, geomEnvelope, dateFilter, true);

        auto datasets = dataSeries->datasetList;

        for(auto dataset : datasets)
        {

          contextDataSeries = context->getContextDataset(dataset->id, dateFilter);
          if(!contextDataSeries)
          {
            continue;
          }


          std::vector<uint32_t> indexes;
          uint32_t countValues = 0;
          std::vector<double> values;

          terrama2::core::SynchronizedDataSetPtr syncDs = contextDataSeries->series.syncDataSet;

          if(syncDs->size() == 0)
          {
            continue;
          }
          else
          {
            auto geomResult = createBuffer(buffer, moGeom);

            // Converts the monitored object to the same moSrid of the occurrences
            auto firstOccurrence = syncDs->getGeometry(0, contextDataSeries->geometryPos);
            geomResult->transform(firstMO->getSRID());

            // Searchs in the spatial index for the occurrences that intersects the monitored object box
            contextDataSeries->rtree.search(*geomResult->getMBR(), indexes);



            int attributeType = 0;
            if(!attribute.empty())
            {
              auto property = contextDataSeries->series.teDataSetType->getProperty(attribute);

              // only operation COUNT can be done without attribute.
              if(!property && statisticOperation != StatisticOperation::COUNT)
              {
                QString errMsg(QObject::tr("Invalid attribute name"));
                throw InvalidParameterException() << terrama2::ErrorDescription(errMsg);
              }
              attributeType = property->getType();
            }

            if(aggregationStatisticOperation != StatisticOperation::INVALID)
            {
              if(indexes.empty())
                continue;

              auto bufferDs = createAggregationBuffer(indexes, contextDataSeries, aggregationBuffer,
                                                      aggregationStatisticOperation, attribute);

              if(!bufferDs)
              {
                continue;
              }

              // Allocate memory for dataset size
              values.reserve(bufferDs->size());
              for(unsigned int i = 0; i < bufferDs->size(); ++i)
              {
                bufferDs->move(i);
                auto occurrenceGeom = bufferDs->getGeometry(0);

                if(occurrenceGeom->intersects(geomResult.get()))
                {
                  ++countValues;

                  try
                  {
                    if(!attribute.empty())
                    {
                      hasData = true;
                      // second column contains the value
                      double value = bufferDs->getDouble(1);

                      if(std::isnan(value))
                        continue;

                      values.push_back(value);
                    }
                  }
                  catch(...)
                  {
                    // In case the dataset doesn't have the specified attribute
                    continue;
                  }
                }
              }
            }
            else
            {

              // Allocate memory for indexes size
              values.reserve(indexes.size());
              for(uint32_t i : indexes)
              {
                // Verifies if the occurrence intersects the monitored object
                auto occurrenceGeom = syncDs->getGeometry(i, contextDataSeries->geometryPos);

                if(occurrenceGeom->intersects(geomResult.get()))
                {
                  ++countValues;

                  try
                  {
                    if(!attribute.empty() && !syncDs->isNull(i, attribute))
                    {
                      hasData = true;
                      double value = terrama2::services::analysis::core::getValue(syncDs, attribute, i, attributeType);

                      if(std::isnan(value))
                        continue;

                      values.push_back(value);
                    }
                  }
                  catch(...)
                  {
                    // In case the dataset doesn't have the specified attribute
                    continue;
                  }
                }
              }
            }



            terrama2::services::analysis::core::calculateStatistics(values, cache);

            if(statisticOperation == StatisticOperation::COUNT)
              cache.count = countValues;
          }

        }
      }
      catch(const EmptyDataSeriesException& e)
      {
        if(statisticOperation == StatisticOperation::COUNT)
          return 0;
        else
          return std::nan("");
      }
      catch(const terrama2::core::NoDataException& e)
      {
        if(statisticOperation == StatisticOperation::COUNT)
          return 0;
        else
          return std::nan("");
      }
      catch(const terrama2::Exception& e)
      {
        context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
        exceptionOccurred = true;
      }
      catch(const std::exception& e)
      {
        context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
        exceptionOccurred = true;
      }
      catch(...)
      {
        QString errMsg = QObject::tr("An unknown exception occurred.");
        context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
        exceptionOccurred = true;
      }

    // Destroy the OperatorLock object and acquires the lock
    }

    if(exceptionOccurred)
      return std::nan("");


    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return std::nan("");
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return std::nan("");
  }
  catch(const std::exception& e)
  {
    context->addLogMessage(BaseContext::ERROR_MESSAGE, e.what());
    return std::nan("");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addLogMessage(BaseContext::ERROR_MESSAGE, errMsg.toStdString());
    return std::nan("");
  }
}

int terrama2::services::analysis::core::occurrence::count(const std::string& dataSeriesName,
    const std::string& dateFilter, Buffer buffer, const std::string& restriction)
{
  return (int) operatorImpl(StatisticOperation::COUNT, dataSeriesName, buffer, dateFilter, Buffer(), "",
                            StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::min(const std::string& dataSeriesName,
                                                           const std::string& attribute,
                                                           const std::string& dateFilter,
                                                           terrama2::services::analysis::core::Buffer buffer,
                                                           const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::max(const std::string& dataSeriesName,
    const std::string& attribute,
    const std::string& dateFilter, Buffer buffer, const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::mean(const std::string& dataSeriesName,
                                                            const std::string& attribute,
                                                            const std::string& dateFilter,
                                                            Buffer buffer,
                                                            const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::median(const std::string& dataSeriesName,
                                                              const std::string& attribute,
                                                              const std::string& dateFilter,
                                                              Buffer buffer,
                                                              const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::standardDeviation(const std::string& dataSeriesName,
                                                                         const std::string& attribute,
                                                                         const std::string& dateFilter,
                                                                         Buffer buffer,
                                                                         const std::string& restriction)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::variance(const std::string& dataSeriesName,
                                                                const std::string& attribute,
                                                                const std::string& dateFilter,
                                                                Buffer buffer,
                                                                const std::string& restriction)
{
  return operatorImpl(StatisticOperation::VARIANCE, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::sum(const std::string& dataSeriesName,
                                                           const std::string& attribute,
                                                           const std::string& dateFilter,
                                                           Buffer buffer,
                                                           const std::string& restriction)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}
