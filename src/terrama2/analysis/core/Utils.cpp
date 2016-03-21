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
  \file terrama2/analysis/core/Utils.cpp

  \brief Utility functions for TerraMA2 Analysis module.

  \author Paulo R. M. Oliveira
*/

#include "Utils.hpp"
#include "Analysis.hpp"
#include "../../Exception.hpp"

#include <QObject>

terrama2::analysis::core::Analysis::Type terrama2::analysis::core::ToType(uint64_t type)
{
  switch (type)
  {
    case 2:
      return Analysis::PCD_TYPE;
    case 3:
      return Analysis::MONITORED_OBJECT_TYPE;
    case 4:
      return Analysis::GRID_TYPE;
    case 5:
      return Analysis::TERRAME_TYPE;
  }

  throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid analysis type"));

}

terrama2::analysis::core::Analysis::ScriptLanguage terrama2::analysis::core::ToScriptLanguage(uint64_t scriptLanguage)
{
  switch (scriptLanguage)
  {
    case 1:
      return Analysis::PYTHON;
    case 2:
      return Analysis::LUA;
  }
  throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Invalid script language"));
}

