#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>

#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <terrama2/impl/Utils.hpp>
#include <terrama2/Config.hpp>


#include <extra/data/DCPSerramarInpe.hpp>
#include <extra/data/ResultAnalysisPostGis.hpp>


#include <iostream>

// QT
#include <QTimer>
#include <QCoreApplication>
#include <QUrl>

using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{
    terrama2::core::TerraMA2Init terramaRaii("example", 0);
    Q_UNUSED(terramaRaii);

    terrama2::core::registerFactories();

    {
        terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

        QCoreApplication app(argc, argv);

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

        auto dataProviderDCP = terrama2::serramar::dataProviderPostGisDCP();
        dataManager->add(dataProviderDCP);

        auto dcpSerramar = terrama2::serramar::dataSeriesDcpSerramarPostGis(dataProviderDCP);
        dataManager->add(dcpSerramar);

        AnalysisDataSeries dcpADS;
        dcpADS.id = 1;
        dcpADS.dataSeriesId = dcpSerramar->id;
        dcpADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
        dcpADS.metadata["identifier"] = "table_name";


        auto dataProvider = terrama2::resultanalysis::dataProviderResultAnalysis();
        dataManager->add(dataProvider);

        auto dataSeriesResult = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProvider, terrama2::resultanalysis::tablename::analysis_dcp_result, dcpSerramar);
        dataManager->add(dataSeriesResult);


        std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

        std::string script = R"z(moBuffer = Buffer(BufferType.Out_union, 2., "km")
ids = dcp.influence.by_rule(moBuffer)
x = dcp.max("pluvio", ids)
add_value("max", x))z";

        analysis->id = 1;
        analysis->name = "Min DCP";
        analysis->script = script;
        analysis->scriptLanguage = ScriptLanguage::PYTHON;
        analysis->type = AnalysisType::DCP_TYPE;
        analysis->active = true;
        analysis->outputDataSeriesId = dataSeriesResult->id;
        analysis->outputDataSetId = dataSeriesResult->datasetList.front()->id;
        analysis->serviceInstanceId = 1;

        analysis->metadata["INFLUENCE_TYPE"] = "1";
        analysis->metadata["INFLUENCE_RADIUS"] = "50";
        analysis->metadata["INFLUENCE_RADIUS_UNIT"] = "km";


        std::vector<AnalysisDataSeries> analysisDataSeriesList;
        analysisDataSeriesList.push_back(dcpADS);
        analysis->analysisDataSeriesList = analysisDataSeriesList;

        dataManager->add(analysis);


        service.addToQueue(analysis->id, terrama2::core::TimeUtils::nowUTC());

        QTimer timer;
        QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
        timer.start(10000);

        app.exec();
  }

  return 0;
}
