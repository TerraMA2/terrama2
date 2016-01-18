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
  \file terrama2/gui/config/ConfigAppWeatherTab.cpp

  \brief Definition of terrama2::gui::config::ConfigAppWeatherTab

  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "ConfigApp.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/Utils.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/Logger.hpp"

// TerraMA2 SubTabs
#include "ConfigAppWeatherServer.hpp"
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigAppWeatherPcd.hpp"
#include "ConfigAppWeatherOccurrence.hpp"

// STL
#include <vector>

// QT
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>

terrama2::gui::config::ConfigAppWeatherTab::ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  ui_->weatherDataTree->header()->hide();
  ui_->weatherDataTree->setCurrentItem(ui_->weatherDataTree->topLevelItem(0));
  ui_->weatherDataTree->setExpanded(ui_->weatherDataTree->model()->index(0, 0), true);

  connect(ui_->saveBtn, SIGNAL(clicked()), SLOT(onSaveRequested()));
  connect(ui_->cancelBtn, SIGNAL(clicked()), SLOT(onCancelRequested()));
  connect(ui_->importServerBtn, SIGNAL(clicked()), SLOT(onImportServer()));

  // Bind the data series type with respective group view
  connect(ui_->serverDeleteBtn, SIGNAL(clicked()), SLOT(onDeleteServerClicked()));
  connect(ui_->exportServerBtn, SIGNAL(clicked()), SLOT(onExportServerClicked()));
  connect(ui_->updateServerBtn, SIGNAL(clicked()), SLOT(onUpdateServerClicked()));
  connect(ui_->weatherDataTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                                SLOT(onWeatherDataTreeClicked(QTreeWidgetItem*)));

  // Tabs
  QSharedPointer<ConfigAppWeatherServer> serverTab(new ConfigAppWeatherServer(app, ui));
  QSharedPointer<ConfigAppWeatherGridTab> gridTab(new ConfigAppWeatherGridTab(app, ui));
  QSharedPointer<ConfigAppWeatherPcd> pcdTab(new ConfigAppWeatherPcd(app, ui));
  QSharedPointer<ConfigAppWeatherOccurrence> occurrenceTab(new ConfigAppWeatherOccurrence(app, ui));
  subTabs_.append(serverTab);
  subTabs_.append(gridTab);
  subTabs_.append(pcdTab);
  subTabs_.append(occurrenceTab);

  // Lock for cannot allow multiple selection
  ui_->weatherDataTree->setSelectionMode(QAbstractItemView::SingleSelection);

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

  // Delete all children from remote services
  clearList();

  // Disable series button
  showDataSeries(false);
  hideDataSetButtons();

  // disable menu buttons
  ui_->showConsoleAct->setEnabled(false);
  ui_->studyAct->setEnabled(false);
  ui_->archivingAct->setEnabled(false);
  ui_->terraMEPlayerAct->setEnabled(false);
  ui_->refreshAct->setEnabled(false);

  //loading server pixmap from icon theme and set it to label
  QPixmap pixmap = QIcon::fromTheme("servers").pixmap(64);
  ui_->labelServerInfo->setPixmap(pixmap);

  ui_->weatherPageStack->setCurrentWidget(ui_->ServerGroupPage);
}

terrama2::gui::config::ConfigAppWeatherTab::~ConfigAppWeatherTab()
{

}

void terrama2::gui::config::ConfigAppWeatherTab::clearList()
{
  qDeleteAll(ui_->weatherDataTree->topLevelItem(0)->takeChildren());
  providers_.clear();
  datasets_.clear();
}

