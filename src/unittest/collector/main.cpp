
#include "TestCollectorService.hpp"
#include "TestCollectorFile.hpp"
#include "TestDataSetTimer.hpp"
#include "TestParserOGR.hpp"
#include "TestFilter.hpp"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
  int returnVal = 0;
  QApplication app(argc, argv);

  TestCollectorFile testCollectorFile;
  returnVal += QTest::qExec(&testCollectorFile, argc, argv);

  TestCollectorService testCollectorService;
  returnVal += QTest::qExec(&testCollectorService, argc, argv);

  TestDataSetTimer testDataSetTimer;
  returnVal += QTest::qExec(&testDataSetTimer, argc, argv);

  TestParserOGR testParserOGR;
  returnVal += QTest::qExec(&testParserOGR, argc, argv);

  TestFilter testFilter;
  returnVal += QTest::qExec(&testFilter, argc, argv);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();

  return returnVal;
}
