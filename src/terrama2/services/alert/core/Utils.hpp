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
  \file terrama2/services/alert/core/Utils.hpp

  \brief Utility funtions for core classes.

  \author Vinicius Campanha
 */


#ifndef __TERRAMA2_SERVICES_ALERT_CORE_UTILS_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_UTILS_HPP__

// TerraMA2
#include "Report.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>


namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {

        //! Conver a TimeInstantTZ to a default formated string in GMT.
        TMALERTCOREEXPORT std::string dateTimeToString(const std::shared_ptr<te::dt::TimeInstantTZ> dateTimeTZ);

        /*!
         * \brief Receives a TerraLib dataSet and returns all his data as a HTML table
         * \param dataSet The data set with data to put in table
         * \return A string with the dataSet data formated as a HTML table
         */
        TMALERTCOREEXPORT std::string dataSetHtmlTable(const std::shared_ptr<te::da::DataSet>& dataSet);

        TMALERTCOREEXPORT void replaceReportTags(std::string& text, ReportPtr report);

      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_CORE_UTILS_HPP__
