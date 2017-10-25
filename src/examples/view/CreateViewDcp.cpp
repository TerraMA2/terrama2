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
  \file examples/view/CreateViewDcp.cpp

  \author Jano Simas
 */

// TerraMA2
#include "ViewLoggerMock.hpp"

#include <terrama2/services/view/core/View.hpp>
#include <terrama2/services/view/core/DataManager.hpp>
#include <terrama2/services/view/core/Shared.hpp>
#include <terrama2/services/view/core/Utils.hpp>
#include <terrama2/services/view/core/Service.hpp>

#include <terrama2/core/Shared.hpp>

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>

#include <terrama2/impl/Utils.hpp>

#include <QCoreApplication>
#include <QTimer>
#include <QString>

// STL
#include <memory>
#include <string>

using ::testing::_;

void addFileDataProviders(terrama2::core::DataManagerPtr dataManager)
{
   QString dataProviderJson = QString(R"x({"DataProviders": [{"class": "DataProvider","id": 11,"project_id": 5,"data_provider_type": "FILE","intent": 1,"name": "Local Folder","options": {"timeout": "","active_mode": ""},"description": "Local Folder data server","uri": "%1","active": true}]})x").arg(QString::fromStdString(TERRAMA2_DATA_DIR));

   QJsonDocument doc = QJsonDocument::fromJson(dataProviderJson.toUtf8());
   QJsonObject obj = doc.object();
   dataManager->addJSon(obj);
}

