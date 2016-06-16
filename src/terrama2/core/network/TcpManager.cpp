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

bool terrama2::core::TcpManager::updateListeningPort(uint32_t port)
{
  if(serverPort() == port)
    return true;

  if(isListening())
    close();

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

void terrama2::core::TcpManager::updateService(const QByteArray& bytearray)
{
  TERRAMA2_LOG_DEBUG() << "JSon size: " << bytearray.size();
  TERRAMA2_LOG_DEBUG() << QString(bytearray);
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
  TERRAMA2_LOG_DEBUG() << "JSon size: " << bytearray.size();
  TERRAMA2_LOG_DEBUG() << QString(bytearray);
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
  TERRAMA2_LOG_DEBUG() << "JSon size: " << bytearray.size();
  TERRAMA2_LOG_DEBUG() << QString(bytearray);
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

bool terrama2::core::TcpManager::sendLog(const QByteArray& bytearray)
{
  QJsonParseError error;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray, &error);

  if(error.error != QJsonParseError::NoError)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error: %1\n").arg(error.errorString());
    return false;
  }
  else
  {
    QByteArray logArray;
    QDataStream out(&logArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    out << static_cast<uint32_t>(0);
    out << static_cast<uint32_t>(TcpSignal::LOG_SIGNAL);
    //TODO: pegar log do processLogger
    // out << log.c_str();
    out.device()->seek(0);
    out << static_cast<uint32_t>(logArray.size() - sizeof(uint32_t));

    // wait while sending message
    qint64 written = tcpSocket_->write(logArray);
    if(written == -1 || !tcpSocket_->waitForBytesWritten(30000))
    {
      // couldn't write to socket
      return false;
    }
    else
      return true;
  }
}

void terrama2::core::TcpManager::readReadySlot()
{
  {
    //Raii block
    RaiiBlock block(blockSize_);

    QDataStream in(tcpSocket_.get());
    TERRAMA2_LOG_DEBUG() << "bytes available: " << tcpSocket_->bytesAvailable();

    Q_UNUSED(block)
    if(blockSize_ == 0)
    {
      if(tcpSocket_->bytesAvailable() < static_cast<int>(sizeof(uint32_t)))
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nInvalid message size.");
        return;
      }


      in >> blockSize_;
      TERRAMA2_LOG_DEBUG() << "message size: " << blockSize_;
    }

    if(tcpSocket_->bytesAvailable() < blockSize_)
    {
      auto bytearray = tcpSocket_->readAll();
      TERRAMA2_LOG_DEBUG() << bytearray.right(bytearray.size()-4).data();
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nWrong message size.");
      return;
    }

    int sigInt = -1;
    in >> sigInt;

    //read signal
    TcpSignal signal = static_cast<TcpSignal>(sigInt);
    //update left blockSize
    blockSize_-=sizeof(TcpSignal);

   if(signal != TcpSignal::UPDATE_SERVICE_SIGNAL && !serviceManager_->serviceLoaded())
    {
      // wait for TcpSignals::UPDATE_SERVICE_SIGNAL
      return;
    }

    switch(signal)
    {
      case TcpSignal::UPDATE_SERVICE_SIGNAL:
      {
        QByteArray bytearray = tcpSocket_->read(blockSize_);

        updateService(bytearray);
        break;
      }
      case TcpSignal::TERMINATE_SERVICE_SIGNAL:
      {
        TERRAMA2_LOG_DEBUG() << "TERMINATE_SERVICE_SIGNAL";

        serviceManager_->setShuttingDownProcessInitiated();

        emit stopSignal();
        break;
      }
      case TcpSignal::ADD_DATA_SIGNAL:
      {
        TERRAMA2_LOG_DEBUG() << "ADD_DATA_SIGNAL";
        QByteArray bytearray = tcpSocket_->read(blockSize_);

        addData(bytearray);
        break;
      }
      case TcpSignal::REMOVE_DATA_SIGNAL:
      {
        TERRAMA2_LOG_DEBUG() << "REMOVE_DATA_SIGNAL";
        QByteArray bytearray = tcpSocket_->read(blockSize_);

        removeData(bytearray);
        break;
      }
      case TcpSignal::START_PROCESS_SIGNAL:
      {
        TERRAMA2_LOG_DEBUG() << "START_PROCESS_SIGNAL";
        uint32_t dataId;
        in >> dataId;

        emit startProcess(dataId);

        break;
      }
      case TcpSignal::STATUS_SIGNAL:
      {
        TERRAMA2_LOG_DEBUG() << "STATUS_SIGNAL";
        QByteArray bytearray;
        QDataStream out(&bytearray, QIODevice::WriteOnly);

        auto jsonObj = ServiceManager::getInstance().status();
        QJsonDocument doc(jsonObj);

        out << static_cast<uint32_t>(0);
        out << static_cast<uint32_t>(TcpSignal::STATUS_SIGNAL);
        out << doc.toJson(QJsonDocument::Compact);
        bytearray.remove(8, 4);//Remove QByteArray header
        out.device()->seek(0);
        out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

        // wait while sending message
        qint64 written = tcpSocket_->write(bytearray);
        if(written == -1 || !tcpSocket_->waitForBytesWritten(30000))
          TERRAMA2_LOG_WARNING() << QObject::tr("Unable to establish connection with server.");

        break;
      }
      case TcpSignal::LOG_SIGNAL:
      {
        TERRAMA2_LOG_DEBUG() << "LOG_SIGNAL";

        // removeData(bytearray);
        break;
      }
      default:
        TERRAMA2_LOG_ERROR() << QObject::tr("Error\n Unknown signal received.");
        break;
    }
  }//end of Raii block

  if(tcpSocket_.get() && !tcpSocket_->atEnd())
    readReadySlot();
}

void terrama2::core::TcpManager::receiveConnection()
{
  TERRAMA2_LOG_INFO() << QObject::tr("Receiving new configuration...");

  tcpSocket_.reset(nextPendingConnection());
  if(!tcpSocket_.get())
    return;

  connect(tcpSocket_.get(), &QTcpSocket::readyRead, this, &terrama2::core::TcpManager::readReadySlot);
}
