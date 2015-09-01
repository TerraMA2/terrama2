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

//! Carrega lista com configura��es dispon�veis no diret�rio recebido como par�mentro
void ConfigDataManager::loadAvailableConfigurations(QString dirname)
{
  QDir dir(dirname);
  
  QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files | QDir::Readable, QDir::Name);

  foreach(QString filename, files)
  {
    QString    err;
    ConfigData cfg;
    if(cfg.load(filename, err)) // N�o vamos mostrar msg de erro... Pode ser at� que o arquivo seja um xml n�o gerado por n�s
    {
      _dataList.append(cfg);  
      _fileList.append(dir.absolutePath() + "/" + filename);
    }  
  }    
}

//! Adiciona uma nova configura��o na lista, se esta j� n�o estiver presente
bool ConfigDataManager::loadConfiguration(QString filename, QString& err)
{
  // Obt�m o path completo do arquivo
  QFileInfo info(filename);
  if(!info.exists())
  {
    err = tr("Configura��o %1 n�o foi encontrada.").arg(filename);
    return false;
  }
  filename = info.absoluteFilePath();
  
  // Verifica se ele j� n�o est� na lista de arquivos abertos
  if(hasConfigurationFile(filename))
  {
    err = tr("Configura��o %1 j� foi carregada previamente.").arg(filename);
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


/*! \brief Salva nova configura��o no arquivo e atualiza lista de configura��es 
           dispon�veis.  Arquivo n�o deve ser igual a uma outra configura��o carregada
*/
bool ConfigDataManager::saveNewConfiguration(QString filename, const ConfigData& newdata, QString& err)
{
  QFileInfo info(filename);
  filename = info.absoluteFilePath();

  // Verifica se o arquivo j� existe na lista 
  if(hasConfigurationFile(filename))
  {
    err = tr("Arquivo %1 j� armazena uma configura��o carregada e n�o pode ser usado para salvar uma nova.")
             .arg(filename);
    return false;
  }

  // Arquivo n�o est� na lista de configura��es carregadas
  bool ok = newdata.save(filename, err);
  if(ok)
  {
    _dataList.append(newdata);  
    _fileList.append(filename);
  }
  return ok;
}

//! Sobrescreve configura��o com novos dados
bool ConfigDataManager::saveConfiguration(int id, const ConfigData& newdata, QString& err)
{
  assert(id >= 0 && id < _dataList.size());
  
  bool ok = newdata.save(_fileList[id], err);
  if(ok)
    _dataList[id] = newdata;
  
  return ok;
}

//! Remove configura��o.  Par�metro removefile indica se o arquivo tamb�m deve ser deletado ou apenas retirado da lista
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

//! Retorna true se o arquivo recebido como par�metro est� associado a alguma configura��o
bool ConfigDataManager::hasConfigurationFile(QString filename) 
{ 
  QFileInfo info(filename);
#ifdef WIN32
  // No windows temos um problema que algumas formas de obten��o do path incluem o 
  // drive em letra mai�scula e outras em letra min�scula...
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
