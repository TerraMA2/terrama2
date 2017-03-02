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

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataChangedRisk() const
{
  std::vector<std::size_t> positions;

  for(std::size_t i = 0; i < alertDataSet_->size(); i++)
  {
    if(alertDataSet_->getInt32("comparison_previous") != 0)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::mem::DataSet> filteredDataSet  = std::make_shared<te::mem::DataSet>(alertDataSetType_.get());

  te::da::FilteredDataSet ds(alertDataSet_.get(), positions);

  filteredDataSet->copy(ds);

  addLevelsNamesProperty(filteredDataSet);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataUnchangedRisk() const
{
  return alertDataSet_;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataIncreasedRisk() const
{
  return alertDataSet_;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataDecreasedRisk() const
{
  return alertDataSet_;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataAtRisk(const int risk) const
{
  std::vector<std::size_t> positions;

  for(std::size_t i = 0; i < alertDataSet_->size(); i++)
  {
    if(alertDataSet_->getInt32("risk_level") == risk)
    {
      positions.push_back(i);
    }
  }

  std::shared_ptr<te::mem::DataSet> filteredDataSet  = std::make_shared<te::mem::DataSet>(alertDataSetType_.get());

  te::da::FilteredDataSet ds(alertDataSet_.get(), positions);

  filteredDataSet->copy(ds);

  addLevelsNamesProperty(filteredDataSet);

  return  filteredDataSet;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataAboveRisk(const int risk) const
{
  return alertDataSet_;
}

std::shared_ptr<te::da::DataSet> terrama2::services::alert::core::Report::retrieveDataBelowRisk(const int risk) const
{
  return alertDataSet_;
}


void terrama2::services::alert::core::Report::addLevelsNamesProperty(std::shared_ptr<te::mem::DataSet> dataSet) const
{
  // TODO: get risk properties from alert
  std::vector<std::string> riskProperties;
  riskProperties.push_back("risk_level");
  riskProperties.push_back("risk_level_2");
  riskProperties.push_back("risk_level_3");


  // add risk name properties
  for(const auto& riskProperty : riskProperties)
  {
    dataSet->add(riskProperty + "_name", te::dt::STRING_TYPE);
  }

  // compare and add risk name values
  while(dataSet->moveNext())
  {
    for(std::size_t i = 0; dataSet->getNumProperties(); i++)
    {
      auto pos = std::find(riskProperties.begin(),riskProperties.end(), dataSet->getPropertyName(i));

      if(pos != riskProperties.end())
      {
        double numericRisk = dataSet->getDouble(i);
        dataSet->setString(*pos + "_name", std::get<1>(alert_->risk.riskLevel(numericRisk)));
      }
    }
  }

  // remove numeric risk properties
  for(const auto& riskProperty : riskProperties)
  {
    auto pos = terrama2::core::propertyPosition(dataSet.get(), riskProperty);

    if(pos == std::numeric_limits<size_t>::max())
    {
      QString errMsg = QObject::tr("Can't find property %1").arg(QString::fromStdString(riskProperty));
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ReportException() << ErrorDescription(errMsg);
    }

    dataSet->drop(pos);
  }
}
