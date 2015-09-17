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
  \file terrama2/gui/core/ConfigManager.hpp

  \brief This class handling the terrama2 configuration file.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CORE_CONFIGMANAGER_HPP__
#define __TERRAMA2_GUI_CORE_CONFIGMANAGER_HPP__

// TerraMA2
#include "DataType.hpp"

// QT
#include <QJsonObject>
#include <QMainWindow>

// Boost
#include <boost/noncopyable.hpp>

class ConfigManager : private boost::noncopyable
{
  public:
    ConfigManager(QMainWindow* app);

    virtual ~ConfigManager();

    Database* getDatabase();

    //! Open the json configuration file and try to parse it
    virtual void loadConfiguration(QString filepath);

    //! It opens a file from filepath and apply parse to json
    QJsonObject open(QString filepath);

    //! Get the TerraMA2 database struct
    Database* getDatabase() const;

    //! Get the TerraMA2 collection struct
    Collection* getCollection() const;

    //! Get the TerraMA2 name
    QString getName() const;

  private:
    QMainWindow* app_; //< Dialog to display error message if there is.

    QString name_; //< TerraMA2 config name
    Collection* collection_;
    Database* database_;
};

#endif // __TERRAMA2_GUI_CORE_CONFIGMANAGER_HPP__
