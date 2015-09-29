
#include "TsCollectorService.hpp"
#include "TsCollectorFile.hpp"
#include "TsDataSetTimer.hpp"
#include "TsParserOGR.hpp"
#include "TsFilter.hpp"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
  int returnVal = 0;
  QApplication app(argc, argv);

  TsCollectorFile tsCollectorFile;
  returnVal += QTest::qExec(&tsCollectorFile, argc, argv);

  TsCollectorService tsCollectorService;
  returnVal += QTest::qExec(&tsCollectorService, argc, argv);

  TsDataSetTimer tsDataSetTimer;
  returnVal += QTest::qExec(&tsDataSetTimer, argc, argv);

  TsParserOGR tsParserOGR;
  returnVal += QTest::qExec(&tsParserOGR, argc, argv);

  TsFilter tsFilter;
  returnVal += QTest::qExec(&tsFilter, argc, argv);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();

  return returnVal;
}
