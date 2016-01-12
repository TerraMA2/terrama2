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
  \file terrama2/collector/TsFactory.cpp

  \brief Tests for the Factory namespace.

  \author Jano Simas
*/

#include "TsFactory.hpp"
#include "Utils.hpp"

//STL
#include <string>

//QT
#include <QUrl>

//Terrama2
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/Factory.hpp>
#include <terrama2/collector/Parser.hpp>
#include <terrama2/collector/ParserPostgis.hpp>
#include <terrama2/collector/ParserOGR.hpp>
#include <terrama2/collector/ParserPcdInpe.hpp>
#include <terrama2/collector/ParserPcdToa5.hpp>
#include <terrama2/collector/ParserFirePoint.hpp>
#include <terrama2/collector/Storager.hpp>
#include <terrama2/collector/StoragerPostgis.hpp>
#include <terrama2/collector/DataRetriever.hpp>
#include <terrama2/collector/DataRetrieverFTP.hpp>

#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataProvider.hpp>

void TsFactory::TestMakeParser()
{
  try
  {
    //test postgis uri
    QUrl postgisUri;
    postgisUri.setScheme("postgis");
    postgisUri.setHost("localhost");
    postgisUri.setPort(5432);
    postgisUri.setUserName("postgres");
    postgisUri.setPassword("postgres");
    postgisUri.setPath("/basedeteste");

    terrama2::core::DataSetItem unknownTypeItem(terrama2::core::DataSetItem::UNKNOWN_TYPE);

    //FIXME: create a DataSetItem kind for ParserPostgis
//    terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(unknownTypeItem);
//    terrama2::collector::ParserPostgis* parserPostgis = dynamic_cast<terrama2::collector::ParserPostgis*>(parser.get());
//    QVERIFY(parserPostgis);

    QUrl fileUri;
    fileUri.setScheme("file");
    fileUri.setPath("/tmp");

    terrama2::collector::ParserPtr parser = terrama2::collector::Factory::makeParser(unknownTypeItem);
    terrama2::collector::ParserOGR* parserOGR = dynamic_cast<terrama2::collector::ParserOGR*>(parser.get());
    QVERIFY(parserOGR);

    terrama2::core::DataSetItem pcdInpeTypeItem(terrama2::core::DataSetItem::PCD_INPE_TYPE);
    parser = terrama2::collector::Factory::makeParser(pcdInpeTypeItem);
    terrama2::collector::ParserPcdInpe* parserPdcInpe = dynamic_cast<terrama2::collector::ParserPcdInpe*>(parser.get());
    QVERIFY(parserPdcInpe);

    terrama2::core::DataSetItem pcdToaTypeItem(terrama2::core::DataSetItem::PCD_TOA5_TYPE);
    parser = terrama2::collector::Factory::makeParser(pcdToaTypeItem);
    terrama2::collector::ParserPcdToa5* parserPdctoa5 = dynamic_cast<terrama2::collector::ParserPcdToa5*>(parser.get());
    QVERIFY(parserPdctoa5);

    terrama2::core::DataSetItem firePointTypeItem(terrama2::core::DataSetItem::FIRE_POINTS_TYPE);
    parser = terrama2::collector::Factory::makeParser(firePointTypeItem);
    terrama2::collector::ParserFirePoint* parserFirePoint = dynamic_cast<terrama2::collector::ParserFirePoint*>(parser.get());
    QVERIFY(parserFirePoint);

    try
    {
      terrama2::core::DataSetItem diseaseTypeItem(terrama2::core::DataSetItem::DISEASE_OCCURRENCE_TYPE);
      parser = terrama2::collector::Factory::makeParser(diseaseTypeItem);

      QFAIL(NO_EXCEPTION_THROWN);
    }
    catch(terrama2::collector::ConflictingParserTypeSchemeException& e)
    {

    }
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsFactory::TestMakeStorager()
{
  try
  {
    terrama2::core::DataSetItem unknownTypeItem(terrama2::core::DataSetItem::UNKNOWN_TYPE);

    try
    {
      terrama2::collector::StoragerPtr storager = terrama2::collector::Factory::makeStorager(unknownTypeItem);
      terrama2::collector::StoragerPostgis* storagerPostgis = dynamic_cast<terrama2::collector::StoragerPostgis*>(storager.get());

      Q_UNUSED(storagerPostgis)

      QFAIL(NO_EXCEPTION_THROWN);
    }
    catch(terrama2::collector::UnableToCreateStoragerException& e)
    {

    }

    std::map<std::string, std::string> storageMetadata{ {"KIND", "POSTGIS"},
                                                        {"PG_HOST", "localhost"},
                                                        {"PG_PORT", "5432"},
                                                        {"PG_USER", "postgres"},
                                                        {"PG_PASSWORD", "postgres"},
                                                        {"PG_DB_NAME", "basedeteste"},
                                                        {"PG_CONNECT_TIMEOUT", "4"},
                                                        {"PG_CLIENT_ENCODING", "UTF-8"},
                                                        {"PG_TABLENAME", "terrama2.teste_postgis"} };
    unknownTypeItem.setStorageMetadata(storageMetadata);

    terrama2::collector::StoragerPtr storager = terrama2::collector::Factory::makeStorager(unknownTypeItem);
    terrama2::collector::StoragerPostgis* storagerPostgis = dynamic_cast<terrama2::collector::StoragerPostgis*>(storager.get());
    QVERIFY(storagerPostgis);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}

void TsFactory::TestMakeRetriever()
{
  try
  {
    terrama2::core::DataProvider unknownTypeProvider("dummy", terrama2::core::DataProvider::FTP_TYPE);
    terrama2::collector::DataRetrieverPtr retriever = terrama2::collector::Factory::makeRetriever(unknownTypeProvider);
    QVERIFY(retriever.get());

    terrama2::core::DataProvider ftpTypeProvider("dummy", terrama2::core::DataProvider::FTP_TYPE);
    retriever = terrama2::collector::Factory::makeRetriever(ftpTypeProvider);
    terrama2::collector::DataRetrieverFTP* retrieverFtp = dynamic_cast<terrama2::collector::DataRetrieverFTP*>(retriever.get());
    QVERIFY(retrieverFtp);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
}































