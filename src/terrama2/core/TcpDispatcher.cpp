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
  \file terrama2/core/TcpDispatcher.hpp

  \brief A class to send data through a socket.

  \author Jano Simas
*/

#include "TcpSignals.hpp"
#include "TcpDispatcher.hpp"
#include "Logger.hpp"

//Qt
#include <QByteArray>
#include <QDataStream>
#include <QJsonDocument>

terrama2::core::TcpDispatcher::TcpDispatcher(const ServiceData& serviceData)
  : serviceData_(serviceData)
{

}

void terrama2::core::TcpDispatcher::stopService()
{
  std::shared_ptr<QTcpSocket> tcpSocket = socket();

  QByteArray bytearray;
  QDataStream out(&bytearray, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_2);

  out << static_cast<uint16_t>(0);
  out << TcpSignals::STOP_SIGNAL;
  out.device()->seek(0);
  out << static_cast<uint16_t>(bytearray.size() - sizeof(uint16_t));

  qint64 written = tcpSocket->write(bytearray);
  if(written == -1 || !tcpSocket->waitForBytesWritten())
  {
    QString errMsg = QObject::tr("Could not send stop signal to host: %1").arg(serviceData_.name.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UnableToConnect() << ErrorDescription(errMsg);
  }
}

bool terrama2::core::TcpDispatcher::pingService() noexcept
{
  try
  {
    std::shared_ptr<QTcpSocket> tcpSocket = socket(false);

    QByteArray bytearray;
    QDataStream out(&bytearray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    out << static_cast<uint16_t>(0);
    out << TcpSignals::PING_SIGNAL;
    out.device()->seek(0);
    out << static_cast<uint16_t>(bytearray.size() - sizeof(uint16_t));

    //wait while sending message
    qint64 written = tcpSocket->write(bytearray);
    if(written == -1 || !tcpSocket->waitForBytesWritten())
    {
      TERRAMA2_LOG_WARNING() << QObject::tr("Unable to establish connection with remote service: %1.").arg(QString::fromStdString(serviceData_.name));
      //unable to send
      return false;
    }

    //wait for answer
    if(tcpSocket->waitForReadyRead())
    {
      //check message size
      if (tcpSocket->bytesAvailable() < static_cast<int>(sizeof(uint16_t)))
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving PONG_SIGNAL from remote service %1.\nWrong message size.").arg(QString::fromStdString(serviceData_.name));
        return false;
      }

      //read message size
      QDataStream in(tcpSocket.get());
      in.setVersion(QDataStream::Qt_5_2);
      uint16_t block = 0;
      in >> block;

      //check message size
      if (tcpSocket->bytesAvailable() < block)
      {
        TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving PONG_SIGNAL from remote service %1.\nWrong message size.").arg(QString::fromStdString(serviceData_.name));
        return false;
      }

      //read signal
      int sigInt = -1;
      in >> sigInt;

      //check signal
      TcpSignals::TcpSignal signal = static_cast<TcpSignals::TcpSignal >(sigInt);
      if(signal == TcpSignals::PONG_SIGNAL)
        return true;
      else
      {
        //wrong signal received
        TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving PONG_SIGNAL from remote service %1.\nWrong answer to PING_SIGNAL.").arg(QString::fromStdString(serviceData_.name));
        return false;
      }
    }
  }
  catch(const UnableToConnect&)
  {
    //service not running
    return false;
  }

  catch(...)
  {
    //TODO: specify catch?
    TERRAMA2_LOG_WARNING() << QObject::tr("Unable to establish connection with remote service: %1.").arg(QString::fromStdString(serviceData_.name));
    return false;
  }

  return false;
}

void terrama2::core::TcpDispatcher::startProcessing(int dataId)
{
  std::shared_ptr<QTcpSocket> tcpSocket = socket();

  QByteArray bytearray;
  QDataStream out(&bytearray, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_2);

  out << static_cast<uint16_t>(0);
  out << TcpSignals::START_SIGNAL;
  out << dataId;
  out.device()->seek(0);
  out << static_cast<uint16_t>(bytearray.size() - sizeof(uint16_t));

  qint64 written = tcpSocket->write(bytearray);
  if(written == -1 || !tcpSocket->waitForBytesWritten())
  {
    QString errMsg = QObject::tr("Could not send stop signal to host: %1").arg(serviceData_.name.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UnableToConnect() << ErrorDescription(errMsg);
  }
}

void terrama2::core::TcpDispatcher::sendData(const QJsonArray& jsonArray)
{
  try
  {
    std::shared_ptr<QTcpSocket> tcpSocket = socket();

    QJsonDocument jsonDoc(jsonArray);

    QByteArray bytearray;
    QDataStream out(&bytearray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    out << static_cast<uint16_t>(0);
    out << TcpSignals::DATA_SIGNAL;
    out << jsonDoc.toJson();
    out.device()->seek(0);
    out << static_cast<uint16_t>(bytearray.size() - sizeof(uint16_t));

    qint64 written = tcpSocket->write(bytearray);
    if(written == -1 || !tcpSocket->waitForBytesWritten())
    {
      QString errMsg = QObject::tr("Could not send data to host: %1").arg(serviceData_.name.c_str());
      TERRAMA2_LOG_ERROR() << errMsg;
      throw UnableToConnect() << ErrorDescription(errMsg);
    }

    tcpSocket->disconnectFromHost();
  }
  catch(...)//TODO: catch instance not found...
  {

  }
}

std::shared_ptr<QTcpSocket> terrama2::core::TcpDispatcher::socket(bool log)
{
  std::shared_ptr<QTcpSocket> socket = std::make_shared<QTcpSocket>();
  socket->connectToHost(serviceData_.host.c_str(), serviceData_.servicePort);

  QTcpSocket::SocketError socketError = socket->error();
  if(socketError != QTcpSocket::UnknownSocketError || !socket->waitForConnected(30000))
  {
    QString errMsg = QObject::tr("Could not connect to host: %1\nQSocket error code: %2").arg(QString::fromStdString(serviceData_.name)).arg(static_cast<int>(socketError));

    if(log) TERRAMA2_LOG_ERROR() << errMsg;

    throw UnableToConnect() << ErrorDescription(errMsg);
  }

  return socket;
}
