#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/GeoUtils.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <examples/data/RiscoFogo.hpp>

#include <terrama2/impl/Utils.hpp>
#include <terrama2/Config.hpp>

#include <iostream>

// QT
#include <QTimer>
#include <QCoreApplication>
#include <QUrl>


using namespace terrama2::services::analysis::core;

std::string script()
{
  std::string script = R"z(import math
# Calculate the 'A' factor acording to vegetation type
def calculateA (tipo_vegetacao):
  if tipo_vegetacao == 0  or tipo_vegetacao == 13 or tipo_vegetacao == 15 or tipo_vegetacao == 16:
      return 0
  elif tipo_vegetacao == 1 or tipo_vegetacao == 3 or tipo_vegetacao == 5:
      return  2
  elif tipo_vegetacao == 2 or tipo_vegetacao == 11:
      return 1.5
  elif tipo_vegetacao == 4:
      return 1.72
  elif tipo_vegetacao == 6 or tipo_vegetacao == 8:
      return 2.4
  elif tipo_vegetacao == 7 or tipo_vegetacao == 9:
      return  3
  elif tipo_vegetacao == 10:
      return 6
  elif tipo_vegetacao == 12 or tipo_vegetacao == 14:
      return  4
  return  0

# Converte IGBP vegetation class to INPE classes
def convertIGBP2INPE(igbp):
  if igbp in {0,  13, 15, 16}:
      return 0

  if igbp in {10}:
      return 1

  if igbp in {12, 14}:
      return 2

  if igbp in {7, 9}:
      return 3

  if igbp in {6, 8}:
      return 4

  if igbp in {1, 5, 3}:
      return 5

  if igbp in {4}:
      return 6

  if igbp in {2, 11}:
      return 7

  return None


# Check for max value for PSE

def maxPSE(igbp, pse):
  ibge = convertIGBP2INPE(igbp)
  if ibge == 1:
      return pse if pse < 30 else 30
  elif ibge == 2:
      return pse if pse < 45 else 45
  elif ibge == 3:
      return pse if pse < 60 else 60
  elif ibge == 4:
      return pse if pse < 75 else 75
  elif ibge == 5:
      return pse if pse < 90 else 90
  elif ibge == 6:
      return pse if pse < 105 else 105
  elif ibge == 7:
      return pse if pse < 120 else 120
  return None

# 1 - retrieve rain data from the last 120 days
prec1 = grid.history.interval.sum("precipitacao", "1d", "0d", 0)
prec2 = grid.history.interval.sum("precipitacao", "2d", "1d", 0)
prec3 = grid.history.interval.sum("precipitacao", "3d", "2d", 0)
prec4 = grid.history.interval.sum("precipitacao", "4d", "3d", 0)
prec5 = grid.history.interval.sum("precipitacao", "5d", "4d", 0)
prec6_10 = grid.history.interval.sum("precipitacao", "10d", "5d", 0)
prec11_15 = grid.history.interval.sum("precipitacao", "15d", "10d", 0)
prec16_30 = grid.history.interval.sum("precipitacao", "30d", "15d", 0)
prec31_60 = grid.history.interval.sum("precipitacao", "60d", "30d", 0)
prec61_90 = grid.history.interval.sum("precipitacao", "90d", "60d", 0)
prec91_120 = grid.history.interval.sum("precipitacao", "120d", "90d", 0)



# 2 - calculate rain factors
fp1 = math.exp(-0.14 * prec1)
fp2 = math.exp(-0.07 * prec2)
fp3 = math.exp(-0.04 * prec3)
fp4 = math.exp(-0.03 * prec4)
fp5 = math.exp(-0.02 * prec5)
fp6_10 = math.exp(-0.01 * prec6_10)
fp11_15 = math.exp(-0.008 * prec11_15)
fp16_30 = math.exp(-0.004 * prec16_30)
fp31_60 = math.exp(-0.002 * prec31_60)
fp61_90 = math.exp(-0.001 * prec61_90)
fp91_120 = math.exp(-0.0007 * prec91_120)


# 3 - calculate days without rain
pse = 105. * fp1 * fp2 * fp3 * fp4 * fp5 * fp6_10 *  fp11_15 * fp16_30 * fp31_60 * fp61_90 * fp91_120

# 4 - Base vegetation fire risk
tipo_vegetacao = grid.sample("lancover")

a = calculateA(tipo_vegetacao)
if not a: return 0


# Adjust max PSE
PSE = maxPSE(tipo_vegetacao, pse)
if not PSE: return 0

rb = 0.9 * (1. + math.sin((a*PSE-90.)*3.1416/180.)) * 0.5
if(rb > 0.9):
  rb = 0.9

# 5 - humidity factor
ur = grid.history.interval.sum("umidade", "1d", "0d", 0)
fu = ur * -0.006 + 1.3


# 6 - temperature factor
tempMax =  grid.history.interval.sum("temperatura", "1d", "0d", 0)
ft = (tempMax-273.15) * 0.02 + 0.4

# 7 - observable risk
rf = rb * ft * fu


if(rf > 1):
  rf = 1


return  rf)z";

  return script;
}

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  Q_UNUSED(terramaRaii);


  terrama2::core::registerFactories();

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  {
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


    auto dataProvider = terrama2::riscofogo::dataProviderFileGrid();
    dataManager->add(dataProvider);


    auto outputDataSeriesGrid = terrama2::riscofogo::dataSeriesResultAnalysisGrid(dataProvider);
    dataManager->add(outputDataSeriesGrid);



    /*
     *  dataSeries and AnalysisDataSeries Humidity
    */
    auto dataSeriesHum = terrama2::riscofogo::dataSeriesHumidity(dataProvider);
    dataManager->add(dataSeriesHum);

    AnalysisDataSeries gridADS1;
    gridADS1.id = 1;
    gridADS1.dataSeriesId = dataSeriesHum->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    /*
     * dataSeries and AnalysisDataSeries Precipitation
    */
    auto dataSeriesPrec = terrama2::riscofogo::dataSeriesPrecipitation(dataProvider);
    dataManager->add(dataSeriesPrec);

    AnalysisDataSeries gridADS2;
    gridADS2.id = 2;
    gridADS2.dataSeriesId = dataSeriesPrec->id;
    gridADS2.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    /*
     * dataSeries and AnalysisDataSeries Temperature
    */
    auto dataSeriesTemp = terrama2::riscofogo::dataSeriesTemperature(dataProvider);
    dataManager->add(dataSeriesTemp);

    AnalysisDataSeries gridADS3;
    gridADS3.id = 3;
    gridADS3.dataSeriesId = dataSeriesTemp->id;
    gridADS3.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    /*
     * dataSeries and AnalysisDataSeries Lancover
    */
    auto dataSeriesLan = terrama2::riscofogo::dataSeriesLancover(dataProvider);
    dataManager->add(dataSeriesLan);

    AnalysisDataSeries gridADS4;
    gridADS4.id = 4;
    gridADS4.dataSeriesId = dataSeriesLan->id;
    gridADS4.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    analysis->id = 1;
    analysis->name = "Grid Sample";
    analysis->script = script();
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::GRID_TYPE;
    analysis->active = true;
    analysis->outputDataSeriesId = outputDataSeriesGrid->id;
    analysis->outputDataSetId = outputDataSeriesGrid->datasetList.front()->id;
    analysis->serviceInstanceId = 1;



    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(gridADS1);
    analysisDataSeriesList.push_back(gridADS2);
    analysisDataSeriesList.push_back(gridADS3);
    analysisDataSeriesList.push_back(gridADS4);
    analysis->analysisDataSeriesList = analysisDataSeriesList;



    std::shared_ptr<AnalysisOutputGrid> outputGrid = std::make_shared<AnalysisOutputGrid>();

    outputGrid->analysisId = 1;
    outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
    outputGrid->interestAreaType = InterestAreaType::CUSTOM;
    outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
    outputGrid->resolutionDataSeriesId = dataSeriesPrec->id;
    outputGrid->interpolationDummy = 0;
    outputGrid->interestAreaBox = terrama2::core::ewktToGeom("SRID=4326;POLYGON((-53 -25,-53 -19,-44 -19,-44 -25,-53 -25))");

    analysis->outputGridPtr = outputGrid;

    dataManager->add(analysis);



    service.addToQueue(analysis, terrama2::core::TimeUtils::stringToTimestamp("2017-09-20T13:00:00-03", terrama2::core::TimeUtils::webgui_timefacet));

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &service, &Service::stopService);
    QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
    timer.start(10000);

    app.exec();

  }



  return 0;
}
