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
  \file terrama2/core/data-access/DataAccessorDcpInpe.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataAccessorDcpInpe.hpp"
#include "../core/data-access/DataRetriever.hpp"

std::string terrama2::core::DataAccessorDcpInpe::retrieveData(const DataRetrieverPtr dataRetriever, const DataSetDcp& dataset, const Filter& filter)
{
  std::string mask = getMask(dataset);
  return dataRetriever->retrieveData(mask, filter);
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::getMask(const DataSetDcp& dataset) const
{
  return dataset.format.at("mask");
}

te::gm::Point terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::getPosition(const DataSetDcp& dataset) const
{
  std::string wkt = dataset.format.at("position");
  te::gm::Point position; //from WKT;

  return position;
}

std::vector<std::string> terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::validDataSetNames(const DataSetDcp& dataSetDcp ) const
{
  //FIXME: implement
  //filterNames(datasetDcp, filter, transactor->getDataSetNames()
  return std::vector<std::string>();
}

std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::dataSourceTye() const
{
  return "OGR";
}
std::string terrama2::core::DataAccessorDcpInpe::DataAccessorDcpInpe::typePrefix() const
{
  return "CSV:";
}
