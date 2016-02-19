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

#ifndef __TERRAMA2_CORE_TCPDISPATCHER_HPP__
#define __TERRAMA2_CORE_TCPDISPATCHER_HPP__

#include "ServiceData.hpp"

//STL
#include <memory>

//Qt
#include <QJsonArray>
#include <QTcpSocket>



namespace terrama2
{
  namespace core
  {
    //! The TcpDispatcher controls the communication with a remote service through a TCP socket.
    class TcpDispatcher
    {
    public:
      //! Constructor
      TcpDispatcher(const ServiceData& serviceData);

      /*!
         \brief Sends a signal to stop the remote service.

         \exception ErrorDescription Raised if could not send the signal.
       */
      void stopService();
      /*!
         \brief Check if remote service is in reach.

         A false return may mean:
          - the service is not running
          - the service is unreachable
          - the service is not listening to the specified port

         \return True if communication with service is possible.
       */
      bool pingService() noexcept;
      /*!
         \brief Starts the processing of specfied data.

         Processing data vary for each service,
         a collector service will start collecting the core::DataSet with dataId,
         a analysis service will start to analyze core::Analysis with dataId.

         \param dataId Id of the data to be processed.
       */
      void startProcessing(int dataId);

      /*!
         \brief Sends data to be processed by service.
         \param jsonArray Serialized data.
       */
      void sendData(const QJsonArray& jsonArray);

    private:
      /*!
         \brief  Creates socket to the remote service.
         \param Should errors be logged? used in pingService() where errors result in false return.
         \return Shared pointer of the socket.
         \exception UnableToConnect Raised when unable to reach remote service.
       */
      std::shared_ptr<QTcpSocket> socket(bool log = true);

      //! Remote service communication especifications.
      ServiceData serviceData_;
    };

    typedef std::shared_ptr<TcpDispatcher> TcpDispatcherPtr;//!< Shared pointer to a TcpDispatcher.
  }
}

#endif//__TERRAMA2_CORE_TCPDISPATCHER_HPP__
