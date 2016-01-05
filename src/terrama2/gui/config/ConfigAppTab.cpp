// TerraMA2
#include "ConfigAppTab.hpp"
#include "ConfigApp.hpp"
#include "Exception.hpp"

// TerraMA2 Logger
#include "../../core/Logger.hpp"


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

void ConfigAppTab::validateAndSaveChanges()
{
  try
  {
    save();
  }
  catch(const terrama2::Exception& e)
  {
    const QString* msg = boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_ERROR() << *msg;
    QMessageBox::warning(app_, tr("TerraMA2 - Error"), *msg);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_FATAL() << e.what();
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

void ConfigAppTab::onSubTabEdited()
{
  changed_ = true;
}

bool ConfigAppTab::isActive() const
{
  return active_;
}

void ConfigAppTab::setActive(bool state)
{
  active_ = state;
}

bool ConfigAppTab::dataChanged()
{
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
      throw terrama2::gui::config::DataSetError() << terrama2::ErrorDescription(tr("Invalid dataset selected"));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(app_, tr("TerraMA2"),
                                  tr("Would you like to remove dataset?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                  QMessageBox::Yes);

    return reply == QMessageBox::Yes;
  }
  return false;
}

void ConfigAppTab::checkMask(const QString mask)
{
  if (mask.trimmed().isEmpty())
    throw terrama2::gui::FieldError() << terrama2::ErrorDescription(tr("Data mask is invalid."));

  if (mask.contains("%h") || mask.contains("%m") || mask.contains("%s"))
  {
    if ((!mask.contains("%a") && !mask.contains("%A")) || !mask.contains("%M") || !mask.contains("%d"))
      throw terrama2::gui::ValueError() << terrama2::ErrorDescription(tr("An mask with time must have date typed"));
  }
}
