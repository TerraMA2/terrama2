// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/impl/DataRetrieverFTP.hpp>
#include <terrama2/core/data-access/DataRetriever.hpp>
#include <terrama2/core/Exception.hpp>

#include "MockDataRetriever.hpp"
#include "TsDataRetrieverFTP.hpp"

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


using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;


QUrl TsDataRetrieverFTP::BuildUriInvalid()
{
  QUrl url;
  url.setHost("ftp.dgi.inpe.br");
  url.setPath("/operacao/"); // uri inválida
  url.setScheme("FTP");
  url.setPort(21);
  url.setUserName("queimadas");
  url.setPassword("inpe_2012");
  return url;
}

QUrl TsDataRetrieverFTP::BuildLoginInvalid()
{
  QUrl url;
  url.setHost("ftp.dgi.inpe.br");
  url.setPath("/focos_operacao/");
  url.setScheme("FTP");
  url.setPort(21);
  url.setUserName("queimadas");
  url.setUserName("2012"); // login errado
  return url;
}


QUrl TsDataRetrieverFTP::BuildUriAndLoginValid()
{
  QUrl url;
  url.setHost("ftp.dgi.inpe.br");
  url.setPath("/focos_operacao/");
  url.setScheme("FTP");
  url.setPort(21);
  url.setUserName("queimadas");
  url.setPassword("inpe_2012");
  return url;
}

std::string TsDataRetrieverFTP::BuildMaskInvalid()
{
  return "exporta";
}

std::string TsDataRetrieverFTP::BuildMaskValid()
{
  return "exporta_20160101_0130.csv";
}

void TsDataRetrieverFTP::TestOk()
{
  try
  {
   QUrl url = BuildUriAndLoginValid();

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
   //accessing data

   std::string path;
   std::string mask = BuildMaskValid();

   std::shared_ptr<MockDataRetriever> mock_ = std::make_shared<MockDataRetriever>(dataProviderPtr);


   EXPECT_CALL(*mock_.get(), retrieveData(mask,_)).WillOnce(Return(path));

   try
   {
    path = mock_->retrieveData(mask, filter);    
    QFAIL("Exception expected!");
   }
   catch(terrama2::Exception& e)
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

void TsDataRetrieverFTP::TestFail()
{

}
