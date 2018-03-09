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
  \file src/unittest/view/MockViewLogger.hpp

  \brief Mock for TerraMA2 view Logger

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_CORE_MOCK_VIEW_PROCESSLOGGER_HPP__
#define __TERRAMA2_UNITTEST_CORE_MOCK_VIEW_PROCESSLOGGER_HPP__

#include "Config.hpp"
#include "../core/ViewLogger.hpp"

#include <gmock/gmock.h>

namespace terrama2
{
  namespace core
  {

    /*!
     * \brief The MockViewLogger class for ProcessLog unittests
     */
    class TMVIEWMOCKEXPORT MockViewLogger : public terrama2::services::view::core::ViewLogger
    {
    public:

      /*!
       * \brief Class constructor
       */
      MockViewLogger() = default;

      /*!
       * \brief Class destructor
       */
      virtual ~MockViewLogger() = default;

      MOCK_CONST_METHOD1(start,
                         RegisterId(ProcessId processId));
      MOCK_CONST_METHOD1(setTableName,
                   void(std::string tableName));
      MOCK_METHOD1(setConnectionInfo,
                   void(const te::core::URI&));
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
                         std::shared_ptr<terrama2::core::ProcessLogger>());

      MOCK_CONST_METHOD0(isValid, bool());

      MOCK_CONST_METHOD0(getConnectionInfo,te::core::URI());

      MOCK_CONST_METHOD2(setStartProcessingTime,
                        void(
                             const std::shared_ptr< te::dt::TimeInstantTZ > processingStartTime,
                             const RegisterId registerId
                            )
                        );

      MOCK_CONST_METHOD2(setEndProcessingTime,
                        void(
                             const std::shared_ptr< te::dt::TimeInstantTZ > processingEndTime,
                             const RegisterId registerId
                            )
                        );

      MOCK_CONST_METHOD3(result,
                         void(
                              Status status,
                              const std::shared_ptr<te::dt::TimeInstantTZ> &dataTimestamp,
                              const RegisterId registerId
                           )
                        );
    };

  }  // namespace core
}  // namespace terrama2

#endif // __TERRAMA2_UNITTEST_CORE_MOCK_VIEW_PROCESSLOGGER_HPP__
