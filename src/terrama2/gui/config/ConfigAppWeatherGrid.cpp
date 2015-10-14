// TerraMA2
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigApp.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "../../core/DataSet.hpp"
#include "../../core/DataManager.hpp"
#include "../../core/Utils.hpp"

// QT
#include <QMessageBox>


ConfigAppWeatherGridTab::ConfigAppWeatherGridTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : ConfigAppTab(app, ui)
{
  connect(ui_->serverInsertGridBtn, SIGNAL(clicked()), SLOT(onDataGridClicked()));
  connect(ui_->gridFormatDataName, SIGNAL(textEdited(QString)), SLOT(onSubTabChanged()));
}

ConfigAppWeatherGridTab::~ConfigAppWeatherGridTab()
{

}

bool ConfigAppWeatherGridTab::dataChanged()
{
  return active_ && changed_;
}

void ConfigAppWeatherGridTab::load()
{
}

void ConfigAppWeatherGridTab::save()
{
  std::cout << ui_->weatherDataTree->currentItem()->text(0).toStdString() << std::endl;
  terrama2::core::DataManager::getInstance().load();
  terrama2::core::DataProviderPtr dataProvider = terrama2::core::DataManager::getInstance().findDataProvider(
      ui_->weatherDataTree->currentItem()->text(0).toStdString());
  terrama2::core::DataSet::Kind kind = terrama2::core::DataSet::GRID_TYPE;
  std::string name = ui_->gridFormatDataName->text().toStdString();

  terrama2::core::DataSetPtr dataSetPtr(new terrama2::core::DataSet(dataProvider, name, kind));
  dataSetPtr->setDescription(ui_->gridFormatDataDescription->toPlainText().toStdString());
  dataSetPtr->setStatus(terrama2::core::BoolToDataSetStatus(ui_->gridFormatStatus->isChecked()));

  terrama2::core::DataManager::getInstance().add(dataSetPtr);

  QTreeWidgetItem* item = new QTreeWidgetItem;
  item->setIcon(0, QIcon::fromTheme("grid"));
  item->setText(0, ui_->gridFormatDataName->text());
  ui_->weatherDataTree->currentItem()->addChild(item);
}

void ConfigAppWeatherGridTab::discardChanges(bool restore_data)
{
  if (restore_data)
  {

  }

  for(QLineEdit* widget: ui_->DataGridPage->findChildren<QLineEdit*>())
    widget->clear();

  changed_ = false;
}

bool ConfigAppWeatherGridTab::validate()
{
  if (ui_->gridFormatDataName->text().trimmed().isEmpty())
  {
    ui_->gridFormatDataName->setFocus();
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("The Data Set Item name cannot be empty."));
  }

  // TODO: Projection and Filter
  return true;
}

void ConfigAppWeatherGridTab::onDataGridClicked()
{
  if (ui_->weatherDataTree->currentItem() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent() != nullptr &&
      ui_->weatherDataTree->currentItem()->parent()->parent() == nullptr)
    app_->getWeatherTab()->changeTab(*this, *ui_->DataGridPage);
  else
    QMessageBox::warning(app_, tr("TerraMA2 Data Set"), tr("Please select a data provider to the new dataset"));
}

void ConfigAppWeatherGridTab::onSubTabChanged()
{
  changed_ = true;
}