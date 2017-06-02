
// TerraMA2
#include <terrama2/core/Shared.hpp>
#include <terrama2/impl/DataRetrieverWCS.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/Filter.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
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

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terrama2Utility("WCS", 0);

  std::string path;
  std::string mask = "";

  auto remover = std::make_shared<terrama2::core::FileRemover>();
  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "http://terrabrasilis.info/terraamazon/ows";
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "WCS";
    dataProvider->active = true;
//    dataProvider->options.insert({"active_mode", "true"});

    //empty filter
    terrama2::core::Filter filter;
    //accessing data
    terrama2::core::DataRetrieverWCS retrieverWCS(dataProviderPtr);

    path = retrieverWCS.retrieveData(mask, filter, "UTC+00", remover);
  }

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
