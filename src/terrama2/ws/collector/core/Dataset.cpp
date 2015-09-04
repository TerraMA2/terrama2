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
  \file terrama2/ws/collector/core/Dataset.cpp

  \brief Dataset...

  \author Paulo R. M. Oliveira
*/

#include "Dataset.hpp"
#include "DataProvider.hpp"
#include "Data.hpp"

// QT
#include <QDebug>
#include <QList>
#include <QTimer>


terrama2::ws::collector::core::Dataset::Dataset(int id,
                                                std::shared_ptr<terrama2::ws::collector::core::DataProvider> dataProvider,
                                                QList<std::shared_ptr<terrama2::ws::collector::core::Data>> dataList):
  id_(id),
  timer_(nullptr),
  dataProvider_(dataProvider),
  dataList_(dataList)
{

  connect(&timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);
  timer_.start(5000);
}

terrama2::ws::collector::core::Dataset::~Dataset()
{
}

std::shared_ptr<terrama2::ws::collector::core::DataProvider> terrama2::ws::collector::core::Dataset::getDataProvider() const
{
  return dataProvider_;
}

QList<std::shared_ptr<terrama2::ws::collector::core::Data>> terrama2::ws::collector::core::Dataset::getDataList() const
{
  return dataList_;
}

void terrama2::ws::collector::core::Dataset::timeoutSlot()
{
  qDebug() << "Signal emited!";
  emit timerSignal(this);
}
