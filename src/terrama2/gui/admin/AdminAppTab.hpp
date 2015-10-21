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
  \file terrama2/gui/admin/AdminAppTab.hpp

  \brief 

  \author Evandro Delatin
  \author Raphael Willian da Costa
  
*/

#ifndef __TERRAMA2_INTERNAL_GUI_ADMIN_ADMINAPPTAB_HPP__
#define __TERRAMA2_INTERNAL_GUI_ADMIN_ADMINAPPTAB_HPP__

// TerraMA2
#include "AdminApp.hpp"
#include <QMap>

class QJsonObject;
 
class AdminAppTab
{
 public:

//! Constructor
  AdminAppTab(AdminApp* app, Ui::AdminAppForm* ui);

//! Destructor
 virtual ~AdminAppTab();

 virtual bool validate() = 0;
 virtual bool load() = 0;
 virtual QMap<QString, QJsonObject> toJson() = 0;

 protected:

  AdminApp* app_; //!< Main Window Administration
  Ui::AdminAppForm* ui_;
};

#endif // __TERRAMA2_INTERNAL_GUI_ADMIN_ADMINAPPTAB_HPP__
