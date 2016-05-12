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

bool terrama2::core::TcpManager::updateListeningPort(int port)
{
  if(serverPort() == port)
    return true;

  return listen(serverAddress(), port);
}

terrama2::core::TcpManager::TcpManager(std::weak_ptr<terrama2::core::DataManager> dataManager, QObject* parent)
  : QTcpServer(parent),
    blockSize_(0),
    dataManager_(dataManager)
{
  QObject::connect(this, &terrama2::core::TcpManager::newConnection, this, &terrama2::core::TcpManager::receiveConnection);
  serviceManager_ = &terrama2::core::ServiceManager::getInstance();
}

terrama2::core::TcpManager::~TcpManager()
{
}

void terrama2::core::TcpManager::updateService(const QByteArray& bytearray)
{
  QJsonParseError error;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray, &error);

  if(error.error != QJsonParseError::NoError)
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error: %1\n").arg(error.errorString());
  else
  {
    if(jsonDoc.isObject())
    {
      auto obj = jsonDoc.object();
      serviceManager_->updateService(obj);
    }
    else
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson is not an object.\n");
  }
}

void terrama2::core::TcpManager::addData(const QByteArray& bytearray)
{
  QJsonParseError error;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray, &error);

  if(error.error != QJsonParseError::NoError)
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error: %1\n").arg(error.errorString());
  else
  {
    std::shared_ptr<terrama2::core::DataManager> dataManager = dataManager_.lock();
    if(jsonDoc.isObject())
    {
      auto obj = jsonDoc.object();
      dataManager->addJSon(obj);
    }
    else
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson is not an object.\n");
  }
}

void terrama2::core::TcpManager::removeData(const QByteArray& bytearray)
{
  QJsonParseError error;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray, &error);

  if(error.error != QJsonParseError::NoError)
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error: %1\n").arg(error.errorString());
  else
  {
    std::shared_ptr<terrama2::core::DataManager> dataManager = dataManager_.lock();
    if(jsonDoc.isObject())
    {
      auto obj = jsonDoc.object();
      dataManager->removeJSon(obj);
    }
    else
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson is not an object.\n");
  }
}

bool terrama2::core::TcpManager::sendLog(std::string log)
{
  QByteArray bytearray;
  QDataStream out(&bytearray, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_2);

  out << static_cast<uint32_t>(0);
  out << TcpSignals::LOG_SIGNAL;
  out << log.c_str();
  out.device()->seek(0);
  out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

  // wait while sending message
  qint64 written = tcpSocket_->write(bytearray);
  if(written == -1 || !tcpSocket_->waitForBytesWritten())
  {
    // couldn't write to socket
    return false;
  }
  else
    return true;
}

void terrama2::core::TcpManager::readReadySlot()
{
  RaiiBlock block(blockSize_);

  QDataStream in(tcpSocket_.get());

  Q_UNUSED(block)
  if(blockSize_ == 0)
  {
    if(tcpSocket_->bytesAvailable() < static_cast<int>(sizeof(uint32_t)))
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nInvalid message size.");
      return;
    }

    in >> blockSize_;
  }

  if(tcpSocket_->bytesAvailable() != blockSize_)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nWrong message size.");
    return;
  }

  int sigInt = -1;
  in >> sigInt;

  TcpSignals::TcpSignal signal = static_cast<TcpSignals::TcpSignal>(sigInt);
  if(signal != TcpSignals::UPDATE_SERVICE_SIGNAL && !serviceManager_->serviceLoaded())
  {
    TERRAMA2_LOG_ERROR() << tr("Signal received before service load information.");

    //FIXME: remove comment when web interface start sending TcpSignals::UPDATE_SERVICE_SIGNAL
    // emit stopSignal();
    // return;
  }

  switch(signal)
  {
    case TcpSignals::UPDATE_SERVICE_SIGNAL:
    {
      QByteArray bytearray = tcpSocket_->readAll();

      updateService(bytearray);
      break;
    }
    case TcpSignals::TERMINATE_SERVICE_SIGNAL:
    {
      TERRAMA2_LOG_DEBUG() << "TERMINATE_SERVICE_SIGNAL";

      emit stopSignal();

      //TODO: send back TERMINATE_SERVICE_SIGNAL
      break;
    }
    case TcpSignals::ADD_DATA_SIGNAL:
    {
      TERRAMA2_LOG_DEBUG() << "ADD_DATA_SIGNAL";
      QByteArray bytearray = tcpSocket_->readAll();

      addData(bytearray);
      break;
    }
    case TcpSignals::REMOVE_DATA_SIGNAL:
    {
      TERRAMA2_LOG_DEBUG() << "REMOVE_DATA_SIGNAL";
      QByteArray bytearray = tcpSocket_->readAll();

      removeData(bytearray);
      break;
    }
    case TcpSignals::START_PROCESS_SIGNAL:
    {
      TERRAMA2_LOG_DEBUG() << "START_PROCESS_SIGNAL";
      int dataId;
      in >> dataId;

      emit startProcess(dataId);

      break;
    }
    case TcpSignals::STATUS_SIGNAL:
    {
      TERRAMA2_LOG_DEBUG() << "STATUS_SIGNAL";
      QByteArray bytearray;
      QDataStream out(&bytearray, QIODevice::WriteOnly);

      auto jsonObj = ServiceManager::getInstance().status();
      QJsonDocument doc(jsonObj);

      out << static_cast<uint32_t>(0);
      out << TcpSignals::STATUS_SIGNAL;
      out << doc.toJson(QJsonDocument::Compact);
      bytearray.remove(8, 4);//Remove QByteArray header
      out.device()->seek(0);
      out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

      // wait while sending message
      qint64 written = tcpSocket_->write(bytearray);
      if(written == -1 || !tcpSocket_->waitForBytesWritten())
        TERRAMA2_LOG_WARNING() << QObject::tr("Unable to establish connection with server.");

      break;
    }
    default:
      TERRAMA2_LOG_ERROR() << QObject::tr("Error\n Unknown signal received.");
      break;
  }

  blockSize_ = 0;
}

void terrama2::core::TcpManager::receiveConnection()
{
  TERRAMA2_LOG_INFO() << QObject::tr("Receiving new configuration...");

  tcpSocket_.reset(nextPendingConnection());
  if(!tcpSocket_.get())
    return;

  connect(tcpSocket_.get(), &QTcpSocket::readyRead, this, &terrama2::core::TcpManager::readReadySlot);
}
