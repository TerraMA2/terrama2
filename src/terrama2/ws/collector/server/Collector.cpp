
#include "Collector.hpp"
#include "../../../core/DataSet.hpp"

//Boost
#include <boost/log/trivial.hpp>

bool terrama2::ws::collector::server::Collector::collect(DataSetTimerPtr datasetTimer)
{
  BOOST_LOG_TRIVIAL(trace) << "Collector::collect: " << datasetTimer->getDataSet()->id();

  LockMutex lock(mutex_);
  if(lock.tryLock())
  {
    //JANO: implement collect
    return true;
  }
  else
    return false;
}
