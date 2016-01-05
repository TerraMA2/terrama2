
#include "TsCollectorService.hpp"
#include "TsDataSetTimer.hpp"
#include "TsIntegration.hpp"
#include "TsDataFilter.hpp"
#include "TsParserOGR.hpp"
#include "TsUtils.hpp"

#include "Utils.hpp"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
  initializeTerralib();

  int returnVal = 0;
  QApplication app(argc, argv);

  TsCollectorService tsCollectorService;
  returnVal += QTest::qExec(&tsCollectorService, argc, argv);

  TsDataFilter tsDataFilter;
  returnVal += QTest::qExec(&tsDataFilter, argc, argv);

  //DataRetriever

  TsDataSetTimer tsDataSetTimer;
  returnVal += QTest::qExec(&tsDataSetTimer, argc, argv);

  //Factory

  //Intersection Operation

  //Log

  //Parser

  TsParserOGR tsParserOGR;
  returnVal += QTest::qExec(&tsParserOGR, argc, argv);

  //Storager

  TsUtils tsUtils;
  returnVal += QTest::qExec(&tsUtils, argc, argv);

//  TsIntegration tsIntegration;
//  returnVal += QTest::qExec(&tsIntegration, argc, argv);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();

  finalizeTerralib();

  return returnVal;
}