void addPostgisDataProviders(terrama2::core::DataManagerPtr dataManager)
{
   QString dataProviderJson = QString(R"x({"DataProviders": [{"class": "DataProvider","id": 12,"project_id": 5,"data_provider_type": "POSTGIS","intent": 1,"name": "Local Database PostGIS","options": {"timeout": "","active_mode": ""},"description": "Local Database PostGIS data server","uri": "%1","active": true}]})x").arg(QString::fromStdString("pgsql://"+TERRAMA2_DATABASE_USERNAME+ ":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME));

   QJsonDocument doc = QJsonDocument::fromJson(dataProviderJson.toUtf8());
   QJsonObject obj = doc.object();
   dataManager->addJSon(obj);
}

void addDataSeries(terrama2::core::DataManagerPtr dataManager) {
  QString dataSeriesJson = R"x({"DataSeries":[{"class":"DataSeries","id":22,"project_id":5,"name":"dcp inpe_input","description":null,"data_provider_id":11,"semantics":"DCP-inpe","active":true,"datasets":[{"class":"DataSet","id":66,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"93010","projection":"4326","timezone":"0","alias":"dcp_31990","mask":"PCD_serrmar_INPE/31990.txt"},"position":"SRID=4326;POINT(-46.803 -23.889)"},{"class":"DataSet","id":64,"data_series_id":22,"active":false,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"31069","projection":"4326","timezone":"0","alias":"dcp_30891","mask":"PCD_serrmar_INPE/30891.txt"},"position":"SRID=4326;POINT(-45.72 -23.755)"},{"class":"DataSet","id":71,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"30517","projection":"4326","timezone":"0","alias":"dcp_32663","mask":"PCD_serrmar_INPE/32663.txt"},"position":"SRID=4326;POINT(-45.467 -22.917)"},{"class":"DataSet","id":72,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"96199","projection":"4326","timezone":"0","alias":"dcp_69030","mask":"PCD_serrmar_INPE/69030.txt"},"position":"SRID=4326;POINT(-45.552 -23.787)"},{"class":"DataSet","id":73,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"33111","projection":"4326","timezone":"0","alias":"dcp_69031","mask":"PCD_serrmar_INPE/69031.txt"},"position":"SRID=4326;POINT(-45.118 -23.391)"},{"class":"DataSet","id":65,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"36067","projection":"4326","timezone":"0","alias":"dcp_31000","mask":"PCD_serrmar_INPE/31000.txt"},"position":"SRID=4326;POINT(-45.002 -22.675)"},{"class":"DataSet","id":68,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"33303","projection":"4326","timezone":"0","alias":"dcp_32524","mask":"PCD_serrmar_INPE/32524.txt"},"position":"SRID=4326;POINT(-45.189 -22.801)"},{"class":"DataSet","id":62,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"91303","projection":"4326","timezone":"0","alias":"dcp_30889","mask":"PCD_serrmar_INPE/30889.txt"},"position":"SRID=4326;POINT(-46.79 -24.174)"},{"class":"DataSet","id":67,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"61119","projection":"4326","timezone":"0","alias":"dcp_32521","mask":"PCD_serrmar_INPE/32521.txt"},"position":"SRID=4326;POINT(-45.425 -23.69)"},{"class":"DataSet","id":76,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"60015","projection":"4326","timezone":"0","alias":"dcp_69034","mask":"PCD_serrmar_INPE/69034.txt"},"position":"SRID=4326;POINT(-46.492 -23.889)"},{"class":"DataSet","id":69,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"13193","projection":"4326","timezone":"0","alias":"dcp_32533","mask":"PCD_serrmar_INPE/32533.txt"},"position":"SRID=4326;POINT(-44.839 -22.804)"},{"class":"DataSet","id":59,"data_series_id":22,"active":false,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"08666","projection":"4326","timezone":"0","alias":"dcp_30886","mask":"PCD_serrmar_INPE/30886.txt"},"position":"SRID=4326;POINT(-46.121 -23.758)"},{"class":"DataSet","id":74,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"10030","projection":"4326","timezone":"0","alias":"dcp_69032","mask":"PCD_serrmar_INPE/69032.txt"},"position":"SRID=4326;POINT(-45.424 -23.59)"},{"class":"DataSet","id":58,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"01683","projection":"4326","timezone":"0","alias":"dcp_30885","mask":"PCD_serrmar_INPE/30885.txt"},"position":"SRID=4326;POINT(-44.941 -23.074)"},{"class":"DataSet","id":70,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"00101","projection":"4326","timezone":"0","alias":"dcp_32659","mask":"PCD_serrmar_INPE/32659.txt"},"position":"SRID=4326;POINT(-45 -22.667)"},{"class":"DataSet","id":60,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"78503","projection":"4326","timezone":"0","alias":"dcp_30887","mask":"PCD_serrmar_INPE/30887.txt"},"position":"SRID=4326;POINT(-44.85 -23.355)"},{"class":"DataSet","id":75,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"93059","projection":"4326","timezone":"0","alias":"dcp_69033","mask":"PCD_serrmar_INPE/69033.txt"},"position":"SRID=4326;POINT(-45.486 -23.693)"},{"class":"DataSet","id":61,"data_series_id":22,"active":false,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"00103","projection":"4326","timezone":"0","alias":"dcp_30888","mask":"PCD_serrmar_INPE/30888.txt"},"position":"SRID=4326;POINT(-45.6 -23.408)"},{"class":"DataSet","id":63,"data_series_id":22,"active":true,"format":{"output_timestamp_property":"datetime","timestamp_property":"N/A","_id":"01001","projection":"4326","timezone":"0","alias":"dcp_30890","mask":"PCD_serrmar_INPE/30890.txt"},"position":"SRID=4326;POINT(-46.74 -23.99)"}]},{"class":"DataSeries","id":23,"project_id":5,"name":"dcp inpe","description":null,"data_provider_id":12,"semantics":"DCP-postgis","active":true,"datasets":[{"class":"DataSet","id":82,"data_series_id":23,"active":true,"format":{"table_name":"dcp_30890","_id":"01001","projection":"4326","longitude":" -46.74","latitude":" -23.99","timezone":"0","alias":"dcp_30890","mask":"PCD_serrmar_INPE/30890.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-46.74 -23.99)"},{"class":"DataSet","id":93,"data_series_id":23,"active":true,"format":{"table_name":"dcp_69032","_id":"10030","projection":"4326","longitude":" -45.424","latitude":" -23.59","timezone":"0","alias":"dcp_69032","mask":"PCD_serrmar_INPE/69032.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.424 -23.59)"},{"class":"DataSet","id":89,"data_series_id":23,"active":true,"format":{"table_name":"dcp_32659","_id":"00101","projection":"4326","longitude":" -45","latitude":" -22.667","timezone":"0","alias":"dcp_32659","mask":"PCD_serrmar_INPE/32659.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45 -22.667)"},{"class":"DataSet","id":83,"data_series_id":23,"active":false,"format":{"table_name":"dcp_30891","_id":"31069","projection":"4326","longitude":" -45.72","latitude":" -23.755","timezone":"0","alias":"dcp_30891","mask":"PCD_serrmar_INPE/30891.txt","active":"false","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.72 -23.755)"},{"class":"DataSet","id":77,"data_series_id":23,"active":true,"format":{"table_name":"dcp_30885","_id":"01683","projection":"4326","longitude":" -44.941","latitude":" -23.074","timezone":"0","alias":"dcp_30885","mask":"PCD_serrmar_INPE/30885.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-44.941 -23.074)"},{"class":"DataSet","id":91,"data_series_id":23,"active":true,"format":{"table_name":"dcp_69030","_id":"96199","projection":"4326","longitude":" -45.552","latitude":" -23.787","timezone":"0","alias":"dcp_69030","mask":"PCD_serrmar_INPE/69030.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.552 -23.787)"},{"class":"DataSet","id":85,"data_series_id":23,"active":true,"format":{"table_name":"dcp_31990","_id":"93010","projection":"4326","longitude":" -46.803","latitude":" -23.889","timezone":"0","alias":"dcp_31990","mask":"PCD_serrmar_INPE/31990.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-46.803 -23.889)"},{"class":"DataSet","id":78,"data_series_id":23,"active":false,"format":{"table_name":"dcp_30886","_id":"08666","projection":"4326","longitude":" -46.121","latitude":" -23.758","timezone":"0","alias":"dcp_30886","mask":"PCD_serrmar_INPE/30886.txt","active":"false","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-46.121 -23.758)"},{"class":"DataSet","id":80,"data_series_id":23,"active":false,"format":{"table_name":"dcp_30888","_id":"00103","projection":"4326","longitude":" -45.6","latitude":" -23.408","timezone":"0","alias":"dcp_30888","mask":"PCD_serrmar_INPE/30888.txt","active":"false","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.6 -23.408)"},{"class":"DataSet","id":94,"data_series_id":23,"active":true,"format":{"table_name":"dcp_69033","_id":"93059","projection":"4326","longitude":" -45.486","latitude":" -23.693","timezone":"0","alias":"dcp_69033","mask":"PCD_serrmar_INPE/69033.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.486 -23.693)"},{"class":"DataSet","id":95,"data_series_id":23,"active":true,"format":{"table_name":"dcp_69034","_id":"60015","projection":"4326","longitude":" -46.492","latitude":" -23.889","timezone":"0","alias":"dcp_69034","mask":"PCD_serrmar_INPE/69034.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-46.492 -23.889)"},{"class":"DataSet","id":81,"data_series_id":23,"active":true,"format":{"table_name":"dcp_30889","_id":"91303","projection":"4326","longitude":" -46.79","latitude":" -24.174","timezone":"0","alias":"dcp_30889","mask":"PCD_serrmar_INPE/30889.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-46.79 -24.174)"},{"class":"DataSet","id":79,"data_series_id":23,"active":true,"format":{"table_name":"dcp_30887","_id":"78503","projection":"4326","longitude":" -44.85","latitude":" -23.355","timezone":"0","alias":"dcp_30887","mask":"PCD_serrmar_INPE/30887.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-44.85 -23.355)"},{"class":"DataSet","id":90,"data_series_id":23,"active":true,"format":{"table_name":"dcp_32663","_id":"30517","projection":"4326","longitude":" -45.467","latitude":" -22.917","timezone":"0","alias":"dcp_32663","mask":"PCD_serrmar_INPE/32663.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.467 -22.917)"},{"class":"DataSet","id":84,"data_series_id":23,"active":true,"format":{"table_name":"dcp_31000","_id":"36067","projection":"4326","longitude":" -45.002","latitude":" -22.675","timezone":"0","alias":"dcp_31000","mask":"PCD_serrmar_INPE/31000.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.002 -22.675)"},{"class":"DataSet","id":92,"data_series_id":23,"active":true,"format":{"table_name":"dcp_69031","_id":"33111","projection":"4326","longitude":" -45.118","latitude":" -23.391","timezone":"0","alias":"dcp_69031","mask":"PCD_serrmar_INPE/69031.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.118 -23.391)"},{"class":"DataSet","id":86,"data_series_id":23,"active":true,"format":{"table_name":"dcp_32521","_id":"61119","projection":"4326","longitude":" -45.425","latitude":" -23.69","timezone":"0","alias":"dcp_32521","mask":"PCD_serrmar_INPE/32521.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.425 -23.69)"},{"class":"DataSet","id":87,"data_series_id":23,"active":true,"format":{"table_name":"dcp_32524","_id":"33303","projection":"4326","longitude":" -45.189","latitude":" -22.801","timezone":"0","alias":"dcp_32524","mask":"PCD_serrmar_INPE/32524.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-45.189 -22.801)"},{"class":"DataSet","id":88,"data_series_id":23,"active":true,"format":{"table_name":"dcp_32533","_id":"13193","projection":"4326","longitude":" -44.839","latitude":" -22.804","timezone":"0","alias":"dcp_32533","mask":"PCD_serrmar_INPE/32533.txt","active":"true","timestamp_property":"datetime"},"position":"SRID=4326;POINT(-44.839 -22.804)"}]}],"Collectors":[{"class":"Collector","id":7,"project_id":5,"service_instance_id":1,"input_data_series":22,"output_data_series":23,"input_output_map":[{"input":58,"output":77},{"input":59,"output":78},{"input":60,"output":79},{"input":61,"output":80},{"input":62,"output":81},{"input":63,"output":82},{"input":64,"output":83},{"input":65,"output":84},{"input":66,"output":85},{"input":67,"output":86},{"input":68,"output":87},{"input":69,"output":88},{"input":70,"output":89},{"input":71,"output":90},{"input":72,"output":91},{"input":73,"output":92},{"input":74,"output":93},{"input":75,"output":94},{"input":76,"output":95}],"schedule_type":3,"schedule":{},"filter":{},"intersection":{},"active":true}]})x";

  QJsonDocument doc = QJsonDocument::fromJson(dataSeriesJson.toUtf8());
  QJsonObject obj = doc.object();
  dataManager->addJSon(obj);
}

