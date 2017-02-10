
#include <QCoreApplication>
#include <QTimer>

//TerraMA2
#include <terrama2/Exception.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terralib/Exception.h>

// GoogleMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "IntRasterTs.hpp"

int main(int argc, char *argv[])
{

  int ret = 0;
  QCoreApplication app(argc, argv);

  try
  {
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    terrama2::core::TerraMA2Init terramaRaii("unittest", 0);
    terrama2::core::registerFactories();
    terrama2::core::disableLogger();

    try
    {
      IntRasterTs intRasterTs;
      ret += QTest::qExec(&intRasterTs, argc, argv);
    }
    catch(...)
    {

    }
  }
  catch (const terrama2::Exception& e)
  {
    std::cerr << boost::get_error_info<terrama2::ErrorDescription>(e) << std::endl;
    return 1;
  }
  catch (const te::Exception& e)
  {
    std::cerr << boost::get_error_info<te::ErrorDescription>(e) << std::endl;
    return 1;
  }
  catch (const boost::exception& e)
  {
    std::cerr << boost::diagnostic_information(e) << std::endl;
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << QObject::tr("Unexpected exception...").toStdString() << std::endl;
    return 1;
  }

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return ret;
}
