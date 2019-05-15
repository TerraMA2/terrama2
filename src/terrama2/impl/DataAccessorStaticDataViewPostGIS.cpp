#include "DataAccessorStaticDataViewPostGIS.hpp"

// TerraMA2 Core
#include "../core/Exception.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"

terrama2::core::DataAccessorStaticDataViewPostGIS::DataAccessorStaticDataViewPostGIS(terrama2::core::DataProviderPtr dataProvider,
                                                                                     terrama2::core::DataSeriesPtr dataSeries,
                                                                                     const bool checkSemantics)
  : DataAccessor (dataProvider, dataSeries),
    DataAccessorGeometricObject (dataProvider, dataSeries),
    DataAccessorPostGIS (dataProvider, dataSeries)
{
  if(checkSemantics && dataSeries->semantics.driver != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException()  << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorStaticDataViewPostGIS::getDataSetTableName(terrama2::core::DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("view_name");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined View name in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorStaticDataViewPostGIS::dataSourceType() const
{
  return "POSTGIS";
}

//std::string
//terrama2::core::DataAccessorStaticDataViewPostGIS::whereConditions(terrama2::core::DataSetPtr dataSet,
//                                                                   const std::string/*datetimeColumnName*/,
//                                                                   const terrama2::core::Filter&/*filter*/) const
//{
//  std::string queryBuilder;

//  try
//  {
//    queryBuilder = dataSet->format.at("query_builder");
//  }
//  catch(...)
//  {
//  }

//  return queryBuilder;
//}
