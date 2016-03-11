
#include "../core/shared.hpp"
#include "../core/data-model/DataProvider.hpp"
#include "../implementation/DataAccessorDcpInpe.hpp"


int main(int argc, char* argv[])
{
  terrama2::core::DataProvider dataProvider;
  terrama2::core::DataSeries dataSeries;
  terrama2::core::Filter filter;

  terrama2::core::DataAccessorDcpInpe accessor(dataProvider, dataSeries);
  terrama2::core::DcpSeriesPtr dcpSeries = accessor.getDcpSeries(filter);

  terrama2::core::DataProvider dataProvider2;
  terrama2::core::DataSeries dataSeries2;
  // terrama2::core::DcpStoragerPtr storager = Factory::getDcpStorager(dataProvider2, dataSeries2);
  // storager->store(dcpSeries);

  return 0;
}