ViewId addView(terrama2::core::DataManagerPtr dataManager) {
  ViewId viewId = 14;
  QString viewJson = QString(R"x({"Views":[{"class":"View","id":%1,"name":"view de analise de dcp","description":null,"dataseries_id":23,"schedule":{},"automatic_schedule":{},"active":true,"service_instance_id":1,"project_id":5,"legend":{"class":"ViewStyleLegend","id":12,"column":null,"type":2,"colors":[{"id":87,"title":"Default","color":"#FFFFFFFF","value":"","isDefault":true,"view_style_id":12},{"id":86,"title":"Color 1","color":"#EEABABFF","value":"10","isDefault":false,"view_style_id":12},{"id":85,"title":"Color 2","color":"#E24A4AFF","value":"50","isDefault":false,"view_style_id":12},{"id":84,"title":"Color 3","color":"#FF0000FF","value":"100","isDefault":false,"view_style_id":12}],"metadata":{"creation_type":"editor","column":"pluvio"},"band_number":null},"private":false,"schedule_type":3,"source_type":3}]})x").arg(viewId);

  QJsonDocument doc = QJsonDocument::fromJson(viewJson.toUtf8());
  QJsonObject obj = doc.object();
  dataManager->addJSon(obj);
  return viewId;
}

int main(int argc, char** argv)
{
  try
  {

    QCoreApplication app(argc, argv);

    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    std::locale::global(std::locale::classic());

    terrama2::core::TerraMA2Init terramaRaii("example", 0);
    terrama2::core::registerFactories();

    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    serviceManager.setInstanceId(1);

    auto dataManager = std::make_shared<terrama2::services::view::core::DataManager>();
    addFileDataProviders(dataManager);
    addPostgisDataProviders(dataManager);
    addDataSeries(dataManager);
    ViewId viewId = addView(dataManager);

    terrama2::services::view::core::Service service(dataManager);

    auto logger = std::make_shared<ViewLoggerMock>();
    ::testing::DefaultValue<RegisterId>::Set(1);
    EXPECT_CALL(*logger, setConnectionInfo(_)).Times(::testing::AtLeast(1));
    EXPECT_CALL(*logger, start(_)).WillRepeatedly(::testing::Return(1));
    EXPECT_CALL(*logger, result(_, _, _));
    EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(logger));
    te::core::URI connInfoURI("postgis://postgres:postgres@localhost:5432/terrama2");
    EXPECT_CALL(*logger, getConnectionInfo()).WillRepeatedly(::testing::Return(connInfoURI));
    EXPECT_CALL(*logger,setStartProcessingTime(_,_));
    EXPECT_CALL(*logger,setEndProcessingTime(_,_));
    EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

    logger->setConnectionInfo(te::core::URI());

    service.setLogger(logger);

    QJsonObject additionalIfo;
    additionalIfo.insert("maps_server", QString("http://admin:geoserver@localhost:8080/geoserver"));

    service.updateAdditionalInfo(additionalIfo);

    service.start(1);

    service.addToQueue(viewId, terrama2::core::TimeUtils::nowUTC());

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);
    app.exec();

    service.stopService();

    //    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(logger.get()));

  }
  catch(...)
  {
    std::cout << "\n\nException...\n" << std::endl;
  }

  return 0;
}
