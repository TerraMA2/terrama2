#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/Raii.hpp>

#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// STL
#include <iostream>

std::shared_ptr<te::da::DataSet> getDataSet(const std::string& fileUri, const std::string& dataSetName)
{
    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make("OGR", fileUri));

    //RAII for open/closing the datasource
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
    std::shared_ptr<te::da::DataSet> dataset(transactor->getDataSet(dataSetName));
    assert(dataset);

    return dataset;
}

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

int main(int , char* [])
{
  terrama2::core::TerraMA2Init terrama2Init("", 0);

  QFile file("/home/jsimas/MyDevel/dpi/data/cemaden/hidrologico.json");
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    auto readingsArray = doc.object()["cemaden"].toArray();
    for(auto reading : readingsArray) {
      auto obj = reading.toObject();
      for(auto val = obj.begin(); val != obj.end(); ++val) {
        std::cout << val.key().toStdString() << "\t" << val.value().toString().toStdString() << "\n";
      }
    }
  }

  return 0;
}
