
#include "CollectorOGR.hpp"
#include "Exception.hpp"
#include "Filter.hpp"

#include "../core/Data.hpp"

//TerraLib
#include <terralib/ogr/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

terrama2::collector::CollectorOGR::CollectorOGR(const terrama2::core::DataProviderPtr dataProvider, QObject *parent)
  : Collector(dataProvider, parent)
{

  if(dataProvider->kind() != core::DataProvider::FILE_TYPE)
    throw WrongDataProviderKindException() << terrama2::ErrorDescription(
                                                tr("Wrong DataProvider Kind received in CollectorOGR constructor."));

  //Writing in OGR
  dataSource_ = te::da::DataSourceFactory::make("OGR");

  std::map<std::string, std::string> connInfo;

  connInfo["URI"] = dataProvider->uri();

  dataSource_->setConnectionInfo(connInfo);
}

bool terrama2::collector::CollectorOGR::isOpen() const
{
  return dataSource_->isOpened();
}

void terrama2::collector::CollectorOGR::open()
{
  try
  {
    dataSource_->open();
  }
  catch(te::ogr::Exception& e)
  {
    //TODO: What to do?
  }
  catch(...)
  {
    //TODO: What to do?
  }
}

void terrama2::collector::CollectorOGR::close()
{
  return dataSource_->close();
}

void terrama2::collector::CollectorOGR::getData(const terrama2::collector::DataProcessorPtr dataProcessor)
{
  //FIXME: is data implemented?
  core::DataPtr data = dataProcessor->data();
  std::string dataMask = data->mask();

  std::vector<std::string> names = dataSource_->getDataSetNames();
  FilterPtr filter = dataProcessor->filter();

  filter->setMask(dataMask);
  names = filter->filterNames(names);


}
