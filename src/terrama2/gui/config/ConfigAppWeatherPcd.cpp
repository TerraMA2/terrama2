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
  \file terrama2/gui/config/ConfigAppWeatherPcd.cpp

  \brief Definition of terrama2::gui::config::ConfigAppWeatherPcd

  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigAppWeatherPcd.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "PcdDialog.hpp"
#include "SurfaceDialog.hpp"
#include "CollectorRuleDialog.hpp"
#include "PcdWfsDialog.hpp"
#include "../core/Utils.hpp"
#include "../../core/Logger.hpp"


// TerraLib
#include <terralib/qt/widgets/srs/SRSManagerDialog.h>

// Qt
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QUrl>
#include <QDoubleValidator>
#include <QIntValidator>

terrama2::gui::config::ConfigAppWeatherPcd::ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->pointFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onDataPointBtnClicked()));

  connect(ui_->btnPointPCDInsertFileNameLocation, SIGNAL(clicked()), SLOT(onPCDInsertFileClicked()));
  connect(ui_->btnPointPCDDeleteFileNameLocation, SIGNAL(clicked()), SLOT(onPCDRemoveClicked()));
  connect(ui_->tblPointPCDFileNameLocation, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(onPCDTableDoubleClicked(QTableWidgetItem*)));
  connect(ui_->pointFormatSurfaceConfigBtn, SIGNAL(clicked()), SLOT(onSurfaceBtnClicked()));
  connect(ui_->btnUpdatePcdCollectionRule, SIGNAL(clicked()), SLOT(onCollectorRuleClicked()));
  connect(ui_->btnPCDWFSConfiguration, SIGNAL(clicked(bool)), SLOT(onPcdWfsClicked()));
  connect(ui_->pointFormatDataInfluenceCmb, SIGNAL(activated(int)), SLOT(onInfluenceTypeSelected(int)));



  // export pcd button
  connect(ui_->exportDataPointBtn, SIGNAL(clicked()), SLOT(onPCDExportClicked()));

  // todo: implement it to list pcd attributes after collection
  ui_->updateDataPointBtn->setEnabled(false);

  ui_->pointFormatInfluenceDatasetCmb->setEnabled(true);
  ui_->pointFormatDataAttributeCmb->setEnabled(true);

  ui_->pointFormatDataPrefix->setEnabled(true);
  ui_->pointFormatDataUnit->setEnabled(true);

  ui_->btnPCDWFSConfiguration->setEnabled(true);
  ui_->btnPCDInformationPlane->setEnabled(true);

  tableClean();

  auto menuMask = terrama2::gui::core::makeMaskHelpers();

  ui_->filePointDiffMaskBtn->setMenu(menuMask);
  ui_->filePointDiffMaskBtn->setPopupMode(QToolButton::InstantPopup);

  // connecting the menumask to display mask field values
  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(onMenuMaskClicked(QAction*)));

  ui_->pointFormatDataHour->setValidator(new QIntValidator(ui_->pointFormatDataHour));
  ui_->pointFormatDataMinute->setValidator(new QIntValidator(ui_->pointFormatDataMinute));
  ui_->pointFormatDataSecond->setValidator(new QIntValidator(ui_->pointFormatDataSecond));
  ui_->pointFormatDataRadius->setValidator(new QDoubleValidator(ui_->pointFormatDataRadius));
}

terrama2::gui::config::ConfigAppWeatherPcd::~ConfigAppWeatherPcd()
{

}

void terrama2::gui::config::ConfigAppWeatherPcd::load()
{
  fillAdditionalMapList();
  tableClean();
}

void terrama2::gui::config::ConfigAppWeatherPcd::fillAdditionalMapList()
{
  ui_->pointFormatInfluenceDatasetCmb->clear();
  ui_->pointFormatInfluenceDatasetCmb->addItem("");
  std::vector<terrama2::core::DataSet> dsList;
  app_->getClient()->listDataSet(dsList);
  for(auto ds : dsList)
  {
    if(ds.kind() == terrama2::core::DataSet::STATIC_DATA)
    {
      ui_->pointFormatInfluenceDatasetCmb->addItem(ds.name().c_str());
    }
  }
}

