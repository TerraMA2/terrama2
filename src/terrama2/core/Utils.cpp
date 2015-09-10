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
#include "../Config.hpp"

// Boost
#include <boost/filesystem.hpp>

std::string
terrama2::core::FindInTerraMA2Path(const std::string& p)
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

