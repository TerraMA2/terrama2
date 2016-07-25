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



#include "Operator.hpp"
#include "../ContextManager.hpp"

#include <QTextStream>

#include "../../../../core/data-model/Filter.hpp"

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
  readInfoFromDict(cache);
  // Inside Py_BEGIN_ALLOW_THREADS it's not allowed to return any value because it doesn' have the interpreter lock.
  // In case an exception is thrown, we need to set this boolean. Once the code left the lock is acquired we should return NAN.
  bool exceptionOccurred = false;
  auto context = ContextManager::getInstance().getMonitoredObjectContext(cache.analysisHashCode);

  try
  {
    // In case an error has already occurred, there is nothing to be done
    if(!context->getErrors().empty())
    {
      return NAN;
    }


    bool hasData = false;

    auto dataManagerPtr = context->getDataManager().lock();
    if(!dataManagerPtr)
    {
      QString errMsg(QObject::tr("Invalid data manager."));
      throw terrama2::core::InvalidDataManagerException() << terrama2::ErrorDescription(errMsg);
    }

    AnalysisPtr analysis = context->getAnalysis();



    std::shared_ptr<ContextDataSeries> moDsContext = getMonitoredObjectContextDataSeries(context, dataManagerPtr);
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
    auto firstOccurrence = moDsContext->series.syncDataSet->getGeometry(0, moDsContext->geometryPos);
    int srid = firstOccurrence->getSRID();

    std::shared_ptr<te::gm::Geometry> geomEnvelope(te::gm::GetGeomFromEnvelope(moEnvelope.get(), srid));

    auto moGeom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
    if(!moGeom.get())
    {
      QString errMsg(QObject::tr("Could not recover monitored object geometry."));
      throw InvalidDataSetException() << terrama2::ErrorDescription(errMsg);
    }


    // Save thread state before entering multi-thread zone

    Py_BEGIN_ALLOW_THREADS

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


          std::vector<uint64_t> indexes;
          uint32_t countValues = 0;
          terrama2::core::SynchronizedDataSetPtr syncDs = contextDataSeries->series.syncDataSet;

          if(syncDs->size() == 0)
          {
            continue;
          }
          else
          {
            auto geomResult = createBuffer(buffer, moGeom);

            // Converts the monitored object to the same srid of the occurrences
            auto firstOccurrence = syncDs->getGeometry(0, contextDataSeries->geometryPos);
            geomResult->transform(firstOccurrence->getSRID());

            // Searchs in the spatial index for the occurrences that intersects the monitored object box
            contextDataSeries->rtree.search(*geomResult->getMBR(), indexes);


            std::vector<double> values;

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

              auto bufferDs = createAggregationBuffer(indexes, contextDataSeries, aggregationBuffer, aggregationStatisticOperation, attribute);

              if(!bufferDs)
              {
                continue;
              }

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
                      cache.count++;
                      double value = bufferDs->getDouble(1);

                      values.push_back(value);
                      cache.sum += value;
                      if(value > cache.max)
                        cache.max = value;
                      if(value < cache.min)
                        cache.min = value;
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
              for(uint64_t i : indexes)
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
                      cache.count++;
                      double value = terrama2::services::analysis::core::getValue(syncDs, attribute, i, attributeType);

                      values.push_back(value);
                      cache.sum += value;
                      if(value > cache.max)
                        cache.max = value;
                      if(value < cache.min)
                        cache.min = value;
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
      catch(const terrama2::Exception& e)
      {
        context->addError( boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
        exceptionOccurred = true;
      }
      catch(const std::exception& e)
      {
        context->addError(e.what());
        exceptionOccurred = true;
      }
      catch(...)
      {
        QString errMsg = QObject::tr("An unknown exception occurred.");
        context->addError(errMsg.toStdString());
        exceptionOccurred = true;
      }

      // All operations are done, acquires the GIL and set the return value
    Py_END_ALLOW_THREADS

    if(exceptionOccurred)
      return NAN;


    if(!hasData && statisticOperation != StatisticOperation::COUNT)
    {
      return NAN;
    }

    return terrama2::services::analysis::core::getOperationResult(cache, statisticOperation);
  }
  catch(const terrama2::Exception& e)
  {
    context->addError( boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString());
    return NAN;
  }
  catch(const std::exception& e)
  {
    context->addError(e.what());
    return NAN;
  }
  catch(...)
  {
    QString errMsg = QObject::tr("An unknown exception occurred.");
    context->addError(errMsg.toStdString());
    return NAN;
  }
}

int terrama2::services::analysis::core::occurrence::count(const std::string& dataSeriesName, Buffer buffer,
                                                          const std::string& dateFilter, const std::string& restriction)
{
  return (int) operatorImpl(StatisticOperation::COUNT, dataSeriesName, buffer, dateFilter, Buffer(), "",
                            StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::min(const std::string& dataSeriesName, terrama2::services::analysis::core::Buffer buffer,
                                                           const std::string& dateFilter, const std::string& attribute, const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MIN, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::max(const std::string& dataSeriesName, Buffer buffer,
                                                           const std::string& dateFilter,
                                                           const std::string& attribute, const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MAX, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::mean(const std::string& dataSeriesName, Buffer buffer,
                                                            const std::string& dateFilter,
                                                            const std::string& attribute,
                                                            const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MEAN, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::median(const std::string& dataSeriesName, Buffer buffer,
                                                              const std::string& dateFilter,
                                                              const std::string& attribute,
                                                              const std::string& restriction)
{
  return operatorImpl(StatisticOperation::MEDIAN, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::standardDeviation(const std::string& dataSeriesName,
                                                                         Buffer buffer,
                                                                         const std::string& dateFilter,
                                                                         const std::string& attribute,
                                                                         const std::string& restriction)
{
  return operatorImpl(StatisticOperation::STANDARD_DEVIATION, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}

double terrama2::services::analysis::core::occurrence::sum(const std::string& dataSeriesName, Buffer buffer,
                                                           const std::string& dateFilter,
                                                           const std::string& attribute, const std::string& restriction)
{
  return operatorImpl(StatisticOperation::SUM, dataSeriesName, buffer, dateFilter, Buffer(), attribute,
                      StatisticOperation::INVALID, restriction);
}
