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
  \file terrama2/collector/FileOpener.hpp

  \brief File Opener.

 \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_FILEOPENER_HPP__
#define __TERRAMA2_COLLECTOR_FILEOPENER_HPP__

// STL
#include <memory>
#include <cassert>


namespace terrama2
{
  namespace collector
  {
    /*!
         * \brief The FileOpener class implements the RAII technique to control file operations.
         *
    */

    class FileOpener
    {
      public:
        /*!
          \brief When FileOpener constructor is called, is done opening the file and verified that the file exists.

          \param filename - contains the name of the file will open.

          \param attribute - attribute - contains the file operation mode. Ex. RW (read-write).

          \exception std::runtime_error - when unable to open the file.
         */

        FileOpener(const char* filename, const char* attribute);

//! Assume ownership of newfile.
        FileOpener(std::FILE* newfile);

        std::FILE* file() const;

//! When FileOpener destructor is called, is done the file closing.
        ~FileOpener();

       private:
         std::FILE* file_; //!< Attribute for Handler File.
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_FILEOPENER_HPP__
