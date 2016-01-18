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
  \file terrama2/gui/config/ConfigAppWeatherOccurrence.cpp

  \brief Definition of terrama2::gui::config::ConfigAppWeatherOccurrence

  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigAppWeatherOccurrence.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "FilterDialog.hpp"
#include "IntersectionDialog.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "../core/Utils.hpp"
#include "../../core/Filter.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "../../core/Logger.hpp"

// TerraLib
#include <terralib/qt/widgets/srs/SRSManagerDialog.h>

// Qt
#include <QMessageBox>
#include <QUrl>

terrama2::gui::config::ConfigAppWeatherOccurrence::ConfigAppWeatherOccurrence(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui), filter_(new terrama2::core::Filter)
{
  connect(ui_->serverInsertPointDiffBtn, SIGNAL(clicked()), SLOT(onDataSetBtnClicked()));
  connect(ui_->serverRemovePointDiffBtn, SIGNAL(clicked()), SLOT(onRemoveOccurrenceBtnClicked()));
  connect(ui_->filterPointDiffBtn, SIGNAL(clicked()), SLOT(onFilterClicked()));
  connect(ui_->projectionPointDiffBtn, SIGNAL(clicked()), SLOT(onProjectionClicked()));
  connect(ui_->intersectionBtn, SIGNAL(clicked()), SLOT(onIntersectionBtnClicked()));
  connect(ui_->pointDiffFormatDataName, SIGNAL(textEdited(QString)), SLOT(onSubTabEdited()));


  ui_->updateDataPointDiffBtn->setEnabled(false);
  ui_->exportDataPointDiffBtn->setEnabled(false);

  ui_->pointDiffFormatDataHour->setValidator(new QIntValidator(ui_->pointDiffFormatDataHour));
  ui_->pointDiffFormatDataMinute->setValidator(new QIntValidator(ui_->pointDiffFormatDataMinute));
  ui_->pointDiffFormatDataSecond->setValidator(new QIntValidator(ui_->pointDiffFormatDataSecond));

  ui_->pointDiffFormatDataTimeZoneCmb->setCurrentText("+00:00");
  ui_->occurenceProjectionTxt->setText("0");
  srid_ = 0;
}

terrama2::gui::config::ConfigAppWeatherOccurrence::~ConfigAppWeatherOccurrence()
{

}

void terrama2::gui::config::ConfigAppWeatherOccurrence::load()
{
}

