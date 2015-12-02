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
  \file terrama2/collector/Filter.cpp

  \brief Filters data.

  \author Jano Simas
*/

// TerraMA2
#include "../core/DataSetItem.hpp"
#include "../core/Filter.hpp"
#include "DataFilter.hpp"

// STL
#include <iostream>
#include <iterator>
#include <string>

// BOOST
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

//terralib
#include <terralib/memory/DataSetItem.h>
#include <terralib/memory/DataSet.h>
#include <terralib/datatype/Enums.h>

std::vector<std::string> terrama2::collector::DataFilter::filterNames(const std::vector<std::string>& namesList) const
{
  std::string mask = datasetItem_.mask();

  std::vector<std::string> matchNames;
  for(const std::string &name : namesList)
  {
    // match regex
    boost::replace_all(mask, "%A", "(\\d{4})"); //("%A - ano com quatro digitos"))
    boost::replace_all(mask, "%a", "(\\d{2})"); //("%a - ano com dois digitos"))
    boost::replace_all(mask, "%d", "(\\d{2})"); //("%d - dia com dois digitos"))
    boost::replace_all(mask, "%M", "(\\d{2})"); //("%M - mes com dois digitos"))
    boost::replace_all(mask, "%h", "(\\d{2})"); //("%h - hora com dois digitos"))
    boost::replace_all(mask, "%m", "(\\d{2})"); //("%m - minuto com dois digitos"))
    boost::replace_all(mask, "%s", "(\\d{2})"); //("%s - segundo com dois digitos"))
    boost::replace_all(mask, "%.", "(\\w{1})");  //("%. - um caracter qualquer"))

    boost::regex regex(mask);

    bool res = boost::regex_match(name,regex);
    if (res)
      matchNames.push_back(name);

  }

  return matchNames;
}

std::shared_ptr<te::da::DataSet> terrama2::collector::DataFilter::filterDataSet(const std::shared_ptr<te::da::DataSet> &dataSet, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  //Find DateTime column
  int dateColumn = -1;
  for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
  {
    if( dataSet->getPropertyDataType(i) == te::dt::DATETIME_TYPE)
    {
      dateColumn = i;
      break;
    }
  }

  //Find Geometry column
  int geomColumn = -1;
  for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
  {
    if( dataSet->getPropertyDataType(i) == te::dt::GEOMETRY_TYPE)
    {
      geomColumn = i;
      break;
    }
  }

  //If there is no DateTime or geometry column, nothing to be done
  if(dateColumn < 0 && geomColumn < 0)
    return dataSet;

  //Copy dataset to an in-memory dataset filtering the data
  const core::Filter& filter = datasetItem_.filter();
  auto memDataSet = std::make_shared<te::mem::DataSet>(datasetType.get());
  while(dataSet->moveNext())
  {
    //Filter Time
    if(dateColumn > 0)
    {
      std::unique_ptr<te::dt::DateTime> dateTime(dataSet->getDateTime(dateColumn));
      if(filter.discardBefore() && *dateTime < *filter.discardBefore())
        continue;

      if(filter.discardAfter() && *dateTime > *filter.discardAfter())
        continue;

      //TODO: filter last collection time
    }

    te::mem::DataSetItem* dataItem = new te::mem::DataSetItem(dataSet.get());
    //Copy each property
    for(uint i = 0, size = dataSet->getNumProperties(); i < size; ++i)
      dataItem->setValue(i, dataSet->getValue(i).release());
    //add item to the new dataset
    memDataSet->add(dataItem);
  }

  //TODO: Implement filter geometry
  return memDataSet;
}

terrama2::collector::DataFilter::DataFilter(const core::DataSetItem& datasetItem)
  : datasetItem_(datasetItem)
{

}

terrama2::collector::DataFilter::~DataFilter()
{

}
