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
  \file terrama2/unittest/collector/TsLog.cpp

  \brief Tests for the collector Log.

 \author Vinicius Campanha
*/

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/memory/DataSource.h>
#include <terralib/memory/Transactor.h>

// TerraMA2
#include "TsLog.hpp"
#include "terrama2/collector/Log.hpp"
#include "terrama2/collector/TransferenceData.hpp"
#include "terrama2/core/DataSet.hpp"


void TsLog::initTestCase()
{

}


void TsLog::cleanupTestCase()
{

}


void TsLog::init()
{

}


void TsLog::cleanup()
{

}


void TsLog::TestLog()
{
  // VINICIUS: waiting for TerraLib5 memory transactor implementation
  te::mem::DataSource ds;

  // getTransactor ir returning NULL
  terrama2::collector::Log log(ds.getTransactor());

  std::vector< terrama2::collector::TransferenceData > transferenceDataVec;

  terrama2::collector::TransferenceData transferenceData;

  transferenceData.dataSet = terrama2::core::DataSet();
  transferenceData.dataSetItem = terrama2::core::DataSetItem();
  transferenceData.uriOrigin = "";
  transferenceData.uriTemporary = "";
  transferenceData.uriStorage = "";

  boost::posix_time::ptime pt(boost::posix_time::time_from_string("2002-01-20 23:59:59.000"));
  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("MST-07"));
  boost::local_time::local_date_time time(pt, zone);

  transferenceData.dateData = std::make_shared< te::dt::TimeInstantTZ >(time);
  transferenceData.dateCollect = std::make_shared< te::dt::TimeInstantTZ >(time);

  std::shared_ptr< te::da::DataSet > dataSet;
  transferenceData.teDataSet = dataSet;
  transferenceData.teDataSetType = std::make_shared< te::da::DataSetType >(te::da::DataSetType("DataSetType"));

  transferenceDataVec.push_back(transferenceData);

  log.log(transferenceDataVec, terrama2::collector::Log::Status::NODATA);
}
