#include "ConfigAppWeatherPcd.hpp"
#include "Exception.hpp"
#include "../../core/DataProvider.hpp"
#include "../../core/Utils.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "ProjectionDialog.hpp"
#include "PcdDialog.hpp"
#include "SurfaceDialog.hpp"
#include "CollectorRuleDialog.hpp"
#include "../core/Utils.hpp"

// Qt
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>


ConfigAppWeatherPcd::ConfigAppWeatherPcd(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui), luaScript_("")
{
  connect(ui_->serverInsertPointBtn, SIGNAL(clicked()), SLOT(onInsertPointBtnClicked()));
  connect(ui_->pointFormatDataDeleteBtn, SIGNAL(clicked()), SLOT(onDataPointBtnClicked()));
  connect(ui_->projectionPointBtn, SIGNAL(clicked()), SLOT(onProjectionClicked()));

  connect(ui_->btnPointPCDInsertFileNameLocation, SIGNAL(clicked()), SLOT(onPCDInsertFileClicked()));
  connect(ui_->btnPointPCDDeleteFileNameLocation, SIGNAL(clicked()), SLOT(onPCDRemoveClicked()));
  connect(ui_->tblPointPCDFileNameLocation, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(onPCDTableDoubleClicked(QTableWidgetItem*)));
  connect(ui_->pointFormatSurfaceConfigBtn, SIGNAL(clicked()), SLOT(onSurfaceBtnClicked()));
  connect(ui_->btnUpdatePcdCollectionRule, SIGNAL(clicked()), SLOT(onCollectorRuleClicked()));

  ui_->projectionPointBtn->setEnabled(false);

  // export pcd button
  connect(ui_->exportDataPointBtn, SIGNAL(clicked()), SLOT(onPCDExportClicked()));

  ui_->pointFormatDataMask->setEnabled(false);
  ui_->pointFormatDataFormat->setEnabled(false);

  // todo: implement it to list pcd attributes after collection
  ui_->updateDataPointBtn->setEnabled(false);
  ui_->pointFormatDataThemeCmb->setEnabled(false);
  ui_->pointFormatDataAttributeCmb->setEnabled(false);
  ui_->pointFormatDataPrefix->setEnabled(false);
  ui_->pointFormatDataUnit->setEnabled(false);

  ui_->btnPCDWFSConfiguration->setEnabled(false);
  ui_->btnPCDInformationPlane->setEnabled(false);

  tableClean();

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

  ui_->pointFormatDataHour->setValidator(new QIntValidator(ui_->pointFormatDataHour));
  ui_->pointFormatDataMinute->setValidator(new QIntValidator(ui_->pointFormatDataMinute));
  ui_->pointFormatDataSecond->setValidator(new QIntValidator(ui_->pointFormatDataSecond));
}

ConfigAppWeatherPcd::~ConfigAppWeatherPcd()
{

}

void ConfigAppWeatherPcd::load()
{
  tableClean();
}

void ConfigAppWeatherPcd::load(const terrama2::core::DataSet &dataset)
{
  // TODO: fill the combobox with pcd attribute after the first collect
}

