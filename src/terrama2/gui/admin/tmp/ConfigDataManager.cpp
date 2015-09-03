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
  \file terrama2/gui/admin/ConfigDataManager.cpp

  \brief Definition of ConfigDataManager class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes

*/

/*!
\class ConfigDataManager

Class responsible to store available configuration list and it handles the current configuration
*/

// TerraMA2
#include "ConfigDataManager.hpp"

// STL
#include <assert.h>

// QT
#include <QDir>

//! Construtor.  
ConfigDataManager::ConfigDataManager(QObject* parent)
  : QObject(parent)
{
}

//! Destrutor
ConfigDataManager::~ConfigDataManager()
{
}

//! Carrega lista com configurações disponíveis no diretório recebido como parâmentro
void ConfigDataManager::loadAvailableConfigurations(QString dirname)
{
  QDir dir(dirname);
  
  QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files | QDir::Readable, QDir::Name);

  foreach(QString filename, files)
  {
    QString    err;
    ConfigData cfg;
    if(cfg.load(filename, err)) // Não vamos mostrar msg de erro... Pode ser até que o arquivo seja um xml não gerado por nós
    {
      _dataList.append(cfg);  
      _fileList.append(dir.absolutePath() + "/" + filename);
    }  
  }    
}

//! Adiciona uma nova configuração na lista, se esta já não estiver presente
bool ConfigDataManager::loadConfiguration(QString filename, QString& err)
{
  // Obtém o path completo do arquivo
  QFileInfo info(filename);
  if(!info.exists())
  {
    err = tr("Configuração %1 não foi encontrada.").arg(filename);
    return false;
  }
  filename = info.absoluteFilePath();
  
  // Verifica se ele já não está na lista de arquivos abertos
  if(hasConfigurationFile(filename))
  {
    err = tr("Configuração %1 já foi carregada previamente.").arg(filename);
    return false;
  }
  
  // Carrega os novos dados
  ConfigData cfg;
  bool ok = cfg.load(filename, err);
  if(ok)
  {  
    // Coloca na lista  
    _dataList.append(cfg);  
    _fileList.append(filename);
  }  
  return ok;
}


/*! \brief Salva nova configuração no arquivo e atualiza lista de configurações 
           disponíveis.  Arquivo não deve ser igual a uma outra configuração carregada
*/
bool ConfigDataManager::saveNewConfiguration(QString filename, const ConfigData& newdata, QString& err)
{
  QFileInfo info(filename);
  filename = info.absoluteFilePath();

  // Verifica se o arquivo já existe na lista 
  if(hasConfigurationFile(filename))
  {
    err = tr("Arquivo %1 já armazena uma configuração carregada e não pode ser usado para salvar uma nova.")
             .arg(filename);
    return false;
  }

  // Arquivo não está na lista de configurações carregadas
  bool ok = newdata.save(filename, err);
  if(ok)
  {
    _dataList.append(newdata);  
    _fileList.append(filename);
  }
  return ok;
}

//! Sobrescreve configuração com novos dados
bool ConfigDataManager::saveConfiguration(int id, const ConfigData& newdata, QString& err)
{
  assert(id >= 0 && id < _dataList.size());
  
  bool ok = newdata.save(_fileList[id], err);
  if(ok)
    _dataList[id] = newdata;
  
  return ok;
}

//! Remove configuração.  Parâmetro removefile indica se o arquivo também deve ser deletado ou apenas retirado da lista
bool ConfigDataManager::removeConfiguration(int id, bool removefile, QString& err)
{
  assert(id >= 0 && id < _dataList.size());

  if(removefile)
  {
    if(!QFile::remove(_fileList[id]))
    {
      err = tr("Erro removendo arquivo %1.").arg(_fileList[id]);      
      return false;
    }
  }

  _dataList.remove(id);  
  _fileList.remove(id);  
  return true;
}

//! Retorna true se o arquivo recebido como parâmetro está associado a alguma configuração
bool ConfigDataManager::hasConfigurationFile(QString filename) 
{ 
  QFileInfo info(filename);
#ifdef WIN32
  // No windows temos um problema que algumas formas de obtenção do path incluem o 
  // drive em letra maiúscula e outras em letra minúscula...
  filename = info.absoluteFilePath().toLower();
  foreach(QString f, _fileList)
  {
    if(f.toLower() == filename)
      return true;
  }
  return false;
#else
  return _fileList.contains(info.absoluteFilePath()); 
#endif
}
