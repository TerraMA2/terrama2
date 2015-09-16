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
  \file terrama2/core/Utils.cpp

  \brief Utility functions for TerraMA2 SDK.

  \author Gilberto Ribeiro de Queiroz
*/

// TerraMA2
#include "Utils.hpp"
#include "DataProvider.hpp"
#include "DataSet.hpp"
#include "../Config.hpp"

// Boost
#include <boost/filesystem.hpp>

std::string terrama2::core::FindInTerraMA2Path(const std::string& p)
{
// 1st: look in the neighborhood of the executable
  boost::filesystem::path tma_path = boost::filesystem::current_path();

  boost::filesystem::path eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  tma_path /= "..";

  eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

// 2rd: look for an environment variable defined by macro TERRAMA2_DIR_VAR_NAME
  const char* tma_env = getenv(TERRAMA2_DIR_VAR_NAME);

  if(tma_env != nullptr)
  {
    tma_path = tma_env;

    eval_path = tma_path / p;

    if(boost::filesystem::exists(eval_path))
      return eval_path.string();
  }

// 3th: look into install prefix-path
  tma_path = TERRAMA2_INSTALL_PREFIX_PATH;

  eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

// 4nd: look into the codebase path
  tma_path = TERRAMA2_CODEBASE_PATH;

  eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  return "";
}


bool terrama2::core::DataProviderStatusToBool(terrama2::core::DataProvider::Status status)
{
  switch (status)
  {
    case terrama2::core::DataProvider::ACTIVE:
      return true;
    case terrama2::core::DataProvider::INACTIVE:
      return false;
    default:
      return false;
  }
}


terrama2::core::DataProvider::Status terrama2::core::BoolToDataProviderStatus(bool active)
{
  if(active)
  {
    return terrama2::core::DataProvider::ACTIVE;
  }
  else
  {
    return terrama2::core::DataProvider::INACTIVE;
  }
}


terrama2::core::DataProvider::Kind terrama2::core::IntToDataProviderKind(uint64_t kind)
{
  switch (kind) {
  case 1:
    return terrama2::core::DataProvider::FTP_TYPE;
  case 2:
    return terrama2::core::DataProvider::HTTP_TYPE;
  case 3:
    return terrama2::core::DataProvider::FILE_TYPE;
  case 4:
    return terrama2::core::DataProvider::WFS_TYPE;
  case 5:
    return terrama2::core::DataProvider::WCS_TYPE;
  default:
    return terrama2::core::DataProvider::UNKNOWN_TYPE;
  }
}

std::string terrama2::core::BoolToString(bool b)
{
  return b ? "true" : "false";
}



bool terrama2::core::DataSetStatusToBool(terrama2::core::DataSet::Status status)
{
  switch (status)
  {
    case terrama2::core::DataSet::ACTIVE:
      return true;
    case terrama2::core::DataSet::INACTIVE:
      return false;
    default:
      return false;
  }
}


terrama2::core::DataSet::Status terrama2::core::BoolToDataSetStatus(bool active)
{
  if(active)
  {
    return terrama2::core::DataSet::ACTIVE;
  }
  else
  {
    return terrama2::core::DataSet::INACTIVE;
  }
}



terrama2::core::DataSet::Kind terrama2::core::IntToDataSetKind(uint64_t kind)
{
  switch (kind)
  {
    case 1:
      return terrama2::core::DataSet::PCD_TYPE;
    case 2:
      return terrama2::core::DataSet::OCCURENCE_TYPE;
    case 3:
      return terrama2::core::DataSet::GRID_TYPE;
    default:
      return terrama2::core::DataSet::UNKNOWN_TYPE;
  }
}
