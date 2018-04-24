
// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/impl/DataRetrieverHTTP.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/impl/Utils.hpp>
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
#include <QFile>
#include <QDebug>

// Libcurl
#include <curl/curl.h>



int main(int, char**)
{
  terrama2::core::registerFactories();

  QUrl url("http://dadosabertos.rio.rj.gov.br/apiEducacao/apresentacao/csv/");

  url.setUserName("");
  url.setPassword("");

  curl_global_init(CURL_GLOBAL_ALL);

  std::string path;
  std::string mask = "freqMediaIndAprovMunicipioPorAno.csv";

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();

    std::unique_ptr<terrama2::core::CurlWrapperHttp> curlwrapper(new terrama2::core::CurlWrapperHttp());
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "HTTP";
    dataProvider->active = true;
    dataProvider->timeout = 16;

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataRetrieverHTTP retrieverHTTP(dataProviderPtr, std::move(curlwrapper));

    path = retrieverHTTP.retrieveData(mask, filter, "UTC+00", remover);
  }

  curl_global_cleanup();

  QUrl uriLocal(path.c_str());
  path = uriLocal.path().toStdString() +"/"+ mask;
  QFile file(path.c_str());
  // Check if the file exists before deleting the folder.
  if (file.exists())
    qDebug() << "Download complete!";
  else
    qDebug() << "Test failed!";

  return 0;
}
