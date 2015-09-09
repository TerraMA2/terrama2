
#include "CollectorFactory.hpp"

terrama2::ws::collector::server::CollectorFactory* terrama2::ws::collector::server::CollectorFactory::instance_ = nullptr;

terrama2::ws::collector::server::CollectorFactory& terrama2::ws::collector::server::CollectorFactory::instance()
{
  if(!instance_)
    instance_ = new CollectorFactory();

  return *instance_;
}

terrama2::ws::collector::server::CollectorPtr terrama2::ws::collector::server::CollectorFactory::getCollector(terrama2::core::DataProviderPtr dataProvider)
{
  //JANO: implementar getCollector

  if(!collectorMap_.contains(dataProvider->id()))
  {
    //... instatiate a new collector
  }

  return collectorMap_.value(dataProvider->id());
}
