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
#include "../data-model/DataManager.hpp"

// Qt
#include <QObject>
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>

class RaiiBlock
{
public:
  RaiiBlock(uint32_t& block) : block_(block) {}
  ~RaiiBlock() { block_ = 0; }

  uint32_t& block_;
};

bool terrama2::core::TcpManager::listen(std::weak_ptr<terrama2::core::DataManager> dataManager, const QHostAddress& address, quint16 port)
{
  dataManager_ = dataManager;
  return listen(address, port);
}

terrama2::core::TcpManager::TcpManager(QObject* parent) : QTcpServer(parent), blockSize_(0)
{
  QObject::connect(this, &terrama2::core::TcpManager::newConnection, this, &terrama2::core::TcpManager::receiveConnection);
}

void terrama2::core::TcpManager::parseData(QByteArray bytearray)
{
  QJsonParseError error;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray, &error);

  if(error.error != QJsonParseError::NoError)
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson parse error:\n").arg(error.errorString());
  else
  {
    if(jsonDoc.isArray())
    {
      std::shared_ptr<terrama2::core::DataManager> dataManager = dataManager_.lock();
      auto jsonArray = jsonDoc.array();
      std::for_each(jsonArray.constBegin(), jsonArray.constEnd(),
                    std::bind(&terrama2::core::DataManager::addFromJSON, dataManager, std::placeholders::_1));
    }
    else
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nJson is not an array.\n");
  }
}

bool terrama2::core::TcpManager::sendLog(std::string log)
{
  QByteArray bytearray;
  QDataStream out(&bytearray, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_2);

  out << static_cast<uint16_t>(0);
  out << TcpSignals::ERROR_SIGNAL;
  out << log.c_str();
  out.device()->seek(0);
  out << static_cast<uint16_t>(bytearray.size() - sizeof(uint16_t));

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
  QDataStream in(tcpSocket_);

  TERRAMA2_LOG_DEBUG() << "bytesAvailable: " << tcpSocket_->bytesAvailable();

  RaiiBlock block(blockSize_);
  Q_UNUSED(block)
  if(blockSize_ == 0)
  {
    if(tcpSocket_->bytesAvailable() < static_cast<int>(sizeof(uint32_t)))
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nInvalid message size.");
      return;
    }

    in >> blockSize_;
    TERRAMA2_LOG_DEBUG() << "blockSize: " << blockSize_;
  }

  if(tcpSocket_->bytesAvailable() != blockSize_)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nWrong message size.");
    return;
  }

  int sigInt = -1;
  in >> sigInt;

  TcpSignals::TcpSignal signal = static_cast<TcpSignals::TcpSignal>(sigInt);

  switch(signal)
  {
  case TcpSignals::TERMINATE_SIGNAL:
  {
    TERRAMA2_LOG_DEBUG() << "TERMINATE_SIGNAL";
    emit stopSignal();
    break;
  }
  case TcpSignals::DATA_SIGNAL:
  {
    TERRAMA2_LOG_DEBUG() << "DATA_SIGNAL";
    QByteArray bytearray(blockSize_-4, '\0');
    in.readRawData(bytearray.data(), blockSize_-4);
    // new data received
    parseData(bytearray);
    break;
  }
  case TcpSignals::START_SIGNAL:
  {
    TERRAMA2_LOG_DEBUG() << "START_SIGNAL";
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

    // out << static_cast<uint32_t>(0);
    out << TcpSignals::STATUS_SIGNAL;
    out << 4;
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

  tcpSocket_ = nextPendingConnection();
  if(!tcpSocket_)
    return;

  connect(tcpSocket_, &QTcpSocket::readyRead, this, &terrama2::core::TcpManager::readReadySlot);
}
