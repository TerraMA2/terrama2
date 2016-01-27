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
  \file terrama2/core/TcpListener.cpp

  \brief A class to receive data through a socket.

  \author Jano Simas
*/

#include "TcpListener.hpp"
#include "DataManagerIntermediator.hpp"
#include "Logger.hpp"

//Qt
#include <QObject>
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonDocument>

terrama2::core::TcpListener::TcpListener(QObject* parent)
  : QTcpServer(parent),
    blockSize_(0)
{
  QObject::connect(this, &terrama2::core::TcpListener::newConnection, this, &terrama2::core::TcpListener::receiveConnection);
}

void terrama2::core::TcpListener::receiveConnection()
{
  TERRAMA2_LOG_INFO() << QObject::tr("Receiving new configuration...");

  QTcpSocket *tcpSocket = nextPendingConnection();
  if(!tcpSocket)
    return;

  if(!tcpSocket->waitForReadyRead())
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nSocket timeout.");
    return;
  }

  QDataStream in(tcpSocket);
  in.setVersion(QDataStream::Qt_5_2);

  if(blockSize_ == 0)
  {
    if (tcpSocket->bytesAvailable() < static_cast<int>(sizeof(uint16_t)))
    {
      TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nInvalid message size.");
      return;
    }

    in >> blockSize_;
  }

  if (tcpSocket->bytesAvailable() < blockSize_)
  {
    TERRAMA2_LOG_ERROR() << QObject::tr("Error receiving remote configuration.\nWrong message size.");
    return;
  }

  QByteArray bytearray;
  in >> bytearray;

  QJsonDocument jsonDoc = QJsonDocument::fromJson(bytearray);

  DataManagerIntermediator::fromJSON(jsonDoc.array());

  delete tcpSocket;
  blockSize_ = 0;
}