void terrama2::gui::config::ConfigAppWeatherTab::load()
{
  clearList();

  try
  {
    std::vector<terrama2::core::DataProvider> providers;
    std::vector<terrama2::core::DataSet> datasets;
    app_->getClient()->listDataProvider(providers);

    if (providers.size() > 0)
    {
      // temp code
      showDataSeries(true);
      ui_->dataSeriesBtnGroupBox->hide();
      ui_->serverInsertPointBtn->setVisible(true);
      ui_->serverInsertGridBtn->setVisible(true);
      ui_->serverInsertPointDiffBtn->setVisible(true);
    }
    hideDataSetButtons();

    ui_->importServerBtn->setEnabled(false);
    ui_->updateServerBtn->setEnabled(false);

    app_->getClient()->listDataSet(datasets);
    for(std::vector<terrama2::core::DataProvider>::iterator it = providers.begin(); it != providers.end(); ++it)
    {
      QTreeWidgetItem* item = new QTreeWidgetItem;
      item->setIcon(0, QIcon::fromTheme("server"));
      item->setText(0, QString(it->name().c_str()));
      ui_->weatherDataTree->topLevelItem(0)->addChild(item);

      providers_.insert(it->name(), *it);

      for(std::vector<terrama2::core::DataSet>::iterator dit = datasets.begin(); dit != datasets.end(); ++dit)
      {
        if (dit->provider() == it->id())
        {
          datasets_.insert(dit->name(), *dit);
          QTreeWidgetItem* subItem = new QTreeWidgetItem;
          subItem->setText(0, QString(dit->name().c_str()));
          switch (dit->kind())
          {
            case terrama2::core::DataSet::GRID_TYPE:
              subItem->setIcon(0, QIcon::fromTheme("grid"));
              break;
            case terrama2::core::DataSet::PCD_TYPE:
              subItem->setIcon(0, QIcon::fromTheme("pcd"));
              break;
            case terrama2::core::DataSet::OCCURENCE_TYPE:
              subItem->setIcon(0, QIcon::fromTheme("ocurrence-data"));
              break;
            default:
              break;
          }
          item->addChild(subItem);
        } // endif (dit->provider() == it->id())
      }
    } // endfor

    for(auto tab: subTabs_)
      tab->load();
  }
  catch (const terrama2::Exception& e)
  {
    const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
    app_->unload();
    QMessageBox::warning(app_, tr("TerraMA2"), *message);
    showDataSeries(false);
    TERRAMA2_LOG_ERROR() << *message;
  }
}

bool terrama2::gui::config::ConfigAppWeatherTab::dataChanged()
{
  for(const auto tab: subTabs_)
  {
    if (tab->isActive())
      return true;
  }
  return false;
}

bool terrama2::gui::config::ConfigAppWeatherTab::validate()
{
  for(auto tab: subTabs_)
    if (tab->isActive() && tab->dataChanged())
      return tab->validate();
  return false;
}

void terrama2::gui::config::ConfigAppWeatherTab::save()
{
  for(auto tab: subTabs_)
  {
    if (tab->isActive())
    {
      if (!tab->validate())
        throw terrama2::gui::FieldException() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
      tab->save();
      showDataSeries(true);
      discardChanges(false);
      QMessageBox::information(app_, tr("TerraMA2"), tr("Save successfully"));
    }
  }
}

void terrama2::gui::config::ConfigAppWeatherTab::discardChanges(bool restore_data)
{
  for(auto tab: subTabs_)
    if (tab->dataChanged())
      tab->discardChanges(restore_data);

  hidePanels(ui_->ServerGroupPage);

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

  hideDataSetButtons();
  showDataSeries(false);
}

void terrama2::gui::config::ConfigAppWeatherTab::showDataSeries(bool state)
{
  ui_->exportServerBtn->setVisible(state);
  ui_->updateServerBtn->setVisible(state);
  ui_->serverDeleteBtn->setVisible(state);

  (state) ? ui_->dataSeriesBtnGroupBox->show() : ui_->dataSeriesBtnGroupBox->hide();
}

void terrama2::gui::config::ConfigAppWeatherTab::hideDataSetButtons()
{
  for(QPushButton* button: ui_->dataSeriesBtnGroupBox->findChildren<QPushButton*>())
    button->setVisible(false);
}

void terrama2::gui::config::ConfigAppWeatherTab::hidePanels(QWidget* except)
{
  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(true);
  ui_->cancelBtn->setEnabled(true);

  ui_->weatherPageStack->setCurrentWidget(except);
}

