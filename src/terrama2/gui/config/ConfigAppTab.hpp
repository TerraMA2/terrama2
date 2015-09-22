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
  \file terrama2/gui/config/ConfigApp.hpp

  \brief The base class for tab handling.

  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_CONFIGTAB_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGTAB_HPP__

// TerraMA2
#include "ConfigApp.hpp"

// QT
#include <QString>

// Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core
  {
    class ApplicationController;
  }
}


class ConfigAppTab : public QObject, private boost::noncopyable
{
  Q_OBJECT

  public:
    //! Constructor
    ConfigAppTab(ConfigApp* app, Ui::ConfigAppForm* ui, terrama2::core::ApplicationController* controller);

    //! Destructor
    virtual ~ConfigAppTab();

    /*! \brief Loads the read data to interface.
      It always call when active configuration is changed
    */
    virtual void load() = 0;

    //! It shows if data were changed by user
    virtual bool dataChanged() = 0;

    //! Check if current data are valids and it may be saved
    //! This function is called by validateAndSaveChanges to validate display fields
    virtual bool validate() = 0;

    //! It saves current data
    //! It tries to save the current data. Return true ou false showing the success of operation
    virtual void save() = 0;

    //! Cancel the editions made at current data
    virtual void discardChanges(bool restore_data) = 0;

    //! It calls the save method inside a try/catch block and check if there are any exception has found
    virtual void validateAndSaveChanges();

    virtual void askForChangeTab(const int& index);

  public slots:

    //! Slot triggered on save button. It checks if there any change has made and then call "validateAndSaveChanges"
    virtual void onSaveRequested();

    //! Slot triggered on cancel button to check if the user wish cancel and save.
    virtual void onCancelRequested();

  protected:
    ConfigApp* app_;  //!< Main Window
    Ui::ConfigAppForm* ui_;
    terrama2::core::ApplicationController* controller_;
    bool changed_;
};

#endif
