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

  \brief Mock for TerraLib Data Source Transactor class

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
#include <terralib/dataaccess/dataset/DataSetTypeCapabilities.h>
#include <terralib/dataaccess/datasource/BatchExecutor.h>
#include <terralib/dataaccess/datasource/PreparedQuery.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/geometry/Geometry.h>

// TerraMA2
#include "MockDataSet.hpp"

// pragma to silence auto_ptr warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace te
{
  namespace da
  {

    /*!
     * \brief The MockDataSourceTransactor class has the mocked methods for unittests,
     * using GMock.
     *
     * Some methods in in this class returns a std::auto_ptr, what isn't acceptable
     * by GMock to use in mockeds methods. To solve this, was created some mocked proxies
     * that returns a pointer, used to create the auto_ptr required in method.
     *
     */
    class MockDataSourceTransactor : public DataSourceTransactor
    {
    public:

      /*!
       * \brief Class constructor
      */
      MockDataSourceTransactor() = default;

      /*!
       * \brief Class destructor
      */
      virtual ~MockDataSourceTransactor() = default;


      // Methods that use std::auto_ptr and need proxies:

      // Mocked proxy
      MOCK_METHOD0(DataSetPtrReturn, DataSet*());

      // Overridden method that use the mocked proxy
      virtual std::auto_ptr<DataSet> getDataSet(const std::string&,
                                                te::common::TraverseType,
                                                bool,
                                                te::common::AccessPolicy) override
      {
        return std::auto_ptr<DataSet>(DataSetPtrReturn());
      }


      virtual std::auto_ptr<DataSet> getDataSet(const std::string&,
                                                const std::string&,
                                                const te::gm::Envelope*,
                                                te::gm::SpatialRelation,
                                                te::common::TraverseType,
                                                bool,
                                                const te::common::AccessPolicy) override
      {
        return std::auto_ptr<DataSet>(DataSetPtrReturn());
      }

      virtual std::auto_ptr<DataSet> getDataSet(const std::string&,
                                                const std::string&,
                                                const te::gm::Geometry*,
                                                te::gm::SpatialRelation,
                                                te::common::TraverseType,
                                                bool,
                                                const te::common::AccessPolicy) override
      {
        return std::auto_ptr<DataSet>(DataSetPtrReturn());
      }


      virtual std::auto_ptr<DataSet> query(const Select&,
                                           te::common::TraverseType,
                                           bool,
                                           const te::common::AccessPolicy) override
      {
        return std::auto_ptr<DataSet>(DataSetPtrReturn());
      }

      virtual std::auto_ptr<DataSet> query(const std::string&,
                                           te::common::TraverseType,
                                           bool,
                                           const te::common::AccessPolicy) override
      {
        return std::auto_ptr<DataSet>(DataSetPtrReturn());
      }

      MOCK_METHOD0(PreparedQueryPtrReturn, PreparedQuery*());

      virtual std::auto_ptr<PreparedQuery> getPrepared(const std::string&) override
      {
        return std::auto_ptr<PreparedQuery>(PreparedQueryPtrReturn());
      }

      MOCK_METHOD0(BatchExecutorPtrReturn, BatchExecutor*());

      virtual std::auto_ptr<BatchExecutor> getBatchExecutor() override
      {
        return std::auto_ptr<BatchExecutor>(BatchExecutorPtrReturn());
      }

      MOCK_METHOD0(DataSetTypePtrReturn, DataSetType*());

      virtual std::auto_ptr<te::da::DataSetType> getDataSetType(const std::string& name) override
      {
        return std::auto_ptr<te::da::DataSetType>(DataSetTypePtrReturn());
      }

      MOCK_METHOD0(CapabilitiesPtrReturn, DataSetTypeCapabilities*());

      virtual std::auto_ptr<te::da::DataSetTypeCapabilities> getCapabilities(const std::string& name) override
      {
        return std::auto_ptr<te::da::DataSetTypeCapabilities>(CapabilitiesPtrReturn());
      }

      MOCK_METHOD0(PropertyPtrReturn, te::dt::Property*());

      virtual std::auto_ptr<te::dt::Property> getProperty(const std::string& datasetName, const std::string& name) override
      {
        return std::auto_ptr<te::dt::Property>(PropertyPtrReturn());
      }

      virtual std::auto_ptr<te::dt::Property> getProperty(const std::string& datasetName, std::size_t propertyPos) override
      {
        return std::auto_ptr<te::dt::Property>(PropertyPtrReturn());
      }

      MOCK_METHOD0(PrimaryKeyPtrReturn, PrimaryKey*());

      virtual std::auto_ptr<te::da::PrimaryKey> getPrimaryKey(const std::string& datasetName) override
      {
        return std::auto_ptr<PrimaryKey>(PrimaryKeyPtrReturn());
      }

      MOCK_METHOD0(ForeignKeyPtrReturn, ForeignKey*());

      virtual std::auto_ptr<ForeignKey> getForeignKey(const std::string& datasetName, const std::string& name) override
      {
        return std::auto_ptr<ForeignKey>(ForeignKeyPtrReturn());
      }

      MOCK_METHOD0(UniqueKeyPtrReturn, UniqueKey*());

      virtual std::auto_ptr<te::da::UniqueKey> getUniqueKey(const std::string& datasetName, const std::string& name) override
      {
        return std::auto_ptr<UniqueKey>(UniqueKeyPtrReturn());
      }

      MOCK_METHOD0(CheckConstraintPtrReturn, CheckConstraint*());

      virtual std::auto_ptr<te::da::CheckConstraint> getCheckConstraint(const std::string& datasetName, const std::string& name) override
      {
        return std::auto_ptr<CheckConstraint>(CheckConstraintPtrReturn());
      }

      MOCK_METHOD0(IndexPtrReturn, Index*());

      virtual std::auto_ptr<te::da::Index> getIndex(const std::string& datasetName, const std::string& name) override
      {
        return std::auto_ptr<Index>(IndexPtrReturn());
      }

      MOCK_METHOD0(SequencePtrReturn, Sequence*());

      virtual std::auto_ptr<Sequence> getSequence(const std::string& name) override
      {
        return std::auto_ptr<Sequence>(SequencePtrReturn());
      }

      MOCK_METHOD0(EnvelopePtrReturn, te::gm::Envelope*());

      virtual std::auto_ptr<te::gm::Envelope> getExtent(const std::string& datasetName, const std::string& propertyName) override
      {
        return std::auto_ptr<te::gm::Envelope>(EnvelopePtrReturn());
      }

      virtual std::auto_ptr<te::gm::Envelope> getExtent(const std::string& datasetName, std::size_t propertyPos) override
      {
        return std::auto_ptr<te::gm::Envelope>(EnvelopePtrReturn());
      }

      // Normal mocked methods:

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

      MOCK_METHOD1(execute,
                   void(const Query& command));
      MOCK_METHOD1(execute,
                   void(const std::string& command));

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

      MOCK_METHOD1(getProperties,
                   boost::ptr_vector<te::dt::Property>(const std::string& datasetName));

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

      MOCK_METHOD2(primaryKeyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addPrimaryKey,
                   void(const std::string& datasetName, PrimaryKey* pk));
      MOCK_METHOD1(dropPrimaryKey,
                   void(const std::string& datasetName));

      MOCK_METHOD1(getForeignKeyNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(foreignKeyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addForeignKey,
                   void(const std::string& datasetName, ForeignKey* fk));
      MOCK_METHOD2(dropForeignKey,
                   void(const std::string& datasetName, const std::string& fkName));

      MOCK_METHOD1(getUniqueKeyNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(uniqueKeyExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addUniqueKey,
                   void(const std::string& datasetName, UniqueKey* uk));
      MOCK_METHOD2(dropUniqueKey,
                   void(const std::string& datasetName, const std::string& name));

      MOCK_METHOD1(getCheckConstraintNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(checkConstraintExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD2(addCheckConstraint,
                   void(const std::string& datasetName, CheckConstraint* cc));
      MOCK_METHOD2(dropCheckConstraint,
                   void(const std::string& datasetName, const std::string& name));

      MOCK_METHOD1(getIndexNames,
                   std::vector<std::string>(const std::string& datasetName));
      MOCK_METHOD2(indexExists,
                   bool(const std::string& datasetName, const std::string& name));
      MOCK_METHOD3(addIndex,
                   void(const std::string& datasetName, Index* idx, const std::map<std::string, std::string>& options));
      MOCK_METHOD2(dropIndex,
                   void(const std::string& datasetName, const std::string& idxName));

      MOCK_METHOD0(getSequenceNames,
                   std::vector<std::string>());
      MOCK_METHOD1(sequenceExists,
                   bool(const std::string& name));
      MOCK_METHOD1(addSequence,
                   void(Sequence* sequence));
      MOCK_METHOD1(dropSequence,
                   void(const std::string& name));

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
      MOCK_METHOD5(add,
                   void(const std::string&,
                        DataSet*,
                        const std::map<std::string, std::string>&,
                        std::size_t,
                        bool));
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

// closing "-Wdeprecated-declarations" pragma
#pragma GCC diagnostic pop

#endif // __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCETRANSACTOR_HPP__