bool ConfigAppWeatherPcd::validate()
{
  if (ui_->pointFormatDataName->text().trimmed().isEmpty())
  {
    ui_->pointFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The PCD Name cannot be empty"));
  }

  // TODO: validate all fields

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

  te::dt::TimeDuration dataFrequency(ui_->pointFormatDataHour->text().toInt(),
                                     ui_->pointFormatDataMinute->text().toInt(),
                                     ui_->pointFormatDataSecond->text().toInt());

  dataset.setDataFrequency(dataFrequency);

  terrama2::core::DataSetItem* datasetItem;
  if (dataset.dataSetItems().size() > 0)
    datasetItem = &dataset.dataSetItems()[0];
  else
    datasetItem = new terrama2::core::DataSetItem;

  datasetItem->setKind(terrama2::core::ToDataSetItemKind(ui_->pointFormatDataFormat->currentIndex() + 2));
  datasetItem->setMask(ui_->pointFormatDataMask->text().toStdString());
  datasetItem->setStatus(terrama2::core::DataSetItem::ACTIVE);
  datasetItem->setTimezone(ui_->pointFormatDataTimeZoneCmb->currentText().toStdString());
  datasetItem->setPath(ui_->pointFormatDataPath->text().toStdString());

  //TODO: save the lua script in table
  terrama2::core::DataSet::CollectRule* rule;

  std::map<std::string, std::string> datasetMetadata;
  datasetMetadata["PREFIX"] = ui_->pointFormatDataPrefix->text().toStdString();
  datasetMetadata["UNIT"] = ui_->pointFormatDataUnit->text().toStdString();

  dataset.setMetadata(datasetMetadata);

  if (!luaScript_.trimmed().isEmpty())
  {
    rule = new terrama2::core::DataSet::CollectRule;
    rule->id = 0;
    rule->script = luaScript_.toStdString();
    std::vector<terrama2::core::DataSet::CollectRule> rules(dataset.collectRules());
    rules.push_back(*rule);
    dataset.setCollectRules(rules);
  }

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
    tableClean();
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

void ConfigAppWeatherPcd::onCollectorRuleClicked()
{
  CollectorRuleDialog dialog(app_);
  dialog.fillGUI(luaScript_);
  if (dialog.exec() == QDialog::Accepted)
    dialog.fillObject(luaScript_);
  else
    luaScript_.clear();
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
    pcd.active = ui_->tblPointPCDFileNameLocation->item(item->row(), 3)->text() == tr("true") ? true : false;
    pcdFormCreation(pcd, true);
    }
}

void ConfigAppWeatherPcd::onSurfaceBtnClicked()
{
  SurfaceDialog dialog(app_);
  dialog.exec();
}

void ConfigAppWeatherPcd::onPCDExportClicked()
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem == nullptr || currentItem->parent() == nullptr || currentItem->parent()->parent() == nullptr)
    throw terrama2::gui::DataSetError() << terrama2::ErrorDescription(tr("Please selected a valid PCD dataset"));

  QString path = QFileDialog::getSaveFileName(app_ ,
                                              tr("Type name and where you intend to save this PCD"),
                                              QString(("./" + currentItem->text(0).toStdString() + ".terrama2").c_str()),
                                              tr("TerraMA2 (*.terrama2)"));

  if (path.isEmpty())
    return;

  QJsonObject json;
  json["name"] = ui_->pointFormatDataName->text();
  json["description"] = ui_->pointFormatDataDescription->toPlainText();
  json["path"] = ui_->pointFormatDataPath->text();
  QJsonObject datasetItemArray;
  datasetItemArray["mask"] = ui_->pointFormatDataMask->text();

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
    QMessageBox::warning(app_, tr("TerraMA2 Error"), message);
  }
}

void ConfigAppWeatherPcd::pcdFormCreation(PCD& pcd, bool editing)
{
  PcdDialog dialog(app_);
  dialog.fill(pcd);
  if (dialog.exec() == QDialog::Accepted)
  {
    dialog.fillObject(pcd);

    if (editing)
    {
      int currentLine = ui_->tblPointPCDFileNameLocation->currentRow();
      ui_->tblPointPCDFileNameLocation->item(currentLine, 0)->setText(pcd.file);
      ui_->tblPointPCDFileNameLocation->item(currentLine, 1)->setText(pcd.latitude);
      ui_->tblPointPCDFileNameLocation->item(currentLine, 2)->setText(pcd.longitude);
      ui_->tblPointPCDFileNameLocation->item(currentLine, 3)->setText(pcd.active ? tr("true") : tr("false"));
      return;
    }

    int line = ui_->tblPointPCDFileNameLocation->rowCount();

    QTableWidgetItem* item = new QTableWidgetItem(pcd.file);

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

void ConfigAppWeatherPcd::tableClean()
{
  // Clear the pcd table
  while(ui_->tblPointPCDFileNameLocation->rowCount() > 0)
    ui_->tblPointPCDFileNameLocation->removeRow(0);
}