void terrama2::gui::config::ConfigAppWeatherPcd::load(const terrama2::core::DataSet& dataset)
{
  dataSet_ = dataset;

  tableClean();

  ui_->pointFormatDataName->setText(dataset.name().c_str());
  ui_->pointFormatDataHour->setText(QString::number(dataset.dataFrequency().getHours()));
  ui_->pointFormatDataMinute->setText(QString::number(dataset.dataFrequency().getMinutes()));
  ui_->pointFormatDataSecond->setText(QString::number(dataset.dataFrequency().getSeconds()));

  std::map<std::string, std::string> datasetMetadata = dataSet_.metadata();

  ui_->pointFormatDataInfluenceCmb->setCurrentText(datasetMetadata["INFLUENCE_TYPE"].c_str());

  if(ui_->pointFormatDataInfluenceCmb->currentIndex() == 2)
  {
    // Region influence type
    ui_->pointFormatInfluenceStack->setCurrentIndex(1);
    ui_->pointFormatInfluenceDatasetCmb->setEnabled(true);
    ui_->pointFormatDataAttributeCmb->setEnabled(true);
    ui_->pointFormatInfluenceDatasetCmb->setCurrentText(datasetMetadata["INFLUENCE_DATASET"].c_str());
    ui_->pointFormatDataAttributeCmb->setCurrentText(datasetMetadata["INFLUENCE_ATTRIBUTE"].c_str());
  }
  else
  {
    // Radius influence type
    ui_->pointFormatInfluenceStack->setCurrentIndex(0);
    ui_->pointFormatDataRadius->setEnabled(true);
    ui_->pointFormatDataRadius->setText(datasetMetadata["INFLUENCE_RADIUS"].c_str());
  }

  ui_->pointFormatDataPrefix->setText(datasetMetadata["PREFIX"].c_str());
  ui_->pointFormatDataUnit->setText(datasetMetadata["UNIT"].c_str());


  auto dsItems = dataset.dataSetItems();

  // Gets the first item to discover the PCD type
  if(!dsItems.empty() && dsItems[0].kind() == terrama2::core::DataSetItem::PCD_INPE_TYPE)
    ui_->pointFormatDataFormat->setCurrentIndex(0);
  else
    ui_->pointFormatDataFormat->setCurrentIndex(1);

  ui_->tblPointPCDFileNameLocation->setRowCount(dsItems.size());

  unsigned int row = 0;
  for(auto item : dsItems)
  {
    auto metadata = item.metadata();

    QTableWidgetItem* maskItem = new QTableWidgetItem(item.mask().c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 0, maskItem);

    QTableWidgetItem* latItem = new QTableWidgetItem(metadata["LATITUDE"].c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 1, latItem);

    QTableWidgetItem* longItem = new QTableWidgetItem(metadata["LONGITUDE"].c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 2, longItem);

    QString status(tr("No"));
    if(item.status() == terrama2::core::DataSetItem::ACTIVE)
      status = tr("Yes");

    QTableWidgetItem* statusItem = new QTableWidgetItem(status);
    ui_->tblPointPCDFileNameLocation->setItem(row, 3, statusItem);

    QTableWidgetItem* sridItem = new QTableWidgetItem(std::to_string(item.srid()).c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 4, sridItem);

    QTableWidgetItem* timezoneItem = new QTableWidgetItem(item.timezone().c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 5, timezoneItem);

    ++row;
  }
}

bool terrama2::gui::config::ConfigAppWeatherPcd::validate()
{
  if (ui_->pointFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointFormatDataName->setFocus();
    throw terrama2::gui::FieldException() << terrama2::ErrorDescription(tr("The PCD Name cannot be empty"));
  }

  // TODO: validate all fields

  return true;
}

