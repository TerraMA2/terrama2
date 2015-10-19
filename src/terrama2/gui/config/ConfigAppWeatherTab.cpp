// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "ConfigApp.hpp"
#include "ProjectionDialog.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/Utils.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/DataProviderDAO.hpp"

// SubTabs
#include "ConfigAppWeatherServer.hpp"
#include "ConfigAppWeatherGrid.hpp"

// QT
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonObject>

ConfigAppWeatherTab::ConfigAppWeatherTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  ui_->weatherDataTree->header()->hide();
  ui_->weatherDataTree->setCurrentItem(ui_->weatherDataTree->topLevelItem(0));
  ui_->weatherDataTree->setExpanded(ui_->weatherDataTree->model()->index(0, 0), true);

  connect(ui_->saveBtn, SIGNAL(clicked()), SLOT(onSaveRequested()));
  connect(ui_->cancelBtn, SIGNAL(clicked()), SLOT(onCancelRequested()));
  connect(ui_->importServerBtn, SIGNAL(clicked()), SLOT(onImportServer()));

  // Bind the data series type with respective group view
  connect(ui_->projectionGridBtn, SIGNAL(clicked()), this, SLOT(onProjectionClicked()));
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->serverInsertPointDiffBtn, SIGNAL(clicked()), SLOT(onInsertPointDiffBtnClicked()));
  connect(ui_->serverDeleteBtn, SIGNAL(clicked()), SLOT(onDeleteServerClicked()));
  connect(ui_->exportServerBtn, SIGNAL(clicked()), SLOT(onExportServerClicked()));
  connect(ui_->weatherDataTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
          SLOT(onWeatherDataTreeClicked(QTreeWidgetItem*)));

  // Tabs
  QSharedPointer<ConfigAppWeatherServer> serverTab(new ConfigAppWeatherServer(app, ui));
  QSharedPointer<ConfigAppWeatherGridTab> gridTab(new ConfigAppWeatherGridTab(app, ui));
  subTabs_.append(serverTab);
  subTabs_.append(gridTab);

  // Lock for cannot allow multiple selection
  ui_->weatherDataTree->setSelectionMode(QAbstractItemView::SingleSelection);

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

  // Delete all children from remote services
  clearList();

  // Disable series button
  showDataSeries(false);
  hideDataSetButtons();
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
  std::auto_ptr<te::da::DataSet> dataProviders = ds->getDataSet("terrama2.data_provider");

  showDataSeries(false);

  clearList();

  while(dataProviders->moveNext())
  {
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("server"));
    item->setText(0, QString(dataProviders->getAsString(1).c_str()));
    ui_->weatherDataTree->topLevelItem(0)->addChild(item);

    // load data set
    std::auto_ptr<te::da::DataSet> dataSet = ds->getDataSet("terrama2.dataset");

    while (dataSet->moveNext() && dataSet->getInt32(4) == dataProviders->getInt32(0))
    {
      QTreeWidgetItem* subItem = new QTreeWidgetItem;
      subItem->setText(0, QString(dataSet->getAsString("name").c_str()));
      switch (terrama2::core::ToDataSetKind(dataSet->getInt32(5)))
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
    }
  }

  for(auto tab: subTabs_)
    tab->load();
}

bool ConfigAppWeatherTab::dataChanged()
{
  for(const auto tab: subTabs_) {
    if (tab->isActive())
      return true;
  }
  return false;
}

bool ConfigAppWeatherTab::validate()
{
  for(const auto tab: subTabs_)
    if (tab->isActive() && tab->dataChanged())
      return tab->validate();
  return false;
}

void ConfigAppWeatherTab::save()
{
  for(auto tab: subTabs_)
  {
    if (tab->isActive())
    {
      if (!tab->validate())
        throw terrama2::Exception() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
      tab->save();
      showDataSeries(true);
      discardChanges(false);
      QMessageBox::information(app_, tr("TerraMA2"), tr("Save successfully"));
    }
  }
}

void ConfigAppWeatherTab::discardChanges(bool restore_data)
{
  for(auto tab: subTabs_)
    if (tab->dataChanged())
      tab->discardChanges(restore_data);

  hidePanels(ui_->ServerGroupPage);

  ui_->saveBtn->setVisible(false);
  ui_->cancelBtn->setVisible(false);

}

