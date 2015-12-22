
#include "TsCollectorService.hpp"
#include "TsDataSetTimer.hpp"
#include "TsIntegration.hpp"
#include "TsDataFilter.hpp"
#include "TsParserOGR.hpp"

#include "Utils.hpp"

#include <QApplication>
#include <QTimer>



#include <boost/date_time/local_time/local_date_time.hpp>
#include <boost/date_time/local_time/posix_time_zone.hpp>
#include <boost/date_time/local_time/local_time_types.hpp>
#include <iostream>
int main(int argc, char *argv[])
{
  initializeTerralib();

  int returnVal = 0;
  QApplication app(argc, argv);

  TsDataFilter tsDataFilter;
  returnVal += QTest::qExec(&tsDataFilter, argc, argv);

  TsDataSetTimer tsDataSetTimer;
  returnVal += QTest::qExec(&tsDataSetTimer, argc, argv);

  TsParserOGR tsParserOGR;
  returnVal += QTest::qExec(&tsParserOGR, argc, argv);

  TsCollectorService tsCollectorService;
  returnVal += QTest::qExec(&tsCollectorService, argc, argv);


  TsIntegration tsIntegration;
  returnVal += QTest::qExec(&tsIntegration, argc, argv);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();

  finalizeTerralib();

  return returnVal;
}
