#include "ConfigAppWeatherPcd.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "ProjectionDialog.hpp"
#include "PcdDialog.hpp"

// Qt
#include <QMessageBox>
#include <QTableWidgetItem>


ConfigAppWeatherPcd::ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->pointFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onDataPointBtnClicked()));
  connect(ui_->projectionPointBtn, SIGNAL(clicked()), SLOT(onProjectionClicked()));

  connect(ui_->btnPointPCDInsertFileNameLocation, SIGNAL(clicked()), SLOT(onPCDInsertFileClicked()));
  connect(ui_->btnPointPCDDeleteFileNameLocation, SIGNAL(clicked()), SLOT(onPCDRemoveClicked()));
  connect(ui_->tblPointPCDFileNameLocation, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(onPCDTableDoubleClicked(QTableWidgetItem*)));
//  ui_->pointFormatDataType->setEnabled(false);
//  ui_->projectionPointBtn->setEnabled(false);
//  ui_->pointFormatDataInfluenceCmb->setEnabled(false);
//  ui_->pointFormatDataThemeCmb->setEnabled(false);
//  ui_->pointFormatDataFrequency->setEnabled(false);
//  ui_->pointFormatDataAttributeCmb->setEnabled(false);
//  ui_->pointFormatDataDescription->setEnabled(false);
//  ui_->pointFormatDataTimeZoneCmb->setEnabled(false);
//  ui_->pointFormatDataPrefix->setEnabled(false);
//  ui_->pointFormatDataUnit->setEnabled(false);
//  ui_->pointFormatSurfaceConfigBtn->setEnabled(false);
//  ui_->pointFormatDataPath->setEnabled(false);
  ui_->pointFormatDataMask->setEnabled(false);
  ui_->pointFormatDataFormat->setEnabled(false);
//  ui_->btnPointPCDInsertFileNameLocation->setEnabled(false);
//  ui_->pointParamsGbx->setEnabled(false);

  ui_->updateDataPointBtn->setEnabled(false);
  ui_->exportDataPointBtn->setEnabled(false);

  // Clear the pcd table
  while(ui_->tblPointPCDFileNameLocation->rowCount() > 0)
    ui_->tblPointPCDFileNameLocation->removeRow(0);
}

ConfigAppWeatherPcd::~ConfigAppWeatherPcd()
{

}

void ConfigAppWeatherPcd::load()
{
  QMenu* menuMask = new QMenu(tr("Máscaras"));
  menuMask->addAction(tr("%a - ano com dois digitos"));
  menuMask->addAction(tr("%A - ano com quatro digitos"));
  menuMask->addAction(tr("%d - dia com dois digitos"));
  menuMask->addAction(tr("%M - mes com dois digitos"));
  menuMask->addAction(tr("%h - hora com dois digitos"));
  menuMask->addAction(tr("%m - minuto com dois digitos"));
  menuMask->addAction(tr("%s - segundo com dois digitos"));
  menuMask->addAction(tr("%. - um caracter qualquer"));

  ui_->filePointDiffMaskBtn->setMenu(menuMask);
  ui_->filePointDiffMaskBtn->setPopupMode(QToolButton::InstantPopup);

  // connecting the menumask to display mask field values
  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(onMenuMaskClicked(QAction*)));
}

bool ConfigAppWeatherPcd::validate()
{
  if (ui_->pointFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The PCD Name cannot be empty"));
  }
  //TODO: validate correctly all fields
  return true;
}

void ConfigAppWeatherPcd::save()
{
  terrama2::core::DataProvider provider = app_->getWeatherTab()->getProvider(ui_->weatherDataTree->currentItem()->text(0).toStdString());
  terrama2::core::DataSet dataset = app_->getWeatherTab()->getDataSet(selectedData_.toStdString());

  dataset.setName(ui_->pointFormatDataName->text().toStdString());
  dataset.setDescription(ui_->pointFormatDataDescription->toPlainText().toStdString());
  dataset.setKind(terrama2::core::DataSet::PCD_TYPE);

  dataset.setStatus(terrama2::core::ToDataSetStatus(ui_->pointFormatStatus->isChecked()));

  te::dt::TimeDuration dataFrequency(ui_->pointFormatDataFrequency->text().toInt(), 0, 0);
  dataset.setDataFrequency(dataFrequency);

  terrama2::core::DataSetItem datasetItem;
  datasetItem.setDataSet(dataset.id());
  datasetItem.setMask(ui_->pointFormatDataMask->text().toStdString());

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
  {
    selectedData_.clear();
    app_->getWeatherTab()->changeTab(*this, *ui_->DataPointPage);
  }
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void ConfigAppWeatherPcd::onDataPointBtnClicked()
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

void ConfigAppWeatherPcd::onProjectionClicked()
{
  ProjectionDialog dialog(app_);
  dialog.exec();
}

void ConfigAppWeatherPcd::onMenuMaskClicked(QAction* action)
{
  ui_->pointDiffFormatDataMask->setText(
        ui_->pointDiffFormatDataMask->text() + action->text().left(2));
}

void ConfigAppWeatherPcd::onPCDInsertFileClicked()
{
  PCD pcd;
  pcdFormCreation(pcd);
}

void ConfigAppWeatherPcd::onPCDRemoveClicked()
{
  int row = ui_->tblPointPCDFileNameLocation->currentRow();
  if (row != -1)
    ui_->tblPointPCDFileNameLocation->removeRow(row);
}

void ConfigAppWeatherPcd::onPCDTableDoubleClicked(QTableWidgetItem* item)
{
  if (item != nullptr)
  {
    PCD pcd;
    pcd.file = ui_->tblPointPCDFileNameLocation->item(item->row(), 0)->text();
    pcd.latitude = ui_->tblPointPCDFileNameLocation->item(item->row(), 1)->text();
    pcd.longitude = ui_->tblPointPCDFileNameLocation->item(item->row(), 2)->text();
//    pcd.active = ui_->tblPointPCDFileNameLocation->item(item->row(), 3)->text();
    pcdFormCreation(pcd);
  }
}

void ConfigAppWeatherPcd::pcdFormCreation(PCD& pcd)
{
  PcdDialog dialog(app_);
  //TODO: fill the form with data and then, populate the table widget
  dialog.fill(pcd);
  if (dialog.exec() == QDialog::Accepted)
  {
    dialog.fillObject(pcd);
    QTableWidgetItem* item = new QTableWidgetItem(pcd.file);

    int line = ui_->tblPointPCDFileNameLocation->rowCount();
    ui_->tblPointPCDFileNameLocation->insertRow(line);
    ui_->tblPointPCDFileNameLocation->setItem(line, 0, item);

    item = new QTableWidgetItem();
    item->setText(pcd.latitude);
    ui_->tblPointPCDFileNameLocation->setItem(line, 1, item);

    item = new QTableWidgetItem();
    item->setText(pcd.longitude);
    ui_->tblPointPCDFileNameLocation->setItem(line, 2, item);

    item = new QTableWidgetItem();
    item->setText(pcd.active ? tr("true") : tr("false"));
    ui_->tblPointPCDFileNameLocation->setItem(line, 3, item);
  }
}