void ConfigAppWeatherTab::showDataSeries(bool state)
{
  ui_->exportServerBtn->setVisible(state);
  ui_->updateServerBtn->setVisible(state);
  ui_->serverDeleteBtn->setVisible(state);

  (state) ? ui_->dataSeriesBtnGroupBox->show() : ui_->dataSeriesBtnGroupBox->hide();
}

void ConfigAppWeatherTab::hideDataSetButtons()
{
  for(QPushButton* button: ui_->dataSeriesBtnGroupBox->findChildren<QPushButton*>())
    button->setVisible(false);
}

void ConfigAppWeatherTab::hidePanels(QWidget* except)
{
  ui_->saveBtn->setVisible(true);
  ui_->cancelBtn->setVisible(true);

  ui_->saveBtn->setEnabled(true);
  ui_->cancelBtn->setEnabled(true);

  ui_->weatherPageStack->setCurrentWidget(except);
}

void ConfigAppWeatherTab::onImportServer()
{
  QString file = QFileDialog::getOpenFileName(app_, tr("Choose file"), ".", tr("TerraMA2 ( *.terrama2"));
  if (!file.isEmpty())
  {
    QMessageBox::information(app_, tr("TerraMA2 Server"), tr("Opened Server File"));
  }
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

    hidePanels(ui_->ServerGroupPage);

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

        std::string sql = "SELECT * FROM terrama2.data_provider WHERE name = '";
        sql += selectedItem->text(0).toStdString() + "'";

        std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
        std::auto_ptr<te::da::DataSet> dataSet = dataSource->query(sql);

        if (dataSet->size() != 1)
          throw terrama2::Exception() << terrama2::ErrorDescription(tr("It cannot be a valid provider selected."));

        dataSet->moveFirst();

        displayOperationButtons(true);
        changeTab(*(subTabs_[0].data()), *ui_->ServerPage);

        subTabs_[0]->setSelectedData(selectedItem->text(0));
        ui_->serverName->setText(QString(dataSet->getAsString(1).c_str()));
        ui_->serverDescription->setText(QString(dataSet->getAsString(2).c_str()));
        ui_->connectionProtocol->setCurrentIndex(dataSet->getInt32(3));
        ui_->connectionAddress->setText(QString(dataSet->getAsString(4).c_str()));
        ui_->serverActiveServer->setChecked(dataSet->getBool(5));

        subTabs_[0]->load();

        ui_->updateDataGridBtn->hide();
        ui_->exportDataGridBtn->hide();
        ui_->gridFormatDataDeleteBtn->hide();
      }
      else {

        std::string sql = "SELECT * FROM terrama2.dataset WHERE name = '";
        sql += selectedItem->text(0).toStdString() + "'";

        std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
        std::auto_ptr<te::da::DataSet> dataSet = dataSource->query(sql);

        if (dataSet->size() != 1)
          throw terrama2::Exception() << terrama2::ErrorDescription(tr("It cannot be a valid dataset selected."));

        dataSet->moveFirst();

        displayOperationButtons(true);
        changeTab(*(subTabs_[1].data()), *ui_->DataGridPage);
        subTabs_[1]->setSelectedData(selectedItem->text(0));

        ui_->gridFormatDataName->setText(dataSet->getAsString("name").c_str());
        hideDataSetButtons();
        showDataSeries(false);
        ui_->dataSeriesBtnGroupBox->setVisible(true);
        ui_->updateDataGridBtn->setVisible(true);
        ui_->exportDataGridBtn->setVisible(true);
        ui_->gridFormatDataDeleteBtn->setVisible(true);
      }
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

void ConfigAppWeatherTab::displayOperationButtons(bool state)
{
  ui_->saveBtn->setVisible(state);
  ui_->cancelBtn->setVisible(state);

  ui_->saveBtn->setEnabled(state);
  ui_->cancelBtn->setEnabled(state);
}

void ConfigAppWeatherTab::changeTab(ConfigAppTab &sender, QWidget &widget) {
  for(auto tab: subTabs_)
  {
    if (tab->isActive())
    {
      if (tab->dataChanged())
      {
        // todo: ASK FOR would like to save
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
      }
      tab->discardChanges(false);
      tab->setActive(false);
    }
  }
  sender.setActive(true);
  ui_->weatherPageStack->setCurrentWidget(&widget);
  hidePanels(&widget);
}
