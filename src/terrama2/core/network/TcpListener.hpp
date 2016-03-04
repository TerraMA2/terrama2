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
  \file terrama2/core/TcpListener.hpp

  \brief A class to receive data through a socket.

  \author Jano Simas
*/


#ifndef __TERRAMA2_CORE_TCPLISTENER_HPP__
#define __TERRAMA2_CORE_TCPLISTENER_HPP__

#include <QTcpServer>

namespace terrama2
{
  namespace core
  {
    /*!
       \brief The TcpListener class waits for a connection from the remote TerraMA² server for new terrama2::core::* classes.

       TcpListener should be started in a remote core::Service to wait for core::DataProvider and core::DataSet,
       It will process add, update and remove of these classes.

       \note More information in QTcpServer documentation.

     */
    class TcpListener : public QTcpServer
    {
      Q_OBJECT

    public:
      //! Constructor, connects signal.
      TcpListener(QObject * parent = 0);
      //! Default destructor.
      virtual ~TcpListener(){}

    signals:
      void stopSignal();

    private slots:
      //! Slot called when a new conenction arrives.
      void receiveConnection();

    private:
      uint16_t blockSize_; //!< Size of the message received.
      void parseData(QByteArray bytearray);
    };
  }
}

#endif //__TERRAMA2_CORE_TCPLISTENER_HPP__