void terrama2::gui::config::ConfigAppWeatherTab::onImportServer()
{
  QString file = QFileDialog::getOpenFileName(app_, tr("Choose file"), ".", tr("TerraMA2 ( *.terrama2"));
  if (!file.isEmpty())
  {
    QMessageBox::information(app_, tr("TerraMA2 Server"), tr("Opened Server File"));
  }
}

void terrama2::gui::config::ConfigAppWeatherTab::onDeleteServerClicked()
{
  try
  {
    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
    if (selectedItem == nullptr || selectedItem->parent() == nullptr)
      throw terrama2::gui::ValueException() << terrama2::ErrorDescription(tr("Please select a data provider to remove"));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(app_, tr("TerraMA2 Remove Data Provider"),
                                        tr("Would you like to remove data provider?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);

    if (reply == QMessageBox::No || reply == QMessageBox::Cancel)
      return;

    terrama2::core::DataProvider cachedProvider = providers_.take(selectedItem->text(0).toStdString());

    try
    {
      app_->getClient()->removeDataProvider(cachedProvider.id());
      QMessageBox::information(app_, tr("TerraMA2"), tr("Data provider has been successfully removed."));

      ui_->weatherDataTree->removeItemWidget(ui_->weatherDataTree->currentItem(), 0);
      TERRAMA2_LOG_INFO() << "DataProvider " + cachedProvider.name() + " has been removed.";
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      QMessageBox::warning(app_, tr("TerraMA2 Data Provider Remove Error"), *message);
      TERRAMA2_LOG_ERROR() << "DataProvider remove error - " + message->toStdString();
    }

    hidePanels(ui_->ServerGroupPage);

    delete selectedItem;
    displayOperationButtons(false);
  }
  catch(const terrama2::Exception& e)
  {
    const QString* error = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::warning(app_, tr("TerraMA2 Error"), *error);
    TERRAMA2_LOG_WARNING() << *error;
  }
  catch(...)
  {
    throw "Error in removing terrama2 data provider";
  }
}

void terrama2::gui::config::ConfigAppWeatherTab::onWeatherDataTreeClicked(QTreeWidgetItem* selectedItem)
{
  if (selectedItem->parent() != nullptr)
  {
    try
    {
      // If it does not have parent, so it has to be DataProvider. Otherwise, selectedItem is DataSet
      if (selectedItem->parent()->parent() == nullptr)
      {
        QObject::disconnect(ui_->serverDescription->document(), 0, 0, 0);
        hideDataSetButtons();
        showDataSeries(true);

        ui_->serverInsertPointBtn->setVisible(true);
        ui_->serverInsertGridBtn->setVisible(true);
        ui_->serverInsertPointDiffBtn->setVisible(true);

        if (providers_.empty())
        {
          std::cout << "Providers empty. Refreshing list..." << std::endl;
          std::vector<terrama2::core::DataProvider> providers;
          app_->getClient()->listDataProvider(providers);
          for(std::vector<terrama2::core::DataProvider>::iterator it = providers.begin(); it != providers.end(); ++it)
            providers_.insert(it->name(), *it);
        }

        terrama2::core::DataProvider provider = getProvider(selectedItem->text(0).toStdString());

        if (provider.id() == 0)
          throw terrama2::gui::config::DataProviderException() << terrama2::ErrorDescription(tr("It cannot be a valid provider selected."));

        displayOperationButtons(true);
        changeTab((subTabs_[0].data()), *ui_->ServerPage);
        subTabs_[0]->setSelectedData(selectedItem->text(0));

        QUrl url(provider.uri().c_str());
        int port = url.port();

        if (port > 0)
          ui_->connectionPort->setText(QString::number(port));

        switch(provider.kind())
        {
          case terrama2::core::DataProvider::FILE_TYPE:
            ui_->serverPath->setText(url.path());
            break;
          case terrama2::core::DataProvider::FTP_TYPE:
            ui_->connectionAddress->setText(url.host());
            ui_->serverPath->setText(url.path());
            break;
          default:
            ui_->connectionAddress->setText(url.host());
        }

        ui_->connectionUserName->setText(url.userName());
        ui_->connectionPassword->setText(url.password());
        ui_->serverName->setText(QString(provider.name().c_str()));
        ui_->serverDescription->setText(QString(provider.description().c_str()));
        ui_->connectionProtocol->setCurrentIndex(provider.kind() - 2);
        ui_->serverActiveServer->setChecked(terrama2::core::ToBool(provider.status()));

        subTabs_[0]->load();

        ui_->updateDataGridBtn->hide();
        ui_->exportDataGridBtn->hide();
        ui_->gridFormatDataDeleteBtn->hide();
      } // endif
      else
      {
        for(auto dataset: datasets_)
        {
          if (dataset.name() == selectedItem->text(0).toStdString())
          {
            displayOperationButtons(true);

            //fill up fields
            switch(dataset.kind())
            {
              case terrama2::core::DataSet::GRID_TYPE:
              {
                changeTab((subTabs_[1].data()), *ui_->DataGridPage);
                ConfigAppWeatherGridTab* gridTab = dynamic_cast<ConfigAppWeatherGridTab*>(subTabs_[1].data());

                gridTab->setSelectedData(selectedItem->text(0));

                ui_->gridFormatDataName->setText(dataset.name().c_str());
                ui_->gridFormatDataDescription->setText(dataset.description().c_str());
                ui_->gridFormatDataHour->setText(QString::number(dataset.dataFrequency().getHours()));
                ui_->gridFormatDataMinute->setText(QString::number(dataset.dataFrequency().getMinutes()));
                ui_->gridFormatDataSecond->setText(QString::number(dataset.dataFrequency().getSeconds()));

                ui_->gridFormatDataInterval->setValue(dataset.schedule().getMinutes());

                const std::map<std::string, std::string> metadata = dataset.metadata();
                auto it = metadata.find("UNIT");
                if (it != metadata.end())
                  ui_->gridFormatDataUnit->setText(it->second.c_str());

                it = metadata.find("PREFIX");
                if (it != metadata.end())
                  ui_->gridFormatDataPrefix->setText(it->second.c_str());

                it = metadata.find("RESOLUTION");
                if (it != metadata.end())
                  ui_->gridFormatDataResolution->setText(it->second.c_str());

                it = metadata.find("FORMAT");
                if (it != metadata.end())
                {
                  int formatIndex = ui_->gridFormatDataFormat->findText(it->second.c_str());
                  switch(formatIndex)
                  {
                    case 0:
                      {
                        it = metadata.find("COORDINATES_UNIT");
                        if (it != metadata.end())
                        {
                          if (it->second == tr("Decimal Degree").toStdString())
                            ui_->rbGridAscUnidGrausDec->setChecked(true);
                          else if (it->second == tr("Thousandth Degree").toStdString())
                            ui_->rbGridAscUnidGrausMil->setChecked(true);
                        }
                      }
                      break;
                    case 1:
                      {
                        it = metadata.find("NAVIGATION_FILE");
                        if (it != metadata.end())
                          ui_->ledGridTIFFArqNavegacao->setText(it->second.c_str());
                      }
                      break;
                    case 2:
                      {
                        it = metadata.find("CONTROL_FILE");
                        if (it != metadata.end())
                          ui_->ledGridGrADSArqControle->setText(it->second.c_str());
                        it = metadata.find("MULTIPLICATOR");
                        if (it != metadata.end())
                          ui_->ledGridGrADSMultiplicador->setText(it->second.c_str());
                        it = metadata.find("ORDER");
                        if (it != metadata.end())
                          ui_->cmbGridGrADSByteOrder->setCurrentIndex(ui_->cmbGridGrADSByteOrder->findText(it->second.c_str()));
                        it = metadata.find("BANDS");
                        if (it != metadata.end())
                          ui_->spbGridGrADSNumBands->setValue(std::atoi(it->second.c_str()));
                        it = metadata.find("BAND_PREFIX");
                        if (it != metadata.end())
                          ui_->spbGridGrADSHeaderSize->setValue(std::atoi(it->second.c_str()));
                        it = metadata.find("TIME_OFFSET");
                        if (it != metadata.end())
                          ui_->spbGridGrADSTimeOffset->setValue(std::atoi(it->second.c_str()));
                        it = metadata.find("POSFIX");
                        if (it != metadata.end())
                          ui_->spbGridGrADSTraillerSize->setValue(std::atoi(it->second.c_str()));
                        it = metadata.find("DUMMY");
                        if (it != metadata.end())
                          ui_->ledGridWCSDummy->setText(it->second.c_str());
                      }
                      break;
                    default:
                      ;
                  }
                  ui_->gridFormatDataFormat->setCurrentIndex(formatIndex);
                }

                // temp code
                if (dataset.dataSetItems().size() > 0)
                {
                  terrama2::core::DataSetItem datasetItem = dataset.dataSetItems().at(dataset.dataSetItems().size() - 1);
                  ui_->gridFormatDataPath->setText(datasetItem.path().c_str());
                  ui_->gridFormatDataTimeZoneCmb->setCurrentText(datasetItem.timezone().c_str());
                  ui_->gridFormatDataMask->setText(datasetItem.mask().c_str());
                  ui_->gridFormatStatus->setChecked(datasetItem.status() == terrama2::core::DataSetItem::ACTIVE ? true : false);
                  gridTab->fillFilter(datasetItem.filter());
                  gridTab->setSrid(datasetItem.srid());
                  ui_->gridProjectionTxt->setText(std::to_string(datasetItem.srid()).c_str());
                }

                hideDataSetButtons();
                showDataSeries(false);
                ui_->dataSeriesBtnGroupBox->setVisible(true);
                ui_->updateDataGridBtn->setVisible(true);
                ui_->exportDataGridBtn->setVisible(true);
                ui_->gridFormatDataDeleteBtn->setVisible(true);

                break;
              }
              case terrama2::core::DataSet::PCD_TYPE:
              {
                ConfigAppWeatherPcd* pcdTab  = dynamic_cast<ConfigAppWeatherPcd*>(subTabs_[2].data());
                changeTab((subTabs_[2].data()), *ui_->DataPointPage);
                subTabs_[2]->setSelectedData(selectedItem->text(0));

                hideDataSetButtons();
                showDataSeries(false);


                ui_->dataSeriesBtnGroupBox->setVisible(true);
                ui_->updateDataPointBtn->setVisible(true);
                ui_->exportDataPointBtn->setVisible(true);
                ui_->pointFormatDataDeleteBtn->setVisible(true);


                pcdTab->load(dataset);

                break;
              }
              case terrama2::core::DataSet::OCCURENCE_TYPE:
              {
                changeTab((subTabs_[3].data()), *ui_->DataPointDiffPage);
                subTabs_[3]->setSelectedData(selectedItem->text(0));

                ui_->pointDiffFormatDataName->setText(dataset.name().c_str());
                ui_->pointDiffFormatDataDescription->setText(dataset.description().c_str());

                ui_->pointDiffFormatDataHour->setText(QString::number(dataset.dataFrequency().getHours()));
                ui_->pointDiffFormatDataMinute->setText(QString::number(dataset.dataFrequency().getMinutes()));
                ui_->pointDiffFormatDataSecond->setText(QString::number(dataset.dataFrequency().getSeconds()));

                const std::map<std::string, std::string> metadata = dataset.metadata();
                auto it = metadata.find("UNIT");
                if (it != metadata.end())
                  ui_->pointDiffFormatDataUnit->setText(it->second.c_str());

                it = metadata.find("PREFIX");
                if (it != metadata.end())
                  ui_->pointDiffFormatDataPrefix->setText(it->second.c_str());

                ConfigAppWeatherOccurrence* occurrenceTab = dynamic_cast<ConfigAppWeatherOccurrence*>(subTabs_[3].data());

                occurrenceTab->setIntersection(dataset.intersection());
                if (dataset.dataSetItems().size() > 0)
                {
                  terrama2::core::DataSetItem datasetItem = dataset.dataSetItems().at(dataset.dataSetItems().size() - 1);
                  occurrenceTab->fillFilter(datasetItem.filter());
                  occurrenceTab->setSrid(datasetItem.srid());
                  ui_->occurenceProjectionTxt->setText(std::to_string(datasetItem.srid()).c_str());
                  ui_->pointDiffFormatDataPath->setText(datasetItem.path().c_str());
                  ui_->pointDiffFormatDataMask->setText(datasetItem.mask().c_str());
                  ui_->pointDiffFormatDataTimeZoneCmb->setCurrentText(datasetItem.timezone().c_str());
                  ui_->pointDiffFormatStatus->setChecked(datasetItem.status() == terrama2::core::DataSetItem::ACTIVE ? true : false);

                  switch(datasetItem.kind())
                  {
                    case terrama2::core::DataSetItem::UNKNOWN_TYPE:
                      ui_->pointDiffFormatDataType->setCurrentIndex(2);
                      break;
                    case terrama2::core::DataSetItem::FIRE_POINTS_TYPE:
                      ui_->pointDiffFormatDataType->setCurrentIndex(0);
                      break;
                    default:
                      ui_->pointDiffFormatDataType->setCurrentIndex(1);
                  }
                }

                hideDataSetButtons();
                showDataSeries(false);
                ui_->dataSeriesBtnGroupBox->setVisible(true);
                ui_->updateDataPointDiffBtn->setVisible(true);
                ui_->exportDataPointDiffBtn->setVisible(true);
                ui_->serverRemovePointDiffBtn->setVisible(true);
                break;
              }
              default:
              {
                // UNKNOWN
              }
            }
            return;
          } // endif
        } // endfor

        throw terrama2::gui::config::DataSetException() << terrama2::ErrorDescription(tr("It cannot be a valid dataset selected."));

      } //end else
    }
    catch (const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      QMessageBox::warning(app_, tr("TerraMA2"), *message);
    }
  }
  else
  {
    showDataSeries(false);
    discardChanges(true);
  }
}

void terrama2::gui::config::ConfigAppWeatherTab::onExportServerClicked()
{
  try
  {
    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
    if (selectedItem == nullptr || selectedItem->parent() == nullptr)
      throw terrama2::gui::ValueException() << terrama2::ErrorDescription(tr("Please select a data provider to export"));

    QJsonObject json;
    json["name"] = ui_->serverName->text();
    json["description"] = ui_->serverDescription->toPlainText();
    json["kind"] = ui_->connectionProtocol->currentText();
    json["address"] = ui_->connectionAddress->text();
    json["port"] = ui_->connectionPort->text();
    json["username"] = ui_->connectionUserName->text();
    json["password"] = ui_->connectionPassword->text();

    QString path = QFileDialog::getSaveFileName(app_, tr("Type where is to save dataprovider"),
                                                QString(("./" + selectedItem->text(0).toStdString() + ".terrama2").c_str()),
                                                "TerraMA2 (*.terrama2)");

    if (path.isEmpty())
      return;

    terrama2::gui::core::saveTerraMA2File(app_, path, json);

    QMessageBox::information(app_, tr("TerraMA2 Export Data Provider"), tr("The data provider has been successfully exported!"));

  }
  catch(const terrama2::Exception& e)
  {
    const QString* error = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::warning(app_, tr("TerraMA2 Error"), *error);
    TERRAMA2_LOG_WARNING() << *error;
  }
  catch(...)
  {
    throw "Error in exporting terrama2 data provider";
  }
}

void terrama2::gui::config::ConfigAppWeatherTab::onUpdateServerClicked()
{
  try
  {
    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
    if (selectedItem == nullptr || selectedItem->parent() == nullptr)
      throw terrama2::gui::ValueException() << terrama2::ErrorDescription(tr("Please select a data provider to update"));

    QString path = QFileDialog::getOpenFileName(app_, tr("Open dataprovider file"), ".", "TerraMA2 (*.terrama2)");

    if (path.isEmpty())
      return;

    QJsonDocument document = terrama2::core::ReadJsonFile(path.toStdString());
    QJsonObject json = document.object();

    ui_->serverName->setText(json["name"].toString());
    ui_->serverDescription->setText(json["description"].toString());
    ui_->connectionProtocol->setCurrentText(json["kind"].toString());
    ui_->connectionAddress->setText(json["address"].toString());
    ui_->connectionPort->setText(json["port"].toString());
    ui_->connectionUserName->setText(json["username"].toString());
    ui_->connectionPassword->setText(json["password"].toString());

    setChanged(true);

  }
  catch(const terrama2::Exception& e)
  {
    const QString* error = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::warning(app_, tr("TerraMA2 Error"), *error);
  }
  catch(...)
  {
    throw "Error in updating terrama2 data provider";
  }
}

void terrama2::gui::config::ConfigAppWeatherTab::displayOperationButtons(bool state)
{
  ui_->saveBtn->setVisible(state);
  ui_->cancelBtn->setVisible(state);
  ui_->saveBtn->setEnabled(state);
  ui_->cancelBtn->setEnabled(state);
}

void terrama2::gui::config::ConfigAppWeatherTab::changeTab(ConfigAppTab* sender, QWidget &widget) {
  for(auto tab: subTabs_)
  {
    if (tab->isActive())
    {
      if (tab->dataChanged())
      {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(app_, tr("TerraMA2"),
                                      tr("Would you like to try save before cancel?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::Yes)
        {
          try
          {
            if (tab->validate())
              tab->save();
          }
          catch (const terrama2::Exception& e)
          {
            QString message(tr("Error while saving. \n\n"));
            message.append(boost::get_error_info<terrama2::ErrorDescription>(e));

            QMessageBox::warning(app_, tr("TerraMA2"), message);
          }
        }
      } // endif tab->dataChanged()
      tab->setActive(false);
    } // endif tab->isActive()
  } // endfor
  sender->discardChanges(false);
  sender->setActive(true);
  ui_->weatherPageStack->setCurrentWidget(&widget);
  hidePanels(&widget);
}

QMap<std::string,terrama2::core::DataProvider> terrama2::gui::config::ConfigAppWeatherTab::providers()
{
  return providers_;
}

terrama2::core::DataProvider terrama2::gui::config::ConfigAppWeatherTab::getProvider(const std::string &identifier)
{
  for(auto& provider: providers_)
    if (provider.name() == identifier)
      return provider;
  return terrama2::core::DataProvider();
}

terrama2::core::DataSet terrama2::gui::config::ConfigAppWeatherTab::getDataSet(const std::string& identifier)
{
  for(auto dataset: datasets_)
  {
    if (dataset.name() == identifier)
      return dataset;
  }

  return terrama2::core::DataSet();
}

void terrama2::gui::config::ConfigAppWeatherTab::addCachedProvider(const terrama2::core::DataProvider& provider)
{
  providers_.insert(provider.name(), provider);
}

void terrama2::gui::config::ConfigAppWeatherTab::removeCachedDataProvider(const terrama2::core::DataProvider &provider)
{
  providers_.take(provider.name());
}

void terrama2::gui::config::ConfigAppWeatherTab::addCachedDataSet(const terrama2::core::DataSet &dataset)
{
  datasets_.insert(dataset.name(), dataset);
}

void terrama2::gui::config::ConfigAppWeatherTab::updateCachedDataSet(const terrama2::core::DataSet &dataset)
{
  datasets_[dataset.name()] =  dataset;
}

void terrama2::gui::config::ConfigAppWeatherTab::removeCachedDataSet(const terrama2::core::DataSet &dataset)
{
  datasets_.take(dataset.name());
}

void terrama2::gui::config::ConfigAppWeatherTab::refreshList(QTreeWidgetItem* widget, const QString searchFor, const QString replace)
{
  QTreeWidgetItemIterator it(widget);
  while(*it)
  {
    if ((*it)->text(0) == searchFor)
    {
      (*it)->setText(0, replace);
      break;
    }
    ++it;
  }
}
