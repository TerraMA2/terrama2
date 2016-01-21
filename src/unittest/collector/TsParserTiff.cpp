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
  \file terrama2/collector/TsParserTiff.cpp

  \brief Tests for the ParserTiff class.

  \author Jano Simas
*/

#include "TsParserTiff.hpp"
#include "Utils.hpp"

//terrama2
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/ParserTiff.hpp>
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/TransferenceData.hpp>

//Qt
#include <QFileInfo>
#include <QString>
#include <QUrl>

void TsParserTiff::TestOpenFile()
{
  QString tifFile = terrama2::core::FindInTerraMA2Path("data/geotiff/L5219076_07620040908_r3g2b1.tif").c_str();
  QFileInfo info(tifFile);

  QUrl uri;
  uri.setScheme("FILE");
  uri.setPath(tifFile);

  try
  {
    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uriTemporary = uri.url().toStdString();

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);

    terrama2::core::DataSetItem item;
    item.setMask(info.fileName().toStdString());

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::ParserTiff parser;
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
