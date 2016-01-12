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
  \file terrama2/gui/config/Utils.cpp

  \brief Definition of Helpers for TerraMA2 GUI Configuration module.

  \author Raphael Willian da Costa

*/

// TerraMA2
#include "Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "../../core/DataSet.hpp"
#include "../../core/Logger.hpp"

// QT
#include <QString>


void terrama2::gui::config::saveDataSet(terrama2::core::DataSet& dataset, terrama2::core::DataSetItem& datasetItem, const uint64_t provider, ConfigApp* app, const QString& selectedName, const QString& inputValue, const QString& iconName)
{
  if (dataset.id() >= 1)
  {
    datasetItem.setDataSet(dataset.id());
    app->getClient()->updateDataSet(dataset);
    app->getWeatherTab()->refreshList(app->ui()->weatherDataTree->currentItem(),
                                      selectedName,
                                      inputValue);
    app->getWeatherTab()->setSelectedData(inputValue);
    TERRAMA2_LOG_INFO() << ("Dataset ID " + std::to_string(dataset.id()) + " updated!");
  }
  else
  {
    dataset.setProvider(provider);
    app->getClient()->addDataSet(dataset);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme(iconName));
    item->setText(0, inputValue);
    app->ui()->weatherDataTree->currentItem()->addChild(item);
    TERRAMA2_LOG_INFO() << "New Dataset " + dataset.name() + " saved!";
  }
  app->getWeatherTab()->addCachedDataSet(dataset);
  app->getWeatherTab()->setChanged(false);
}
