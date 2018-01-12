#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetGrid.hpp>

#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <extra/data/Geotiff.hpp>


#include <terrama2/impl/Utils.hpp>
#include <terrama2/Config.hpp>

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

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  {
    QCoreApplication app(argc, argv);

    auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();

    /*
     * DataProvider and dataSeries
    */

    auto dataProvider = terrama2::geotiff::dataProviderFileGrid();
    dataManager->add(dataProvider);


    auto outputDataSeries = terrama2::geotiff::dataSeriesResultAnalysisGrid(dataProvider,terrama2::geotiff::nameoutputgrid::output_grid);
    dataManager->add(outputDataSeries);



    auto dataSeries = terrama2::geotiff::dataSeriesL5219076(dataProvider);
    dataManager->add(dataSeries);


    AnalysisDataSeries gridADS1;
    gridADS1.id = 1;
    gridADS1.dataSeriesId = dataSeries->id;
    gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;



    std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();

    analysis->id = 1;
    analysis->name = "Grid Sample";
    analysis->script = R"x(return grid.sample("geotiff 1"))x";
    analysis->scriptLanguage = ScriptLanguage::PYTHON;
    analysis->type = AnalysisType::GRID_TYPE;
    analysis->active = true;\
    analysis->outputDataSeriesId = outputDataSeries->id;
    analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
    analysis->serviceInstanceId = 1;



    std::vector<AnalysisDataSeries> analysisDataSeriesList;
    analysisDataSeriesList.push_back(gridADS1);
    analysis->analysisDataSeriesList = analysisDataSeriesList;




    std::shared_ptr<AnalysisOutputGrid> outputGrid = std::make_shared<AnalysisOutputGrid>();

    outputGrid->analysisId = 1;
    outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
    outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
    outputGrid->interestAreaDataSeriesId = dataSeries->id;
    outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
    outputGrid->resolutionDataSeriesId = dataSeries->id;
    outputGrid->interpolationDummy = 266;

    analysis->outputGridPtr = outputGrid;

    dataManager->add(analysis);

    terrama2::services::analysis::core::AnalysisExecutor executor;
    auto result = executor.validateAnalysis(dataManager, analysis);

    std::cout << "Validate result for grid analysis: " << (result.valid ? "OK" : "Not OK") << std::endl;
    for (const auto& message : result.messages)
    {
      std::cout << message << std::endl;
    }


    app.exec();

  }


  return 0;
}
