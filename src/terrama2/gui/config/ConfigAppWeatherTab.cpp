// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/Utils.hpp"

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

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

  ui_->weatherDataTree->clear();

  // Call the default configuration
  load();
}

ConfigAppWeatherTab::~ConfigAppWeatherTab()
{

}

void ConfigAppWeatherTab::load()
{
  // Disable series button
  showDataSeries(false);

  // Connect to database and list the values

}

bool ConfigAppWeatherTab::dataChanged()
{
  return serverTabChanged_ || dataGridSeriesChanged_ || dataPointDiffSeriesChanged_ || dataPointSeriesChanged_;
}

bool ConfigAppWeatherTab::validate()
{
  if (ui_->serverName->text().isEmpty())
  {
    ui_->serverName->setFocus();
    return false;
  }

  QTreeWidgetItemIterator it(ui_->weatherDataTree);
  while(*it)
  {
    if ((*it)->text(0) == ui_->serverName->text())
    {
      ui_->serverName->setFocus();
      throw terrama2::Exception() << terrama2::ErrorDescription(tr("Invalid server name. It is already in use"));
    }

    ++it;
  }

  isValidConnection();

  return true;
}

void ConfigAppWeatherTab::save()
{
  if (!validate())
  {
    throw terrama2::Exception() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
  }
  // Apply save process


  std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/core/data/project.json");

//  std::shared_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("POSTGIS");
  terrama2::core::ApplicationController::getInstance().loadProject(path);
  std::shared_ptr<te::da::DataSource> ds = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSet> dset = ds->query("create table abacate (id serial primary key, nome varchar(200))");

  // TODO: save in database
  QTreeWidgetItem* newServer = new QTreeWidgetItem();
  newServer->setText(0, ui_->serverName->text());
  ui_->weatherDataTree->addTopLevelItem(newServer);

  showDataSeries(true);

  discardChanges(false);
  QMessageBox::information(app_, tr("TerraMA2"), tr("Save successfully"));
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
  ui_->cancelBtn->setEnabled(false);

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
