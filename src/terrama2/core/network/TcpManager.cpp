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
  \file terrama2/core/TcpManager.cpp
  \brief A class to receive data through a socket.
  \author Jano Simas
*/

#include "TcpManager.hpp"
#include "TcpSignals.hpp"
#include "../utility/Logger.hpp"
#include "../utility/ServiceManager.hpp"
#include "../utility/TimeUtils.hpp"
#include "../data-model/DataManager.hpp"

// Qt
#include <QObject>
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class RaiiBlock
{
  public:
    RaiiBlock(uint32_t& block) : block_(block) {}
    ~RaiiBlock() {block_ = 0;}

    uint32_t& block_;
};

bool terrama2::core::TcpManager::updateListeningPort(uint32_t port) noexcept
{
  try
  {
    if(serverPort() == port)
      return true;

    if(isListening())
      close();

    return listen(serverAddress(), static_cast<quint16>(port));
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    return false;
  }
}

terrama2::core::TcpManager::TcpManager(std::weak_ptr<terrama2::core::DataManager> dataManager,
                                       std::weak_ptr<terrama2::core::ProcessLogger> logger,
                                       QObject* parent)
  : QTcpServer(parent),
    tcpSocket_(nullptr),
    blockSize_(0),
    dataManager_(dataManager),
    logger_(logger)
{
  QObject::connect(this, &terrama2::core::TcpManager::newConnection, this, &terrama2::core::TcpManager::receiveConnection);
  serviceManager_ = &terrama2::core::ServiceManager::getInstance();
}

void terrama2::core::TcpManager::sendStartProcess(const QJsonObject& obj)
{
  std::shared_ptr< te::dt::TimeInstantTZ > executionDate;
  if(obj.contains("execution_date"))
  {
    auto dateStr = obj["execution_date"].toString().toStdString();
    executionDate = TimeUtils::stringToTimestamp(dateStr, TimeUtils::webgui_timefacet);
  }
  else
    executionDate = terrama2::core::TimeUtils::nowUTC();

  auto array = obj["ids"].toArray();
  for(auto value : array)
  {
    emit startProcess(value.toInt(), executionDate);
  }
}

QJsonObject terrama2::core::TcpManager::logToJson(const terrama2::core::ProcessLogger::Log& log)
{
  QJsonObject obj;
  obj.insert("process_id", static_cast<int>(log.processId));
  obj.insert("status", static_cast<int>(log.status));
  obj.insert("start_timestamp", QString::fromStdString(TimeUtils::getISOString(log.start_timestamp)));
  obj.insert("data_timestamp", QString::fromStdString(TimeUtils::getISOString(log.data_timestamp)));
  obj.insert("last_process_timestamp", QString::fromStdString(TimeUtils::getISOString(log.last_process_timestamp)));
  obj.insert("data", QString::fromStdString(log.data));

  QJsonArray msgArray;
  for(const auto& msg  : log.messages)
  {
    QJsonObject msgObj;
    msgObj.insert("type", static_cast<int>(msg.type));
    msgObj.insert("description", QString::fromStdString(msg.description));
    msgObj.insert("timestamp", QString::fromStdString(TimeUtils::getISOString(msg.timestamp)));

    msgArray.append(msgObj);
  }
  obj.insert("messages", msgArray);

  return obj;
}

void terrama2::core::TcpManager::sendLog(const QJsonObject& jsonObject, QTcpSocket* tcpSocket)
{
  auto idsArray = jsonObject.value("process_ids").toArray();

  uint32_t begin = static_cast<uint32_t>(jsonObject.value("begin").toInt());
  uint32_t end = static_cast<uint32_t>(jsonObject.value("end").toInt());

  QJsonArray logList;
  for(auto value : idsArray)
  {
    auto processId = static_cast<ProcessId>(value.toInt());

    QJsonArray processLogList;
    auto logger = logger_.lock();
    auto logs = logger->getLogs(processId, begin, end);
    for(const auto& log : logs)
    {
      processLogList.append(logToJson(log));
    }

    QJsonObject obj;
    obj.insert("process_id",  static_cast<int>(processId));
    obj.insert("instance_id",  static_cast<int>(serviceManager_->instanceId()));
    obj.insert("log", processLogList);

    logList.push_back(obj);
  }

  QJsonDocument doc(logList);
  sendSignalSlot(tcpSocket, TcpSignal::LOG_SIGNAL, doc);
}