void terrama2::gui::config::ConfigAppWeatherPcd::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());

  dataSet_.setName(ui_->pointFormatDataName->text().toStdString());
  dataSet_.setDescription(ui_->pointFormatDataDescription->toPlainText().toStdString());
  dataSet_.setKind(terrama2::core::DataSet::PCD_TYPE);

  dataSet_.setStatus(terrama2::core::ToDataSetStatus(ui_->pointFormatStatus->isChecked()));

  te::dt::TimeDuration dataFrequency(ui_->pointFormatDataHour->text().toInt(),
                                     ui_->pointFormatDataMinute->text().toInt(),
                                     ui_->pointFormatDataSecond->text().toInt());

  dataSet_.setDataFrequency(dataFrequency);

  std::map<std::string, std::string> datasetMetadata = dataSet_.metadata();

  datasetMetadata["INFLUENCE_TYPE"] = ui_->pointFormatDataInfluenceCmb->currentText().toStdString();

  if(ui_->pointFormatDataInfluenceCmb->currentIndex() == 2)
  {
    datasetMetadata["INFLUENCE_DATASET"] = ui_->pointFormatInfluenceDatasetCmb->currentText().toStdString();
    datasetMetadata["INFLUENCE_ATTRIBUTE"] = ui_->pointFormatDataAttributeCmb->currentText().toStdString();
  }
  else
  {
    datasetMetadata["INFLUENCE_RADIUS"] = ui_->pointFormatDataRadius->text().toStdString();
  }

  datasetMetadata["PREFIX"] = ui_->pointFormatDataPrefix->text().toStdString();
  datasetMetadata["UNIT"] = ui_->pointFormatDataUnit->text().toStdString();

  dataSet_.setMetadata(datasetMetadata);

  if (dataSet_.id() > 0)
  {
    app_->getClient()->updateDataSet(dataSet_);
    app_->getWeatherTab()->updateCachedDataSet(dataSet_);
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->currentItem(), selectedData_, ui_->pointFormatDataName->text());
    selectedData_ = ui_->pointFormatDataName->text();
    changed_ = false;
    return;
  }
  else
  {
    dataSet_.setProvider(provider.id());
    app_->getClient()->addDataSet(dataSet_);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("pcd"));
    item->setText(0, ui_->pointFormatDataName->text());
    ui_->weatherDataTree->currentItem()->addChild(item);

  }
  app_->getWeatherTab()->addCachedDataSet(dataSet_);
  changed_ = false;
}

void terrama2::gui::config::ConfigAppWeatherPcd::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataPointPage->findChildren<QLineEdit*>())
    widget->clear();
  changed_ = false;
}

void terrama2::gui::config::ConfigAppWeatherPcd::onInsertPointBtnClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
  {
    selectedData_.clear();
    app_->getWeatherTab()->changeTab(this, *ui_->DataPointPage);
    tableClean();

    ui_->occurenceProjectionTxt->setText("0");
  }
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void terrama2::gui::config::ConfigAppWeatherPcd::onDataPointBtnClicked()
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
        app_->getWeatherTab()->removeCachedDataSet(dataset);

        QMessageBox::information(app_, tr("TerraMA2"), tr("DataSet PCD successfully removed!"));
        delete currentItem;
      }
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      QMessageBox::warning(app_, tr("TerraMA2"), *message);
    }
  }
  ui_->cancelBtn->clicked();
}

void terrama2::gui::config::ConfigAppWeatherPcd::onCollectorRuleClicked()
{
  std::map<std::string, std::string> metadata = dataSet_.metadata();

  CollectorRuleDialog dialog(app_);
  dialog.fillGUI(metadata["COLLECT_RULE"].c_str());
  if (dialog.exec() == QDialog::Accepted)
    metadata["COLLECT_RULE"] = dialog.getCollectRule();

  dataSet_.setMetadata(metadata);
}

void terrama2::gui::config::ConfigAppWeatherPcd::onMenuMaskClicked(QAction* action)
{
  ui_->pointDiffFormatDataMask->setText(
        ui_->pointDiffFormatDataMask->text() + action->text().left(2));
}

void terrama2::gui::config::ConfigAppWeatherPcd::onPCDInsertFileClicked()
{
  terrama2::core::DataSetItem::Kind kind = terrama2::core::DataSetItem::PCD_INPE_TYPE;
  if(ui_->pointFormatDataFormat->currentIndex() == 1)
    kind = terrama2::core::DataSetItem::PCD_TOA5_TYPE;

  terrama2::core::DataSetItem item(kind, 0, dataSet_.id());
  pcdFormCreation(item);
}

void terrama2::gui::config::ConfigAppWeatherPcd::onPCDRemoveClicked()
{
  int row = ui_->tblPointPCDFileNameLocation->currentRow();
  if (row != -1)
    ui_->tblPointPCDFileNameLocation->removeRow(row);

  auto dsItem = dataSet_.dataSetItems()[row];
  if(dsItem.id() != 0)
    dataSet_.removeDataSetItem(dsItem.id());
}

void terrama2::gui::config::ConfigAppWeatherPcd::onPCDTableDoubleClicked(QTableWidgetItem* item)
{
  if (item != nullptr)
  {
    terrama2::core::DataSetItem pcdItem = dataSet_.dataSetItems()[item->row()];
    pcdFormCreation(pcdItem, true);
  }
}

void terrama2::gui::config::ConfigAppWeatherPcd::onSurfaceBtnClicked()
{
  SurfaceDialog dialog(app_);
  dialog.exec();
}

