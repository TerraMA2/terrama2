#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
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
#include <terrama2/services/analysis/core/AnalysisExecutor.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>

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

  terrama2::core::registerFactories();


  auto& serviceManager = terrama2::core::ServiceManager::getInstance();
te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);
  serviceManager.setLogConnectionInfo(uri);

  terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;

  {
    QCoreApplication app(argc, argv);



    auto &semanticsManager = terrama2::core::SemanticsManager::getInstance();

    {

      DataManagerPtr dataManager(new DataManager());
      // GRID ANALYSIS

      // DataProvider information
      terrama2::core::DataProvider *dataProvider = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
      dataProvider->uri = "file://";
      dataProvider->uri += TERRAMA2_DATA_DIR;
      dataProvider->uri += "/geotiff";

      dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
      dataProvider->dataProviderType = "FILE";
      dataProvider->active = true;
      dataProvider->id = 1;
      dataProvider->name = "Local Geotiff";

      dataManager->add(dataProviderPtr);



      //DataSeries information
      terrama2::core::DataSeries *outputDataSeries = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
      outputDataSeries->semantics = semanticsManager.getSemantics("GRID-gdal");
      outputDataSeries->name = "Output Grid";
      outputDataSeries->id = 5;
      outputDataSeries->dataProviderId = 1;

      terrama2::core::DataSetGrid *outputDataSet = new terrama2::core::DataSetGrid();
      outputDataSet->active = true;
      outputDataSet->format.emplace("mask", "output_grid.tif");
      outputDataSet->dataSeriesId = outputDataSeries->id;
      outputDataSeries->datasetList.emplace_back(outputDataSet);

      dataManager->add(outputDataSeriesPtr);

      dataManager->add(dataProviderPtr);



      //////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Data Series 1
      //////////////////////////////////////////////////////////////////////////////////////////////////////////

      terrama2::core::DataSeries *dataSeries1 = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr dataSeries1Ptr(dataSeries1);
      dataSeries1->semantics = semanticsManager.getSemantics("GRID-gdal");
      dataSeries1->name = "geotiff 1";
      dataSeries1->id = 1;
      dataSeries1->dataProviderId = 1;

      terrama2::core::DataSetGrid *dataSet1 = new terrama2::core::DataSetGrid();
      dataSet1->active = true;
      dataSet1->format.emplace("mask", "L5219076_07620040908_r3g2b1.tif");
      dataSet1->id = 1;

      dataSeries1->datasetList.emplace_back(dataSet1);

      AnalysisDataSeries gridADS1;
      gridADS1.id = 1;
      gridADS1.dataSeriesId = dataSeries1Ptr->id;
      gridADS1.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;


      dataManager->add(dataSeries1Ptr);

      //////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Analysis
      //////////////////////////////////////////////////////////////////////////////////////////////////////////

      Analysis *analysis = new Analysis;
      AnalysisPtr analysisPtr(analysis);


      analysis->id = 1;
      analysis->name = "Grid Sample";
      analysis->script = "return grid.sample(\"geotiff 1\")";
      analysis->scriptLanguage = ScriptLanguage::PYTHON;
      analysis->type = AnalysisType::GRID_TYPE;
      analysis->active = true;
      analysis->outputDataSeriesId = 5;
      analysis->serviceInstanceId = 1;


      std::vector<AnalysisDataSeries> analysisDataSeriesList;
      analysisDataSeriesList.push_back(gridADS1);
      analysis->analysisDataSeriesList = analysisDataSeriesList;


      analysis->schedule.frequency = 1;
      analysis->schedule.frequencyUnit = "min";


      AnalysisOutputGrid *outputGrid = new AnalysisOutputGrid();
      AnalysisOutputGridPtr outputGridPtr(outputGrid);

      outputGrid->analysisId = 1;
      outputGrid->interpolationMethod = InterpolationMethod::BILINEAR;
      outputGrid->interestAreaType = InterestAreaType::SAME_FROM_DATASERIES;
      outputGrid->interestAreaDataSeriesId = 1;
      outputGrid->resolutionType = ResolutionType::SAME_FROM_DATASERIES;
      outputGrid->resolutionDataSeriesId = 1;
      outputGrid->interpolationDummy = 266;

      analysis->outputGridPtr = outputGridPtr;

      dataManager->add(analysisPtr);

      terrama2::services::analysis::core::AnalysisExecutor executor;
      auto result = executor.validateAnalysis(dataManager, analysisPtr);

      std::cout << "Validate result for grid analysis: " << (result.valid ? "OK" : "Not OK") << std::endl;
      for (auto message : result.messages)
      {
        std::cout << message << std::endl;
      }
    }

    // MONITORED OBJECT ANALYSIS

    {
      DataManagerPtr dataManager(new DataManager());

      // DataProvider information
      terrama2::core::DataProvider* outputDataProvider = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr outputDataProviderPtr(outputDataProvider);
      outputDataProvider->id = 4;
      outputDataProvider->name = "DataProvider postgis";
      outputDataProvider->uri = uri.uri();
      outputDataProvider->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
      outputDataProvider->dataProviderType = "POSTGIS";
      outputDataProvider->active = true;


      dataManager->add(outputDataProviderPtr);


      // DataSeries information
      terrama2::core::DataSeries* outputDataSeries = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr outputDataSeriesPtr(outputDataSeries);
      outputDataSeries->id = 4;
      outputDataSeries->name = "Analysis result";
      outputDataSeries->semantics = semanticsManager.getSemantics("ANALYSIS_MONITORED_OBJECT-postgis");
      outputDataSeries->dataProviderId = outputDataProviderPtr->id;


      // DataSet information
      terrama2::core::DataSet* outputDataSet = new terrama2::core::DataSet();
      outputDataSet->active = true;
      outputDataSet->id = 4;
      outputDataSet->dataSeriesId = outputDataSeries->id;
      outputDataSet->format.emplace("table_name", "occurrence_analysis_result");

      outputDataSeries->datasetList.emplace_back(outputDataSet);


      dataManager->add(outputDataSeriesPtr);


      Analysis* analysis = new Analysis;
      AnalysisPtr analysisPtr(analysis);

      analysis->id = 2;
      analysis->name = "Analysis";
      analysis->active = true;

      std::string script = "x = occurrences.count(\"Occurrence\", \"500d\")\n"
          "add_value(\"count\", x)\n";


      analysis->script = script;
      analysis->outputDataSeriesId = outputDataSeries->id;
      analysis->scriptLanguage = ScriptLanguage::PYTHON;
      analysis->type = AnalysisType::MONITORED_OBJECT_TYPE;
      analysis->serviceInstanceId = 1;

      terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
      std::shared_ptr<const terrama2::core::DataProvider> dataProviderPtr(dataProvider);
      dataProvider->name = "Provider";
      dataProvider->uri += TERRAMA2_DATA_DIR;
      dataProvider->uri += "/shapefile";
      dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
      dataProvider->dataProviderType = "FILE";
      dataProvider->active = true;
      dataProvider->id = 5;

      dataManager->add(dataProviderPtr);


      terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
      dataSeries->dataProviderId = dataProvider->id;
      dataSeries->semantics = semanticsManager.getSemantics("STATIC_DATA-ogr");
      dataSeries->semantics.dataSeriesType = terrama2::core::DataSeriesType::GEOMETRIC_OBJECT;
      dataSeries->name = "Monitored Object";
      dataSeries->id = 5;
      dataSeries->dataProviderId = dataProvider->id;

      //DataSet information
      terrama2::core::DataSet* dataSet = new terrama2::core::DataSet;
      terrama2::core::DataSetPtr dataSetPtr(dataSet);
      dataSet->active = true;
      dataSet->format.emplace("mask", "estados_2010.shp");
      dataSet->format.emplace("srid", "4326");
      dataSet->id = 5;
      dataSet->dataSeriesId = dataSeries->id;

      dataSeries->datasetList.push_back(dataSetPtr);
      dataManager->add(dataSeriesPtr);

      AnalysisDataSeries monitoredObjectADS;
      monitoredObjectADS.id = 5;
      monitoredObjectADS.dataSeriesId = dataSeriesPtr->id;
      monitoredObjectADS.type = AnalysisDataSeriesType::DATASERIES_MONITORED_OBJECT_TYPE;
      monitoredObjectADS.metadata["identifier"] = "nome";


      //DataProvider information
      terrama2::core::DataProvider* dataProvider2 = new terrama2::core::DataProvider();
      terrama2::core::DataProviderPtr dataProvider2Ptr(dataProvider2);
      dataProvider2->id = 6;
      dataProvider2->name = "DataProvider queimadas postgis";
      dataProvider2->uri = uri.uri();
      dataProvider2->intent = terrama2::core::DataProviderIntent::PROCESS_INTENT;
      dataProvider2->dataProviderType = "POSTGIS";
      dataProvider2->active = true;

      dataManager->add(dataProvider2Ptr);

      //DataSeries information
      terrama2::core::DataSeries* occurrenceDataSeries = new terrama2::core::DataSeries();
      terrama2::core::DataSeriesPtr occurrenceDataSeriesPtr(occurrenceDataSeries);
      occurrenceDataSeries->id = 6;
      occurrenceDataSeries->name = "Occurrence";

      occurrenceDataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-postgis");

      occurrenceDataSeries->dataProviderId = dataProvider2Ptr->id;


      //DataSet information
      terrama2::core::DataSetOccurrence* occurrenceDataSet = new terrama2::core::DataSetOccurrence();
      occurrenceDataSet->active = true;
      occurrenceDataSet->id = 6;
      occurrenceDataSet->dataSeriesId = occurrenceDataSeries->id;
      occurrenceDataSet->format.emplace("table_name", "queimadas_test_table");
      occurrenceDataSet->format.emplace("timestamp_property", "data_pas");
      occurrenceDataSet->format.emplace("geometry_property", "geom");
      occurrenceDataSet->format.emplace("timezone", "UTC-03");

      occurrenceDataSeries->datasetList.emplace_back(occurrenceDataSet);

      dataManager->add(occurrenceDataSeriesPtr);

      AnalysisDataSeries occurrenceADS;
      occurrenceADS.id = 6;
      occurrenceADS.dataSeriesId = occurrenceDataSeriesPtr->id;
      occurrenceADS.type = AnalysisDataSeriesType::ADDITIONAL_DATA_TYPE;
      occurrenceADS.alias = "occ";

      std::vector<AnalysisDataSeries> analysisDataSeriesList;
      analysisDataSeriesList.push_back(monitoredObjectADS);
      analysisDataSeriesList.push_back(occurrenceADS);

      analysis->analysisDataSeriesList = analysisDataSeriesList;

      analysis->schedule.id = 2;
      analysis->schedule.frequency = 30;
      analysis->schedule.frequencyUnit = "sec";

      dataManager->add(analysisPtr);

      terrama2::services::analysis::core::AnalysisExecutor executor;
      auto result = executor.validateAnalysis(dataManager, analysisPtr);

      std::cout << "Validate result for monitored object analysis: " <<  (result.valid ? "OK" : "Not OK") << std::endl;
      for(auto message : result.messages)
      {
        std::cout << message << std::endl;
      }

    }


    app.exec();

  }



  return 0;
}
