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
  \file terrama2/services/alert/impl/ReportTxt.cpp

  \brief

  \author Jano Simas
 */

#include "ReportTxt.hpp"
#include "../core/Shared.hpp"
#include "../core/Exception.hpp"
#include "../../../core/utility/Logger.hpp"

#include <fstream>

#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/dataaccess/dataset/DataSet.h>

#include <QObject>

terrama2::services::alert::core::ReportTxt::ReportTxt(std::map<std::string, std::string> reportMetadata)
 : ReportText(reportMetadata)
{

}

void terrama2::services::alert::core::ReportTxt::process(AlertPtr alertPtr,
                                                         terrama2::core::DataSetPtr dataset,
                                                         std::shared_ptr<te::dt::TimeInstantTZ> alertTime,
                                                         std::shared_ptr<te::da::DataSet> alertDataSet)
{
  //get file path
  std::string filePath = reportMetadata_[ReportTags::DESTINATION_FOLDER]+"/"+reportMetadata_[ReportTags::FILE_NAME];

  //replace wilcards for values
  filePath = refactorMask(filePath, alertPtr, dataset, alertTime);

  //open file
  std::fstream fileStream;
  fileStream.open(filePath, std::ios::out);
  if(fileStream.is_open())
  {
    fileStream << processInternal(alertPtr, dataset, alertTime, alertDataSet);
  }
  else
  {
    QString errMsg = QObject::tr("Unable to open file: %1").arg(QString::fromStdString(filePath));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ReportException() << ErrorDescription(errMsg);
  }
}

terrama2::services::alert::core::ReportPtr terrama2::services::alert::core::ReportTxt::make(std::map<std::string, std::string> reportMetadata)
{
  return std::make_shared<ReportTxt>(reportMetadata);
}

std::string terrama2::services::alert::core::ReportTxt::refactorMask(const std::string& filePath,
                                                                     AlertPtr alert,
                                                                     terrama2::core::DataSetPtr dataset,
                                                                     std::shared_ptr<te::dt::TimeInstantTZ> alertTime) const
{
  //TODO: implement ReportTxt::refactorMask
  return filePath;
}
