
#include "TsCollectorService.hpp"
#include "TsDataRetrieverWCS.hpp"
#include "TsDataSetTimer.hpp"
#include "TsIntegration.hpp"
#include "TsDataFilter.hpp"
#include "TsLog.hpp"
#include "TsParserOGR.hpp"
#include "TsParserPcdInpe.hpp"
#include "TsParserPcdToa5.hpp"
#include "TsParserTiff.hpp"
#include "TsParserAscGrid.hpp"
#include "TsFactory.hpp"
#include "TsUtils.hpp"
#include "TsTeste.hpp"

#include "Utils.hpp"

#include <QCoreApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
  initializeTerralib();

  int returnVal = 0;
  QCoreApplication app(argc, argv);

  TsTeste tsTeste;
  returnVal += QTest::qExec(&tsTeste, argc, argv);

  //  TsCollectorService tsCollectorService;
//  returnVal += QTest::qExec(&tsCollectorService, argc, argv);

//  TsDataFilter tsDataFilter;
//  returnVal += QTest::qExec(&tsDataFilter, argc, argv);

//  //DataRetriever

//  TsDataSetTimer tsDataSetTimer;
//  returnVal += QTest::qExec(&tsDataSetTimer, argc, argv);

//  TsFactory tsFactory;
//  returnVal += QTest::qExec(&tsFactory, argc, argv);

//  //Intersection Operation

  //Log

//  TsLog tsLog;
//  returnVal += QTest::qExec(&tsLog, argc, argv);

//  //Parser

//  TsParserOGR tsParserOGR;
//  returnVal += QTest::qExec(&tsParserOGR, argc, argv);

//  TsParserPcdInpe tsParserInpe;
//  returnVal += QTest::qExec(&tsParserInpe, argc, argv);

//  TsParserPcdToa5 tsParserToa5;
//  returnVal += QTest::qExec(&tsParserToa5, argc, argv);

//  TsParserTiff tsParserTiff;
//  returnVal += QTest::qExec(&tsParserTiff, argc, argv);

//  TsParserAscGrid tsParserAscGrid;
//  returnVal += QTest::qExec(&tsParserAscGrid, argc, argv);

  //Storager

//  TsUtils tsUtils;
//  returnVal += QTest::qExec(&tsUtils, argc, argv);

//  TsIntegration tsIntegration;
//  returnVal += QTest::qExec(&tsIntegration, argc, argv);

  // DataRetriever

//  TsDataRetrieverWCS tsDataRetrieverWCS;
//  returnVal += QTest::qExec(&tsDataRetrieverWCS, argc, argv);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();

  finalizeTerralib();

  return returnVal;
}
