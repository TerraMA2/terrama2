// TerraMA2
#include "ConfigAppTab.hpp"
#include "Exception.hpp"

// QT
#include <QMessageBox>

ConfigAppTab::ConfigAppTab(ConfigApp* app, Ui::ConfigAppForm* ui)
  : QObject(app), app_(app), ui_(ui)
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
    delete msg;
  }
  catch(const std::exception& e)
  {
    QMessageBox::critical(app_, tr("TerraMA2 - Error"), e.what());
  }
}

bool ConfigAppTab::verifyAndEnableChange(bool restore)
{
  return false;
}

QString ConfigAppTab::verifyAndEnableChangeMsg()
{
  return QString("");
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
