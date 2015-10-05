// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "ProjectionDialog.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/Utils.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/DataProviderDAO.hpp"

// QT
#include <QMessageBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QWidget>
#include <QJsonObject>

ConfigAppWeatherTab::ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui),
    serverTabChanged_(false),
    dataGridSeriesChanged_(false),
    dataPointSeriesChanged_(false),
    dataPointDiffSeriesChanged_(false)
{
  ui_->weatherDataTree->header()->hide();
  ui_->weatherDataTree->setCurrentItem(ui_->weatherDataTree->topLevelItem(0));
  ui_->weatherDataTree->setExpanded(ui_->weatherDataTree->model()->index(0, 0), true);

  connect(ui_->saveBtn, SIGNAL(clicked()), SLOT(onSaveRequested()));
  connect(ui_->insertServerBtn, SIGNAL(clicked()), SLOT(onEnteredWeatherTab()));
  connect(ui_->cancelBtn, SIGNAL(clicked()), SLOT(onCancelRequested()));
  connect(ui_->importServerBtn, SIGNAL(clicked()), SLOT(onImportServer()));
  connect(ui_->serverCheckConnectionBtn, SIGNAL(clicked()), SLOT(onCheckConnection()));

  // Bind the inputs
  connect(ui_->serverName, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->serverDescription, SIGNAL(textChanged(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionAddress, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionPort, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionUserName, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionPassword, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));
  connect(ui_->connectionProtocol, SIGNAL(currentIndexChanged(int)), SLOT(onWeatherTabEdited()));
  connect(ui_->serverDataBasePath, SIGNAL(textEdited(QString)), SLOT(onWeatherTabEdited()));

  // Bind the data series type with respective group view
  connect(ui_->projectionGridBtn, SIGNAL(clicked()), this, SLOT(onProjectionClicked()));
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridBtnClicked()));
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->serverInsertPointDiffBtn, SIGNAL(clicked()), SLOT(onInsertPointDiffBtnClicked()));
  connect(ui_->serverDeleteBtn, SIGNAL(clicked()), SLOT(onDeleteServerClicked()));
  connect(ui_->exportServerBtn, SIGNAL(clicked()), SLOT(onExportServerClicked()));
  connect(ui_->weatherDataTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onWeatherDataTreeClicked(QTreeWidgetItem*)));

  // Lock for cannot allow multiple selection
  ui_->weatherDataTree->setSelectionMode(QAbstractItemView::SingleSelection);

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

  // Delete all children from remote services
  clearList();

  // Disable series button
  showDataSeries(false);
  hideDataSetButtons(false);

}

ConfigAppWeatherTab::~ConfigAppWeatherTab()
{

}

void ConfigAppWeatherTab::clearList()
{
  qDeleteAll(ui_->weatherDataTree->topLevelItem(0)->takeChildren());
}

void ConfigAppWeatherTab::load()
{
  std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();
  std::auto_ptr<te::da::DataSet> dataSet = ds->getDataSet("terrama2.data_provider");

  if (dataSet->size() != -1)
    showDataSeries(true);

  // clear list
  clearList();

  while(dataSet->moveNext())
  {
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("server"));
    item->setText(0, QString(dataSet->getAsString(1).c_str()));
    item->setIcon(0, QIcon::fromTheme("server"));
    ui_->weatherDataTree->topLevelItem(0)->addChild(item);
  }
}

bool ConfigAppWeatherTab::dataChanged()
{
  return serverTabChanged_ || dataGridSeriesChanged_ || dataPointDiffSeriesChanged_ || dataPointSeriesChanged_;
}

