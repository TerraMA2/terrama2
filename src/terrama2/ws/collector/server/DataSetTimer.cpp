
#include "DataSetTimer.hpp"
#include "DataProcessor.hpp"

#include "../../../core/DataSet.hpp"

terrama2::ws::collector::server::DataSetTimer::DataSetTimer(terrama2::core::DataSetPtr dataSet, CollectorPtr collector, QObject *parent)
  : QObject(parent),
    dataSet_(dataSet),
    collector_(collector)
{
  connect(&timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);

  populateDataLst();
  prepareTimer();
}

void terrama2::ws::collector::server::DataSetTimer::timeoutSlot() const
{
  emit timerSignal(dataSet_->id());
}

void terrama2::ws::collector::server::DataSetTimer::prepareTimer()
{
  //JANO: implementar prepareTimer
  timer_.start(10000);
}

void terrama2::ws::collector::server::DataSetTimer::populateDataLst()
{
//JANO: implementar populateDataLst
}
