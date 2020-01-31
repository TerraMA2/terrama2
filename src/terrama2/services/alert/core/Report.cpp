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

// Qt
#include <QObject>
#include <QTemporaryDir>

// STD
#include <memory>

terrama2::services::alert::core::Report::Report(AlertPtr alert,
                                                terrama2::core::LegendPtr legend,
                                                terrama2::core::DataSeriesPtr alertDataSeries,
                                                std::shared_ptr<te::da::DataSet> alertDataSet,
                                                std::vector<std::shared_ptr<te::dt::TimeInstantTZ>> riskDates)
  : alert_(alert),
    legend_(legend),
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

  size_t compPos = terrama2::core::propertyPosition(dataSet_.get(), COMPARISON_PROPERTY_NAME);

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
    updateReportGridDataset(dataSet);
    return;
  }

  if(alertDataSeries_->semantics.dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
  {
    updateReportMonitoredObjectDataset(dataSet);
    return;
  }
}

void terrama2::services::alert::core::Report::updateReportGridDataset(const std::shared_ptr<te::da::DataSet> dataSet)
{
  dataSet_ = std::dynamic_pointer_cast<te::mem::DataSet>(dataSet);
  std::string riskName;
  std::tie(maxRisk_, riskName) = legend_->riskLevel(retrieveMaxValue());
  std::tie(minRisk_, riskName) = legend_->riskLevel(retrieveMinValue());
}

void terrama2::services::alert::core::Report::updateReportMonitoredObjectDataset(const std::shared_ptr<te::da::DataSet> dataSet)
{
  dataSet_ = std::make_shared<te::mem::DataSet>(*dataSet);

  // Replace risk values
  for(auto it = riskDates_.begin(); it != riskDates_.end(); ++it)
  {
    const auto& riskDate = *it;
    std::string property = dateTimeToString(riskDate);
    auto pos = terrama2::core::propertyPosition(dataSet_.get(), property);

    if(pos == std::numeric_limits<size_t>::max())
    {
      QString errMsg = QObject::tr("Can't find property %1 !").arg(QString::fromStdString(property));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ReportException() << ErrorDescription(errMsg);
    }

    dataSet_->moveBeforeFirst();
    //risk value to risk name
    while(dataSet_->moveNext())
    {
      if(!dataSet_->isNull(pos))
      {
        uint32_t numericRisk = static_cast<uint32_t>(dataSet_->getInt32(pos));
        dataSet_->setString(property, legend_->riskName(numericRisk));

        // if it's the default risk, don't update min and max
        if(numericRisk == terrama2::core::DefaultRiskLevel)
          continue;

        //update max and min risk values
        if(it == riskDates_.end()-1)
        {
          if(numericRisk > maxRisk_)
            maxRisk_ = numericRisk;

          if(numericRisk < minRisk_)
            minRisk_ = numericRisk;
        }
      }
    }

    dataSet_->setPropertyDataType(te::dt::STRING_TYPE, pos);
  }

  // Replace comparison property
  auto posComparison = terrama2::core::propertyPosition(dataSet_.get(), COMPARISON_PROPERTY_NAME);
  if(posComparison != std::numeric_limits<size_t>::max())
  {
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
        {
          riskChanged_ = true;
          comp = "INCREASED";
        }
        else if(resComp == -1)
        {
          riskChanged_= true;
          comp = "DECREASED";
        }
        else
          comp = "UNKNOW";
      }

      dataSet_->setString(COMPARISON_PROPERTY_NAME , comp);
    }

    dataSet_->setPropertyDataType(te::dt::STRING_TYPE, posComparison);
  }
}


double terrama2::services::alert::core::Report::retrieveMaxValue() const
{
  if(alertDataSeries_->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for this data series type!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  if(dataSet_->isEmpty())
  {
    QString errMsg = QObject::tr("No data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  dataSet_->moveFirst();
  std::size_t pos = te::da::GetFirstPropertyPos(dataSet_.get(), te::dt::RASTER_TYPE);
  if(!terrama2::core::isValidColumn(pos) || dataSet_->isNull(pos))
  {
    QString errMsg = QObject::tr("No raster data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  try
  {
    size_t band = static_cast<size_t>(std::stoi(alert_->riskAttribute));
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
  if(alertDataSeries_->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for this data series type!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  if(dataSet_->isEmpty())
  {
    QString errMsg = QObject::tr("No data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  dataSet_->moveFirst();
  std::size_t pos = te::da::GetFirstPropertyPos(dataSet_.get(), te::dt::RASTER_TYPE);

  if(pos == std::string::npos)
  {
    QString errMsg = QObject::tr("No raster data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  try
  {
    size_t band = static_cast<size_t>(std::stoi(alert_->riskAttribute));
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
  if(alertDataSeries_->semantics.dataSeriesType != terrama2::core::DataSeriesType::GRID)
  {
    QString errMsg = QObject::tr("Not implemented for this data series type!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  if(dataSet_->isEmpty())
  {
    QString errMsg = QObject::tr("No data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  dataSet_->moveFirst();
  std::size_t pos = te::da::GetFirstPropertyPos(dataSet_.get(), te::dt::RASTER_TYPE);

  if(pos == std::string::npos)
  {
    QString errMsg = QObject::tr("No raster data in data set!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  try
  {
    size_t band = static_cast<size_t>(std::stoi(alert_->riskAttribute));
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

te::core::URI terrama2::services::alert::core::Report::documentURI() const
{
  QTemporaryDir dir;
  if(!dir.isValid())
  {
    QString errMsg = QObject::tr("Temporary directory to store could not be created! ");
    throw NotifierException() << ErrorDescription(errMsg);
  }

  dir.setAutoRemove(false);

  std::string documentURI = dir.path().toStdString() + "/" + terrama2::core::simplifyString(title());
  fileRemover_.addTemporaryFolder(dir.path().toStdString());

  return te::core::URI( "file://"+ documentURI);
}

void terrama2::services::alert::core::Report::includeImage(const te::core::URI& imageUri)
{
  imageUri_ = imageUri;
}

const te::core::URI& terrama2::services::alert::core::Report::imageURI() const
{
  return imageUri_;
}
