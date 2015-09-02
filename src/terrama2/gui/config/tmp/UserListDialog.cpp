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
  \file terrama2/gui/config/UserListDialog.cpp

  \brief Definition of Class UserListDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Raphael Meloni
*/

// STL
#include <assert.h>

// TerraMA2  
#include "UserListDialog.hpp"

//! Construtor
UserListDialog::UserListDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  // Connect
  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject())); 
}

//! Destrutor
UserListDialog::~UserListDialog()
{
}

//! Preenche valores da WidgetList conforme lista de usuários selecionadas
bool UserListDialog::setFields(const std::vector<struct wsUser>& users, UserAnalysisList* userList)
{
  QListWidgetItem* item;
  wsUser user;
  bool ret = false;
  listWidgetUser->clear();

  for (int i=0; i<userList->size(); i++)
  {
    user = userList->at(i)->data();

	if (!findUser(users, user))
	{
		QString ico = ":/data/icons/usuario.png";
		item = new QListWidgetItem(QIcon(ico), QString::fromStdString(user.login));
		item->setData(Qt::UserRole, QString(user.login.c_str()));

		listWidgetUser->addItem(item);
		ret = true;
	}
  }

  return ret;
}

//! Procura o usuário na lista de usuários já selecionados
bool UserListDialog::findUser(const std::vector<struct wsUser>& users, const wsUser& user)
{
	bool ret = false;
	wsUser userAt;

	for (unsigned i=0; i<users.size() && !ret; i++)
	{
		userAt = users.at(i);
		ret = (userAt.login == user.login);
	}
	
	return ret;
}

//! Preenche lista de usuários selecionados conforme WidgetList
void UserListDialog::getFields(QList<std::string>& newIDSelectedAnalyses, bool& changed)
{
  QListWidgetItem* item;

  newIDSelectedAnalyses.clear();

  changed = (listWidgetUser->selectedItems().size()>0);

  for (int i=0; i<listWidgetUser->selectedItems().size(); i++)
  {
	  item = listWidgetUser->selectedItems().at(i);
	  newIDSelectedAnalyses.push_back(item->data(Qt::UserRole).toString().toStdString());
  }
}
