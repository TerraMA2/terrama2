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
  \file terrama2/gui/config/UserAnalysisList.hpp

  \brief Definition of methods in class UserAnalysisList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


#ifndef _USERANALYSIS_LIST_H_
#define _USERANALYSIS_LIST_H_

// QT
#include <QList>

// TerraMA2
#include "UserAnalysis.hpp"

class Services;
class ModNotificacao;

/*! \brief Classe responsável por manter e gerenciar a lista de usuarios disponíveis
*/           
class UserAnalysisList : public QList<UserAnalysis*>
{
//	Q_OBJECT

public:
  UserAnalysisList(Services* manager, ModNotificacao* service);
  ~UserAnalysisList();
  
  bool loadData();
  bool loadRiskMapThemeData();
  bool addNewUserAnalysis(const UserAnalysis* ua);
  bool updateUserAnalysis(const UserAnalysis* ua); 
  bool deleteUserAnalysis(QString login);
  bool deleteUserAnalysisIndex(int index);

  int      findLoginIndex(QString login) const;
  UserAnalysis* findLogin(QString login) const;

private:
  UserAnalysisList(const UserAnalysisList&);             //!< Remove construtor de cópia default
  UserAnalysisList& operator=(const UserAnalysisList&);  //!< Remove operador de atrib. default
  bool deleteUserAnalysisLoginIndex(QString login, int index);

  Services*   _manager; //! Gerenciador de serviços
  ModNotificacao* _service; //! Referência para acesso ao serviço remoto de Notificação
};


#endif