bool ConfigAppWeatherTab::validate()
{
  // If serverAdd tab is active
  if (serverTabChanged_)
  {
    if (ui_->serverName->text().isEmpty())
    {
      ui_->serverName->setFocus();
      return false;
    }

    // Check if has already been saved before
    std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();

    std::string sqlProvider("SELECT name FROM terrama2.data_provider WHERE name = '");
    sqlProvider += ui_->serverName->text().toStdString() + "'";
    std::auto_ptr<te::da::DataSet> dset = ds->query(sqlProvider);

    if (dset->size() > 0)
    {
      ui_->serverName->setFocus();
      throw terrama2::Exception() << terrama2::ErrorDescription(tr("The server name has already been saved. Please change server name"));
    }

    validateConnection();

    return true;
  }

  if (dataGridSeriesChanged_)
  {
    if (ui_->gridFormatDataName->text().isEmpty())
    {
      ui_->gridFormatDataName->setFocus();
      throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The DataSet name cannot be empty."));
    }



    return true;
  }

  if (dataPointDiffSeriesChanged_)
    return false;

  if (dataPointSeriesChanged_)
    return false;
  return true;
}

void ConfigAppWeatherTab::save()
{
  if (!validate())
  {
    throw terrama2::Exception() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
  }

  if (serverTabChanged_)
    saveServer();
  else if (dataGridSeriesChanged_)
    saveGridDataSeries();

  showDataSeries(true);
  discardChanges(false);
  QMessageBox::information(app_, tr("TerraMA2"), tr("Save successfully"));
}

void ConfigAppWeatherTab::saveServer()
{
  // Get DataSource
  std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();

  // Temp code
  terrama2::core::DataProviderPtr dataProvider = terrama2::core::DataManager::getInstance().findDataProvider(ui_->serverName->text().toStdString());
  if (dataProvider != nullptr)

  // If there data provider in database
//  if (terrama2::core::DataProviderPtr dataProvider = terrama2::core::DataProviderDAO::find(ui_->serverName->text().toStdString()))
  {
    dataProvider->setName(ui_->serverName->text().toStdString());
    dataProvider->setDescription(ui_->serverDescription->toPlainText().toStdString());
    dataProvider->setKind(static_cast<terrama2::core::DataProvider::Kind>(ui_->connectionProtocol->currentIndex()));
    dataProvider->setUri(ui_->connectionAddress->text().toStdString());
    dataProvider->setStatus(terrama2::core::BoolToDataProviderStatus(ui_->serverActiveServer->isChecked()));

    terrama2::core::DataManager::getInstance().update(dataProvider);
    ui_->weatherDataTree->currentItem()->setText(0, ui_->serverName->text());
  }
  else
  {
    dataProvider.reset(new terrama2::core::DataProvider(ui_->serverName->text().toStdString(),
                                                  terrama2::core::IntToDataProviderKind(ui_->connectionProtocol->currentIndex())));
    dataProvider->setDescription(ui_->serverDescription->toPlainText().toStdString());
    dataProvider->setUri(ui_->connectionAddress->text().toStdString());
    dataProvider->setStatus(ui_->serverActiveServer->isChecked() ? terrama2::core::DataProvider::ACTIVE :
                                                                   terrama2::core::DataProvider::INACTIVE);

    terrama2::core::DataManager::getInstance().add(dataProvider);

    QTreeWidgetItem* newServer = new QTreeWidgetItem();
    newServer->setText(0, ui_->serverName->text());
    newServer->setIcon(0, QIcon::fromTheme("server"));
    ui_->weatherDataTree->topLevelItem(0)->addChild(newServer);
  }
}

void ConfigAppWeatherTab::saveGridDataSeries()
{

}

void ConfigAppWeatherTab::discardChanges(bool restore_data)
{
  if (restore_data)
  {
    // Make the save procedure
  }

// Clear all inputs
  const auto* tab = ui_->ServerPage;

// Clear QLineEdits
  for(QLineEdit* widget: tab->findChildren<QLineEdit*>())
  {
    widget->clear();
  }

  ui_->serverDescription->clear();

// Hide the form
  hidePanels(0);

  serverTabChanged_ = false;
  dataGridSeriesChanged_ = false;
  dataPointDiffSeriesChanged_ = false;
  dataPointSeriesChanged_ = false;

// Set visible false on server buttons
  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

}

