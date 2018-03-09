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
  \file terrama2/services/collector/core/CollectorLogger.hpp

  \brief Class to log the steps of Terrama2 Collector Service

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_COLLECTOR_CORE_COLLECTORLOG_HPP__
#define __TERRAMA2_SERVICES_COLLECTOR_CORE_COLLECTORLOG_HPP__

// TerraMa2
#include "Config.hpp"
#include "../core/Typedef.hpp"
#include "../../../core/utility/ProcessLogger.hpp"

namespace terrama2
{
  namespace services
  {
    namespace collector
    {
      namespace core
      {
        /*!
         * \brief The CollectorLogger class is responsible for uses the Process Logger to
         * log the processes in the Collector Service.
         */
        class TMCOLLECTOREXPORT CollectorLogger : public terrama2::core::ProcessLogger
        {
          Q_OBJECT
        public:


          /*!
           * \brief Class constructor, it will pass the connection information about the Log DB and
           * set the collector log table.
           * \param connInfo Has the access information to the log Database
           */
          CollectorLogger();

          /*!
            * \brief Class destructor
            */
          virtual ~CollectorLogger() {}

          /*!
           * \brief This method will log a file input for a determinated process log.
           * \param value The input file to add in logger
           * \param registerID The table id to update with the input file.
           */
          void addInput(std::string value, RegisterId registerID);

          /*!
           * \brief This method will log a file output for a determinated process log.
           * \param value The output file to add in logger
           * \param registerID The table id to update with the output file.
           */
          void addOutput(std::string value, RegisterId registerID);

          virtual void setConnectionInfo(const te::core::URI& uri) override;

          virtual std::shared_ptr<ProcessLogger> clone() const override;

        };
      }
    }
  }
}
#endif // __TERRAMA2_SERVICES_COLLECTOR_CORE_COLLECTORLOG_HPP__
