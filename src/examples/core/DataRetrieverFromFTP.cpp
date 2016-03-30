
// TerraMA2
#include <terrama2/core/shared.hpp>
#include <terrama2/impl/DataRetrieverFTP.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/Filter.hpp>
#include <terrama2/core/utility/Raii.hpp>
#include <terrama2/core/utility/FilterUtils.hpp>

// STL
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

// QT
#include <QUrl>
#include <QObject>
#include <QDir>

// Libcurl
#include <curl/curl.h>


int main(int argc, char* argv[])
{
  QUrl url;	

  // Fire
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
  dataProvider->dataProviderType = 0;
  dataProvider->active = true;

  //empty filter
  terrama2::core::Filter filter;
  //accessing data
  terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr);

  std::string path;
  path = retrieverFTP.retrieveData("exporta_20160101_0130.csv", filter);

  curl_global_cleanup();

  // Remove paste of download files.
  QUrl uriLocal(path.c_str());
  QDir dir(uriLocal.path());
  if (dir.exists())
    dir.removeRecursively();

  return 0; 
}
