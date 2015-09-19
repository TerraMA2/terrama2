// TerraMA2
#include "ConfigAppTab.hpp"
#include "Exception.hpp"

// QT
#include <QMessageBox>

ConfigAppTab::ConfigAppTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : QObject(app), app_(app), ui_(ui), changed_(false)
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
    QMessageBox::critical(app_, tr("TerraMA2 - Error"), *msg);
  }
  catch(const std::exception& e)
  {
    QMessageBox::critical(app_, tr("TerraMA2 - Error"), e.what());
  }
}

void ConfigAppTab::askForChangeTab(const int index)
{
  if (!dataChanged())
    return;

  // Used here to avoid change tab
  ui_->mainTabWidget->setCurrentIndex(app_->getCurrentTabIndex());

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(app_, tr("There are modifications not saved yet"),
                                      tr("Would you like to save them?"),
                                      QMessageBox::Save | QMessageBox::Discard,
                                      QMessageBox::Save);

  if (reply == QMessageBox::Save)
  {
    app_->setCurrentTabIndex(index);
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
