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
  \file terrama2/services/alert/impl/ReportText.cpp

  \brief

  \author Jano Simas
 */

#include "ReportText.hpp"

#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/DataSet.h>

#include <QObject>

terrama2::services::alert::core::ReportText::ReportText(std::map<std::string, std::string> reportMetadata)
  : Report(reportMetadata)
{

}

std::string terrama2::services::alert::core::ReportText::processInternal(AlertPtr alertPtr,
    terrama2::core::DataSetPtr dataset,
    std::shared_ptr<te::dt::TimeInstantTZ> alertTime,
    std::shared_ptr<te::da::DataSet> alertDataSet)
{
  const int columnWidth = 20;

  std::stringstream stream;
  stream << reportMetadata_[ReportTags::TITLE] << "\n" << reportMetadata_[ReportTags::SUBTITLE] << "\n" << std::endl;
  stream << reportMetadata_[ReportTags::DESCRIPTION] << "\n" << reportMetadata_[ReportTags::AUTHOR] << "\t" << reportMetadata_[ReportTags::CONTACT] << "\n" << std::endl;

  //TODO: format alertTime >> reportMetadata_[ReportTags::TIMESTAMP_FORMAT]
  stream << QObject::tr("Execution date: %1").arg(QString::fromStdString(alertTime->toString())).toStdString() << "\n" << std::endl;

  //add table headers
  auto columns = alertDataSet->getNumProperties();
  for(size_t i = 0; i < columns; ++i)
  {
    stream << std::setw(columnWidth) << alertDataSet->getPropertyName(i) << "\t";
  }
  stream << "\n" << std::endl;

  //add table headers content
  alertDataSet->moveBeforeFirst();
  while(alertDataSet->moveNext())
  {
    for(size_t i = 0; i < columns; ++i)
    {
      try
      {
        if(alertDataSet->isNull(i))
          stream << std::setw(columnWidth) << "NULL" << "\t";
        else
        {
          stream << std::setw(columnWidth) << alertDataSet->getAsString(i) << "\t";
        }
      }
      catch(...)
      {
        stream << std::setw(columnWidth) << "NULL" << "\t";
      }
    }

    stream << std::endl;
  }

  stream << "\n" << reportMetadata_[ReportTags::COPYRIGHT] << std::endl;

  return stream.str();
}
