/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/gui/admin/AdminAppCollectTab.cpp

 \brief Class responsible for handling CollectTab actions

  \author Evandro Delatin

*/


// TerraMA2
#include "AdminApp.hpp"
#include "AdminAppCollectTab.hpp"  
#include "ui_AdminAppForm.h"
#include "Exception.hpp"
#include <QJsonObject>
#include <QString>

terrama2::gui::admin::AdminAppCollectTab::AdminAppCollectTab(terrama2::gui::admin::AdminApp* app, Ui::AdminAppForm* ui)
 :AdminAppTab(app, ui)
{

} 

terrama2::gui::admin::AdminAppCollectTab::~AdminAppCollectTab()
{
  
}

QMap<QString, QJsonObject> terrama2::gui::admin::AdminAppCollectTab::toJson()
{
  QJsonObject collect;

  collect["data_path"] = ui_->aqDirNameLed->text();
  collect["log_file"] = ui_->aqLogFileLed->text();
  collect["connection_timeout"] = ui_->aqTimeoutMinSpb->value() * 60 + ui_->aqTimeoutSecSpb->value();
  collect["address"] = ui_->aqAddressLed->text();
  collect["port"] = ui_->aqPortLed->text().toInt();
  collect["parameters"] = app_->getConfigManager()->getCollection()->params_;
  collect["command"] = app_->getConfigManager()->getCollection()->cmd_;

  QMap<QString, QJsonObject> json;
  json.insert(QString("collector_web_service"), collect);

  return json;
}

bool terrama2::gui::admin::AdminAppCollectTab::load()
{
    return false;
}

bool terrama2::gui::admin::AdminAppCollectTab::validate()
{
    return false;
}
