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
  \file terrama2/gui/config/ConfigAppWeatherGrid.cpp

  \brief Definition of terrama2::gui::config::ConfigAppWeatherGrid

  \author Raphael Willian da Costa
*/


// TerraMA2
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "../core/Utils.hpp"
#include "../../core/DataSet.hpp"
#include "../../core/Filter.hpp"
#include "../../core/DataSetItem.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/Utils.hpp"
#include "FilterDialog.hpp"

// STL
#include <inttypes.h>
#include "../../core/Logger.hpp"

// Terralib
#include <terralib/datatype/TimeInstant.h>
#include <terralib/qt/widgets/srs/SRSManagerDialog.h>

// QT
#include <QMessageBox>
#include <QUrl>


terrama2::gui::config::ConfigAppWeatherGridTab::ConfigAppWeatherGridTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui), srid_(0), filter_(new terrama2::core::Filter)
{
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridClicked()));
  connect(ui_->filterGridBtn, SIGNAL(clicked()), SLOT(onFilterClicked()));
  connect(ui_->gridFormatDataName, SIGNAL(textEdited(QString)), SLOT(onSubTabEdited()));
  connect(ui_->gridFormatDataFormat, SIGNAL(currentIndexChanged(const QString&)), SLOT(onGridFormatChanged()));
  connect(ui_->gridFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onRemoveDataGridBtnClicked()));
  connect(ui_->projectionGridBtn, SIGNAL(clicked()), this, SLOT(onProjectionClicked()));

  // data frequency int validator
  ui_->gridFormatDataHour->setValidator(new QIntValidator(ui_->gridFormatDataHour));
  ui_->gridFormatDataMinute->setValidator(new QIntValidator(ui_->gridFormatDataMinute));
  ui_->gridFormatDataSecond->setValidator(new QIntValidator(ui_->gridFormatDataSecond));

  ui_->gridFormatDataTimeZoneCmb->setCurrentText("+00:00");

}

terrama2::gui::config::ConfigAppWeatherGridTab::~ConfigAppWeatherGridTab()
{
  delete filter_;
}

void terrama2::gui::config::ConfigAppWeatherGridTab::load()
{
  auto menuMask = terrama2::gui::core::makeMaskHelpers();

  ui_->fileGridMaskBtn->setMenu(menuMask);
  ui_->fileGridMaskBtn->setPopupMode(QToolButton::InstantPopup);

  // connecting the menumask to display mask field values
  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(onMenuMaskClicked(QAction*)));
}

