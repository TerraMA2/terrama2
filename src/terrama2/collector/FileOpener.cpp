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
  \file terrama2/collector/FileOpener.cpp

  \brief File Opener.

 \author Evandro Delatin
*/

// TerraMA2
#include "FileOpener.hpp"
#include "../Exception.hpp"

// QT
#include <QObject>


terrama2::collector::FileOpener::FileOpener(const char* filename, const char* attribute)
{
  file_ = std::fopen(filename, attribute);

  if (!file_)
  {
    QString err_msg(QObject::tr("Could not open file: %1."));
    err_msg = err_msg.arg(filename);
    throw terrama2::FileOpenException() << terrama2::ErrorDescription(err_msg);
  }
}

terrama2::collector::FileOpener::FileOpener(std::FILE* newfile)
{
  file_ = newfile;
}

terrama2::collector::FileOpener::~FileOpener()
{
  if (file_)
    std::fclose(file_);
}

std::FILE* terrama2::collector::FileOpener::file() const
{
  return file_;
}
