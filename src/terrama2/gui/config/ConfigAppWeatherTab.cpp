// TerraMA2
#include "ConfigAppWeatherTab.hpp"
#include "ConfigApp.hpp"
#include "ProjectionDialog.hpp"
#include "Exception.hpp"
#include "../core/Utils.hpp"
#include "../../core/Utils.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/DataProviderDAO.hpp"

// SubTabs
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
  connect(ui_->serverDeleteBtn, SIGNAL(clicked()), SLOT(onDeleteServerClicked()));
  connect(ui_->exportServerBtn, SIGNAL(clicked()), SLOT(onExportServerClicked()));
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
}

ConfigAppWeatherTab::~ConfigAppWeatherTab()
{

}

void ConfigAppWeatherTab::clearList()
{
  qDeleteAll(ui_->weatherDataTree->topLevelItem(0)->takeChildren());
  providers_.clear();
}

void ConfigAppWeatherTab::load()
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
        }
      }
    }
  }
  catch (const terrama2::Exception& e)
  {
    const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::warning(app_, tr("TerraMA2"), *message);
    showDataSeries(false);
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
        throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Could not save. There are empty fields!!"));
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

void ConfigAppWeatherTab::onDeleteServerClicked()
{
  try
  {
    QTreeWidgetItem* selectedItem = ui_->weatherDataTree->currentItem();
    if (selectedItem == nullptr || selectedItem->parent() == nullptr)
      throw terrama2::gui::ValueError() << terrama2::ErrorDescription(tr("Please select a data provider to remove"));

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
      //FIXME: It throws exception at Server side and shutdown both
      app_->getClient()->removeDataProvider(cachedProvider.id());
      QMessageBox::information(app_, tr("TerraMA2"), tr("Data provider has been successfully removed."));

      ui_->weatherDataTree->removeItemWidget(ui_->weatherDataTree->currentItem(), 0);
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      QMessageBox::warning(app_, tr("TerraMA2 Data Provider Remove Error"), *message);
    }

    hidePanels(ui_->ServerGroupPage);

    delete selectedItem;
    displayOperationButtons(false);
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
          throw terrama2::gui::DataProviderError() << terrama2::ErrorDescription(tr("It cannot be a valid provider selected."));

        displayOperationButtons(true);
        changeTab(*(subTabs_[0].data()), *ui_->ServerPage);

        subTabs_[0]->setSelectedData(selectedItem->text(0));
        ui_->serverName->setText(QString(provider.name().c_str()));
        ui_->serverDescription->setText(QString(provider.description().c_str()));

        ui_->connectionAddress->setText(QString(provider.uri().c_str()));

        ui_->connectionProtocol->setCurrentIndex(provider.kind() - 1);
        ui_->serverActiveServer->setChecked(terrama2::core::ToBool(provider.status()));

        subTabs_[0]->load();

        ui_->updateDataGridBtn->hide();
        ui_->exportDataGridBtn->hide();
        ui_->gridFormatDataDeleteBtn->hide();

      }
      else
      {
        for(auto dataset: datasets_)
        {
          if (dataset.name() == selectedItem->text(0).toStdString())
          {
            displayOperationButtons(true);
            //TODO: generic function giving dataprovider
            switch(dataset.kind())
            {
              case terrama2::core::DataSet::GRID_TYPE:
                changeTab(*(subTabs_[1].data()), *ui_->DataGridPage);
                subTabs_[1]->setSelectedData(selectedItem->text(0));

                ui_->gridFormatDataName->setText(dataset.name().c_str());
                hideDataSetButtons();
                showDataSeries(false);
                ui_->dataSeriesBtnGroupBox->setVisible(true);
                ui_->updateDataGridBtn->setVisible(true);
                ui_->exportDataGridBtn->setVisible(true);
                ui_->gridFormatDataDeleteBtn->setVisible(true);
                break;
              case terrama2::core::DataSet::PCD_TYPE:
                changeTab(*(subTabs_[2].data()), *ui_->DataPointPage);
                subTabs_[2]->setSelectedData(selectedItem->text(0));

                ui_->pointFormatDataName->setText(dataset.name().c_str());
                hideDataSetButtons();
                showDataSeries(false);
                ui_->dataSeriesBtnGroupBox->setVisible(true);
                ui_->updateDataPointBtn->setVisible(true);
                ui_->exportDataPointBtn->setVisible(true);
                ui_->pointFormatDataDeleteBtn->setVisible(true);
                break;
              case terrama2::core::DataSet::OCCURENCE_TYPE:
                changeTab(*(subTabs_[3].data()), *ui_->DataPointDiffPage);
                subTabs_[3]->setSelectedData(selectedItem->text(0));

                ui_->pointDiffFormatDataName->setText(dataset.name().c_str());
                hideDataSetButtons();
                showDataSeries(false);
                ui_->dataSeriesBtnGroupBox->setVisible(true);
                ui_->updateDataPointDiffBtn->setVisible(true);
                ui_->exportDataPointDiffBtn->setVisible(true);
                ui_->serverRemovePointDiffBtn->setVisible(true);
                break;
              default:
              {
                // UNKNOWN
              }
            }
            return;
          }
        }

        throw terrama2::gui::DataSetError() << terrama2::ErrorDescription(tr("It cannot be a valid dataset selected."));

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
      throw terrama2::gui::ValueError() << terrama2::ErrorDescription(tr("Please select a data provider to remove"));

    QJsonObject json;
    json["name"] = ui_->serverName->text();
    json["description"] = ui_->serverDescription->toPlainText();

    json["kind"] = ui_->connectionProtocol->currentText();
    json["address"] = ui_->connectionAddress->text();
    json["port"] = ui_->connectionPort->text();
    json["username"] = ui_->connectionUserName->text();
    json["password"] = ui_->connectionPort->text();

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

QMap<std::string,terrama2::core::DataProvider> ConfigAppWeatherTab::providers()
{
  return providers_;
}

terrama2::core::DataProvider ConfigAppWeatherTab::getProvider(const std::string &identifier)
{
  for(auto& provider: providers_)
    if (provider.name() == identifier)
      return provider;
  return terrama2::core::DataProvider();
}

terrama2::core::DataSet ConfigAppWeatherTab::getDataSet(const std::string& identifier)
{
  for(auto dataset: datasets_)
  {
    if (dataset.name() == identifier)
      return dataset;
  }

  return terrama2::core::DataSet();
}

void ConfigAppWeatherTab::addCachedProvider(const terrama2::core::DataProvider& provider)
{
  providers_.insert(provider.name(), provider);
}

void ConfigAppWeatherTab::removeCachedDataProvider(const terrama2::core::DataProvider &provider)
{
  providers_.take(provider.name());
}

void ConfigAppWeatherTab::addCachedDataSet(const terrama2::core::DataSet &dataset)
{
  datasets_.insert(dataset.name(), dataset);
}

void ConfigAppWeatherTab::removeCachedDataSet(const terrama2::core::DataSet &dataset)
{
  datasets_.take(dataset.name());
}

void ConfigAppWeatherTab::refreshList(QTreeWidgetItem* widget, const QString searchFor, const QString replace)
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
