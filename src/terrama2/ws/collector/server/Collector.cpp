
#include "Collector.hpp"
#include "../../../core/DataSet.hpp"

//Boost
#include <boost/log/trivial.hpp>

bool terrama2::ws::collector::server::Collector::isCollecting() const
{
  LockMutex lock(mutex_);
  if(lock.tryLock())
    return false;
  else
    return true;
}

bool terrama2::ws::collector::server::Collector::collect(DataSetTimerPtr datasetTimer)
{
  LockMutex lock(mutex_);
  if(lock.tryLock())
  {
    //JANO: implement collect
    //thread....

    return true;
  }
  else
    return false;
}
