#include <QCoreApplication>
#include <QTimer>

int main(int argc, char *argv[])
{

  int returnVal = 0;
  QCoreApplication app(argc, argv);

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(1000);
  app.exec();

  return returnVal;
}
