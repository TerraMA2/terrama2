
#include "CollectorFactory.hpp"

terrama2::ws::collector::server::CollectorFactory* terrama2::ws::collector::server::CollectorFactory::instance_ = nullptr;

terrama2::ws::collector::server::CollectorFactory& terrama2::ws::collector::server::CollectorFactory::instance()
{
  if(!instance_)
    instance_ = new CollectorFactory();

  return *instance_;
}