void ConfigAppWeatherTab::validateConnection()
{
  switch (terrama2::core::IntToDataProviderKind(ui_->connectionProtocol->currentIndex())) {
    case terrama2::core::DataProvider::FTP_TYPE:
      terrama2::gui::core::checkFTPConnection(ui_->connectionAddress->text(),
                                              ui_->connectionPort->text().toInt(),
                                              ui_->serverDataBasePath->text(),
                                              ui_->connectionUserName->text(),
                                              ui_->connectionPassword->text());
      break;
    case terrama2::core::DataProvider::HTTP_TYPE:
      terrama2::gui::core::checkServiceConnection(ui_->connectionAddress->text(),
                                                  ui_->connectionPort->text().toInt(),
                                                  ui_->connectionUserName->text(),
                                                  ui_->connectionPassword->text());
      break;
    case terrama2::core::DataProvider::FILE_TYPE:
      terrama2::gui::core::checkLocalFilesConnection(ui_->serverDataBasePath->text());
      break;
    default:
      throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Not implemented yet"));
      break;
    }
}

void ConfigAppWeatherTab::showDataSeries(bool state)
{
  ui_->exportServerBtn->setVisible(state);
  ui_->updateServerBtn->setVisible(state);
  ui_->serverDeleteBtn->setVisible(state);

  (state) ? ui_->groupBox_25->show() : ui_->groupBox_25->hide();
}

void ConfigAppWeatherTab::hideDataSetButtons(const bool state)
{
  ui_->gridFormatDataDeleteBtn->setVisible(state);
  ui_->pointFormatDataDeleteBtn->setVisible(state);
  ui_->serverRemovePointDiffBtn->setVisible(state);
  ui_->updateDataPointBtn->setVisible(state);
  ui_->updateDataGridBtn->setVisible(state);
  ui_->updateDataPointDiffBtn->setVisible(state);
  ui_->exportDataGridBtn->setVisible(state);
  ui_->exportDataPointBtn->setVisible(state);
  ui_->exportDataPointDiffBtn->setVisible(state);
}

void ConfigAppWeatherTab::hidePanels(const int indexOfExcept)
{
  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(false);
  ui_->cancelBtn->setEnabled(true);

  ui_->weatherPageStack->setCurrentIndex(indexOfExcept);
}

void ConfigAppWeatherTab::onEnteredWeatherTab()
{
  // Set visible the buttons
  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(false);
  ui_->cancelBtn->setEnabled(true);

  ui_->weatherPageStack->setCurrentIndex(1);
}

void ConfigAppWeatherTab::onWeatherTabEdited()
{
  ui_->saveBtn->setEnabled(true);
  ui_->cancelBtn->setEnabled(true);
  serverTabChanged_ = true;
}

void ConfigAppWeatherTab::onImportServer()
{
  QString file = QFileDialog::getOpenFileName(app_, tr("Choose file"), ".", tr("TerraMA2 ( *.terrama2"));
  if (!file.isEmpty())
  {
    QMessageBox::information(app_, tr("TerraMA2 Server"), tr("Opened Server File"));
  }
}

void ConfigAppWeatherTab::onCheckConnection()
{
  // For handling error message
  QString message;
  try
  {
    validateConnection();
    // FIX
    QMessageBox::information(app_, tr("TerraMA2"), tr("Connection OK"));
    return;
  }
  catch(const terrama2::Exception& e)
  {
    message.append(boost::get_error_info<terrama2::ErrorDescription>(e));
  }
  catch(const std::exception& e)
  {
    message.fromUtf8(e.what());
  }
  catch(...)
  {
    message.append("Unknown Error");
  }
  QMessageBox::critical(app_, tr("TerraMA2 Error"), message);
}

void ConfigAppWeatherTab::onDataGridBtnClicked()
{
  // Index 2 represents DataGridPage
  hidePanels(2);
}

void ConfigAppWeatherTab::onInsertPointBtnClicked()
{
  // Index 3 represents DataPointPage
  hidePanels(3);
}

void ConfigAppWeatherTab::onInsertPointDiffBtnClicked()
{
  // Index 4 represents DataPointDiffPage
  hidePanels(4);
}

