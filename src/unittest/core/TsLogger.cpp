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
  \file unittest/core/TsLogger.cpp

  \brief Test for TerraMA2 logging module

  \author Raphael Willian da Costa
*/

#include "TsLogger.hpp"

#include <terrama2/core/Logger.hpp>
#include <terrama2/core/Exception.hpp>


void TsLogger::init()
{

}

void TsLogger::testLoad()
{
  terrama2::core::Logger::getInstance().addStream("/home/raphael/Documents/tmp/terrama2.log");
  terrama2::core::Logger::getInstance().initialize();

  auto exception = terrama2::core::DataAccessException() << terrama2::ErrorDescription("**Expected DataAccess Error**");

  for(int i = 0; i < 20; ++i)
    TERRAMA2_LOG_TRACE() << "Trace Message Lorem ipsu " + std::to_string(i);

  // logging an exception
  TERRAMA2_LOG_ERROR() << exception;
}

void TsLogger::testUnload()
{

}

void TsLogger::cleanup()
{

}
