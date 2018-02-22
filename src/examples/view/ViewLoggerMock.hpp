#ifndef TERRAMA2_VIEW_LOGGERMOCK_HPP
#define TERRAMA2_VIEW_LOGGERMOCK_HPP


#include <iostream>

#include <terrama2/services/view/core/ViewLogger.hpp>

// GoogleMock
#include <gmock/gmock.h>

class ViewLoggerMock : public terrama2::services::view::core::ViewLogger
{

  public:

    MOCK_CONST_METHOD1(start, RegisterId(ProcessId processId));
    MOCK_CONST_METHOD3(result, void(ProcessLogger::Status status, const std::shared_ptr<te::dt::TimeInstantTZ> &dataTimestamp, RegisterId registerId));
    MOCK_CONST_METHOD1(getLastProcessTimestamp, std::shared_ptr< te::dt::TimeInstantTZ >(const ProcessId processId));
    MOCK_CONST_METHOD1(getDataLastTimestamp, std::shared_ptr< te::dt::TimeInstantTZ >(const ProcessId processId));
    MOCK_CONST_METHOD1(processID, ProcessId(const RegisterId registerId));
    MOCK_CONST_METHOD3(update, void(std::string& column, std::string& value, std::string& whereCondition));
    MOCK_CONST_METHOD2(updateStatus, void(std::vector<ProcessLogger::Status> oldStatus, ProcessLogger::Status newStatus));
    MOCK_CONST_METHOD1(internalClone, void(std::shared_ptr<terrama2::core::ProcessLogger> loggerCopy));
    MOCK_CONST_METHOD0(clone, std::shared_ptr<ProcessLogger>());
    MOCK_METHOD1(setConnectionInfo, void(const te::core::URI& uri));
    MOCK_CONST_METHOD0(getConnectionInfo, te::core::URI(void));
    MOCK_CONST_METHOD2(setStartProcessingTime, void(const std::shared_ptr< te::dt::TimeInstantTZ > processingStartTime, const RegisterId registerId));
    MOCK_CONST_METHOD2(setEndProcessingTime, void(const std::shared_ptr< te::dt::TimeInstantTZ > processingEndTime, const RegisterId registerId));
    MOCK_CONST_METHOD0(isValid, bool());

    void log(MessageType messageType, const std::string &description, RegisterId) const override
    {
      std::cout << "Message Type: " << static_cast<int>(messageType) << " Message: " << description << std::endl;
    }

};


#endif //TERRAMA2_VIEW_LOGGERMOCK_HPP