void terrama2::core::TcpManager::readReadySlot(QTcpSocket* tcpSocket) noexcept
{
  try
  {
    {
      QDataStream in(tcpSocket);
      TERRAMA2_LOG_DEBUG() << "bytes available: " << tcpSocket->bytesAvailable();

      if(blockSize_ == 0)
      {
        if(tcpSocket->bytesAvailable() < static_cast<int>(sizeof(uint32_t)))
        {
          TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nInvalid message size.");
          return;
        }

        in >> blockSize_;
        TERRAMA2_LOG_DEBUG() << "message size: " << blockSize_;
      }

      if(tcpSocket->bytesAvailable() < blockSize_)
      {
        //The message isn't complete, wait for next readReady signal
        return;
      }

      // clear socket after message processing
      RaiiSocket raiiSocket(tcpSocket); Q_UNUSED(raiiSocket);

      // clear block after message processing
      RaiiBlock block(blockSize_);  Q_UNUSED(block)

      int sigInt = -1;
      in >> sigInt;

      //read signal
      TcpSignal signal = static_cast<TcpSignal>(sigInt);
      //update left blockSize
      blockSize_-=sizeof(TcpSignal);

      if(signal != TcpSignal::UPDATE_SERVICE_SIGNAL
         && signal != TcpSignal::STATUS_SIGNAL
         && !serviceManager_->serviceLoaded())
      {
        // wait for TcpSignals::UPDATE_SERVICE_SIGNAL
        return;
      }

      // read data from buffer
      QByteArray bytearray = tcpSocket->read(blockSize_);
      TERRAMA2_LOG_DEBUG() << "JSon size: " << bytearray.size();
      TERRAMA2_LOG_DEBUG() << QString(bytearray);

      QJsonParseError error;
      QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray, &error);

      if(error.error != QJsonParseError::NoError)
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error: %1\n").arg(error.errorString());
        return;
      }
      auto jsonObject = jsonDoc.object();
      if(!jsonObject.contains("webAppId"))
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nNo webAppId available.");
        return;
      }

      auto remoteWebAppId = jsonObject["webAppId"].toString().toStdString();
      auto localWebAppId = serviceManager_->webAppId();
      if(serviceManager_->serviceLoaded() && localWebAppId != remoteWebAppId)
      {
        // not the service they are looking for
        return;
      }

      switch(signal)
      {
        case TcpSignal::UPDATE_SERVICE_SIGNAL:
        {
          serviceManager_->updateService(jsonObject);
          break;
        }
        case TcpSignal::TERMINATE_SERVICE_SIGNAL:
        {
          TERRAMA2_LOG_DEBUG() << "TERMINATE_SERVICE_SIGNAL";

          serviceManager_->setShuttingDownProcessInitiated();

          emit stopSignal();

          sendSignalSlot(tcpSocket, TcpSignal::TERMINATE_SERVICE_SIGNAL);

          emit closeApp();

          break;
        }
        case TcpSignal::ADD_DATA_SIGNAL:
        {
          TERRAMA2_LOG_DEBUG() << "ADD_DATA_SIGNAL";
          try
          {
            std::shared_ptr<terrama2::core::DataManager> dataManager = dataManager_.lock();
            dataManager->addJSon(jsonObject);
          }
          catch(const terrama2::Exception& exception)
          {
            const auto msg = boost::get_error_info<terrama2::ErrorDescription>(exception);
            QString message;
            if (msg != nullptr)
              message = *msg;
            else
              message = QObject::tr("Unknown error occurred at data insertion.");

            QJsonObject obj;
            obj.insert("add_data_error", message);
            QJsonDocument answer(obj);
            sendSignalSlot(tcpSocket, TcpSignal::ADD_DATA_SIGNAL, answer);
          }
          break;
        }
        case TcpSignal::VALIDATE_PROCESS_SIGNAL:
        {
          TERRAMA2_LOG_DEBUG() << "VALIDATE_PROCESS_SIGNAL";
          std::shared_ptr<terrama2::core::DataManager> dataManager = dataManager_.lock();
          dataManager->validateJSon(jsonObject);
          break;
        }
        case TcpSignal::REMOVE_DATA_SIGNAL:
        {
          TERRAMA2_LOG_DEBUG() << "REMOVE_DATA_SIGNAL";
          std::shared_ptr<terrama2::core::DataManager> dataManager = dataManager_.lock();
          dataManager->removeJSon(jsonObject);
          break;
        }
        case TcpSignal::START_PROCESS_SIGNAL:
        {
          TERRAMA2_LOG_DEBUG() << "START_PROCESS_SIGNAL";
          sendStartProcess(jsonObject);

          break;
        }
        case TcpSignal::STATUS_SIGNAL:
        {
          auto jsonObj = ServiceManager::getInstance().status();
          jsonObj.insert("instance_id", static_cast<int>(serviceManager_->instanceId()));
          QJsonDocument doc(jsonObj);
          sendSignalSlot(tcpSocket, TcpSignal::STATUS_SIGNAL, doc);
          break;
        }
        case TcpSignal::LOG_SIGNAL:
        {
          TERRAMA2_LOG_DEBUG() << "LOG_SIGNAL";
          sendLog(jsonObject, tcpSocket);
          break;
        }
        default:
          TERRAMA2_LOG_ERROR() << QObject::tr("Error\n Unknown signal received.");
          break;
      }
    }//end of Raii block

    if(tcpSocket && !tcpSocket->atEnd())
      readReadySlot(tcpSocket);
  }
  catch(const LogException&)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Erro in logger, check your log database connection information.");
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }
}

