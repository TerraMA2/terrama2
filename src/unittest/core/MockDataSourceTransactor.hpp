/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file unittest/core/MockDataSourceTransactor.hpp

  \brief Mock for TerraLib Data Source Transactor

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCETRANSACTOR_HPP__
#define __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCETRANSACTOR_HPP__

// GMock
#include <gmock/gmock.h>

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/query/Select.h>
#include <terralib/dataaccess/dataset/ObjectIdSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/geometry/Geometry.h>

namespace te
{
  namespace da
  {

    class MockDataSourceTransactor : public DataSourceTransactor
    {
    public:
      MOCK_CONST_METHOD0(getDataSource,
                         DataSource*());
      MOCK_METHOD0(begin,
                   void());
      MOCK_METHOD0(commit,
                   void());
      MOCK_METHOD0(rollBack,
                   void());
      MOCK_CONST_METHOD0(isInTransaction,
                         bool());
      MOCK_METHOD4(getDataSet,
                   std::auto_ptr<DataSet>(const std::string&,
                                          te::common::TraverseType,
                                          bool,
                                          te::common::AccessPolicy));
      MOCK_METHOD7(getDataSet,
                   std::auto_ptr<DataSet>(const std::string&,
                                          const std::string&,
                                          const te::gm::Envelope*,
                                          te::gm::SpatialRelation,
                                          te::common::TraverseType,
                                          bool,
                                          const te::common::AccessPolicy));
      MOCK_METHOD7(getDataSet,
                   std::auto_ptr<DataSet>(const std::string&,
                                          const std::string&,
                                          const te::gm::Geometry*,
                                          te::gm::SpatialRelation,
                                          te::common::TraverseType,
                                          bool,
                                          const te::common::AccessPolicy));
      MOCK_METHOD4(query,
                   std::auto_ptr<DataSet>(const Select&,
                                          te::common::TraverseType,
                                          bool,
                                          const te::common::AccessPolicy));
      MOCK_METHOD4(query,
                   std::auto_ptr<DataSet>(const std::string&,
                                          te::common::TraverseType,
                                          bool,
                                          const te::common::AccessPolicy));
      MOCK_METHOD1(execute,
                   void(const Query& command));
      MOCK_METHOD1(execute,
                   void(const std::string& command));
      MOCK_METHOD1(getPrepared,
                   std::auto_ptr<PreparedQuery>(const std::string&));
      MOCK_METHOD0(getBatchExecutor,
                   std::auto_ptr<BatchExecutor>());
      MOCK_METHOD0(cancel,
                   void());
      MOCK_METHOD0(getLastGeneratedId,
                   boost::int64_t());
      MOCK_METHOD1(escape,
                   std::string(const std::string& value));
      MOCK_METHOD1(isDataSetNameValid,
                   bool(const std::string& datasetName));
      MOCK_METHOD1(isPropertyNameValid,
                   bool(const std::string& propertyName));
      MOCK_METHOD0(getDataSetNames,
                   std::vector<std::string>());
      MOCK_METHOD0(getNumberOfDataSets,
                   std::size_t());
      MOCK_METHOD1(getDataSetType,
                   std::auto_ptr<te::da::DataSetType>(const std::string& name));
      MOCK_METHOD1(getCapabilities,
                   std::auto_ptr<te::da::DataSetTypeCapabilities>(const std::string& name));
      MOCK_METHOD1(getProperties,
                   boost::ptr_vector<te::dt::Property>(const std::string& datasetName));
      MOCK_METHOD2(getProperty,
                   std::auto_ptr<te::dt::Property>(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(getProperty,
                   std::auto_ptr<te::dt::Property>(const std::string& datasetName, std::size_t propertyPos));
      MOCK_METHOD1(getPropertyNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD1(getNumberOfProperties,
                   std::size_t(const std::string& datasetName));
      MOCK_METHOD2(propertyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addProperty,
                   void(const std::string& datasetName, te::dt::Property* p));
      MOCK_METHOD2(dropProperty,
                   void(const std::string& datasetName, const std::string& name));
      MOCK_METHOD3(renameProperty,
                   void(const std::string& datasetName, const std::string& propertyName, const std::string& newPropertyName));
      MOCK_METHOD3(changePropertyDefinition,
                   void(const std::string& datasetName, const std::string& propName, te::dt::Property* newProp));
      MOCK_METHOD1(getPrimaryKey,
                   std::auto_ptr<te::da::PrimaryKey>(const std::string& datasetName));
      MOCK_METHOD2(primaryKeyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addPrimaryKey,
                   void(const std::string& datasetName, PrimaryKey* pk));
      MOCK_METHOD1(dropPrimaryKey,
                   void(const std::string& datasetName));
      MOCK_METHOD2(getForeignKey,
                   std::auto_ptr<ForeignKey>(const std::string& datasetName, const std::string& name));
      MOCK_METHOD1(getForeignKeyNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(foreignKeyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addForeignKey,
                   void(const std::string& datasetName, ForeignKey* fk));
      MOCK_METHOD2(dropForeignKey,
                   void(const std::string& datasetName, const std::string& fkName));
      MOCK_METHOD2(getUniqueKey,
                   std::auto_ptr<te::da::UniqueKey>(const std::string& datasetName, const std::string& name));
      MOCK_METHOD1(getUniqueKeyNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(uniqueKeyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addUniqueKey,
                   void(const std::string& datasetName, UniqueKey* uk));
      MOCK_METHOD2(dropUniqueKey,
                   void(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(getCheckConstraint,
                   std::auto_ptr<te::da::CheckConstraint>(const std::string& datasetName, const std::string& name));
      MOCK_METHOD1(getCheckConstraintNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(checkConstraintExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addCheckConstraint,
                   void(const std::string& datasetName, CheckConstraint* cc));
      MOCK_METHOD2(dropCheckConstraint,
                   void(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(getIndex,
                   std::auto_ptr<te::da::Index>(const std::string& datasetName, const std::string& name));
      MOCK_METHOD1(getIndexNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(indexExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD3(addIndex,
                   void(const std::string& datasetName, Index* idx, const std::map<std::string, std::string>& options));
      MOCK_METHOD2(dropIndex,
                   void(const std::string& datasetName, const std::string& idxName));
      MOCK_METHOD1(getSequence,
                   std::auto_ptr<Sequence>(const std::string& name));
      MOCK_METHOD0(getSequenceNames,
                   std::vector<std::string>());
      MOCK_METHOD1(sequenceExists,
                   bool(const std::string& name));
      MOCK_METHOD1(addSequence,
                   void(Sequence* sequence));
      MOCK_METHOD1(dropSequence,
                   void(const std::string& name));
      MOCK_METHOD2(getExtent,
                   std::auto_ptr<te::gm::Envelope>(const std::string& datasetName, const std::string& propertyName));
      MOCK_METHOD2(getExtent,
                   std::auto_ptr<te::gm::Envelope>(const std::string& datasetName, std::size_t propertyPos));
      MOCK_METHOD1(getNumberOfItems,
                   std::size_t(const std::string& datasetName));
      MOCK_METHOD0(hasDataSets,
                   bool());
      MOCK_METHOD1(dataSetExists,
                   bool(const std::string& name));
      MOCK_METHOD0(getEncoding,
                   te::core::EncodingType());
      MOCK_METHOD2(createDataSet,
                   void(DataSetType* dt, const std::map<std::string, std::string>& options));
      MOCK_METHOD3(cloneDataSet,
                   void(const std::string& name, const std::string& cloneName, const std::map<std::string, std::string>& options));
      MOCK_METHOD1(dropDataSet,
                   void(const std::string& name));
      MOCK_METHOD2(renameDataSet,
                   void(const std::string& name, const std::string& newName));
      MOCK_METHOD4(add,
                   void(const std::string&,
                        DataSet*,
                        const std::map<std::string, std::string>&,
                        std::size_t));
      MOCK_METHOD2(remove,
                   void(const std::string&,
                        const ObjectIdSet*));
      MOCK_METHOD6(update,
                   void(const std::string&,
                        DataSet*,
                        const std::vector<std::size_t>& properties,
                        const ObjectIdSet*,
                        const std::map<std::string, std::string>&,
                        std::size_t));
      MOCK_METHOD4(update,
                   void(const std::string& datasetName, DataSet* dataset, const std::vector< std::set<int> >& properties, const std::vector<size_t>& ids));
      MOCK_METHOD1(optimize,
                   void(const std::map<std::string, std::string>& opInfo));
    };

  }  // namespace da
}  // namespace te

#endif // __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCETRANSACTOR_HPP__
