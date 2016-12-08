
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetOccurrence.hpp>
#include <terrama2/impl/DataAccessorOccurrenceWfp.hpp>
#include <terrama2/core/data-access/OccurrenceSeries.hpp>
#include <terrama2/core/utility/Raii.hpp>

#include <iostream>

#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/geometry/GeometryProperty.h>
#include <terralib/dataaccess/utils/Utils.h>

int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  {
    //DataProvider information
    terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
    terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
    dataProvider->uri = "file://";
    dataProvider->uri += TERRAMA2_DATA_DIR;

    dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
    dataProvider->dataProviderType = "FILE";
    dataProvider->active = true;

    auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

    //DataSeries information
    terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
    terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
    dataSeries->semantics = semanticsManager.getSemantics("OCCURRENCE-wfp");

    terrama2::core::DataSetOccurrence* dataSet =new terrama2::core::DataSetOccurrence();
    dataSet->active = true;
    dataSet->format.emplace("mask", "exporta_%YYYY%MM%DD_%hh%mm.csv");
    dataSet->format.emplace("folder", "fire_system");
    dataSeries->datasetList.emplace_back(dataSet);

    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-05-01 08:29:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-05-01 08:31:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    //accessing data
    terrama2::core::DataAccessorOccurrenceWfp accessor(dataProviderPtr, dataSeriesPtr);
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    terrama2::core::OccurrenceSeriesPtr occurrenceSeries = accessor.getOccurrenceSeries(filter, remover);

    std::cout << "Last data timestamp: " << accessor.lastDateTime()->toString() << std::endl;

    assert(occurrenceSeries->occurrencesMap().size() == 1);

    auto teDataSet = (*occurrenceSeries->occurrencesMap().begin()).second.syncDataSet->dataset();
    auto teDataSetType = (*occurrenceSeries->occurrencesMap().begin()).second.teDataSetType;

    auto outputDataSetType = static_cast<te::da::DataSetType*>(teDataSetType->clone());
    outputDataSetType->setName("occurrence");

    te::core::URI csvUri("file:///home/jsimas/MyDevel/dpi/terrama2-build/data/csv/occurrence.csv?&DRIVER=CSV");
    std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make("OGR", csvUri));
    terrama2::core::OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination);
//    assert(datasourceDestination->isOpened());

    std::shared_ptr<te::da::DataSourceTransactor> transactorDestination(datasourceDestination->getTransactor());
    te::da::ScopedTransaction scopedTransaction(*transactorDestination);

    std::map<std::string, std::string> options;
    transactorDestination->createDataSet(outputDataSetType,options);
    teDataSet->moveBeforeFirst();
    transactorDestination->add(outputDataSetType->getName(), teDataSet.get(), options);

    scopedTransaction.commit();
  }



  return 0;
}
