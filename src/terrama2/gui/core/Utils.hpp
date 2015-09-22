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
  \file terrama2/core/ApplicationController.cpp

  \brief The base gui module utils used

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CORE_UTILS_HPP__
#define __TERRAMA2_GUI_CORE_UTILS_HPP__

// Boost
#include <boost/noncopyable.hpp>

// Forward declaration
class QString;

namespace terrama2
{
  namespace gui
  {
    namespace core
    {
      enum ConnectionType
      {
        FTP,
        LOCALFILES,
        WEBSERVICE
      };

      //! Function to check if there is any service listening on host (WebServices)
      bool checkServiceConnection(const QString& host, const int& port, const QString& user, const QString& password);

      //! Function to check if there is any in FTP connections
      bool checkFTPConnection(const QString& host, const int& port, const QString& basepath, const QString& user, const QString& password);

      //! Function to check if there is a valid path
      bool checkLocalFilesConnection(const QString& path);
    }
  }
}

#endif // __TERRAMA2_GUI_CORE_UTILS_HPP__