void terrama2::gui::config::ConfigAppWeatherPcd::onPCDExportClicked()
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem == nullptr || currentItem->parent() == nullptr || currentItem->parent()->parent() == nullptr)
    throw terrama2::gui::config::DataSetException() << terrama2::ErrorDescription(tr("Please selected a valid PCD dataset"));

  QString path = QFileDialog::getSaveFileName(app_ ,
                                              tr("Type name and where you intend to save this PCD"),
                                              QString(("./" + currentItem->text(0).toStdString() + ".terrama2").c_str()),
                                              tr("TerraMA2 (*.terrama2)"));

  if (path.isEmpty())
    return;

  // TODO: export PCD
  QJsonObject json;
  json["name"] = ui_->pointFormatDataName->text();
  json["description"] = ui_->pointFormatDataDescription->toPlainText();
  json["path"] = ui_->pointFormatDataPath->text();
  QJsonObject datasetItemArray;

  json["datasetItems"] = datasetItemArray;

  try
  {
    terrama2::gui::core::saveTerraMA2File(app_, path, json);
    QMessageBox::information(app_, tr("TerraMA2 PCD Export"), tr("The pcd has successfully saved"));
  }
  catch(const terrama2::Exception& e)
  {
    QString message = "TerraMA2 error while exporting pcd file. \n\n";
    if (const QString* msg = boost::get_error_info<terrama2::ErrorDescription>(e))
      message.append(*msg);
    TERRAMA2_LOG_ERROR() << message;
    QMessageBox::warning(app_, tr("TerraMA2 Error"), message);
  }
}


void terrama2::gui::config::ConfigAppWeatherPcd::onPcdWfsClicked()
{
  PcdWfsDialog dialog(app_);
  if (dialog.exec() == QDialog::Accepted)
  {
    // todo: do some operation, fill object
  }
}


void terrama2::gui::config::ConfigAppWeatherPcd::pcdFormCreation(terrama2::core::DataSetItem item, bool editing)
{
  PcdDialog dialog(app_);
  dialog.fill(item);
  if (dialog.exec() == QDialog::Accepted)
  {
    auto newItem = dialog.getDataSetItem();

    int row = ui_->tblPointPCDFileNameLocation->rowCount();

    if (editing)
      row = ui_->tblPointPCDFileNameLocation->currentRow();
    else
      ui_->tblPointPCDFileNameLocation->setRowCount(ui_->tblPointPCDFileNameLocation->rowCount() + 1);

    auto metadata = newItem.metadata();

    QTableWidgetItem* maskItem = new QTableWidgetItem(newItem.mask().c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 0, maskItem);

    QTableWidgetItem* latItem = new QTableWidgetItem(metadata["LATITUDE"].c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 1, latItem);

    QTableWidgetItem* longItem = new QTableWidgetItem(metadata["LONGITUDE"].c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 2, longItem);

    QString status(tr("No"));
    if(newItem.status() == terrama2::core::DataSetItem::ACTIVE)
      status = tr("Yes");

    QTableWidgetItem* statusItem = new QTableWidgetItem(status);
    ui_->tblPointPCDFileNameLocation->setItem(row, 3, statusItem);

    QTableWidgetItem* sridItem = new QTableWidgetItem(std::to_string(newItem.srid()).c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 4, sridItem);

    QTableWidgetItem* timezoneItem = new QTableWidgetItem(newItem.timezone().c_str());
    ui_->tblPointPCDFileNameLocation->setItem(row, 5, timezoneItem);

    terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
    metadata = terrama2::gui::core::makeStorageMetadata(dataSet_.kind(), newItem.metadata(), provider.uri().c_str(), *app_->getConfiguration());
    newItem.setMetadata(metadata);

    if(editing)
    {
      dataSet_.update(newItem);
    }
    else
    {
      dataSet_.add(newItem);
    }
  }

}

void terrama2::gui::config::ConfigAppWeatherPcd::tableClean()
{
  // Clear the pcd table
  while(ui_->tblPointPCDFileNameLocation->rowCount() > 0)
    ui_->tblPointPCDFileNameLocation->removeRow(0);
}

void terrama2::gui::config::ConfigAppWeatherPcd::onInfluenceTypeSelected(int index)
{
  if(ui_->pointFormatDataInfluenceCmb->currentIndex() == 2)
  {
    // Region influence type
    ui_->pointFormatInfluenceStack->setCurrentIndex(1);
    ui_->pointFormatInfluenceDatasetCmb->setEnabled(true);
    ui_->pointFormatDataAttributeCmb->setEnabled(true);
  }
  else
  {
    // Radius influence type
    ui_->pointFormatInfluenceStack->setCurrentIndex(0);
    ui_->pointFormatDataRadius->setEnabled(true);
  }
}
