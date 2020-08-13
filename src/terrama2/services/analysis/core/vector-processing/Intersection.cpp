#include "Intersection.hpp"
#include "../../../../core/utility/Utils.hpp"
#include "../../../../core/utility/Raii.hpp"

#include "../Analysis.hpp"

//Boost
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

terrama2::services::analysis::core::vp::Intersection::Intersection(terrama2::services::analysis::core::AnalysisPtr analysis,
                                                                   terrama2::core::DataSeriesPtr monitoredDataSeries,
                                                                   terrama2::core::DataSeriesPtr dynamicDataSeries,
                                                                   std::vector<terrama2::core::DataSeriesPtr> additionalDataSeries,
                                                                   te::core::URI outputDataProviderURI, std::string outputTableName)
  : Operator::Operator(analysis, monitoredDataSeries, dynamicDataSeries,
                       additionalDataSeries, outputDataProviderURI, outputTableName)
{

}

terrama2::services::analysis::core::vp::Intersection::~Intersection()
{
}

double terrama2::services::analysis::core::vp::Intersection::getLastId()
{
  auto transactor = dataSource_->getTransactor();
  std::string dynamicLayerTableName = terrama2::core::getTableNameProperty(dynamicDataSeries_->datasetList[0]);

  //VARIFY PK FROM TABLE
  std::string columnPk = "";

  std::string queryColumnPkStr = "SELECT kcu.column_name FROM INFORMATION_SCHEMA.TABLES t ";
  queryColumnPkStr = queryColumnPkStr + "LEFT JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS tc ";
  queryColumnPkStr = queryColumnPkStr + "ON tc.table_catalog = t.table_catalog ";
  queryColumnPkStr = queryColumnPkStr + "AND tc.table_schema = t.table_schema ";
  queryColumnPkStr = queryColumnPkStr + "AND tc.table_name = t.table_name ";
  queryColumnPkStr = queryColumnPkStr + "AND tc.constraint_type = 'PRIMARY KEY' ";
  queryColumnPkStr = queryColumnPkStr + "LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE kcu ";
  queryColumnPkStr = queryColumnPkStr + "ON kcu.table_catalog = tc.table_catalog ";
  queryColumnPkStr = queryColumnPkStr + "AND kcu.table_schema = tc.table_schema ";
  queryColumnPkStr = queryColumnPkStr + "AND kcu.table_name = tc.table_name ";
  queryColumnPkStr = queryColumnPkStr + "AND kcu.constraint_name = tc.constraint_name ";
  queryColumnPkStr = queryColumnPkStr + "WHERE   t.table_schema NOT IN ('pg_catalog', 'information_schema') and t.table_name = '"+dynamicLayerTableName+"'";

  std::cout << queryColumnPkStr << std::endl;

  boost::format queryColumnPk(queryColumnPkStr);

  std::shared_ptr<te::da::DataSet> tempDataSetColumnPk(transactor->query(queryColumnPk.str()));

  if(!tempDataSetColumnPk)
  {
    QString errMsg = QObject::tr("Can not find log message table name!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  if(!tempDataSetColumnPk->moveFirst())
  {
      QString errMsg = QObject::tr("Can not find column PK!");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }
  else
  {
    tempDataSetColumnPk->moveFirst();
    columnPk = tempDataSetColumnPk->getString("column_name");
  }

  double quantityRealtable;

  //RETRIEVE ID FROM LAST DATA INSERTED
  boost::format queryRealTable("SELECT "+columnPk+" FROM (SELECT * FROM public." + dynamicLayerTableName + " ORDER BY " + columnPk + " DESC LIMIT 1) AS last_data");

  std::shared_ptr<te::da::DataSet> tempDataSetRealtable(transactor->query(queryRealTable.str()));

  if(!tempDataSetRealtable)
  {
    QString errMsg = QObject::tr("Can not find log message table name!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  if(!tempDataSetRealtable->moveFirst())
  {
    //CASE TABLE IS EMPTY
    quantityRealtable = 0;
  }
  else
  {
    tempDataSetRealtable->moveFirst();
    quantityRealtable = tempDataSetRealtable->getInt32(columnPk);
  }

  return quantityRealtable;
}

void terrama2::services::analysis::core::vp::Intersection::execute()
{
  auto transactor = dataSource_->getTransactor();

  std::string staticLayerTableName = terrama2::core::getTableNameProperty(monitoredDataSeries_->datasetList[0]);
  std::string dynamicLayerTableName = terrama2::core::getTableNameProperty(dynamicDataSeries_->datasetList[0]);

  //Attributes handler
  std::string attributes = analysis_->metadata.find("outputlayer")->second;
  std::replace(attributes.begin(), attributes.end(), '{', ' ');
  std::replace(attributes.begin(), attributes.end(), '}', ' ');
  
  QRegExp reg("(\\,)");
  auto parts = QString(attributes.c_str()).split(reg);

  std::string attributeFilter("");
  reg = QRegExp("(AS)");

  for(const auto& part: parts)
  {
    auto fieldParts = part.split(reg);
    attributeFilter += fieldParts[1].replace("\"", "").toStdString() + ",";
  }

  attributeFilter.pop_back();

  std::cout << QString(attributes.c_str()).split(reg).join(" ").toStdString() << std::endl;

  //Date filter handler
  if((analysis_->metadata.find("startDate") != analysis_->metadata.end()) &&
     (analysis_->metadata.find("endDate") != analysis_->metadata.end()))
  {
    std::string startDate = analysis_->metadata.find("startDate")->second;
    std::string endDate = analysis_->metadata.find("endDate")->second;

    setWhereCondition(startDate+ ";" +endDate);
  }

  //Class attributes handler
  std::string finalWhereCondition = "";

  if((analysis_->metadata.find("classColumnSelected") != analysis_->metadata.end()) &&
     (analysis_->metadata.find("classNameSelected") != analysis_->metadata.end()))
  {
    std::string classColumnSelected = analysis_->metadata.find("classColumnSelected")->second;
    QString splitClassColumnSelected = QString::fromStdString(classColumnSelected);
    splitClassColumnSelected = splitClassColumnSelected.split(":")[1];

    std::string classNameSelected = analysis_->metadata.find("classNameSelected")->second;
    std::cout << classNameSelected << std::endl;
    std::replace(classNameSelected.begin(), classNameSelected.end(), '{', ' ');
    std::replace(classNameSelected.begin(), classNameSelected.end(), '}', ' ');

    QString stringCast = QString::fromStdString(classNameSelected);
    QStringList classAttributes = stringCast.split(",");

    for(auto attribute : classAttributes)
    {
      std::string condition = dynamicLayerTableName + "." + splitClassColumnSelected.toUtf8().data() + " = " + "''" + attribute.toUtf8().data() + "''" + " OR ";
      finalWhereCondition += condition;
    }

    finalWhereCondition = finalWhereCondition.substr(0, finalWhereCondition.length() - 3);
  }
  else
    finalWhereCondition = "1 = 1";
  finalWhereCondition = analysis_->script;

  if(analysis_->script.empty()){
      finalWhereCondition = "1 = 1";
  }

  std::string sql = "SELECT table_name, affected_rows::double precision FROM vectorial_processing_intersection("+ std::to_string(analysis_->id) +", '" +
                     outputTableName_ + "', '" + staticLayerTableName + "', '" + dynamicLayerTableName + "', '" + attributes + "', '" + whereCondition_ +
                     "', E'" + finalWhereCondition + "', '" + attributeFilter + "'" + ")";
  std::cout << sql << std::endl;

  resultDataSet_ = transactor->query(sql);
}
