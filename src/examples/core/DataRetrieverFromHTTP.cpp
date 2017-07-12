
// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/impl/DataRetrieverHTTP.hpp>
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
#include <QFile>
#include <QDebug>

// Libcurl
#include <curl/curl.h>



int main(int argc, char* argv[])
{
  QUrl url("https://prodwww-queimadas.dgi.inpe.br/users/cbm_mt/focos24h/");

  url.setUserName("cbm_mt");
  url.setPassword("xae9aiva");

  curl_global_init(CURL_GLOBAL_ALL);

  std::string path;
  std::string mask = "focosINPE_24H_20170301.txt";

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();

    std::unique_ptr<te::ws::core::CurlWrapper> curlwrapper(new te::ws::core::CurlWrapper());
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "HTTP";
    dataProvider->active = true;

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
