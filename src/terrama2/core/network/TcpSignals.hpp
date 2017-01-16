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
  \file terrama2/core/TcpSignals.hpp

  \brief Signals for TCP messages.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_TCP_SIGNALS_HPP__
#define __TERRAMA2_CORE_TCP_SIGNALS_HPP__


namespace terrama2
{
  namespace core
  {
    /*!
      \enum TcpSignal
      \brief  Signals for TCP communication.
      \see https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/tcp
    */
    enum class TcpSignal
    {
      TERMINATE_SERVICE_SIGNAL = 0,//!< Stop service signal.
      STATUS_SIGNAL = 1,//!< Check if the connection is alive and get service information
      ADD_DATA_SIGNAL = 2,//!< Add data signal, followed by data. Can be used for updates.
      START_PROCESS_SIGNAL = 3,//!< Start signal, queue the collection, analysis, ...
      LOG_SIGNAL = 4,//!< Log signal, send error message to node.js server
      REMOVE_DATA_SIGNAL = 5, //!< Remove data signal
      PROCESS_FINISHED_SIGNAL = 6, //!< Signal emmited when a process has finished
      UPDATE_SERVICE_SIGNAL = 7, //!< Signal to receive service information.
      VALIDATE_PROCESS_SIGNAL = 8 //!< Signal to validate process information.
    };

  }
}

#endif //__TERRAMA2_CORE_TCP_SIGNALS_HPP__
