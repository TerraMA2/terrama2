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
  \file terrama2/services/alert/core/Report.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "Exception.hpp"
#include "Report.hpp"
#include "Utils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/raster/Band.h>
#include <terralib/memory/DataSetItem.h>

// Qt
#include <QObject>

// STD
#include <memory>

terrama2::services::alert::core::Report::Report(AlertPtr alert,
                                                terrama2::core::DataSeriesPtr alertDataSeries,
                                                std::shared_ptr<te::da::DataSet> alertDataSet,
                                                std::vector<std::shared_ptr<te::dt::DateTime>> riskDates)
  : alert_(alert),
    alertDataSeries_(alertDataSeries),
    riskDates_(riskDates)
{
  updateReportDataset(alertDataSet);
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveAllData() const
{
  return dataSet_;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataComparisonValue(const std::vector<int>& values) const
{
  if(alertDataSeries_->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for GRID data!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::vector<std::size_t> positions;

  size_t compPos = terrama2::core::propertyPosition(dataSet_.get(), "comparison_previous");

  for(std::size_t i = 0; i < dataSet_->size(); i++)
  {
    dataSet_->move(i);

    int comp = dataSet_->getInt32(compPos);

    auto it = std::find(values.begin(), values.end(), comp);

    if(it != values.end())
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::da::FilteredDataSet> filteredDataSet = std::make_shared<te::da::FilteredDataSet>(dataSet_.get(), positions);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataChangedRisk() const
{
  std::vector<int> values{-1,1};
  return retrieveDataComparisonValue(values);
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataUnchangedRisk() const
{
  std::vector<int> values{0};
  return retrieveDataComparisonValue(values);
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataIncreasedRisk() const
{
  std::vector<int> values{1};
  return retrieveDataComparisonValue(values);
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataDecreasedRisk() const
{
  std::vector<int> values{-1};
  return retrieveDataComparisonValue(values);
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataAtRisk(const int risk) const
{
  if(alertDataSeries_->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for GRID data!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::vector<std::size_t> positions;

  size_t riskPos = terrama2::core::propertyPosition(dataSet_.get(), riskDates_.at(0)->toString());

  for(std::size_t i = 0; i < dataSet_->size(); i++)
  {
    dataSet_->move(i);
    if(dataSet_->getInt32(riskPos) == risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::da::FilteredDataSet> filteredDataSet = std::make_shared<te::da::FilteredDataSet>(dataSet_.get(), positions);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataAboveRisk(const int risk) const
{
  if(alertDataSeries_->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for GRID data!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::vector<std::size_t> positions;

  size_t riskPos = terrama2::core::propertyPosition(dataSet_.get(), riskDates_.at(0)->toString());

  for(std::size_t i = 0; i < dataSet_->size(); i++)
  {
    dataSet_->move(i);
    if(dataSet_->getInt32(riskPos) >= risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::da::FilteredDataSet> filteredDataSet = std::make_shared<te::da::FilteredDataSet>(dataSet_.get(), positions);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataBelowRisk(const int risk) const
{
  if(alertDataSeries_->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for GRID data!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::vector<std::size_t> positions;

  size_t riskPos = terrama2::core::propertyPosition(dataSet_.get(), riskDates_.at(0)->toString());

  for(std::size_t i = 0; i < dataSet_->size(); i++)
  {
    dataSet_->move(i);
    if(dataSet_->getInt32(riskPos) <= risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::da::FilteredDataSet> filteredDataSet = std::make_shared<te::da::FilteredDataSet>(dataSet_.get(), positions);

  return  filteredDataSet;
}


void terrama2::services::alert::core::Report::updateReportDataset(const std::shared_ptr<te::da::DataSet> dataSet)
{
  dataSet->moveBeforeFirst();

  if(alertDataSeries_->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID)
  {
    dataSet_ = std::dynamic_pointer_cast<te::mem::DataSet>(dataSet);
    return;
  }

  dataSet_ = std::make_shared<te::mem::DataSet>(*dataSet);

  // Replace risk values
  for(auto riskDate : riskDates_)
  {
    std::string property = dateTimeToString(riskDate);
    auto pos = terrama2::core::propertyPosition(dataSet_.get(), property);

    if(pos == std::numeric_limits<size_t>::max())
    {
      QString errMsg = QObject::tr("Can't find property %1 !").arg(QString::fromStdString(property));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ReportException() << ErrorDescription(errMsg);
    }

    dataSet_->moveBeforeFirst();

    while(dataSet_->moveNext())
    {
      if(!dataSet_->isNull(pos))
      {
        int numericRisk = dataSet_->getInt32(pos);
        dataSet_->setString(property, alert_->risk.riskName(numericRisk));
      }
    }

    dataSet_->setPropertyDataType(te::dt::STRING_TYPE, pos);
  }

  // Replace comparison property
  auto posComparison = terrama2::core::propertyPosition(dataSet_.get(), "comparison_previous");

  if(posComparison == std::numeric_limits<size_t>::max())
  {
    QString errMsg = QObject::tr("Can't find property %1 !").arg(QString("comparison_previous"));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  dataSet_->moveBeforeFirst();

  while(dataSet_->moveNext())
  {
    std::string comp = "NULL";

    if(!dataSet_->isNull(posComparison))
    {
      int resComp = dataSet_->getInt32(posComparison);

      if(resComp == 0)
        comp = "SAME";
      else if(resComp == 1)
        comp = "INCREASED";
      else if(resComp == -1)
        comp = "DECREASED";
      else
        comp = "UNKNOW";
    }

    dataSet_->setString("comparison_previous" , comp);
  }

  dataSet_->setPropertyDataType(te::dt::STRING_TYPE, posComparison);

}


double terrama2::services::alert::core::Report::retrieveMaxValue() const
{
  dataSet_->moveBeforeFirst();

  if(alertDataSeries_->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for this data series type!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  if(!dataSet_->moveNext())
  {
    QString errMsg = QObject::tr("No data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::size_t pos = te::da::GetFirstPropertyPos(dataSet_.get(), te::dt::RASTER_TYPE);

  if(!terrama2::core::isValidColumn(pos) || dataSet_->isNull(pos))
  {
    QString errMsg = QObject::tr("No raster data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  try
  {
    int band = std::stoi(alert_->riskAttribute);

    return dataSet_->getRaster(pos)->getBand(band)->getMaxValue(true).real();
  }
  catch(const std::invalid_argument& /*e*/)
  {
    QString errMsg = QObject::tr("Invalid risk attribute!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }
}


double terrama2::services::alert::core::Report::retrieveMinValue() const
{
  dataSet_->moveBeforeFirst();

  if(alertDataSeries_->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for this data series type!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  if(!dataSet_->moveNext())
  {
    QString errMsg = QObject::tr("No data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::size_t pos = te::da::GetFirstPropertyPos(dataSet_.get(), te::dt::RASTER_TYPE);

  if(pos == std::string::npos)
  {
    QString errMsg = QObject::tr("No raster data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  try
  {
    int band = std::stoi(alert_->riskAttribute);

    return dataSet_->getRaster(pos)->getBand(band)->getMinValue(true).real();
  }
  catch(const std::invalid_argument& /*e*/)
  {
    QString errMsg = QObject::tr("Invalid risk attribute!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }
}


double terrama2::services::alert::core::Report::retrieveMeanValue() const
{
  dataSet_->moveBeforeFirst();

  if(alertDataSeries_->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for this data series type!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  if(!dataSet_->moveNext())
  {
    QString errMsg = QObject::tr("No data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::size_t pos = te::da::GetFirstPropertyPos(dataSet_.get(), te::dt::RASTER_TYPE);

  if(pos == std::string::npos)
  {
    QString errMsg = QObject::tr("No raster data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  try
  {
    int band = std::stoi(alert_->riskAttribute);

    return dataSet_->getRaster(pos)->getBand(band)->getMeanValue().real();
  }
  catch(const std::invalid_argument& /*e*/)
  {
    QString errMsg = QObject::tr("Invalid risk attribute!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }
}

terrama2::core::DataSeriesType terrama2::services::alert::core::Report::dataSeriesType() const
{
  return alertDataSeries_->semantics.dataSeriesType;
}

std::string terrama2::services::alert::core::Report::documentSavePath() const
{
  try
  {
    return alert_->reportMetadata.at("document_save_path");
  }
  catch(const std::out_of_range& /*e*/)
  {

  }

  return "";
}
