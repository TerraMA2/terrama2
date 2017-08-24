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
 * \file InterpolatorLogger.hpp
 *
 * \brief Contains a definition of a logger to be used by interpolation framework.
 *
 * \author Frederico Augusto Bedê
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATORLOG_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATORLOG_HPP__

#include "../../../core/utility/ProcessLogger.hpp"

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        /*!
         * \class InterpolatorLogger
         *
         * \brief A class that defines objects to be used by interpolation mechanism for getting informations about the execution.
         *
         * \ingroup interpolator
         */
        class InterpolatorLogger : public terrama2::core::ProcessLogger
        {
          Q_OBJECT
        public:

          /*!
           * \brief Default constructor.
           */
          InterpolatorLogger();

          /*!
           * \brief Destructor.
           */
          ~InterpolatorLogger() {}

          /*!
           * \brief This method will log a file input for a determinated process log.
           *
           * \param value The input file to add in logger.
           *
           * \param registerID The table id to update with the input file.
           */
          void addInput(std::string value, RegisterId registerID);

          /*!
           * \brief This method will log a file output for a determinated process log.
           *
           * \param value The output file to add in logger.
           *
           * \param registerID The table id to update with the output file.
           */
          void addOutput(std::string value, RegisterId registerID);

          /*!
           * \brief Updates the information about the connection.
           *
           * \param uri The connection informations.
           */
          virtual void setConnectionInfo(const te::core::URI& uri) override;

          /*!
           * \brief Does a copy of the logger.
           *
           * \return A copy of the logger.
           */
          virtual std::shared_ptr<ProcessLogger> clone() const override;
        };
      }   // end namespace core
    }     // end namespace interpolator
  }       // end namespace services
}         // end namespace terrama2

#endif // __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATORLOG_HPP__