void ConfigAppWeatherTab::onDeleteServerClicked()
{
  try
  {
    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
    if (selectedItem == nullptr || selectedItem->parent() == nullptr)
      throw terrama2::Exception() << terrama2::ErrorDescription(tr("Please select a data provider to remove"));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(app_, tr("TerraMA2 Remove Data Provider"),
                                        tr("Would you like to remove data provider?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);

    if (reply == QMessageBox::No || reply == QMessageBox::Cancel)
      return;

    std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();

    std::string sql = "SELECT id FROM terrama2.data_provider WHERE name = '";
    sql += selectedItem->text(0).toStdString() + "'";
    std::auto_ptr<te::da::DataSet> dataSet = ds->query(sql);

    if (dataSet->size() != 1)
      throw terrama2::Exception() << terrama2::ErrorDescription(tr("Invalid data provider"));

    dataSet->moveFirst();
    int64_t id = boost::lexical_cast<int64_t>(dataSet->getAsString(0));

    // Load providers
    terrama2::core::DataManager::getInstance().load();

    // Remove from database and refresh providers list
    terrama2::core::DataManager::getInstance().removeDataProvider(id);

    QMessageBox::information(app_, tr("TerraMA2"), tr("Data provider has been successfully removed."));

    ui_->weatherDataTree->removeItemWidget(ui_->weatherDataTree->currentItem(), 0);

    delete selectedItem;
  }
  catch(const terrama2::Exception& e)
  {
    const QString* error = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::critical(app_, tr("TerraMA2 Error"), *error);
  }
  catch(...)
  {
    throw;
  }
}

void ConfigAppWeatherTab::onWeatherDataTreeClicked(QTreeWidgetItem* selectedItem)
{
  if (selectedItem->parent() != nullptr)
  {
    // If it does not have parent, so it has to be DataProvider. Otherwise, selectedItem is DataSet
    if (selectedItem->parent()->parent() == nullptr)
    {
      std::string sql = "SELECT * FROM terrama2.data_provider WHERE name = '";
      sql += selectedItem->text(0).toStdString() + "'";

      std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
      std::auto_ptr<te::da::DataSet> dataSet = dataSource->query(sql);

      if (dataSet->size() != 1)
        throw terrama2::Exception() << terrama2::ErrorDescription(tr("It cannot be a valid provider selected."));

      dataSet->moveFirst();

      // Call for insert server to display server form
      emit(ui_->insertServerBtn->clicked());

      ui_->serverName->setText(QString(dataSet->getAsString(1).c_str()));
      ui_->serverDescription->setText(QString(dataSet->getAsString(2).c_str()));
      ui_->connectionProtocol->setCurrentIndex(dataSet->getInt32(3));
      ui_->connectionAddress->setText(QString(dataSet->getAsString(4).c_str()));
      ui_->serverActiveServer->setChecked(dataSet->getBool(5));
    }
    else
    {

    }
  }
  else
    emit(ui_->cancelBtn->clicked());
}

void ConfigAppWeatherTab::onExportServerClicked()
{
  try
  {
    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
    if (selectedItem == nullptr || selectedItem->parent() == nullptr)
      throw terrama2::Exception() << terrama2::ErrorDescription(tr("Please select a data provider to remove"));

    QJsonObject json;
    json["name"] = ui_->serverName->text();
    json["description"] = ui_->serverDescription->toPlainText();

    json["kind"] = ui_->connectionProtocol->currentText();
    switch((terrama2::core::DataProvider::Kind) ui_->connectionProtocol->currentIndex())
    {
      case terrama2::core::DataProvider::FILE_TYPE:
        json["path"] = ui_->serverDataBasePath->text();
        break;
      default:
        json["address"] = ui_->connectionAddress->text();
        json["port"] = ui_->connectionPort->text();
        json["username"] = ui_->connectionUserName->text();
        json["password"] = ui_->connectionPort->text();
    }

    json["interval"] = ui_->serverIntervalData->text();

    terrama2::gui::core::saveTerraMA2File(app_, json);

    QMessageBox::information(app_, tr("TerraMA2 Export Data Provider"), tr("The data provider has been successfully exported!"));

  }
  catch(const terrama2::Exception& e)
  {
    const QString* error = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::critical(app_, tr("TerraMA2 Error"), *error);
  }
  catch(...)
  {
    throw;
  }
}

void ConfigAppWeatherTab::onProjectionClicked()
{
  ProjectionDialog projectionDialog(app_);

  projectionDialog.show();
  projectionDialog.exec();
}
