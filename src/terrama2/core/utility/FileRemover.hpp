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
  \file terrama2/core/utility/FileRemover.hpp

  \brief Remove temporary files in the end of a process.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_UTILITY_FILE_REMOVER_HPP__
#define __TERRAMA2_CORE_UTILITY_FILE_REMOVER_HPP__

//STL
#include <set>

//QT
#include <QFile>
#include <QDir>
#include <QUrl>

namespace terrama2
{
  namespace core
  {
    class FileRemover
    {
      public:
        FileRemover() {}

        ~FileRemover()
        {
          for (auto filePath : fileList)
          {
            QFile file(QString::fromStdString(filePath));
            if(file.exists())
              file.remove();
          }

          for (auto folderPath : folderList)
          {
            QString path(QString::fromStdString(folderPath + "/"));

            // Remove schema if it exists
            QUrl folderURL(path);
            if(!folderURL.toLocalFile().isEmpty())
              path = folderURL.toLocalFile();

            QDir folder(path);
            if(folder.exists())
              folder.removeRecursively();
          }
        }

        FileRemover(const FileRemover& other) = delete;
        FileRemover(FileRemover&& other) = default;
        FileRemover& operator=(const FileRemover& other) = delete;
        FileRemover& operator=(FileRemover&& other) = default;

        void addTemporaryFile(const std::string& pathToFile) { fileList.insert(pathToFile); }
        void addTemporaryFolder(const std::string& pathToFolder) { folderList.insert(pathToFolder); }

      private:
        std::set<std::string> fileList;
        std::set<std::string> folderList;
    };
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_UTILITY_FILE_REMOVER_HPP__
