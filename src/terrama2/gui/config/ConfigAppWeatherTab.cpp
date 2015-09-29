// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/Utils.hpp"
#include "../../core/DataManager.hpp"

// QT
#include <QMessageBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QWidget>

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
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridBtnClicked()));
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->serverInsertPointDiffBtn, SIGNAL(clicked()), SLOT(onInsertPointDiffBtnClicked()));

  connect(ui_->serverDeleteBtn, SIGNAL(clicked()),
                                SLOT(onDeleteServerClicked()));

  // Lock for cannot allow multiple selection
  ui_->weatherDataTree->setSelectionMode(QAbstractItemView::SingleSelection);

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

  // Delete all children from remote services
  qDeleteAll(ui_->weatherDataTree->topLevelItem(0)->takeChildren());

  // Disable series button
  showDataSeries(false);
}

ConfigAppWeatherTab::~ConfigAppWeatherTab()
{

}

void ConfigAppWeatherTab::load()
{
  std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();
  std::auto_ptr<te::da::DataSet> dataSet = ds->getDataSet("terrama2.data_provider");

  if (dataSet->size() != -1)
    showDataSeries(true);

  while(dataSet->moveNext())
  {
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, QString(dataSet->getAsString(1).c_str()));
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

    isValidConnection();

    return true;
  }

  if (dataGridSeriesChanged_)
    return false;

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

  terrama2::core::DataProviderPtr dataProvider(
        new terrama2::core::DataProvider(ui_->serverName->text().toStdString(),
                                         (terrama2::core::DataProvider::Kind) ui_->connectionProtocol->currentIndex()));
  dataProvider->setDescription(ui_->serverDescription->toPlainText().toStdString());
  dataProvider->setUri(ui_->connectionAddress->text().toStdString());
  dataProvider->setStatus(ui_->serverActiveServer->isChecked() ? terrama2::core::DataProvider::ACTIVE :
                                                                 terrama2::core::DataProvider::INACTIVE);

  terrama2::core::DataManager::getInstance().add(dataProvider);

  QTreeWidgetItem* newServer = new QTreeWidgetItem();
  newServer->setText(0, ui_->serverName->text());
  ui_->weatherDataTree->topLevelItem(0)->addChild(newServer);
//  ui_->weatherDataTree->addTopLevelItem(newServer);
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
  hidePanels(ui_->ServerGroupPage);

  serverTabChanged_ = false;
  dataGridSeriesChanged_ = false;
  dataPointDiffSeriesChanged_ = false;
  dataPointSeriesChanged_ = false;

// Set visible false on server buttons
  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

}

void ConfigAppWeatherTab::isValidConnection()
{
  terrama2::gui::core::ConnectionType serviceType = (terrama2::gui::core::ConnectionType)ui_->connectionProtocol->currentIndex();
  if (serviceType == terrama2::gui::core::FTP)
      terrama2::gui::core::checkFTPConnection(ui_->connectionAddress->text(),
                                              ui_->connectionPort->text().toInt(),
                                              ui_->serverDataBasePath->text(),
                                              ui_->connectionUserName->text(),
                                              ui_->connectionPassword->text());
  else if (serviceType == terrama2::gui::core::WEBSERVICE)
      terrama2::gui::core::checkServiceConnection(ui_->connectionAddress->text(),
                                                  ui_->connectionPort->text().toInt(),
                                                  ui_->connectionUserName->text(),
                                                  ui_->connectionPassword->text());
  else if (serviceType == terrama2::gui::core::LOCALFILES)
    terrama2::gui::core::checkLocalFilesConnection(ui_->serverDataBasePath->text());
}

void ConfigAppWeatherTab::showDataSeries(bool state)
{
  ui_->exportServerBtn->setVisible(state);
  ui_->updateServerBtn->setVisible(state);
  ui_->serverDeleteBtn->setVisible(state);

  (state) ? ui_->groupBox_25->show() : ui_->groupBox_25->hide();
}

void ConfigAppWeatherTab::hidePanels(QWidget *except)
{
  ui_->ServerGroupPage->hide();
  ui_->ServerPage->hide();
  ui_->DataGridPage->hide();
  ui_->DataPointPage->hide();
  ui_->DataPointDiffPage->hide();

  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(false);
  ui_->cancelBtn->setEnabled(true);

  except->show();
}

void ConfigAppWeatherTab::onEnteredWeatherTab()
{
  // Set visible the buttons
  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(false);
  ui_->cancelBtn->setEnabled(true);

  ui_->ServerGroupPage->hide();
  ui_->ServerPage->show();
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
    isValidConnection();
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
  hidePanels(ui_->DataGridPage);
}

void ConfigAppWeatherTab::onInsertPointBtnClicked()
{
  hidePanels(ui_->DataPointPage);
}

void ConfigAppWeatherTab::onInsertPointDiffBtnClicked()
{
  hidePanels(ui_->DataPointDiffPage);
}

void ConfigAppWeatherTab::onDeleteServerClicked()
{
  try
  {
    if (ui_->weatherDataTree->selectedItems().isEmpty())
      throw terrama2::Exception() << terrama2::ErrorDescription(tr("Please select a data provider to remove"));

    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
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
