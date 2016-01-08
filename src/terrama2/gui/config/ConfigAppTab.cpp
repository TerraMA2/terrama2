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
  \file terrama2/gui/config/ConfigAppTab.cpp

  \brief Definition of base class for tab handling.

  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigAppTab.hpp"
#include "ConfigApp.hpp"
#include "Exception.hpp"

// TerraMA2 Logger
#include "../../core/Logger.hpp"


#include "../../core/DataSet.hpp"

// QT
#include <QMessageBox>

terrama2::gui::config::ConfigAppTab::ConfigAppTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : QObject(app), app_(app), ui_(ui), active_(false), changed_(false)
{

}

terrama2::gui::config::ConfigAppTab::~ConfigAppTab()
{

}

void terrama2::gui::config::ConfigAppTab::validateAndSaveChanges()
{
  try
  {
    save();
  }
  catch(const terrama2::Exception& e)
  {
    const QString* msg = boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_ERROR() << *msg;
    QMessageBox::warning(app_, tr("TerraMA2 - Error"), *msg);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_FATAL() << e.what();
    QMessageBox::critical(app_, tr("TerraMA2 - Error"), e.what());
  }
}

void terrama2::gui::config::ConfigAppTab::askForChangeTab(const int& index)
{
  if (!dataChanged())
  {
    app_->setCurrentTabIndex(index);
    return;
  }

  // Used here to avoid change tab
  ui_->mainTabWidget->setCurrentIndex(app_->getCurrentTabIndex());

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(app_, tr("There are modifications not saved yet"),
                                      tr("Would you like to save them?"),
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                      QMessageBox::Save);

  bool saved = (reply == QMessageBox::Save);
  app_->setCurrentTabIndex(index);
  if (saved || reply == QMessageBox::Discard)
  {
    discardChanges(saved);

    ui_->mainTabWidget->setCurrentIndex(index);
  }
}

void terrama2::gui::config::ConfigAppTab::onSaveRequested()
{
  if(dataChanged())
    validateAndSaveChanges();
}

void terrama2::gui::config::ConfigAppTab::onCancelRequested()
{
  discardChanges(true);
}

void terrama2::gui::config::ConfigAppTab::onFilterClicked()
{

}

void terrama2::gui::config::ConfigAppTab::onProjectionClicked()
{

}

void terrama2::gui::config::ConfigAppTab::onSubTabEdited()
{
  changed_ = true;
}

bool terrama2::gui::config::ConfigAppTab::isActive() const
{
  return active_;
}

void terrama2::gui::config::ConfigAppTab::setActive(bool state)
{
  active_ = state;
}

bool terrama2::gui::config::ConfigAppTab::dataChanged()
{
  return active_ && changed_;
}

void terrama2::gui::config::ConfigAppTab::setChanged(bool state)
{
  changed_ = state;
}

void terrama2::gui::config::ConfigAppTab::setSelectedData(const QString selectedData)
{
  selectedData_ = selectedData;
}

bool terrama2::gui::config::ConfigAppTab::removeDataSet(const terrama2::core::DataSet& dataset)
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem != nullptr && currentItem->parent() != nullptr && currentItem->parent()->parent() != nullptr)
  {
    if (dataset.id() == 0)
      throw terrama2::gui::config::DataSetException() << terrama2::ErrorDescription(tr("Invalid dataset selected"));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(app_, tr("TerraMA2"),
                                  tr("Would you like to remove dataset?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                  QMessageBox::Yes);

    return reply == QMessageBox::Yes;
  }
  return false;
}

void terrama2::gui::config::ConfigAppTab::checkMask(const QString mask)
{
  if (mask.trimmed().isEmpty())
    throw terrama2::gui::FieldException() << terrama2::ErrorDescription(tr("Data mask is invalid."));

  if (mask.contains("%h") || mask.contains("%m") || mask.contains("%s"))
  {
    if ((!mask.contains("%a") && !mask.contains("%A")) || !mask.contains("%M") || !mask.contains("%d"))
      throw terrama2::gui::ValueException() << terrama2::ErrorDescription(tr("An mask with time must have date typed"));
  }
}