void terrama2::gui::config::ConfigAppWeatherGridTab::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  std::string name = ui_->gridFormatDataName->text().toStdString();
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  terrama2::core::DataSet::Kind kind = terrama2::core::DataSet::GRID_TYPE;

  dataset.setName(name);
  dataset.setKind(kind);
  dataset.setDescription(ui_->gridFormatDataDescription->toPlainText().toStdString());
  dataset.setStatus(terrama2::core::ToDataSetStatus(ui_->gridFormatStatus->isChecked()));

  terrama2::core::DataSetItem* datasetItem;

  if (dataset.dataSetItems().size() > 0)
  {
    datasetItem = &dataset.dataSetItems()[dataset.dataSetItems().size() - 1];
    filter_->setDataSetItem(datasetItem->id());
  }
  else
    datasetItem = new terrama2::core::DataSetItem;

  datasetItem->setFilter(*filter_);
  datasetItem->setSrid(srid_);

  datasetItem->setKind(terrama2::core::DataSetItem::GRID_TYPE);
  datasetItem->setMask(ui_->gridFormatDataMask->text().toStdString());
  datasetItem->setStatus(terrama2::core::DataSetItem::ACTIVE);
  datasetItem->setTimezone(ui_->gridFormatDataTimeZoneCmb->currentText().toStdString());
  datasetItem->setPath(ui_->gridFormatDataPath->text().toStdString());

  te::dt::TimeDuration dataFrequency(ui_->gridFormatDataHour->text().toInt(),
                                     ui_->gridFormatDataMinute->text().toInt(),
                                     ui_->gridFormatDataSecond->text().toInt());

  te::dt::TimeDuration schedule(0, ui_->gridFormatDataInterval->value(), 0);
  dataset.setDataFrequency(dataFrequency);

  // todo: define a common key pattern to metadata
  std::map<std::string, std::string> metadata;
  metadata["KIND"] = ui_->gridFormatDataFormat->currentText().toStdString();
  metadata["PREFIX"] = ui_->gridFormatDataPrefix->text().toStdString();
  metadata["UNIT"] = ui_->gridFormatDataUnit->text().toStdString();
  metadata["RESOLUTION"] = ui_->gridFormatDataResolution->text().toStdString();

  switch (ui_->gridFormatDataFormat->currentIndex())
  {
    case 0:
      {
        if (ui_->rbGridAscUnidGrausDec->isChecked())
          metadata["COORDINATES_UNIT"] = tr("Decimal Degree").toStdString();
        else if (ui_->rbGridAscUnidGrausMil->isChecked())
          metadata["COORDINATES_UNIT"] = tr("Thousandth Degree").toStdString();
      }
      break;
    case 1:
      metadata["NAVIGATION_FILE"] = ui_->ledGridTIFFArqNavegacao->text().toStdString();
      break;
    case 2:
      {
        metadata["CONTROL_FILE"] = ui_->ledGridGrADSArqControle->text().toStdString();
        metadata["MULTIPLICATOR"] = ui_->ledGridGrADSMultiplicador->text().toStdString();

        if (ui_->rbGridGrADSTipoDadosInt->isChecked())
          metadata["DATA_KIND"] = "Integer";
        else if (ui_->rbGridGrADSTipoDadosFloat->isChecked())
          metadata["DATA_KIND"] = "Float";

        metadata["ORDER"] = ui_->cmbGridGrADSByteOrder->currentText().toStdString();
        metadata["BANDS"] = ui_->spbGridGrADSNumBands->text().toStdString();
        metadata["BAND_PREFIX"] = ui_->spbGridGrADSHeaderSize->text().toStdString();
        metadata["TIME_OFFSET"] = ui_->spbGridGrADSTimeOffset->text().toStdString();
        metadata["POSFIX"] = ui_->spbGridGrADSTraillerSize->text().toStdString();
        metadata["DUMMY"] = ui_->ledGridWCSDummy->text().toStdString();
      }
      break;
    default:
      ;
  }

  metadata["FORMAT"] = ui_->gridFormatDataFormat->currentText().toStdString();

  dataset.setMetadata(metadata);

  auto storageMetadata = terrama2::gui::core::makeStorageMetadata(provider.uri().c_str(), *app_->getConfiguration());

  datasetItem->setStorageMetadata(storageMetadata);

  if (datasetItem->id() == 0)
    dataset.add(*datasetItem);

  dataset.setSchedule(schedule);

  // Lets save dataset
  terrama2::gui::config::saveDataSet(dataset, *datasetItem, provider.id(), app_, selectedData_, ui_->gridFormatDataName->text(), "grid");
}

void terrama2::gui::config::ConfigAppWeatherGridTab::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataGridPage->findChildren<QLineEdit*>())
    widget->clear();

  ui_->gridFormatDataDescription->setText("");
  ui_->gridFormatDataTimeZoneCmb->setCurrentText("+00:00");

  changed_ = false;
  ui_->dataSeriesBtnGroupBox->setVisible(false);
  ui_->updateDataGridBtn->setVisible(false);
  ui_->exportDataGridBtn->setVisible(false);
  ui_->gridFormatDataDeleteBtn->setVisible(false);
  app_->getWeatherTab()->showDataSeries(true);

  for(QLineEdit* widget: ui_->stkwGridTipoFormato->findChildren<QLineEdit*>())
    widget->clear();

  ui_->gridFormatDataFormat->setCurrentIndex(0);
  ui_->rbGridAscUnidGrausDec->setAutoExclusive(false);
  ui_->rbGridAscUnidGrausDec->setChecked(false);
  ui_->rbGridAscUnidGrausDec->setAutoExclusive(true);

  ui_->rbGridAscUnidGrausMil->setAutoExclusive(false);
  ui_->rbGridAscUnidGrausMil->setChecked(false);
  ui_->rbGridAscUnidGrausMil->setAutoExclusive(true);

  ui_->rbGridGrADSTipoDadosFloat->setAutoExclusive(false);
  ui_->rbGridGrADSTipoDadosFloat->setChecked(false);
  ui_->rbGridGrADSTipoDadosFloat->setAutoExclusive(true);

  ui_->rbGridGrADSTipoDadosInt->setAutoExclusive(false);
  ui_->rbGridGrADSTipoDadosInt->setChecked(false);
  ui_->rbGridGrADSTipoDadosInt->setAutoExclusive(true);

  ui_->cmbGridGrADSByteOrder->setCurrentIndex(0);
}

void terrama2::gui::config::ConfigAppWeatherGridTab::fillFilter(const terrama2::core::Filter& filter)
{
  *filter_ = filter;
  fillFilterLabels();
}

