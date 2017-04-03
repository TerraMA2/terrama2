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
  \file terrama2/services/alert/impl/Utils.hpp

  \brief Utility funtions for impl classes.

  \author Jano Simas
          Vinicius Campanha
 */


// TerraMA2
#include "Utils.hpp"

#include "NotifierEmail.hpp"

#include "../core/utility/NotifierFactory.hpp"

void terrama2::services::alert::core::registerFactories()
{
  NotifierFactory::getInstance().add(terrama2::services::alert::impl::NotifierEmail::notifierCode(),
                                     terrama2::services::alert::impl::NotifierEmail::make);
}

std::string terrama2::services::alert::core::dataSetHtmlTable(const std::shared_ptr<te::da::DataSet>& dataSet)
{
  if(!dataSet.get())
    return "";

  std::size_t numProperties = dataSet->getNumProperties();

  std::string htmlTable = "<table border=\"1\">";

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
      line += "<td>" + dataSet->getAsString(i) +"</td>";
    }

    htmlTable += "<tr>" + line + "</tr>";
  }

  htmlTable += "</table>";

  return htmlTable;
}
