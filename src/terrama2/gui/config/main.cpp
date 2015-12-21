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
  \file terrama2/gui/config/main.cpp

  \brief Main routine for TerraMA2 Config GUI.

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ConfigApp.hpp"
#include "Exception.hpp"
#include "../../core/Utils.hpp"

// TerraMA2 Logger
#include "../../core/Logger.hpp"

// Qt
#include <QApplication>
#include <QMessageBox>


int main(int argc, char* argv[])
{
// initialize Qt
  QApplication app(argc, argv);

// initialize TerraLib
  terrama2::core::initializeTerralib();

  // initialize terrama2 logger
  terrama2::core::initializeLogger();

  try
  {
    ConfigApp terrama2_config;
    terrama2_config.show();

    int retval = app.exec();

  // finalize TerraLib
    terrama2::core::finalizeTerralib();

    return retval;
  }
  catch(const terrama2::Exception& e)
  {
    QString messageError = QObject::tr("TerraMA2 finished with errors!\n\n");
    if (const QString* msg = boost::get_error_info<terrama2::ErrorDescription>(e))
    {
      messageError.append(msg);
    }
    TERRAMA2_LOG_FATAL() << messageError;
    QMessageBox::critical(nullptr, "TerraMA2", messageError);
  }
  catch(const std::exception& e)
  {
    QString messageError = QObject::tr("TerraMA2 finished with errors!\n\n");
    messageError.append(e.what());
    TERRAMA2_LOG_FATAL() << messageError;
    QMessageBox::critical(nullptr, "TerraMA2", messageError);
  }
  catch(...)
  {
    const QString message = QObject::tr("Unknown Error");
    TERRAMA2_LOG_FATAL() << message;
    QMessageBox::critical(nullptr, "TerraMA2", message);
  }

  terrama2::core::finalizeTerralib();
  return EXIT_FAILURE;
}
