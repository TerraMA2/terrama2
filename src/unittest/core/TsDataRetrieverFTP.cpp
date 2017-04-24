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

#include <terralib/ws/core/Exception.h>

using ::testing::Return;
using ::testing::_;


void TsDataRetrieverFTP::TestFailUriInvalid()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillOnce(testing::Throw(te::ws::core::Exception()));

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(mock_));

      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::DataRetrieverException& e)
    {

    }



  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataRetrieverFTP::TestFailLoginInvalid()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillOnce(testing::Throw(te::ws::core::Exception()));

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr,  std::move(mock_));

      QFAIL("Exception expected!");
    }
    catch(const terrama2::core::DataRetrieverException& e)
    {

    }
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataRetrieverFTP::TestOkUriAndLoginValid()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillOnce(Return());

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(mock_));

    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataRetrieverFTP::TestFailVectorFileEmpty()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    std::string path;
    std::string mask = "exporta_20160501_0230.csv";
    std::vector<std::string> vectorFiles; // VectorFiles Empty

    //empty filter
    terrama2::core::Filter filter;

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillOnce(Return());
    EXPECT_CALL(*mock_, listFiles(_)).WillOnce(Return(vectorFiles));

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(mock_));
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      path = retrieverFTP.retrieveData(mask, filter, "UTC+00", remover);

    }
    catch(...)
    {
      QFAIL("Exception expected!");
    }
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataRetrieverFTP::TestOKVectorWithFiles()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    std::string path;
    std::string mask = "exporta_20160501_0230.csv";
    std::vector<std::string> vectorFiles;
    vectorFiles.push_back("exporta_20160501_0230.csv");

    //empty filter
    terrama2::core::Filter filter;

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillOnce(Return());
    EXPECT_CALL(*mock_, listFiles(_)).WillOnce(Return(vectorFiles));
    EXPECT_CALL(*mock_, downloadFile(_,_,_)).WillOnce(Return());

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(mock_));
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      path = retrieverFTP.retrieveData(mask, filter, "UTC+00", remover);

    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;

}

void TsDataRetrieverFTP::TestFailDownloadFile()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    //empty filter
    terrama2::core::Filter filter;
    std::string path;
    std::string mask = "exporta_20160501_0230.csv";

    std::vector<std::string> vectorFiles;
    vectorFiles.push_back("exporta_20160501_0230.csv");

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillOnce(Return());
    EXPECT_CALL(*mock_, listFiles(_)).WillOnce(Return(vectorFiles));
    EXPECT_CALL(*mock_, downloadFile(_,_,_)).WillOnce(Return());

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(mock_));
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      path = retrieverFTP.retrieveData(mask, filter, "UTC+00", remover);
    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;
}

void TsDataRetrieverFTP::TestOKDownloadFile()
{
  try
  {
    QUrl url;

    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    //empty filter
    terrama2::core::Filter filter;
    std::string path;
    std::string mask = "exporta_20160501_0230.csv";

    std::vector<std::string> vectorFiles;
    vectorFiles.push_back("exporta_20160501_0230.csv");

    std::unique_ptr<MockCurlWrapper> mock_(new MockCurlWrapper());

    EXPECT_CALL(*mock_, verifyURL(_,_)).WillRepeatedly(Return());
    EXPECT_CALL(*mock_, listFiles(_)).WillOnce(Return(vectorFiles));
    EXPECT_CALL(*mock_, downloadFile(_,_,_)).WillRepeatedly(Return());

    try
    {
      terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(mock_));
      auto remover = std::make_shared<terrama2::core::FileRemover>();
      path = retrieverFTP.retrieveData(mask, filter, "UTC+00", remover);
    }
    catch(...)
    {
      QFAIL("Unexpected exception!");
    }
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  }

  catch(...)
  {
    QFAIL("Unexpected exception!");
  }

  return;
}
