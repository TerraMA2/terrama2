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
  \file terrama2/gui/admin/PreviewCustom.hpp

  \brief Definition of PreviewCustom Class

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Raphael Meloni
*/


#ifndef _PREVIEW_H_
#define _PREVIEW_H_

#include "PreviewCustom.hpp"

#include "ConfigData.hpp"
#include <QDir>


/*! \brief Classe que gera o pacote de customizacao do módulo de visualizacao
*/      
class PreviewCustom
{

public:
	PreviewCustom(ConfigData::Preview preview, ConfigData::Animation animacao, ConfigData::DataBaseServer database);
  ~PreviewCustom();

  bool generate();

private:

  bool generateFolders();
  bool createFolder(QDir& output, QString dirName, bool cd);
  void recursiveRemove(const QString d);

  void saveFile(QString fileName, QString content);
  QString readFile(QString fileName);

  void processConfig();
  void processHibernate();
  void processHibernateFiles();
  void processHibernateFile(QString filename, QString outfilename);
  void processLogo();
  void zipPackage();

  ConfigData::Preview _preview;			//!< configurações do módulo de visualizacao
  ConfigData::Animation _animation;		//!< configurações do módulo de animacao
  ConfigData::DataBaseServer _database;	//!< tipo da base de dados
};


#endif

