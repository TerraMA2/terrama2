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
  \file terrama2/collector/TsUnpack.cpp

  \brief Tests for the Unpack class.

  \author Evandro Delatin
*/

#include "TsUnpack.hpp"
#include "Utils.hpp"

//Qt
#include <QTemporaryDir>
#include <QFile>

//STL
#include <memory>

//Terrama2
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/Unpack.hpp>

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>

//boost
#include <boost/date_time/gregorian/greg_date.hpp>


void TsUnpack::TestParseCpvOk()
{

 try
 {
    terrama2::core::DataSetItem item;
    item.setMask("30885.txt.zip");

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::TransferenceData transferenceData;
    // 30885.txt.gz
    // data/Eta15km/exemplo.zip
    // data/Eta15km/30885.txt.zip
    // data/Eta15km/serrmar.tar.gz
    // data/Eta15km/30885.txt.bz2
    transferenceData.uriTemporary = "file://"+terrama2::core::FindInTerraMA2Path("data/Eta15km/30885.txt.zip");

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);


    terrama2::collector::Unpack fileCompressed;
    fileCompressed.unpackList(transferenceDataVec);

    QVERIFY(transferenceDataVec.size() > 0);
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

void TsUnpack::TestParseFail()
{
  QFAIL("NOT IMPLEMENTED");
}