bool terrama2::gui::config::ConfigAppWeatherOccurrence::validate()
{
  if (ui_->pointDiffFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointDiffFormatDataName->setFocus();
    throw terrama2::gui::FieldException() << terrama2::ErrorDescription(tr("Occurence Name is invalid"));
  }

  checkMask(ui_->pointDiffFormatDataMask->text());

  //TODO: validate correctly all fields
  return true;
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  dataset.setName(ui_->pointDiffFormatDataName->text().toStdString());
  dataset.setKind(terrama2::core::DataSet::OCCURENCE_TYPE);
  dataset.setDescription(ui_->pointDiffFormatDataDescription->toPlainText().toStdString());

  te::dt::TimeDuration dataFrequency(ui_->pointDiffFormatDataHour->text().toInt(),
                                     ui_->pointDiffFormatDataMinute->text().toInt(),
                                     ui_->pointDiffFormatDataSecond->text().toInt());

  dataset.setDataFrequency(dataFrequency);

  std::map<std::string, std::string> metadata(dataset.metadata());
  metadata["UNIT"] = ui_->pointDiffFormatDataUnit->text().toStdString();
  metadata["KIND"] = ui_->pointDiffFormatDataFormat->currentText().toStdString();
  metadata["PREFIX"] = ui_->pointDiffFormatDataPrefix->text().toStdString();\
  dataset.setMetadata(metadata);

  dataset.setIntersection(intersection_);

  terrama2::core::DataSetItem datasetItem;
  if (dataset.dataSetItems().size() > 0)
  {
    datasetItem = dataset.dataSetItems()[dataset.dataSetItems().size() - 1];
    filter_->setDataSetItem(datasetItem.id());
  }

  auto itemMetadata = terrama2::gui::core::makeStorageMetadata(datasetItem.metadata(), provider.uri().c_str(), *app_->getConfiguration());

  datasetItem.setMetadata(itemMetadata);
  datasetItem.setSrid(srid_);

  terrama2::core::DataSetItem::Kind kind;
  int index = ui_->pointDiffFormatDataType->currentIndex();

  if (index == 0)
    kind = terrama2::core::DataSetItem::FIRE_POINTS_TYPE;
  else if (index == 1)
    kind = terrama2::core::DataSetItem::DISEASE_OCCURRENCE_TYPE;
  else
    kind = terrama2::core::DataSetItem::UNKNOWN_TYPE;

  datasetItem.setKind(kind);
  datasetItem.setPath(ui_->pointDiffFormatDataPath->text().toStdString());
  datasetItem.setMask(ui_->pointDiffFormatDataMask->text().toStdString());
  datasetItem.setTimezone(ui_->pointDiffFormatDataTimeZoneCmb->currentText().toStdString());
  datasetItem.setStatus(terrama2::core::ToDataSetItemStatus(ui_->pointDiffFormatStatus->isChecked()));

  datasetItem.setFilter(*filter_);

  if (datasetItem.id() == 0)
    dataset.add(datasetItem);

  dataset.setStatus(terrama2::core::DataSet::ACTIVE);

  auto storageMetadata = terrama2::gui::core::makeStorageMetadata(dataset, *app_->getConfiguration());

  datasetItem->setStorageMetadata(storageMetadata);

  // Lets save dataset, adding item in dataset
  terrama2::gui::config::saveDataSet(dataset, *datasetItem, provider.id(), app_, selectedData_, ui_->pointDiffFormatDataName->text(), "ocurrence-data");
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataPointDiffPage->findChildren<QLineEdit*>())
    widget->clear();
  changed_ = false;

  filter_.reset(new terrama2::core::Filter);

  ui_->pointDiffFormatDataDescription->setText("");
  ui_->pointDiffFormatDataTimeZoneCmb->setCurrentText("+00:00");

  resetFilterState();
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::fillFilter(const terrama2::core::Filter& filter)
{
  filter_.reset(new terrama2::core::Filter(filter));

  if (filter.discardBefore() != nullptr || filter.discardAfter() != nullptr)
    ui_->dateFilterPointDiffLabel->setText(tr("Yes"));
  else
    ui_->dateFilterPointDiffLabel->setText(tr("No"));

  if (filter.geometry() != nullptr)
    ui_->areaFilterPointDiffLabel->setText(tr("Yes"));
  else
    ui_->areaFilterPointDiffLabel->setText(tr("No"));
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::resetFilterState()
{
  ui_->dateFilterPointDiffLabel->setText(tr("No"));
  ui_->areaFilterPointDiffLabel->setText(tr("No"));
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::onFilterClicked()
{
  FilterDialog dialog(terrama2::gui::config::FilterDialog::DATE, ui_->pointDiffFormatDataTimeZoneCmb->currentText(), app_);
  dialog.fillGUI(*filter_);
  if (dialog.exec() == QDialog::Accepted)
    dialog.fillObject(*filter_);

  if (dialog.isFilterByDate())
    ui_->dateFilterPointDiffLabel->setText(tr("Yes"));
  else
    ui_->dateFilterPointDiffLabel->setText(tr("No"));

  if (dialog.isFilterByArea())
    ui_->areaFilterPointDiffLabel->setText(tr("Yes"));
  else
    ui_->areaFilterPointDiffLabel->setText(tr("No"));
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::onDataSetBtnClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
  {
    selectedData_.clear();
    app_->getWeatherTab()->changeTab(this, *ui_->DataPointDiffPage);

    intersection_ = terrama2::core::Intersection();
    filter_.reset(new terrama2::core::Filter);
    resetFilterState();
  }
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::onRemoveOccurrenceBtnClicked()
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem != nullptr && currentItem->parent() != nullptr && currentItem->parent()->parent() != nullptr)
  {
    try
    {
      terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(currentItem->text(0).toStdString());

      if (removeDataSet(dataset))
      {
        app_->getClient()->removeDataSet(dataset.id());
        TERRAMA2_LOG_INFO() << ("Dataset ID " + std::to_string(dataset.id()) + " has been removed!");
        app_->getWeatherTab()->removeCachedDataSet(dataset);

        QMessageBox::information(app_, tr("TerraMA2"), tr("DataSet Occurrence successfully removed!"));
        delete currentItem;
      }
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      QMessageBox::warning(app_, tr("TerraMA2"), *message);
      TERRAMA2_LOG_WARNING() << "Data Set Occurrence Removing: " << *message;
    }
  }
  ui_->cancelBtn->clicked();
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::onIntersectionBtnClicked()
{
  terrama2::gui::config::IntersectionDialog dialog(intersection_, app_->getConfiguration()->getDatabase());
  if(dialog.exec() == QDialog::Accepted)
  {
    intersection_ = dialog.getIntersection();
  }
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::onProjectionClicked()
{
  te::qt::widgets::SRSManagerDialog srsDialog(app_);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if (srsDialog.exec() == QDialog::Rejected)
    return;


  srid_ = (uint64_t) srsDialog.getSelectedSRS().first;

  ui_->occurenceProjectionTxt->setText(std::to_string(srid_).c_str());

}

void terrama2::gui::config::ConfigAppWeatherOccurrence::setIntersection(const terrama2::core::Intersection& intersection)
{
  intersection_ = intersection;
}

void terrama2::gui::config::ConfigAppWeatherOccurrence::setSrid(const uint64_t srid)
{
  srid_ = srid;
}
