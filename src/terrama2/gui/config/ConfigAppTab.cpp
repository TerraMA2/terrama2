// TerraMA2
#include "ConfigAppTab.hpp"
#include "ConfigApp.hpp"
#include "Exception.hpp"


#include "../../core/DataSet.hpp"

// QT
#include <QMessageBox>

ConfigAppTab::ConfigAppTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : QObject(app), app_(app), ui_(ui), active_(false), changed_(false)
{

}

ConfigAppTab::~ConfigAppTab()
{

}

void ConfigAppTab::load(const terrama2::core::DataSet& dataset)
{

}

void ConfigAppTab::validateAndSaveChanges()
{
  try
  {
    save();
  }
  catch(const terrama2::Exception& e)
  {
    const QString* msg = boost::get_error_info<terrama2::ErrorDescription>(e);
    QMessageBox::critical(app_, tr("TerraMA2 - Error"), *msg);
  }
  catch(const std::exception& e)
  {
    QMessageBox::critical(app_, tr("TerraMA2 - Error"), e.what());
  }
}

void ConfigAppTab::askForChangeTab(const int& index)
{
  if (!dataChanged())
  {
    app_->setCurrentTabIndex(index);
    return;
  }

  // Used here to avoid change tab
  ui_->mainTabWidget->setCurrentIndex(app_->getCurrentTabIndex());

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(app_, tr("There are modifications not saved yet"),
                                      tr("Would you like to save them?"),
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                      QMessageBox::Save);

  bool saved = (reply == QMessageBox::Save);
  app_->setCurrentTabIndex(index);
  if (saved || reply == QMessageBox::Discard)
  {
    discardChanges(saved);

    ui_->mainTabWidget->setCurrentIndex(index);
  }
}

void ConfigAppTab::onSaveRequested()
{
  if(dataChanged())
    validateAndSaveChanges();
}

void ConfigAppTab::onCancelRequested()
{
  discardChanges(true);
}

void ConfigAppTab::onFilterClicked()
{

}

void ConfigAppTab::onProjectionClicked()
{

}

bool ConfigAppTab::isActive() const
{
  return active_;
}

void ConfigAppTab::setActive(bool state)
{
  active_ = state;
}

bool ConfigAppTab::dataChanged() {
  return active_ && changed_;
}

void ConfigAppTab::setChanged(bool state)
{
  changed_ = state;
}

void ConfigAppTab::setSelectedData(const QString selectedData)
{
  selectedData_ = selectedData;
}

bool ConfigAppTab::removeDataSet(const terrama2::core::DataSet& dataset)
{
  QTreeWidgetItem* currentItem = ui_->weatherDataTree->currentItem();
  if (currentItem != nullptr && currentItem->parent() != nullptr && currentItem->parent()->parent() != nullptr)
  {
    if (dataset.id() == 0)
      throw terrama2::gui::DataSetError() << terrama2::ErrorDescription(tr("Invalid dataset selected"));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(app_, tr("TerraMA2"),
                                  tr("Would you like to remove dataset?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                  QMessageBox::Yes);

    return reply == QMessageBox::Yes;
  }
  return false;
}
