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
 * \file terrama2/gui/admin/ConfigDataManager.hpp
 *
 * \brief Definition of ConfigDataManager class
 *
 * \author Evandro Delatin
 * \author Raphael Willian da Costa
 * \author Carlos Augusto Teixeira Mendes
 * \date march, 2009 
 * \version $Id$
 *
 */
 
#ifndef _CONFIG_DATA_MANAGER_H_
#define _CONFIG_DATA_MANAGER_H_

// TerraMA2
#include "ConfigData.hpp"

// QT
#include <QObject>


/*! \brief Class for handling the set of possible configurations
*/      
class ConfigDataManager : public QObject
{
Q_OBJECT

public:

  ConfigDataManager(QObject* parent = 0);
  ~ConfigDataManager();

  void loadAvailableConfigurations(QString dirname);
  bool loadConfiguration(QString filename, QString& err);
  
  bool saveNewConfiguration(QString filename, const ConfigData& newdata, QString& err);
  bool saveConfiguration   (int id, const ConfigData& newdata, QString& err);
  
  bool removeConfiguration(int id, bool removefile, QString& err);
  
  //! Retorna o número de configurações cadastradas
  int numConfigurations() const { return _dataList.size(); }

  //! Retorna a configuração de índice id
  const ConfigData& configuration(int id) const { return _dataList.at(id); }

  //! Retorna o arquivo que armazena a configuração de índice id
  QString configurationFile(int id) const { return _fileList.at(id); }

  bool hasConfigurationFile(QString filename);

public slots:

private:
  QVector<ConfigData> _dataList; //!< Lista de configurações possíveis
  QVector<QString>    _fileList; //!< Lista com nomes dos arquivos associados a cada entrada em _dataList
};

#endif


