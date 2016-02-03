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
  \file terrama2/collector/TsParserPcdCemaden.cpp

  \brief 

  \author Evandro Delatin
*/

#include "TsParserPcdCemaden.hpp"

#include "Utils.hpp"

//Qt
#include <QTemporaryDir>
#include <QFile>
#include <QTimer>

//STL
#include <memory>

//Terrama2
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/ParserPcdCemaden.hpp>

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>

//boost
#include <boost/date_time/gregorian/greg_date.hpp>



void TsParserPcdCemaden::TestCollectorCemaden()
{
  QString cemadenFile = terrama2::core::FindInTerraMA2Path("data/pcd_cemaden/1.txt").c_str();
  QFileInfo info(cemadenFile);

  QUrl uri;
  uri.setScheme("FILE");
  uri.setPath(cemadenFile);

  try
  {
    // Mask
    // 1 - Estação Pluviométrico
    // 3 - Estação Hidrológica

    terrama2::core::DataSetItem item;
    item.setMask(info.fileName().toStdString());
    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uriTemporary = "http://150.163.255.240/CEMADEN/resources/parceiros/SP/1";

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);

    terrama2::collector::ParserPcdCemaden parser;
    parser.read(filter, transferenceDataVec);

    QVERIFY(transferenceDataVec.at(0).teDataSet.get());
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}





