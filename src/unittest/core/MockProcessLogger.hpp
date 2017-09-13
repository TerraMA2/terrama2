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
  \file unittest/core/MockProcessLogger.hpp

  \brief Mock for TerraMA2 Process Logger

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_CORE_MOCKPROCESSLOGGER_HPP__
#define __TERRAMA2_UNITTEST_CORE_MOCKPROCESSLOGGER_HPP__

#include <terrama2/core/utility/ProcessLogger.hpp>

#include <gmock/gmock.h>

namespace terrama2
{
  namespace core
  {

    /*!
     * \brief The MockProcessLogger class for ProcessLog unittests
     */
    class MockProcessLogger : public ProcessLogger
    {
    public:

      /*!
       * \brief Class constructor
       */
      MockProcessLogger() = default;

      /*!
       * \brief Class destructor
       */
      virtual ~MockProcessLogger() = default;

      MOCK_CONST_METHOD1(start,
                         RegisterId(ProcessId processId));
      MOCK_CONST_METHOD2(error,
                         void(const std::string& description, const RegisterId registerId));
      MOCK_CONST_METHOD2(done,
                         void(const std::shared_ptr< te::dt::TimeInstantTZ >& dataTimestamp, const RegisterId registerId));
      MOCK_CONST_METHOD1(getLastProcessTimestamp,
                         std::shared_ptr<te::dt::TimeInstantTZ>(const ProcessId processId));
      MOCK_CONST_METHOD1(getDataLastTimestamp,
                         std::shared_ptr<te::dt::TimeInstantTZ>(const RegisterId registerId));
      MOCK_CONST_METHOD1(processID,
                         ProcessId(const RegisterId registerId));

      MOCK_CONST_METHOD0(clone,
                         std::shared_ptr<ProcessLogger>());
    };

  }  // namespace core
}  // namespace terrama2

#endif // __TERRAMA2_UNITTEST_CORE_MOCKPROCESSLOGGER_HPP__
