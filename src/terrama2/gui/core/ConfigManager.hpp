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
#include <QMap>
#include <QMultiMap>

// Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace gui
  {
    namespace core
    {
      /*!
        \class ConfigManager
        \brief Class responsible for handling TerraMA2 settings

               This class may be used to:
               - Load a TerraMA2 file (extension .terrama2). It fills out the structs Collection, Database.
               - To update the configuration files (AdminApp interface).
      */
      class ConfigManager : private boost::noncopyable
      {
        public:
          /*!
            \brief Constructor.
            \param app A pointer to QMainWindow to define owner window.
            */
          ConfigManager(QMainWindow* app);

          //! Destructor
          virtual ~ConfigManager();

          /*!
            \brief Open the json configuration file and try to parse it
            \param filepath A string value containing TerraMA2 full path to load.
          */
          virtual void loadConfiguration(QString filepath);

          //! Get the TerraMA2 database struct
          Database* getDatabase() const;

          /*!
            \brief It fills out only the Database structure
            \param dbase A QJsonObject containing Database configuration values
          */
          void setDatabase(QJsonObject dbase);

          //! Get the TerraMA2 collection struct
          Collection* getCollection() const;

          //! Get the TerraMA2 configuration name
          QString getName() const;

          /*!
            \brief It fills out the structures (Collection and Database) with QJsonObject values
            \param metadata A QJsonObject object containing TerraMA2 settings values
            \exception terrama2::gui::FileException Raised when the QJsonObject does not contain required values to fill structures out.
                                                    It is a TerraMA2 invalid file.
          */
          void setDataForm(QJsonObject metadata);

          /*!
            \brief It inserts a file in map to be as cached value. After that, it calls setDataForm(metadata).
            \param newname A string containing the file to be stored in map.
            \param metatada A json object to be stored in map identified by newname.
          */
          void insertFile(QString newname, QJsonObject metatada);

          /*!
            \brief It renames the filename in cached map.
            \param selectedName A string value containing a filename to be found and renamed
            \param newname A string value representing the new filename
          */
          void renameFile(QString selectedName, QString newname);

          /*!
            \brief It removes the filename from cached map.
            \param selectedName A string value containing the selected file to be removed
          */
          void removeFile(QString selectedName);

          /*!
            \brief Retrieves a current TerraMA2 mapped files.
            \return A QMap<QString,QJsonObject> with current TerraMA2 mapped files.
          */
          QMap<QString,QJsonObject> getfiles() const;

        private:
          QMainWindow* app_;                   //!< Dialog to display error message if there is.
          QString name_;                       //!< TerraMA2 configuration name
          Collection* collection_;             //!< A struct to store Collection values from TerraMA2 settings
          Database* database_;                 //!< A struct to store Database values from TerraMA2 settings
          QMap<QString,QJsonObject> fileList_; //!< A map of string to json objects for handling active files in TerraMA2 GUI administration.
      };
    }
  }
}
#endif // __TERRAMA2_GUI_CORE_CONFIGMANAGER_HPP__
