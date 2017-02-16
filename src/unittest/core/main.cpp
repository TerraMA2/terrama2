/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file unittest/core/main.cpp

  \brief Main test class to TerraMA2 core module.

  \author Jano Simas
  \author Vinicius Campanha
  \author Evandro Delatin
*/

// Qt
#include <QTimer>

//TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terralib/Exception.h>

// GoogleMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TsUtility.hpp"
#include "TsProcessLogger.hpp"
#include "TsDataRetrieverFTP.hpp"
#include "TsDataAccessorFile.hpp"
#include "TsDataAccessorTxtFile.hpp"
#include "TsDataAccessorDcpInpe.hpp"
#include "TsDataAccessorDcpToa5.hpp"
#include "TsDataAccessorGDal.hpp"
#include "TsDataAccessorOccurrenceWfp.hpp"

int main(int argc, char** argv)
{
  int ret = 0;
  QCoreApplication app(argc, argv);

  try
  {
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    terrama2::core::TerraMA2Init terramaRaii("unittest", 0);
    terrama2::core::disableLogger();

    try
    {
      TsProcessLogger testLogger;
      ret += QTest::qExec(&testLogger, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataRetrieverFTP testDataRetrieverFTP;
      ret += QTest::qExec(&testDataRetrieverFTP, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataAccessorFile testDataAccessorFile;
      ret += QTest::qExec(&testDataAccessorFile, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataAccessorTxtFile testDataAccessorTxtFile;
      ret += QTest::qExec(&testDataAccessorTxtFile, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataAccessorDcpInpe testDataAccessorDcpInpe;
      ret += QTest::qExec(&testDataAccessorDcpInpe, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataAccessorDcpToa5 testDataAccessorDcpToa5;
      ret += QTest::qExec(&testDataAccessorDcpToa5, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataAccessorGDal testDataAccessorGDal;
      ret += QTest::qExec(&testDataAccessorGDal, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsDataAccessorOccurrenceWfp testDataAccessorOccurrenceWfp;
      ret += QTest::qExec(&testDataAccessorOccurrenceWfp, argc, argv);
    }
    catch(...)
    {

    }

    try
    {
      TsUtility testUtility;
      ret += QTest::qExec(&testUtility, argc, argv);
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
    std::cerr << QObject::tr("Unexpected exception...") << std::endl;
    return 1;
  }

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return ret;
}
