
int main(int argc, char* argv[])
{
  DataProvider dataProvider;
  DataSeries dataSeries;

  DataAccessorDcpInpe accessor(DataProvider, DataSeries);
  DcpSeriesPtr dcpSeries = accessor->getDcpSeries(nullptr);



  DataProvider dataProvider2;
  DataSeries dataSeries2;
  DcpStoragerPtr storager = Factory::getDcpStorager(dataProvider2, dataSeries2);
  storager->store(dcpSeries);

  return 0;
}