void terrama2::core::TcpManager::receiveConnection() noexcept
{
  try
  {
    TERRAMA2_LOG_INFO() << QObject::tr("Receiving new configuration...");

    QTcpSocket* tcpSocket(nextPendingConnection());
    if(!tcpSocket)
      return;

//TODO: review if this is needed and if the older socket needs to be closed
    tcpSocket_ = tcpSocket;
//    QObject::connect(tcpSocket_, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
//        [=](QAbstractSocket::SocketError socketError){ TERRAMA2_LOG_ERROR() << QObject::tr("Unable to establish connection with server.\nQAbstractSocket::SocketError code: %1").arg(socketError); });

    connect(tcpSocket, &QTcpSocket::readyRead, this, [this, tcpSocket]() { readReadySlot(tcpSocket);}, Qt::QueuedConnection);
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }

  return;
}

void terrama2::core::TcpManager::sendProcessFinishedSlot(QJsonObject answer) noexcept
{
  answer.insert("instance_id", static_cast<int>(serviceManager_->instanceId()));
  QJsonDocument doc(answer);
  sendSignalSlot(tcpSocket_, TcpSignal::PROCESS_FINISHED_SIGNAL, doc);
}

void terrama2::core::TcpManager::sendValidateProcessSlot(QJsonObject answer) noexcept
{
  answer.insert("instance_id", static_cast<int>(serviceManager_->instanceId()));
  QJsonDocument doc(answer);
  sendSignalSlot(tcpSocket_, TcpSignal::VALIDATE_PROCESS_SIGNAL, doc);
}

void terrama2::core::TcpManager::sendSignalSlot(QTcpSocket* tcpSocket, TcpSignal signal, QJsonDocument answer) noexcept
{
  TERRAMA2_LOG_DEBUG() << QObject::tr("Sending signal information...");

  const std::string bom = "(BOM)";
  const std::string eom = "(EOM)";

  ///////////////////////////////////////////
  // Prepare message
  QByteArray message;
  QDataStream out(&message, QIODevice::WriteOnly);

  out << static_cast<uint32_t>(0);
  out << static_cast<uint32_t>(signal);

  if(!answer.isEmpty())
  {
    out << answer.toJson(QJsonDocument::Compact);
  }

  message.remove(8, 4);
  out.device()->seek(0);
  out << static_cast<uint32_t>(message.size() - sizeof(uint32_t));
  ///////////////////////////////////////////

  // for(auto it = message.cbegin(); it != message.cend(); ++it)
  //   std::cout << *it;
  // std::cout << std::endl;

  ///////////////////////////////////////////
  // add header
  QByteArray buffer;
  QDataStream bufferStream(&buffer, QIODevice::WriteOnly);
  bufferStream << bom.c_str() << message << eom.c_str();

  // for(auto it = buffer.cbegin(); it != buffer.cend(); ++it)
  //   std::cout << *it;
  // std::cout << std::endl;

  buffer.remove(buffer.size() - eom.size() - 5, 4);
  buffer.remove(bom.size()+5, 4);
  buffer.remove(0,4);
  ///////////////////////////////////////////

  // for(auto it = buffer.cbegin(); it != buffer.cend(); ++it)
  //   std::cout << *it;
  // std::cout << std::endl;

  // wait while sending message
  qint64 written = tcpSocket->write(buffer);
  if(written == -1 || !tcpSocket->waitForBytesWritten(30000))
    TERRAMA2_LOG_WARNING() << QObject::tr("Unable to write to server.");
}

terrama2::core::RaiiSocket::~RaiiSocket()
{
  auto bytes = tcpSocket_->readAll();
  if(bytes.size())
  {
    QString errMsg(bytes);
    TERRAMA2_LOG_ERROR() << QObject::tr("Garbage in socket message:\n%1.").arg(errMsg);
  }
}
