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
  \file terrama2/gui/config/UserAnalysis.hpp

  \brief Definition of Class UserAnalysis.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Celso Luiz Ramos Cruz
*/

#ifndef _USERANALYSIS_H_
#define _USERANALYSIS_H_

// QT
#include <QString>
#include <QDate>


#include "soapModAnaliseProxy.h"

//! Classe responsável por representar um usuario e as analises associadas
class UserAnalysis
{
public:
  UserAnalysis();
  UserAnalysis(const wsUser& data);
  ~UserAnalysis();

  //! Retorna o identificador do usuario
  QString login() const { return QString::fromStdString(_data.login); }

  //! Atribui um identificador para o usuario
  void setLogin(QString login) {  _data.login = login.toStdString();  }

  //! Retorna a senha do usuario
  QString password() const { return QString::fromStdString(_data.password); }

  //! Atribui uma senha para o usuario
  void setPassword(QString password)  { _data.password = password.toStdString(); }

  //! Retorna o nome completo do usuário
  QString fullName() const { return QString::fromStdString(_data.fullName); }

  //! Altera o nome completo do usuário
  void setFullName(QString fullName) { _data.fullName = fullName.toStdString(); }

  //! Retorna o e-mail do usuário
  QString email() const { return QString::fromStdString(_data.email); }

  //! Altera o e-mail do usuário
  void setEmail(QString email) { _data.email = email.toStdString(); }

  //! Retorna o celular do usuário
  QString cellPhone() const { return QString::fromStdString(_data.cellular); }

  //! Altera o celular do usuário
  void setCellPhone(QString cellPhone) { _data.cellular = cellPhone.toStdString(); }

  //! Retorna o tamanho do canvas
  wsCanvasSize canvasSize() const { return _data.canvasSize; }

  //! Altera o tamanho do canvas
  void setCanvasSize(wsCanvasSize canvasSize) { _data.canvasSize = canvasSize; }

  //! Retorna a lista das user views
  std::vector<struct wsUserView>& listUserViewsbyUser();

  //! Altera a lista de user views
  void setUserViewsByUser(const std::vector<struct wsUserView>& userViews) { _data.userViews.clear(); _data.userViews = userViews; }  

  //! Retorna a estrutura que empacota os dados do usuário
  const wsUser& data() const { return _data; }

private:
  wsUser _data;  //! Dados do mapa empacotados na estrutura lida / recebida do módulo de análises
};


#endif

