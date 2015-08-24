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
  \file terrama2/gui/admin/DbManager.hpp

  \brief Definition of DbManager Class

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/


#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include "ConfigData.hpp"

#include <QString>

class TeDatabase;

/*! \brief Classe que gerencia acessos à base de dados
*/      
class DbManager 
{
public:
  
  struct AnaData
  {
    int     _id;        // id da análise na base
    QString _name;      // Nome da análise
    int     _instance;  // Instânaia a qual a análise está associada
  };

  static void checkConnection(const ConfigData::DataBaseServer& config);
  static bool createDatabase (const ConfigData::DataBaseServer& config);

  static bool loadAnaInstanceData(const ConfigData::DataBaseServer& config,
                                  QList<AnaData>& data);
                                   
  static bool saveAnaInstanceData(const ConfigData::DataBaseServer& config,
                                  const QList<AnaData>& data);

  static bool dropDatabase (ConfigData::DataBaseServer& config);

private:
  static bool updateTerraMA2Version(TeDatabase* db, ConfigData::DatabaseDriver driver, 
                                    int current_schema, bool isStudyDB, QString& err);

  static bool updateTerraMA2Version5(TeDatabase* db, QString& err);

};

#endif


