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

#include "StoragerFactory.hpp"
#include "ParserFactory.hpp"
#include "DataFilter.hpp"
#include "Storager.hpp"
#include "Parser.hpp"

#include "../core/DataSetItem.hpp"

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>

//std
#include <atomic>

struct terrama2::collector::DataProcessor::Impl
{
    core::DataSetItem      data_;
    DataFilterPtr          filter_;
    ParserPtr              parser_;
    StoragerPtr            storager_;
};

terrama2::collector::DataProcessor::DataProcessor(const core::DataSetItem& data, QObject *parent)
{
  impl_ = new Impl();
  impl_->data_ = data;

  initFilter();
  initParser();
  initStorager();
}

terrama2::collector::DataProcessor::~DataProcessor()
{
  delete impl_;
}

terrama2::core::DataSetItem terrama2::collector::DataProcessor::data() const
{
  return impl_->data_;
}

terrama2::collector::DataFilterPtr terrama2::collector::DataProcessor::filter() const
{
  return impl_->filter_;
}

void terrama2::collector::DataProcessor::import(const std::string &uri)
{
  assert(impl_->parser_);
  assert(impl_->filter_);
  assert(impl_->storager_);

  //get full name list
  std::vector<std::string> names = impl_->parser_->datasetNames(uri);
  //filter names
  names = impl_->filter_->filterNames(names);
  //get dataset
  std::vector<std::shared_ptr<te::da::DataSet> > datasetVec;
  std::shared_ptr<te::da::DataSetType> datasetType;
  impl_->parser_->read(uri, names, datasetVec, datasetType);

  //filter dataset
  for(int i = 0, size = datasetVec.size(); i < size; ++i)
  {
    std::shared_ptr<te::da::DataSet> tempDataSet = datasetVec.at(i);

    //std::vector::at is NON-const, Qt containers use 'at' as const
    datasetVec.at(i) = impl_->filter_->filterDataSet(tempDataSet);
  }

  //store dataset
  impl_->storager_->store(datasetVec, datasetType);

  //JANO: should run in thread ?
  //Call a thread method?
}

void terrama2::collector::DataProcessor::initFilter()
{
  DataFilterPtr newFilter(new DataFilter(impl_->data_));
  impl_->filter_ = newFilter;
}

void terrama2::collector::DataProcessor::initParser()
{
  ParserPtr parser = ParserFactory::getParser(impl_->data_.kind());
  impl_->parser_ = parser;
}

void terrama2::collector::DataProcessor::initStorager()
{
  StoragerPtr storager = StoragerFactory::getStorager(impl_->data_);
  impl_->storager_ = storager;
}
