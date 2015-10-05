// TerraMA2
#include "AdminApp.hpp"
#include "AdminAppCollectTab.hpp"  
#include "ui_AdminAppForm.h"
#include "Exception.hpp"
#include <QJsonObject>
#include <QString>

AdminAppCollectTab::AdminAppCollectTab(AdminApp* app, Ui::AdminAppForm* ui)
 :AdminAppTab(app, ui)
{

} 

AdminAppCollectTab::~AdminAppCollectTab()
{
  
}

QMap<QString, QJsonObject> AdminAppCollectTab::toJson()
{
  QJsonObject collect;

  collect["data_path"] = ui_->aqDirNameLed->text();
  collect["log_file"] = ui_->aqLogFileLed->text();
  collect["connection_timeout"] = ui_->aqTimeoutMinSpb->value() * 60 + ui_->aqTimeoutSecSpb->value();
  collect["connection_timeout"] = ui_->aqTimeoutMinSpb->text();
  collect["address"] = ui_->aqAddressLed->text();
  collect["port"] = ui_->aqPortLed->text();

  QMap<QString, QJsonObject> json;
  json.insert(QString("collector_web_service"), collect);

  return json;
}

bool AdminAppCollectTab::load()
{
    return false;
}

bool AdminAppCollectTab::validate()
{
    return false;
}
