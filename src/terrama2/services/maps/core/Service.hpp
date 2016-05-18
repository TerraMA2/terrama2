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
  \file terrama2/services/maps/core/Service.hpp

  \brief Class for the maps configuration.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_MAPS_SERVICE_HPP__
#define __TERRAMA2_SERVICES_MAPS_SERVICE_HPP__

#include "../../../core/utility/Service.hpp"

namespace terrama2
{
  namespace services
  {
    namespace maps
    {
      namespace core
      {
        class Service : public terrama2::core::Service
        {
          Q_OBJECT

        public:

          Service();

          virtual ~Service();

        public slots:

          virtual void updateNumberOfThreads(int) override;

        protected:

          /*!
             \brief Returns true if the main loop should continue.
             \return True if there is data to be tasked OR is stop is true.
           */
          virtual bool mainLoopWaitCondition() override;


          /*!
             \brief Check if there is data to be processed.
             \return True if there is more data to be processed.
           */
          virtual bool checkNextData() override;


          /*!
           * \brief makeMap
           */
          static void buildMap();
        };
      }
    }
  }

}

#endif // __TERRAMA2_SERVICES_MAPS_SERVICE_HPP__
