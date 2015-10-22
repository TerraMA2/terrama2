#include "ConfigAppWeatherPcd.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"

// Qt
#include <QMessageBox>


ConfigAppWeatherPcd::ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->pointFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onDataPointBtnClicked()));
}

ConfigAppWeatherPcd::~ConfigAppWeatherPcd()
{

}

void ConfigAppWeatherPcd::load()
{

}

bool ConfigAppWeatherPcd::validate()
{
  if (ui_->pointFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Name is invalid"));
  }
  //TODO: validate correctly all fields
  return true;
}

void ConfigAppWeatherPcd::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  dataset.setName(ui_->pointFormatDataName->text().toStdString());
  dataset.setKind(terrama2::core::DataSet::PCD_TYPE);
  //TODO: add checkbox in UI for data series status
  dataset.setStatus(terrama2::core::DataSet::ACTIVE);
  if (dataset.id() > 0)
  {
    app_->getClient()->updateDataSet(dataset);
    app_->getWeatherTab()->refreshList(ui_->weatherDataTree->currentItem(), selectedData_, ui_->pointFormatDataName->text());
    selectedData_ = ui_->pointFormatDataName->text();
  }
  else
  {
    dataset.setProvider(provider.id());
    app_->getClient()->addDataSet(dataset);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, QIcon::fromTheme("pcd"));
    item->setText(0, ui_->pointFormatDataName->text());
    ui_->weatherDataTree->currentItem()->addChild(item);
  }
  app_->getWeatherTab()->addCachedDataSet(dataset);
  changed_ = false;
}

void ConfigAppWeatherPcd::discardChanges(bool restore_data)
{
  for(QLineEdit* widget: ui_->DataPointPage->findChildren<QLineEdit*>())
    widget->clear();
  changed_ = false;
}

void ConfigAppWeatherPcd::onInsertPointBtnClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
    app_->getWeatherTab()->changeTab(*this, *ui_->DataPointPage);
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void ConfigAppWeatherPcd::onDataPointBtnClicked()
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem != nullptr && currentItem->parent() != nullptr && currentItem->parent()->parent() != nullptr)
  {
    // delete from db
    try
    {
      terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(currentItem->text(0).toStdString());

      if (dataset.id() == 0 || dataset.kind() != terrama2::core::DataSet::PCD_TYPE)
        throw terrama2::gui::DataSetError() << terrama2::ErrorDescription(tr("Invalid PCD dataset selected"));

      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(app_, tr("TerraMA2"),
                                    tr("Would you like to try save before cancel?"),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                    QMessageBox::Yes);
      if (reply == QMessageBox::Yes)
      {
        app_->getClient()->removeDataSet(dataset.id());
        app_->getWeatherTab()->removeCachedDataSet(dataset);

        QMessageBox::warning(app_, tr("TerraMA2"), tr("DataSet PCD successfully removed!"));
        delete currentItem;
      }
    }
    catch(const terrama2::Exception& e)
    {
      const QString* message = boost::get_error_info<terrama2::ErrorDescription>(e);
      QMessageBox::warning(app_, tr("TerraMA2"), *message);
    }
  }
}
