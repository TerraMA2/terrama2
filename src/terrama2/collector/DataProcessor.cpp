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
  \file terrama2/collector/DataProcessor.cpp

  \brief Manages the processing, filtering and storaging of aquired data.

  \author Jano Simas
*/

#include "DataProcessor.hpp"

#include "Filter.hpp"
#include "Parser.hpp"
#include "Storager.hpp"

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>

struct terrama2::collector::DataProcessor::Impl
{
    FilterPtr filter_;
    ParserPtr parser_;
    StoragerPtr storager_;
};

terrama2::collector::DataProcessor::DataProcessor(QObject *parent)
{
  impl_ = new Impl();
}

terrama2::collector::DataProcessor::~DataProcessor()
{
  delete impl_;
}

terrama2::core::DataPtr terrama2::collector::DataProcessor::data() const
{
  //JANO: implementar data()
}

terrama2::collector::FilterPtr terrama2::collector::DataProcessor::filter() const
{
  return impl_->filter_;
}

void terrama2::collector::DataProcessor::import(const std::string &uri)
{
  te::da::DataSetPtr tempDataSet = impl_->parser_->read(uri);
  tempDataSet = impl_->filter_->filterDataSet(tempDataSet);
  te::da::DataSetPtr storedDataSet = impl_->storager_->store(tempDataSet);

  //JANO: implementar import
  //should run in thread ?
  //Call a thread method?
}
