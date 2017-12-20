#ifndef TERRAMA2_LOGGERMOCK_HPP
#define TERRAMA2_LOGGERMOCK_HPP


#include <iostream>

#include <terrama2/services/alert/core/AlertLogger.hpp>

// GoogleMock
#include <gmock/gmock.h>

class AlertLoggerMock : public terrama2::services::alert::core::AlertLogger
{

  public:

    MOCK_CONST_METHOD1(start, RegisterId(ProcessId processId));

    MOCK_CONST_METHOD1(setTableName,
                 void(std::string tableName));

    MOCK_CONST_METHOD3(result, void(ProcessLogger::Status status, const std::shared_ptr<te::dt::TimeInstantTZ> &dataTimestamp, RegisterId registerId));
    MOCK_CONST_METHOD1(getLastProcessTimestamp, std::shared_ptr< te::dt::TimeInstantTZ >(const ProcessId processId));
    MOCK_CONST_METHOD1(getDataLastTimestamp, std::shared_ptr< te::dt::TimeInstantTZ >(const ProcessId processId));
    MOCK_CONST_METHOD1(processID, ProcessId(const RegisterId registerId));
    MOCK_CONST_METHOD3(update, void(std::string& column, std::string& value, std::string& whereCondition));
    MOCK_CONST_METHOD2(updateStatus, void(std::vector<ProcessLogger::Status> oldStatus, ProcessLogger::Status newStatus));
    MOCK_CONST_METHOD1(internalClone, void(std::shared_ptr<terrama2::core::ProcessLogger> loggerCopy));
    MOCK_CONST_METHOD0(clone, std::shared_ptr<ProcessLogger>());
    MOCK_CONST_METHOD0(isValid, bool());
    MOCK_METHOD1(setConnectionInfo, void(const te::core::URI& uri));
    MOCK_CONST_METHOD2(done,
                       void(const std::shared_ptr< te::dt::TimeInstantTZ >& dataTimestamp, const RegisterId registerId));

    void log(MessageType messageType, const std::string &description, RegisterId /*registerId*/) const override
    {
      std::cout << "Message Type: " << static_cast<int>(messageType) << " Message: " << description << std::endl;
    }

};


#endif //TERRAMA2_LOGGERMOCK_HPP
