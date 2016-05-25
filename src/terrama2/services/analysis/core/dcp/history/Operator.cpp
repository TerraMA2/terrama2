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
  \file terrama2/services/analysis/core/PythonInterpreter.cpp

  \brief Manages the communication of Python and C.

  \author Paulo R. M. Oliveira
*/



#include "Operator.hpp"
#include "../Operator.hpp"


#include "../../Exception.hpp"
#include "../../../../../core/utility/Logger.hpp"
#include "../../../../../core/data-model/DataSetDcp.hpp"
#include "../../../../../core/data-model/Filter.hpp"
#include "../../../../../core/data-access/SyncronizedDataSet.hpp"
#include "../../../../../core/Shared.hpp"


// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/vp/BufferMemory.h>
#include <terralib/geometry/MultiPolygon.h>


double terrama2::services::analysis::core::dcp::history::operatorImpl(StatisticOperation statisticOperation,
                                                                      const std::string& dataSeriesName,
                                                                      const std::string& attribute, DataSetId dcpId,
                                                                      Buffer buffer, const std::string& dateFilter)
{
  OperatorCache cache;
  readInfoFromDict(cache);

  bool hasData = false;

  auto dataManagerPtr = Context::getInstance().getDataManager().lock();
  if(!dataManagerPtr)
  {
    QString msg(QObject::tr("Invalid data manager."));
    TERRAMA2_LOG_ERROR() << msg;
    return NAN;
  }

  Analysis analysis = Context::getInstance().getAnalysis(cache.analysisId);

  auto moDsContext = getMonitoredObjectContextDataset(analysis, dataManagerPtr);
  if(!moDsContext)
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object dataset."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }

  auto geom = moDsContext->series.syncDataSet->getGeometry(cache.index, moDsContext->geometryPos);
  if(!geom.get())
  {
    QString errMsg(QObject::tr("Analysis: %1 -> Could not recover monitored object geometry."));
    errMsg = errMsg.arg(cache.analysisId);
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }


  bool dataSeriesFound = false;

  // Frees the GIL, from now on can not use the interpreter
  Py_BEGIN_ALLOW_THREADS

    std::shared_ptr<ContextDataSeries> contextDataset;

    try
    {
      for(auto analysisDataSeries : analysis.analysisDataSeriesList)
      {
        terrama2::core::DataSeriesPtr dataSeries = dataManagerPtr->findDataSeries(analysisDataSeries.dataSeriesId);
        if(dataSeries->name == dataSeriesName)
        {
          dataSeriesFound = true;

          if(dataSeries->semantics.dataSeriesType != terrama2::core::DataSeriesSemantics::DCP)
          {
            QString errMsg(QObject::tr("Analysis: %1 -> Given dataset is not from type DCP."));
            errMsg = errMsg.arg(cache.analysisId);
            TERRAMA2_LOG_ERROR() << errMsg;
            return NAN;
          }

          Context::getInstance().addDCPDataSeries(cache.analysisId, dataSeries, dateFilter, false);

          for(auto dataset : dataSeries->datasetList)
          {
            if(dataset->id != dcpId)
              continue;
            contextDataset = Context::getInstance().getContextDataset(cache.analysisId, dataset->id, dateFilter);

            terrama2::core::DataSetDcpPtr dcpDataset = std::dynamic_pointer_cast<const terrama2::core::DataSetDcp>(
                    dataset);
            if(!dcpDataset)
            {
              QString errMsg(QObject::tr("Analysis: %1 -> Could not recover DCP dataset."));
              errMsg = errMsg.arg(cache.analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }


            if(dcpDataset->position == nullptr)
            {
              QString errMsg(QObject::tr("Analysis: %1 -> DCP dataset does not have a valid position."));
              errMsg = errMsg.arg(cache.analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }


            try
            {

              auto influenceType = getInfluenceType(analysis);
              auto dcpInfluenceBuffer = createDCPInfluenceBuffer(analysis, dcpDataset->position, geom->getSRID(),
                                                                 influenceType);

              auto resultGeom = createBuffer(buffer, geom);


              bool intersects = verifyDCPInfluence(influenceType, resultGeom, dcpInfluenceBuffer);

              if(intersects)
              {

                auto syncDs = contextDataset->series.syncDataSet;

                int attributeType = 0;
                if(!attribute.empty())
                {
                  auto property = contextDataset->series.teDataSetType->getProperty(attribute);

                  // only operation COUNT can be done without attribute.
                  if(!property && statisticOperation != COUNT)
                  {
                    QString errMsg(QObject::tr("Analysis: %1 -> Invalid attribute name"));
                    errMsg = errMsg.arg(cache.analysisId);
                    TERRAMA2_LOG_ERROR() << errMsg;
                    return NAN;
                  }
                  attributeType = property->getType();
                }

                uint64_t countValues = 0;

                if(syncDs->size() == 0)
                  continue;


                std::vector<double> values;
                for(unsigned int i = 0; i < syncDs->size(); ++i)
                {
                  try
                  {
                    if(!attribute.empty() && !syncDs->isNull(i, attribute))
                    {
                      hasData = true;
                      countValues++;
                      double value = getValue(syncDs, attribute, i, attributeType);
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
                    // In case the DCP doesn't have the specified column
                    continue;
                  }
                }

                if(countValues == 0)
                  continue;

                calculateStatistics(values, cache);

              }

            }
            catch(std::exception& e)
            {
              QString errMsg(QObject::tr("Analysis: %1 -> %2").arg(e.what()));
              errMsg = errMsg.arg(cache.analysisId);
              TERRAMA2_LOG_ERROR() << errMsg;
              return NAN;
            }
          }

          break;
        }
      }
    }
    catch(std::exception& e)
    {
      TERRAMA2_LOG_ERROR() << e.what();
      return NAN;
    }


    // All operations are done, acquires the GIL and set the return value
  Py_END_ALLOW_THREADS

  if(!dataSeriesFound)
  {
    QString errMsg(
            QObject::tr("Analysis: %1 -> Could not find a data series with the name: %2.").arg(cache.analysisId).arg(
                    dataSeriesName.c_str()));
    TERRAMA2_LOG_ERROR() << errMsg;
    return NAN;
  }

  if(!hasData && statisticOperation != COUNT)
  {
    return NAN;
  }

  return getOperationResult(cache, statisticOperation);

}


double terrama2::services::analysis::core::dcp::history::sum(const std::string& dataSeriesName,
                                                             const std::string& attribute, DataSetId dcpId,
                                                             Buffer buffer,
                                                             const std::string& dateFilter)
{
  return operatorImpl(SUM, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcp::history::mean(const std::string& dataSeriesName,
                                                              const std::string& attribute, DataSetId dcpId,
                                                              Buffer buffer,
                                                              const std::string& dateFilter)
{
  return operatorImpl(MEAN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcp::history::min(const std::string& dataSeriesName,
                                                             const std::string& attribute, DataSetId dcpId,
                                                             Buffer buffer,
                                                             const std::string& dateFilter)
{
  return operatorImpl(MIN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcp::history::max(const std::string& dataSeriesName,
                                                             const std::string& attribute, DataSetId dcpId,
                                                             Buffer buffer,
                                                             const std::string& dateFilter)
{
  return operatorImpl(MAX, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcp::history::median(const std::string& dataSeriesName,
                                                                const std::string& attribute, DataSetId dcpId,
                                                                Buffer buffer, const std::string& dateFilter)
{
  return operatorImpl(MEDIAN, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}

double terrama2::services::analysis::core::dcp::history::standardDeviation(const std::string& dataSeriesName,
                                                                           const std::string& attribute,
                                                                           DataSetId dcpId,
                                                                           Buffer buffer, const std::string& dateFilter)
{
  return operatorImpl(STANDARD_DEVIATION, dataSeriesName, attribute, dcpId, buffer, dateFilter);
}
