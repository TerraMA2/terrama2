// TerraMA2

#include <terrama2/Exception.hpp>
#include <terrama2/core/data-model/DataManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>
#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/network/TcpSignals.hpp>
#include <terrama2/core/utility/JSonUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/Logger.hpp>
#include <terrama2/impl/Utils.hpp>

#include <terrama2/services/analysis/core/utility/JSonUtils.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>

#include <terrama2/impl/Utils.hpp>
#include <terrama2/Config.hpp>

//STL
#include <iostream>

// Qt
#include <QUrl>
#include <QTimer>
#include <QObject>
#include <QCoreApplication>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QDataStream>
#include <QtTest/QTest>

#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <Python.h>

// Boost
#include <boost/exception/diagnostic_information.hpp>

using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{
  try
  {

    terrama2::core::TerraMA2Init terramaRaii("example", 0);

    terrama2::core::registerFactories();

    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
    serviceManager.setLogConnectionInfo(uri);

    terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

    QCoreApplication app(argc, argv);
    // DataProvider information
    terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
    outputDataProvider->id = 3;
    outputDataProvider->name = "DataProvider postgis";
    outputDataProvider->uri = uri.uri();
    outputDataProvider->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
    outputDataProvider->dataProviderType = "POSTGIS";
    outputDataProvider->active = true;


    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

    // DataSeries information
    terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
    outputDataSeries->id = 3;
    outputDataSeries->name = "Analysis result";
    outputDataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
    outputDataSeries->dataProviderId = outputDataProviderPtr->id;


    // DataSet information
    terrama2::core::DataSet* outputDataSet = new terrama2::core::DataSet();
    outputDataSet->active = true;
    outputDataSet->id = 2;
    outputDataSet->dataSeriesId = outputDataSeries->id;
    outputDataSet->format.emplace("table_name", "analysis_result");

    outputDataSeries->datasetList.emplace_back(outputDataSet);


    std::string script = "x = dcp.min(\"Serra do Mar\", \"pluvio\", 2, Buffer.OBJECT_PLUS_EXTERN)\n"
            "add_value(\"min\", x)\n";


    Analysis* analysis = new Analysis;
    AnalysisPtr analysisPtr(analysis);

    analysis->id = 1;
    analysis->name = "Min DCP";
    analysis->script = script;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = 3;
    analysis->serviceInstanceId = 1;

    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->name = "Provider";
    dataProvider->uri += TERRAMA2_DATA_DIR;
    dataProvider->uri += "/shapefile";
    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;
    dataProvider->id = 1;


    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->dataProviderId = dataProvider->id;
    outputDataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");
    dataSeries->name = "Monitored Object";
    dataSeries->id = 1;
    dataSeries->dataProviderId = 1;

    //DataSet information
    terrama2::core::DataSet* dataSet = new terrama2::core::DataSet;
    terrama2::core::DataSetPtr dataSetPtr(dataSet);
    dataSet->active = true;
    dataSet->format.emplace("mask", "municipios_afetados.shp");
    dataSet->format.emplace("srid", "4618");
    dataSet->id = 1;

    dataSeries->datasetList.push_back(dataSetPtr);

    terrama2::core::DataProvider* dataProvider2 = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProvider2Ptr(dataProvider2);
    dataProvider2->name = "Provider";
    dataProvider2->uri += TERRAMA2_DATA_DIR;
    dataProvider2->uri += "/PCD_serrmar_INPE";
    dataProvider2->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider2->dataProviderType = "FILE";
    dataProvider2->active = true;
    dataProvider2->id = 2;


    AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeriesPtr->id;
    monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "objet_id_5";


    //DataSeries information
    terrama2::core::DataSeries* dcpSeries = new terrama2::core::DataSeries;
    terrama2::core::DataSeriesPtr dcpSeriesPtr(dcpSeries);
    dcpSeries->dataProviderId = dataProvider2->id;
    dcpSeries->semantics = semanticsManager.getSemantics("DCP-inpe");
    dcpSeries->semantics.dataSeriesType = terrama2::core::DataSeriesType::DCP;
    dcpSeries->name = "Serra do Mar";
    dcpSeries->id = 2;
    dcpSeries->dataProviderId = 2;

    //DataSet information
    terrama2::core::DataSetDcp* dcpDataset69034 = new terrama2::core::DataSetDcp;
    terrama2::core::DataSetDcpPtr dcpDataset69034Ptr(dcpDataset69034);
    dcpDataset69034->active = true;
    dcpDataset69034->format.emplace("mask", "69033.txt");
    dcpDataset69034->format.emplace("timezone", "-02:00");
    dcpDataset69034->dataSeriesId = 2;
    dcpDataset69034->id = 2;
    dcpDataset69034->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-44.46540, -23.00506, 4618, te::gm::PointType, nullptr));
    dcpSeries->datasetList.push_back(dcpDataset69034Ptr);


    terrama2::core::DataSetDcp* dcpDataset30886 = new terrama2::core::DataSetDcp;
    terrama2::core::DataSetDcpPtr dcpDataset30886Ptr(dcpDataset30886);
    dcpDataset30886->active = true;
    dcpDataset30886->format.emplace("mask", "30886.txt");
    dcpDataset30886->format.emplace("timezone", "-02:00");
    dcpDataset30886->dataSeriesId = 2;
    dcpDataset30886->id = 3;
    dcpDataset30886->position = std::shared_ptr<te::gm::Point>(new te::gm::Point(-46.121, -23.758, 4618, te::gm::PointType, nullptr));
    dcpSeries->datasetList.push_back(dcpDataset30886Ptr);

    AnalysisDataSeries dcpADS;
    dcpADS.id = 2;
    dcpADS.dataSeriesId = dcpSeriesPtr->id;
    dcpADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
    dcpADS.metadata["INFLUENCE_TYPE"] = "RADIUS_CENTER";
    dcpADS.metadata["RADIUS"] = "50";

    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(dcpADS);
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysis->analysisDataSeriesList = analysisDataSeriesList;


    // Serialize objects
    QJsonObject obj;

    QJsonArray providersArray;
    providersArray.push_back(terrama2::core::toJson(outputDataProviderPtr));
    providersArray.push_back(terrama2::core::toJson(dataProviderPtr));
    providersArray.push_back(terrama2::core::toJson(dataProvider2Ptr));
    obj.insert("DataProviders", providersArray);

    QJsonArray seriesArray;
    seriesArray.push_back(terrama2::core::toJson(dataSeriesPtr));
    seriesArray.push_back(terrama2::core::toJson(dcpSeriesPtr));
    seriesArray.push_back(terrama2::core::toJson(outputDataSeriesPtr));
    obj.insert("DataSeries", seriesArray);

    QJsonArray analysisArray;
    analysisArray.push_back(terrama2::services::analysis::core::toJson(analysisPtr));
    obj.insert("Analysis", analysisArray);

    // Creates JSON document
    QJsonDocument doc(obj);

    // Starts the service and TCP manager
    auto dataManager = std::make_shared<DataManager>();
    terrama2::core::TcpManager tcpManager(dataManager, std::weak_ptr<terrama2::core::ProcessLogger>());
    tcpManager.listen(QHostAddress::Any, 30000);
    terrama2::services::analysis::core::Service service(dataManager);
    terrama2::core::ServiceManager::getInstance().setInstanceId(1);

    auto logger = std::make_shared<AnalysisLogger>();
    logger->setConnectionInfo(uri);
    service.setLogger(logger);

    service.start();


    // Sends the data via TCP
    QByteArray bytearray;
    QDataStream out(&bytearray, QIODevice::WriteOnly);

    out << static_cast<uint32_t>(0);
    out << static_cast<uint32_t>(terrama2::core::TcpSignal::ADD_DATA_SIGNAL);
    out << doc.toJson();
    bytearray.remove(8, 4);//Remove QByteArray header
    out.device()->seek(0);
    out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

    QTcpSocket socket;
    socket.connectToHost("localhost", 30001);
    socket.write(bytearray);
    socket.waitForBytesWritten();

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(30000);
    app.exec();

    service.stopService();
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(const std::exception& e)
  {
    QString errMsg(e.what());
    TERRAMA2_LOG_ERROR() << errMsg;
  }
  catch(...)
  {
    QString errMsg(QObject::tr("Unexpected exception"));
    TERRAMA2_LOG_ERROR() << errMsg;
  }




  return 0;
}
