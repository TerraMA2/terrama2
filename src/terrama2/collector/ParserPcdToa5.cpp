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
  \file terrama2/collector/ParserPcdToa5.cpp

  \brief Parser of PCD TOA5 file

  \author Jano Simas
  \author Evandro Delatin
*/

// TerraMA2
#include "ParserOGR.hpp"
#include "ParserPcdToa5.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"
#include "../core/Logger.hpp"

// QT
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QTemporaryFile>
#include <QTemporaryDir>

// STL
#include <memory>

// Boost
#include <boost/format/exceptions.hpp>

//TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/common/Exception.h>

#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/geometry.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype.h>


te::dt::AbstractData* terrama2::collector::ParserPcdToa5::StringToTimestamp(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int /*dstType*/)
{
  assert(indexes.size() == 1);

  std::string dateTime = dataset->getAsString(indexes[0]);

  boost::posix_time::ptime boostDate(boost::posix_time::time_from_string(dateTime));

  boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(dataSetItem_.timezone()));
  boost::local_time::local_date_time date(boostDate.date(), boostDate.time_of_day(), zone, true);

  te::dt::TimeInstantTZ* dt = new te::dt::TimeInstantTZ(date);

  return dt;
}

// Change the string 2014-01-15 17:13:00 - PCD TOA5 format for timestamp
void terrama2::collector::ParserPcdToa5::read(terrama2::collector::DataFilterPtr filter, std::vector<terrama2::collector::TransferenceData>& transferenceDataVec)
{
  QTemporaryDir tempDir;
  if(!tempDir.isValid())
  {
    QString errMsg = QObject::tr("Can't create temporary folder.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw CantCreateTemporaryFolderException() << ErrorDescription(errMsg);
  }

  for(TransferenceData& transferenceData : transferenceDataVec)
  {
    QUrl uri(transferenceData.uriTemporary.c_str());
    QFileInfo originalInfo(uri.path());
    QFile file(uri.path());
    if(!file.open(QIODevice::ReadOnly))
    {
      //TODO: log can't create temporary file
      continue;
    }

    QFile tempFile(tempDir.path()+"/"+originalInfo.fileName());
    tempFile.open(QIODevice::WriteOnly);
    file.readLine();//ignore first line
    tempFile.write(file.readLine()); //headers line

    //ignore third and fourth lines
    file.readLine();
    file.readLine();

    //read all file
    tempFile.write(file.readAll()); //headers line

    //update file path
    QFileInfo info(tempFile);
    transferenceData.uriTemporary = "file://"+info.absoluteFilePath().toStdString();
  }

  ParserOGR::read(filter, transferenceDataVec);
}

void terrama2::collector::ParserPcdToa5::adapt(std::shared_ptr<te::da::DataSetTypeConverter> converter)
{
  std::string timestampName = "TIMESTAMP";
  converter->remove(timestampName);

  te::dt::DateTimeProperty* dtProperty = new te::dt::DateTimeProperty("DateTime", te::dt::TIME_INSTANT_TZ);

  //Find the rigth column to adapt
  std::vector<te::dt::Property*> properties = converter->getConvertee()->getProperties();
  for(size_t i = 0, size = properties.size(); i < size; ++i)
  {
    te::dt::Property* property = properties.at(i);
    if(property->getName() == timestampName)
    {
      //column found
      converter->add(i, dtProperty, boost::bind(&terrama2::collector::ParserPcdToa5::StringToTimestamp, this, _1, _2, _3));
      break;
    }
  }
}
