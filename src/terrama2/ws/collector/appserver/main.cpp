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
  \file terrama2/ws/collector/appserver/main.cpp

  \brief Main routine for TerraMA2 Collector Web Service.

  \author Vinicius Campanha
 */

// STL
#include <iostream>

// TerraMA2
#include "soapWebService.h"


int main(int argc, char* argv[])
{
  WebService server;

// check if a port number was passed as parameter
  if(argv[1] == 0)
  {
    std::cerr << "Inform a port in order to run the collector application server.";

    return EXIT_FAILURE;
  }

// run iterative server on port until fatal error
  if( server.run( std::stoi(argv[1]) ) )
  {
    server.soap_stream_fault(std::cerr);

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
