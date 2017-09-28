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


#include <terrama2/core/utility/TimeUtils.hpp>
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


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include "UtilsPostGis.hpp"
#include "UtilsDCPSerrmarInpe.hpp"

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

    terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

    auto& serviceManager = terrama2::core::ServiceManager::getInstance();

    auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();

    auto loggerCopy = std::make_shared<terrama2::core::MockAnalysisLogger>();

    EXPECT_CALL(*loggerCopy, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*loggerCopy, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*loggerCopy, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*loggerCopy, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*loggerCopy, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*loggerCopy, start(::testing::_)).WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*loggerCopy, isValid()).WillRepeatedly(::testing::Return(true));

    auto logger = std::make_shared<terrama2::core::MockAnalysisLogger>();

    EXPECT_CALL(*logger, setConnectionInfo(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*logger, setTableName(::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*logger, getLastProcessTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*logger, getDataLastTimestamp(::testing::_)).WillRepeatedly(::testing::Return(nullptr));
    EXPECT_CALL(*logger, done(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*logger, start(::testing::_)).WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*logger, clone()).WillRepeatedly(::testing::Return(loggerCopy));
    EXPECT_CALL(*logger, isValid()).WillRepeatedly(::testing::Return(true));

    Service service(dataManager);
    serviceManager.setInstanceId(1);
    serviceManager.setLogger(logger);
    serviceManager.setLogConnectionInfo(te::core::URI(""));

    service.setLogger(logger);
    service.start();


    QCoreApplication app(argc, argv);

    using namespace terrama2::examples::analysis::utilspostgis;
    using namespace terrama2::examples::analysis::utilsdcpserrmarinpe;

    // DataProvider information
    auto dataProvider = dataProviderPostGis();
    dataManager->add(dataProvider);

    // DataSeries information
    auto outputDataSeries = outputDataSeriesPostGis(dataProvider, analysis_result);
    dataManager->add(outputDataSeries);


    std::string script = R"z(moBuffer = Buffer(BufferType.Out_union, 2., "km")
ids = dcp.zonal.influence.by_rule("Serra do Mar", moBuffer)
x = dcp.zonal.min("Serra do Mar", "pluvio",ids)
add_value("min", x))z";


    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    analysis->id = 1;
    analysis->name = "Min DCP";
    analysis->script = script;
    analysis->outputDataSeriesId = outputDataSeries->id;
    analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
    analysis->active = true;
    analysis->serviceInstanceId = 1;


    analysis->metadata["INFLUENCE_TYPE"] = "1";
    analysis->metadata["INFLUENCE_RADIUS"] = "50";
    analysis->metadata["INFLUENCE_RADIUS_UNIT"] = "km";


    auto dataSeriesMunicSerrmar = dataSeriesMunicSerrmarInpe(dataProvider);
    dataManager->add(dataSeriesMunicSerrmar);

    AnalysisDataSeries monitoredObjectADS;
    monitoredObjectADS.id = 1;
    monitoredObjectADS.dataSeriesId = dataSeriesMunicSerrmar->id;
    monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
    monitoredObjectADS.metadata["identifier"] = "objet_id_1";



    //DataProvider PCD_serrmar_INPE type = FILE

    auto dataProviderFileSerrmar = dataProviderFile();
    dataManager->add(dataProviderFileSerrmar);


    //DataSeries PCD_serrmar_INPE
    auto dcpSeriesDCP = dataSeries2DCP(dataProviderFileSerrmar);
    dataManager->add(dcpSeriesDCP);


    AnalysisDataSeries dcpADS;
    dcpADS.id = 2;
    dcpADS.dataSeriesId = dcpSeriesDCP->id;
    dcpADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(dcpADS);
    analysisDataSeriesList.push_back(monitoredObjectADS);
    analysis->analysisDataSeriesList = analysisDataSeriesList;


    dataManager->add(analysis);

    // Serialize objects
    QJsonObject obj;


    QJsonArray providersArray;
    providersArray.push_back(dataProviderPostGisjson());
    providersArray.push_back(dataProviderFileJson());
    obj.insert("DataProviders", providersArray);

    QJsonArray seriesArray;
    seriesArray.push_back(outputDataSeriesPostGisJson(dataProvider, analysis_result));
    seriesArray.push_back(dataSeriesMunicSerrmarInpeJson(dataProvider));
    seriesArray.push_back(dataSeries2DCPJson(dataProviderFileSerrmar));
    obj.insert("DataSeries", seriesArray);

    QJsonArray analysisArray;
    analysisArray.push_back(terrama2::services::analysis::core::toJson(analysis));
    obj.insert("Analysis", analysisArray);

    // Creates JSON document
    QJsonDocument doc(obj);

    // Start TCP manager
    terrama2::core::TcpManager tcpManager(dataManager, std::weak_ptr<terrama2::core::ProcessLogger>());
    tcpManager.listen(QHostAddress::Any, 30001);


    // Sends the data via TCP
    QByteArray bytearray;
    QDataStream out(&bytearray, QIODevice::WriteOnly);

    out << static_cast<uint32_t>(0);
    out << static_cast<uint32_t>(terrama2::core::TcpSignal::ADD_DATA_SIGNAL);
    out << doc.toJson(QJsonDocument::Compact);
    bytearray.remove(8, 4);//Remove QByteArray header
    out.device()->seek(0);
    out << static_cast<uint32_t>(bytearray.size() - sizeof(uint32_t));

    QTcpSocket socket;
    socket.connectToHost("localhost", 30001);
    socket.write(bytearray);
    socket.waitForBytesWritten();



    service.addToQueue(analysis->id, terrama2::core::TimeUtils::stringToTimestamp("2008-07-21T09:30:00-03", terrama2::core::TimeUtils::webgui_timefacet));


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
