
// TerraMA2
#include <terrama2/core/Shared.hpp>
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
#include <QFile>
#include <QDebug>

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

  std::string path;
  std::string mask = "exporta_20160501_0230.csv";

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::CurlWrapperFtp curlwrapper;
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = url.url().toStdString();
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FTP";
    dataProvider->active = true;

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataRetrieverFTP retrieverFTP(dataProviderPtr, std::move(curlwrapper));

    path = retrieverFTP.retrieveData(mask, filter, remover);
  }

  curl_global_cleanup();

  QUrl uriLocal(path.c_str());
  path = uriLocal.path().toStdString() +"/"+ mask;
  QFile file(path.c_str());
  // Check if the file exists before deleting the folder.
  if (file.exists())
    qDebug() << "Successfully Test!";
  else
    qDebug() << "Test failed!";

  return 0;
}
