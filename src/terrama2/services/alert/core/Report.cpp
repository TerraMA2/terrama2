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
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"

// TerraLib

// Qt
#include <QObject>


std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveData() const
{
  return alertDataSet_;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataComparisonValue(const std::vector<int>& values) const
{
  std::vector<std::size_t> positions;

  for(std::size_t i = 0; i < alertDataSet_->size(); i++)
  {
    alertDataSet_->move(i);

    int comp = alertDataSet_->getInt32("comparison_previous");

    auto it = std::find(values.begin(), values.end(), comp);

    if(it != values.end())
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::mem::DataSet> filteredDataSet  = std::make_shared<te::mem::DataSet>(alertDataSetType_.get());

  te::da::FilteredDataSet ds(alertDataSet_.get(), positions);

  filteredDataSet->copy(ds);

  replaceNumberByDescription(filteredDataSet);

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
  std::vector<std::size_t> positions;

  std::string property = terrama2::core::validPropertyName(riskDates_.at(0)->toString());

  for(std::size_t i = 0; i < alertDataSet_->size(); i++)
  {
    alertDataSet_->move(i);
    // TODO: get risk properties from alert
    if(alertDataSet_->getInt32(property) == risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::mem::DataSet> filteredDataSet  = std::make_shared<te::mem::DataSet>(alertDataSetType_.get());

  te::da::FilteredDataSet ds(alertDataSet_.get(), positions);

  filteredDataSet->copy(ds);

  replaceNumberByDescription(filteredDataSet);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataAboveRisk(const int risk) const
{
  std::vector<std::size_t> positions;

  std::string property = terrama2::core::validPropertyName(riskDates_.at(0)->toString());

  for(std::size_t i = 0; i < alertDataSet_->size(); i++)
  {
    alertDataSet_->move(i);
    // TODO: get risk properties from alert
    if(alertDataSet_->getInt32(property) >= risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::mem::DataSet> filteredDataSet  = std::make_shared<te::mem::DataSet>(alertDataSetType_.get());

  te::da::FilteredDataSet ds(alertDataSet_.get(), positions);

  filteredDataSet->copy(ds);

  replaceNumberByDescription(filteredDataSet);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataBelowRisk(const int risk) const
{
  std::vector<std::size_t> positions;

  std::string property = terrama2::core::validPropertyName(riskDates_.at(0)->toString());

  for(std::size_t i = 0; i < alertDataSet_->size(); i++)
  {
    alertDataSet_->move(i);
    // TODO: get risk properties from alert
    if(alertDataSet_->getInt32(property) <= risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::mem::DataSet> filteredDataSet  = std::make_shared<te::mem::DataSet>(alertDataSetType_.get());

  te::da::FilteredDataSet ds(alertDataSet_.get(), positions);

  filteredDataSet->copy(ds);

  replaceNumberByDescription(filteredDataSet);

  return  filteredDataSet;
}


void terrama2::services::alert::core::Report::replaceNumberByDescription(std::shared_ptr<te::mem::DataSet> dataSet) const
{
  // Replace risk values
  for(auto riskDate : riskDates_)
  {
    std::string property = terrama2::core::validPropertyName(riskDate->toString());
    auto pos = terrama2::core::propertyPosition(dataSet.get(), property);

    if(pos == std::numeric_limits<size_t>::max())
    {
      QString errMsg = QObject::tr("Can't find property %1 !").arg(QString::fromStdString(property));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ReportException() << ErrorDescription(errMsg);
    }

    dataSet->moveBeforeFirst();

    while(dataSet->moveNext())
    {
      if(!dataSet->isNull(pos))
      {
        int numericRisk = dataSet->getInt32(pos);
        dataSet->setString(property, alert_->risk.riskName(numericRisk));
      }
    }

    dataSet->setPropertyDataType(te::dt::STRING_TYPE, pos);
  }

  // Replace comparison property
  auto posComparison = terrama2::core::propertyPosition(dataSet.get(), "comparison_previous");

  if(posComparison == std::numeric_limits<size_t>::max())
  {
    QString errMsg = QObject::tr("Can't find property %1 !").arg(QString("comparison_previous"));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  dataSet->moveBeforeFirst();

  while(dataSet->moveNext())
  {
    std::string comp = "NULL";

    if(!dataSet->isNull(posComparison))
    {
      int resComp = dataSet->getInt32(posComparison);

      if(resComp == 0)
        comp = "SAME";
      else if(resComp == 1)
        comp = "INCREASED";
      else if(resComp == -1)
        comp = "DECREASED";
      else
        comp = "UNKNOW";
    }

    dataSet->setString("comparison_previous" , comp);
  }

  dataSet->setPropertyDataType(te::dt::STRING_TYPE, posComparison);
}
