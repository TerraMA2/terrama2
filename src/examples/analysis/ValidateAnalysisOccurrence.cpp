#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/DataAccessorFactory.hpp>
#include <terrama2/core/utility/Logger.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSet.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>

#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/Shared.hpp>


#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>


#include <terrama2/impl/Utils.hpp>

#include <examples/data/ResultAnalysisPostGis.hpp>
#include <examples/data/OccurrenceWFP.hpp>
#include <examples/data/StaticPostGis.hpp>


// STL
#include <iostream>
#include <memory>

// QT
#include <QTimer>
#include <QCoreApplication>
#include <QUrl>

//monitoredDataSeries
using namespace terrama2::services::analysis::core;

int main(int argc, char* argv[])
{

  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  Q_UNUSED(terramaRaii);

  terrama2::core::registerFactories();

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  QCoreApplication app(argc, argv);


  auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();

  /*
   * DataProvider and dataSeries result
  */
  auto dataProviderResult = terrama2::resultanalysis::dataProviderResultAnalysis();
  dataManager->add(dataProviderResult);


  auto outputDataSeries = terrama2::resultanalysis::dataSeriesResultAnalysisPostGis(dataProviderResult, terrama2::resultanalysis::tablename::occurrence_analysis_result);
  dataManager->add(outputDataSeries);


  std::shared_ptr<terrama2::services::analysis::core::Analysis> analysis = std::make_shared<terrama2::services::analysis::core::Analysis>();
  analysis->id = 1;
  analysis->name = "Analysis";
  analysis->active = true;

  std::string script = R"z(x = occurrence.zonal.count("Occurrence", "2d")
add_value("count", x))z";


  analysis->script = script;
  analysis->outputDataSeriesId = outputDataSeries->id;
  analysis->outputDataSetId = outputDataSeries->datasetList.front()->id;
  analysis->scriptLanguage = ScriptLanguage::PYTHON;
  analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
  analysis->serviceInstanceId = 1;

  /*
   * DataProvider and dataSeries Static
  */

  auto dataProviderStatic = terrama2::staticpostgis::dataProviderStaticPostGis();
  dataManager->add(dataProviderStatic);

  auto staticDataSeries = terrama2::staticpostgis::dataSeriesEstados2010(dataProviderStatic);
  dataManager->add(staticDataSeries);

  AnalysisDataSeries monitoredObjectADS;
  monitoredObjectADS.id = 1;
  monitoredObjectADS.dataSeriesId = staticDataSeries->id;
  monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
  monitoredObjectADS.metadata["identifier"] = "fid";


  /*
   * DataProvider and dataSeries Occurrence
  */

  auto dataProviderOcc = terrama2::occurrencewfp::dataProviderPostGisOccWFP();
  dataManager->add(dataProviderOcc);

  auto occurrenceDataSeries = terrama2::occurrencewfp::dataSeriesOccWFPPostGis(dataProviderOcc);
  dataManager->add(occurrenceDataSeries);

  AnalysisDataSeries occurrenceADS;
  occurrenceADS.id = 2;
  occurrenceADS.dataSeriesId = occurrenceDataSeries->id;
  occurrenceADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
  occurrenceADS.alias = "occ";

  std::vector<AnalysisDataSeries> analysisDataSeriesList;
  analysisDataSeriesList.push_back(monitoredObjectADS);
  analysisDataSeriesList.push_back(occurrenceADS);

  analysis->analysisDataSeriesList = analysisDataSeriesList;


  dataManager->add(analysis);

  terrama2::services::analysis::core::AnalysisExecutor executor;
  auto result = executor.validateAnalysis(dataManager, analysis);

  std::cout << "Validate result for monitored object analysis: " <<  (result.valid ? "OK" : "Not OK") << std::endl;
  for(const auto& message : result.messages)
  {
    std::cout << message << std::endl;
  }

  app.exec();




  return 0;
}
