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
  \file terrama2/ws/collector/core/WebService.hpp

  \brief TerraMA2 Collector gSOAP interfaces declarations.

  \author Vinicius Campanha
 */

//gsoap Web service name: Web

// STL
#import "stlvector.h"

// TerraMA2
#import "WebServiceDataTypes.hpp"

/*!
  \brief gSOAP Test Ping, write in 'answer' and return SOAP_OK if executed whitout error.

  \param answer Referenced string where will be the ping answer after method execution

  \return Integer that inform that the method was executed whitout error.
*/
int Web__ping(std::string &answer);

// VINICIUS: write the methods documentation
// VINICIUS: check if need to change the Synchronous One-Way Message methods to receive an answer
/*!
  \brief

  \param

  \return
*/
int Web__addDataProvider(struct DataProvider, void);

/*!
  \brief

  \param

  \return
*/
int Web__addDataset(struct DataSet, void);

/*!
  \brief

  \param

  \return
*/
int Web__updateDataProvider(struct DataProvider, void);

/*!
  \brief

  \param

  \return
*/
int Web__updateDataSet(struct DataSet, void);

/*!
  \brief

  \param

  \return
*/
int Web__removeDataProvider(uint64_t id, void);

/*!
  \brief

  \param

  \return
*/
int Web__removeDataSet(uint64_t id, void);

/*!
  \brief

  \param

  \return
*/
int Web__findDataProvider(uint64_t id, struct DataProvider &r);

/*!
  \brief

  \param

  \return
*/
int Web__findDataSet(uint64_t id, struct DataSet &r);

/*!
  \brief

  \param

  \return
*/

int Web__listDataProvider(std::vector< struct DataProvider > &data_provider_list);

/*!
  \brief

  \param

  \return
*/

int Web__listDataSet(std::vector< struct DataSet > &data_set_list);
