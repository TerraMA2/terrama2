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


// TerraMA2
#include "Utils.hpp"
#include "Exception.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/Utils.hpp"

// Boost
#include <boost/algorithm/string/replace.hpp>

// Qt
#include <QString>
#include <QObject>


std::string terrama2::services::alert::core::dateTimeToString(const std::shared_ptr<te::dt::TimeInstantTZ> dateTimeTZ)
{

  boost::local_time::time_zone_ptr utc(new boost::local_time::posix_time_zone("GMT+00"));
  auto boostLocalTime = dateTimeTZ->getTimeInstantTZ();
  auto utcTime = boostLocalTime.local_time_in(utc);

  std::stringstream ss;
  boost::local_time::local_time_facet* oFacet(new boost::local_time::local_time_facet("%d/%m/%Y %H:%M:%S"));
  ss.imbue(std::locale(ss.getloc(), oFacet));
  ss << utcTime;

  auto dateStr = ss.str();
  return dateStr;
}

std::string terrama2::services::alert::core::dataSetHtmlTable(const std::shared_ptr<te::da::DataSet>& dataSet)
{
  if(!dataSet.get())
  {
    QString errMsg = QObject::tr("Invalid dataSet!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }

  std::size_t numProperties = dataSet->getNumProperties();

  std::string htmlTable = "<table border=\"1\" style=\"width:100%\">";

  htmlTable += "<tr>";

  for(std::size_t i = 0; i < numProperties; i++)
  {
    htmlTable += "<th>" + dataSet->getPropertyName(i) +"</th>";
  }

  htmlTable += "</tr>";

  if(dataSet->isEmpty())
    return htmlTable + "</table>";

  dataSet->moveBeforeFirst();

  while(dataSet->moveNext())
  {
    std::string line;
    for(std::size_t i = 0; i < numProperties; i++)
    {
      if(dataSet->isNull(i))
        line += "<td> NULL </td>";
      else
        line += "<td><span>" +dataSet->getAsString(i, 2)+ "</span></td>";
    }

    htmlTable += "<tr>" + line + "</tr>";
  }

  htmlTable += "</table>";

  htmlTable +="<br />* Comparação entre as duas últimas análises ( = :sem alteração, 🡅 :aumentou, 🡇 :diminuiu e 🚫:houve algum erro no dado)<br />";

  return htmlTable;
}


void terrama2::services::alert::core::replaceReportTags(std::string& text, ReportPtr report)
{
  boost::replace_all(text, "%TITLE%", report->title());
  boost::replace_all(text, "%TITLE%", report->title());
  boost::replace_all(text, "%ABSTRACT%", report->abstract());
  boost::replace_all(text, "%AUTHOR%", report->author());
  boost::replace_all(text, "%COPYRIGHT%", report->copyright());
  boost::replace_all(text, "%DESCRIPTION%", report->description());

  std::string complete_data = "NO DATA!";

  try
  {
    complete_data = terrama2::services::alert::core::dataSetHtmlTable(report->retrieveAllData());
  }
  catch(const ReportException& /*e*/)
  {
  }

  boost::replace_all(text, "%COMPLETE_DATA%", complete_data);

  std::string max_value = "NO DATA!";

  try
  {
    max_value = std::to_string(report->retrieveMaxValue());
  }
  catch(const ReportException& /*e*/)
  {
  }

  boost::replace_all(text, "%MAXVALUE_DATA%", max_value);

  std::string min_value = "NO DATA!";

  try
  {
    min_value = std::to_string(report->retrieveMinValue());
  }
  catch(const ReportException& /*e*/)
  {
  }

  boost::replace_all(text, "%MINVALUE_DATA%", min_value);

  std::string mean_value = "NO DATA!";

  try
  {
    mean_value = std::to_string(report->retrieveMeanValue());
  }
  catch(const ReportException& /*e*/)
  {
  }

  boost::replace_all(text, "%MEANVALUE_DATA%", mean_value);
}
