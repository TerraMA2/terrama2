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
  \file terrama2/services/alert/core/RunAlert.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_RUN_ALERT_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_RUN_ALERT_HPP__

#include "../../../core/Shared.hpp"
#include "../../../core/utility/Service.hpp"
#include "../../../core/data-model/DataSeriesRisk.hpp"
#include "Typedef.hpp"
#include "AlertLogger.hpp"
#include "DataManager.hpp"

namespace te
{
  namespace da
  {
    //! Terralib DataSet forward declaration
    class DataSet;
  } /* da */
} /* te */

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        /*!
          \brief Method to execute the alert.
        */
        void runAlert(terrama2::core::ExecutionPackage executionPackage,
                      std::shared_ptr< AlertLogger > logger,
                      std::weak_ptr<DataManager> weakDataManager);

        //! Get the name of the property used as unique key of the DataSet
        std::string getIdentifierPropertyName(terrama2::core::DataSetPtr dataSet, terrama2::core::DataSeriesPtr dataSeries);
        //! Get the propper function to evaluate the risk level of a value.
        std::function<std::tuple<int, std::string, std::string>(size_t pos)> createGetRiskFunction(terrama2::core::DataSeriesRisk risk, std::shared_ptr<te::da::DataSet> teDataSet);
      } /* core */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_CORE_RUN_ALERT_HPP__
