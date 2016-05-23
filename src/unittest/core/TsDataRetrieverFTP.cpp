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
  \file terrama2/unittest/core/TsDataRetrieverFTP.cpp
  \brief Tests for Class DataRetrieverFTP
  \author Evandro Delatin
*/

// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataRetrieverFTP.hpp>
#include <terrama2/core/data-access/DataRetriever.hpp>
#include <terrama2/core/Exception.hpp>

#include "TsDataRetrieverFTP.hpp"
#include "MockCurlWrapper.hpp"

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

// QT
#include <QUrl>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QDebug>

// Libcurl
#include <curl/curl.h>

// GMock
#include <gtest/gtest.h>


using ::testing::Return;
using ::testing::_;


void TsDataRetrieverFTP::TestFailUriInvalid()
{
  try
  {
    QUrl url;
    url.setHost("ftp.dgi.inpe.br");
    url.setPath("/operacao/"); // uri inválida
    url.setScheme("FTP");
    url.setPort(21);
    url.setUserName("queimadas");
    url.setPassword("inpe_2012");

    curl_global_init(CURL_GLOBAL_ALL);

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    //empty filter
    terrama2::core::Filter filter;

    MockCurlWrapper mock_;

    EXPECT_CALL(mock_, verifyURL(_)).WillOnce(Return(CURLE_COULDNT_RESOLVE_HOST));

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, mock_);

      QFAIL("Exception expected - DataRetrieverException!");
    }
    catch(terrama2::core::DataRetrieverException& e)
    {

    }

    curl_global_cleanup();

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Exception unexpected!");
  }
 
 return;

}


void TsDataRetrieverFTP::TestFailLoginInvalid()
{
  try
  {
    QUrl url;
    url.setHost("ftp.dgi.inpe.br");
    url.setPath("/focos_operacao/");
    url.setScheme("FTP");
    url.setPort(21);
    url.setUserName("queimadas");
    url.setUserName("2012"); // login errado

    curl_global_init(CURL_GLOBAL_ALL);

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    //empty filter
    terrama2::core::Filter filter;

    MockCurlWrapper mock_;

    EXPECT_CALL(mock_, verifyURL(_)).WillOnce(Return(CURLE_LOGIN_DENIED));

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, mock_);

      QFAIL("Exception expected - DataRetrieverException!");
    }
    catch(terrama2::core::DataRetrieverException& e)
    {

    }

    curl_global_cleanup();

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Exception unexpected!");
  }

 return;

}

void TsDataRetrieverFTP::TestOkUriAndLoginValid()
{
  try
  {
    QUrl url;
    url.setHost("ftp.dgi.inpe.br");
    url.setPath("/focos_operacao/");
    url.setScheme("FTP");
    url.setPort(21);
    url.setUserName("queimadas");
    url.setPassword("inpe_2012");

    curl_global_init(CURL_GLOBAL_ALL);

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProvider::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    //empty filter
    terrama2::core::Filter filter;

    MockCurlWrapper mock_;

    EXPECT_CALL(mock_, verifyURL(_)).WillOnce(Return(CURLE_OK));

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, mock_);

    }
    catch(...)
    {
      QFAIL("Exception expected - DataRetrieverException!");
    }

    curl_global_cleanup();

  }
  catch(terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Exception unexpected!");
  }

 return;

}

