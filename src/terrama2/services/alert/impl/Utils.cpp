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

#include "DocumentPDF.hpp"

#include "../core/utility/NotifierFactory.hpp"
#include "../core/utility/DocumentFactory.hpp"


void terrama2::services::alert::core::registerFactories()
{
  #ifdef VMIME_FOUND
  // Notifiers
  NotifierFactory::getInstance().add(terrama2::services::alert::impl::NotifierEmail::notifierCode(),
                                     terrama2::services::alert::impl::NotifierEmail::make);
  #endif

  // Documents
  DocumentFactory::getInstance().add(terrama2::services::alert::impl::documentPDF::documentCode(),
                                     terrama2::services::alert::impl::documentPDF::makeDocument);
}

std::string terrama2::services::alert::core::gridReportText()
{
  return "<!DOCTYPE html><html><head><style>body{background-color:#ffffff;}h1{color:blue;text-align:center;}p{font-family:\"Times New Roman\";}</style></head><body><h1>%TITLE%</h1><p>%ABSTRACT%</p><p>%DESCRIPTION%</p>"
         "<hr><p>Max value: </p>%MAXVALUE_DATA%<hr>"
         "<hr><p>Min value: </p>%MINVALUE_DATA%<hr>"
         "<hr><p>Mean value: </p>%MEANVALUE_DATA%<hr>"
         "<p>%COPYRIGHT%</p></body></html>";
}

std::string terrama2::services::alert::core::monitoredObjectReportText()
{
  return "<!DOCTYPE html><html><head><style>body{background-color:#ffffff;}h1{color:blue;text-align:center;}p{font-family:\"Times New Roman\";}</style></head><body><h1>%TITLE%</h1><p>%ABSTRACT%</p><p>%DESCRIPTION%</p>"
         "<hr>%COMPLETE_DATA%<hr>"
         "<p>%COPYRIGHT%</p></body></html>";
}
