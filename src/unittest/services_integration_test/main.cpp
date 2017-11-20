

// TerraMA2

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/Exception.hpp>

// GoogleMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QTimer>

#include "DCPInpeTS.hpp"


#include <iostream>

int main(int argc, char *argv[])
{

  int returnVal = 0;
  QCoreApplication app(argc, argv);

  try
  {
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    terrama2::core::TerraMA2Init terramaRaii("unittest", 0);
    Q_UNUSED(terramaRaii);

   // terrama2::core::disableLogger();
    terrama2::core::registerFactories();

    try
    {
      DCPInpeTs dcpInpeTS1;
      dcpInpeTS1.createDBaseForTest();
    }
    catch(...)
    {
      std::cout<<"Não deu certo o py";
    }

    try
    {
      DCPInpeTs dcpInpeTS;
      returnVal += QTest::qExec(&dcpInpeTS, argc, argv);
    }
    catch(...)
    {
      returnVal += 1;

      std::cout<<returnVal;
    }

    //If not error in collect and analysis restore ref database for compare tables
   /* if(returnVal == 0)
    {
      try
      {
        DCPInpeTs dcpInpeTS1;
        dcpInpeTS1.restoreCompare();
      }
      catch(...)
      {
        std::cout<<"Não deu certo o restore";
      }
    }*/

  }
  catch (const terrama2::Exception& e)
  {
    std::cerr << boost::get_error_info<terrama2::ErrorDescription>(e) << std::endl;
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
  timer.start(1000);
  app.exec();

  return returnVal;
}
