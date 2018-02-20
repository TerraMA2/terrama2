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
  \file terrama2/core/TcpManager.hpp
  \brief A class to receive data through a socket.
  \author Jano Simas
*/


#ifndef __TERRAMA2_CORE_TCP_MANAGER_HPP__
#define __TERRAMA2_CORE_TCP_MANAGER_HPP__

#include <QTcpServer>
#include <QTcpSocket>

#include <memory>
#include <QtCore/QJsonDocument>

#include "TcpSignals.hpp"
#include "../utility/ProcessLogger.hpp"

class QTcpSocket;

namespace terrama2
{
  namespace core
  {
    class ServiceManager;

    class DataManager;
    /*!
       \brief Manages all TCP data communication between the service and TerraMA² application.

       The TcpManager will listen to signals in the host specified in listen(),
       the received will be forwarded to the DataManager or emit signals as needed.

       ## Message format ##

       The TcpManager receives messages with the format:
       \code
       < size of the message > < TcpSignals > < json >
       \endcode

       The size of the message is a 32 bit integer with the signal size plus the json size.

       The signal is one of the TcpSignals.

       The json format is specified for each signal. Detailed information can be found in
       https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/tcp

       \sa TcpSignals

       \sa Detailed information of data message: https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/tcp

     */
    class TMCOREEXPORT TcpManager : public QTcpServer
    {
        Q_OBJECT

      public:
        //! Constructor, connects signal.
        TcpManager(std::weak_ptr<terrama2::core::DataManager> dataManager,
                   std::weak_ptr<terrama2::core::ProcessLogger> logger,
                   QObject* parent = 0);
        /*!
          \brief Send a finishing service message and destroys the object.
        */
        virtual ~TcpManager() = default;

        TcpManager(const TcpManager& other) = delete;
        TcpManager(TcpManager&& other) = delete;
        TcpManager& operator=(const TcpManager& other) = delete;
        TcpManager& operator=(TcpManager&& other) = delete;

        //! Send log information to the TerraMA² application.
        void sendLog(const QByteArray& bytearray, QTcpSocket* tcpSocket);

        /*!
          \brief Listens to TCP socket connections.

          \see <a href="http://doc.qt.io/qt-5/qtcpserver.html">QTcpServer </a>
        */
        using QTcpServer::listen;

      public slots:
        bool updateListeningPort(uint32_t) noexcept;

        //! Send validate process information.
        void sendValidateProcessSlot(QJsonObject answer) noexcept;

        //! Send process finished information.
        void sendProcessFinishedSlot(QJsonObject answer) noexcept;

        //! Sends the signal information.
        void sendSignalSlot(QTcpSocket* tcpSocket, TcpSignal signal, QJsonDocument answer = QJsonDocument()) noexcept;

      signals:
        //! Emitted when the service should be terminated.
        void stopSignal();
        void closeApp();
        //! Emitted when a process should be started immediately.
        void startProcess(uint32_t, std::shared_ptr<te::dt::TimeInstantTZ> startTime);

      private slots:
        //! Slot called when a new connection arrives.
        void receiveConnection() noexcept;
        //! Slot called when finished receiving a tcp message.
        void readReadySlot(QTcpSocket* tcpSocket) noexcept;

      private:
        QTcpSocket* tcpSocket_;
        uint32_t blockSize_; //!< Size of the message received.
        //! Parse bytearray as a json and add contents to the DataManager.
        void addData(const QByteArray& bytearray);
        //! Parse bytearray as a json and call data manager method to validate the contents.
        void validateData(const QByteArray& bytearray);
        //! Parse bytearray as a json and remove contents from the DataManager.
        void removeData(const QByteArray& bytearray);
        void updateService(const QByteArray& bytearray);
        QJsonObject logToJson(const terrama2::core::ProcessLogger::Log& log);

        void sendStartProcess(const QByteArray& bytearray);

        std::weak_ptr<terrama2::core::DataManager> dataManager_;//!< Weak pointer to the service DataManager.
        std::weak_ptr<terrama2::core::ProcessLogger> logger_;

        ServiceManager* serviceManager_;
    };
  }
}

#endif //__TERRAMA2_CORE_TCP_MANAGER_HPP__
