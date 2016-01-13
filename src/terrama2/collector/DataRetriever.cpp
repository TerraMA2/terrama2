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
  \file terrama2/collector/DataRetriever.cpp

  \brief Interface for getting remote data to a local temporary file.

  \author Jano Simas
*/

// LibCurl
#include <curl/curl.h>
#include "CurlOpener.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// QT
#include <QTranslator>

// TerraMA2
#include "DataFilter.hpp"
#include "DataRetriever.hpp"
#include "Exception.hpp"


terrama2::collector::DataRetriever::DataRetriever(const terrama2::core::DataProvider& dataprovider)
{
//TODO: review exception: should check valid dataprovider?
  dataprovider_ = dataprovider;
}

bool terrama2::collector::DataRetriever::isRetrivable() const noexcept
{
  return false;
}

size_t data_vector(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t sizeRead = size * nmemb;

  std::string* block = (std::string*) data;
  block->append((char *)ptr, sizeRead);

  return sizeRead;
}

std::string terrama2::collector::DataRetriever::retrieveData(const terrama2::core::DataSetItem& datasetitem, DataFilterPtr filter, std::vector<TransferenceData>& TransferenceDataVec)
{
  return "";
}

void terrama2::collector::DataRetriever::open()
{

}

bool terrama2::collector::DataRetriever::isOpen()
{
  return true;
}

void terrama2::collector::DataRetriever::close()
{

}