bool terrama2::gui::config::ConfigAppWeatherGridTab::validate()
{
  if (ui_->gridFormatDataName->text().trimmed().isEmpty())
  {
    ui_->gridFormatDataName->setFocus();
    throw terrama2::gui::FieldException() << terrama2::ErrorDescription(tr("The Data Set Item name cannot be empty."));
  }

  checkMask(ui_->gridFormatDataMask->text());

  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(ui_->gridFormatDataName->text().toStdString());

  if (dataset.id() != 0 && !selectedData_.isEmpty())
  {
    if (ui_->gridFormatDataName->text() != selectedData_)
    {
      ui_->gridFormatDataName->setFocus();
      throw terrama2::gui::FieldException() << terrama2::ErrorDescription(
          tr("The data set grid name has already been saved. Please change server name"));
    }
  }

  // TODO: Complete validation with another fields and Projection and Filter validation
  return true;
}

void terrama2::gui::config::ConfigAppWeatherGridTab::onDataGridClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
  {
    selectedData_.clear();
    app_->getWeatherTab()->changeTab(this, *ui_->DataGridPage);

    ui_->dateFilterLabel->setText(tr("No"));
    ui_->areaFilterLabel->setText(tr("No"));
    ui_->bandFilterLabel->setText(tr("No"));
    ui_->preAnalysisLabel->setText(tr("No"));
    if (filter_ != nullptr)
      delete filter_;

    filter_ = new terrama2::core::Filter;
    ui_->gridProjectionTxt->setText("0");

  }
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void terrama2::gui::config::ConfigAppWeatherGridTab::onGridFormatChanged()
{
  switch (ui_->gridFormatDataFormat->currentIndex())
  {
    case 0:
      ui_->stkwGridTipoFormato->setCurrentWidget(ui_->pgGridAscGRID);
      break;
    case 1:
      ui_->stkwGridTipoFormato->setCurrentWidget(ui_->pgGridTIFF);
      break;
    case 2:
      ui_->stkwGridTipoFormato->setCurrentWidget(ui_->pgGridGrADS);
      break;
    default:
      ;
  }

  ui_->gridProjectionTxt->setText("0");
}

void terrama2::gui::config::ConfigAppWeatherGridTab::onRemoveDataGridBtnClicked()
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

        QMessageBox::information(app_, tr("TerraMA2"), tr("DataSet Grid successfully removed!"));
        delete currentItem;
      }
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      TERRAMA2_LOG_ERROR() << "DataSet Removing: " << *message;
      QMessageBox::warning(app_, tr("TerraMA2"), *message);
    }
  }
  ui_->cancelBtn->clicked();
}

void terrama2::gui::config::ConfigAppWeatherGridTab::onMenuMaskClicked(QAction* action)
{
  ui_->gridFormatDataMask->setText(
        ui_->gridFormatDataMask->text() + action->text().left(2));
}

void terrama2::gui::config::ConfigAppWeatherGridTab::onFilterClicked()
{
  FilterDialog dialog(terrama2::gui::config::FilterDialog::FULL, ui_->gridFormatDataTimeZoneCmb->currentText(), app_);

  dialog.fillGUI(*filter_);

  if (dialog.exec() == QDialog::Accepted)
    dialog.fillObject(*filter_);

  fillFilterLabels();
}

void terrama2::gui::config::ConfigAppWeatherGridTab::onProjectionClicked()
{
  te::qt::widgets::SRSManagerDialog srsDialog(app_);
  srsDialog.setWindowTitle(tr("Choose the SRS"));

  if (srsDialog.exec() == QDialog::Rejected)
    return;


  srid_ = (uint64_t) srsDialog.getSelectedSRS().first;
  ui_->gridProjectionTxt->setText(std::to_string(srid_).c_str());

}

void terrama2::gui::config::ConfigAppWeatherGridTab::setSrid(const uint64_t srid)
{
  srid_ = srid;
}

void terrama2::gui::config::ConfigAppWeatherGridTab::fillFilterLabels()
{
  if (filter_->discardAfter() != nullptr || filter_->discardBefore() != nullptr)
    ui_->dateFilterLabel->setText(tr("Yes"));
  else
    ui_->dateFilterLabel->setText(tr("No"));

  if (filter_->geometry() != nullptr)
    ui_->areaFilterLabel->setText(tr("Yes"));
  else
    ui_->areaFilterLabel->setText(tr("No"));

  if (!filter_->bandFilter().empty())
    ui_->bandFilterLabel->setText(tr("Yes"));
  else
    ui_->bandFilterLabel->setText(tr("No"));

  if (filter_->value() != nullptr)
    ui_->preAnalysisLabel->setText(tr("Yes"));
  else
    ui_->preAnalysisLabel->setText(tr("No"));
}
