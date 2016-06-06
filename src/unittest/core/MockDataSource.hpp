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
  \file unittest/core/MockDataSource.hpp

  \brief Mock for TerraLib Data Source

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCE_HPP__
#define __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCE_HPP__

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceCapabilities.h>
#include <terralib/dataaccess/query/Select.h>
#include <terralib/dataaccess/dataset/ObjectIdSet.h>

// GMock
#include <gmock/gmock.h>

// TerraMA2
#include "MockDataSourceTransactor.hpp"


typedef std::map<std::string, std::string> stringMapReturn;

namespace te
{
  namespace da
  {

    class MockDataSource : public DataSource
    {
    public:

      // std::auto_ptr proxies
      MOCK_METHOD1(DataSourceTransactoPtrParam, void(DataSourceTransactor*));
//      MOCK_METHOD0(DataSourceTransactoPtrReturn, DataSourceTransactor* ());
      MOCK_METHOD0(DataSourceTransactoPtrReturn, MockDataSourceTransactor* ());

      virtual std::auto_ptr<DataSourceTransactor> getTransactor() override
      {
        return std::auto_ptr<DataSourceTransactor>(DataSourceTransactoPtrReturn());
//        return std::auto_ptr<DataSourceTransactor>(new MockDataSourceTransactor());
      }

//      MOCK_METHOD0(DataSetPtrReturn, DataSet*());
      MOCK_METHOD0(DataSetPtrReturn, MockDataSet*());

      virtual std::auto_ptr<DataSet> getDataSet(const std::string& name,
                                                te::common::TraverseType travType = te::common::FORWARDONLY,
                                                const te::common::AccessPolicy accessPolicy = te::common::RAccess) override
      {
        return std::auto_ptr< DataSet >(DataSetPtrReturn());
      }

      std::auto_ptr<DataSet> getDataSet(const std::string& name,
                                        const std::string& propertyName,
                                        const te::gm::Envelope* e,
                                        te::gm::SpatialRelation r,
                                        te::common::TraverseType travType = te::common::FORWARDONLY,
                                        const te::common::AccessPolicy accessPolicy = te::common::RAccess) override
      {
        return std::auto_ptr< DataSet >(DataSetPtrReturn());
      }

      virtual std::auto_ptr<DataSet> getDataSet(const std::string& name,
                                                const std::string& propertyName,
                                                const te::gm::Geometry* g,
                                                te::gm::SpatialRelation r,
                                                te::common::TraverseType travType = te::common::FORWARDONLY,
                                                const te::common::AccessPolicy accessPolicy = te::common::RAccess) override
      {
        return std::auto_ptr< DataSet >(DataSetPtrReturn());
      }

      virtual std::auto_ptr<DataSet> query(const Select& q,
                                           te::common::TraverseType travType = te::common::FORWARDONLY,
                                           const te::common::AccessPolicy accessPolicy = te::common::RAccess) override
      {
        return std::auto_ptr< DataSet >(DataSetPtrReturn());
      }

      virtual std::auto_ptr<DataSet> query(const std::string& query,
                                           te::common::TraverseType travType = te::common::FORWARDONLY,
                                           const te::common::AccessPolicy accessPolicy = te::common::RAccess) override
      {
        return std::auto_ptr< DataSet >(DataSetPtrReturn());
      }

      MOCK_METHOD0(DataSetTypePtrReturn, DataSetType*());

      virtual std::auto_ptr<te::da::DataSetType> getDataSetType(const std::string& name) override
      {
        return std::auto_ptr<DataSetType>(DataSetTypePtrReturn());
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
        return std::auto_ptr<te::da::PrimaryKey>(PrimaryKeyPtrReturn());
      }

      MOCK_METHOD0(ForeignKeyPtrReturn, ForeignKey*());

      virtual std::auto_ptr<ForeignKey> getForeignKey(const std::string& datasetName, const std::string& name) override
      {
        return std::auto_ptr<ForeignKey>(ForeignKeyPtrReturn());
      }

      MOCK_METHOD0(UniqueKeyPtrReturn, te::da::UniqueKey*());

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



      //      MOCK_METHOD0(getTransactor,
      //                   std::auto_ptr<DataSourceTransactor>());
      //      MOCK_METHOD3(getDataSet,
      //                   std::auto_ptr<DataSet>(std::string, te::common::TraverseType, te::common::AccessPolicy));
      //      MOCK_METHOD6(getDataSet,
      //                   std::auto_ptr<DataSet>(std::string, std::string, te::gm::Envelope, te::gm::SpatialRelation, te::common::TraverseType, te::common::AccessPolicy));
      //      MOCK_METHOD6(getDataSet,
      //                   std::auto_ptr<DataSet>(std::string, std::string, te::gm::Geometry*, te::gm::SpatialRelation, te::common::TraverseType, te::common::AccessPolicy));
      //      MOCK_METHOD3(query,
      //                   std::auto_ptr<DataSet>(Select, te::common::TraverseType, te::common::AccessPolicy));
      //      MOCK_METHOD3(query,
      //                   std::auto_ptr<DataSet>(std::string, te::common::TraverseType, te::common::AccessPolicy));
      //      MOCK_METHOD1(getDataSetType,
      //                   std::auto_ptr<te::da::DataSetType>(const std::string& name));
      //      MOCK_METHOD2(getProperty,
      //                   std::auto_ptr<te::dt::Property>(const std::string& datasetName, const std::string& name));
      //      MOCK_METHOD2(getProperty,
      //                   std::auto_ptr<te::dt::Property>(const std::string& datasetName, std::size_t propertyPos));
      //      MOCK_METHOD1(getPrimaryKey,
      //                   std::auto_ptr<te::da::PrimaryKey>(const std::string& datasetName));
      //      MOCK_METHOD2(getForeignKey,
      //                   std::auto_ptr<ForeignKey>(const std::string& datasetName, const std::string& name));
      //      MOCK_METHOD2(getUniqueKey,
      //                   std::auto_ptr<te::da::UniqueKey>(const std::string& datasetName, const std::string& name));
      //      MOCK_METHOD2(getCheckConstraint,
      //                   std::auto_ptr<te::da::CheckConstraint>(const std::string& datasetName, const std::string& name));
      //      MOCK_METHOD2(getIndex,
      //                   std::auto_ptr<te::da::Index>(const std::string& datasetName, const std::string& name));
      //      MOCK_METHOD1(getSequence,
      //                   std::auto_ptr<Sequence>(const std::string& name));
      //      MOCK_METHOD2(getExtent,
      //                   std::auto_ptr<te::gm::Envelope>(const std::string& datasetName, const std::string& propertyName));
      //      MOCK_METHOD2(getExtent,
      //                   std::auto_ptr<te::gm::Envelope>(const std::string& datasetName, std::size_t propertyPos));


      MOCK_CONST_METHOD0(getType,
                         std::string());
      // The following line won't really compile, as the return
      // type has multiple template arguments.  To fix it, use a
      // typedef for the return type.
      MOCK_CONST_METHOD0(getConnectionInfo,
                         const stringMapReturn&());
      MOCK_METHOD1(setConnectionInfo,
                   void(const stringMapReturn& connInfo));
      MOCK_METHOD0(open,
                   void());
      MOCK_METHOD0(close,
                   void());
      MOCK_CONST_METHOD0(isOpened,
                         bool());
      MOCK_CONST_METHOD0(isValid,
                         bool());
      MOCK_CONST_METHOD0(getCapabilities,
                         const DataSourceCapabilities&());
      MOCK_METHOD1(getProperties,
                   boost::ptr_vector<te::dt::Property>(const std::string& datasetName));
      MOCK_CONST_METHOD0(getDialect,
                         const SQLDialect*());
      MOCK_METHOD1(execute,
                   void(const Query& command));
      MOCK_METHOD1(execute,
                   void(const std::string& command));
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
      MOCK_METHOD3(changePropertiesDefinitions,
                   void(const std::string& datasetName, const std::vector<std::string>& propsNames, const std::vector<te::dt::Property*> newProps));
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
      MOCK_METHOD2(createDataSet,
                   void(DataSetType* dt, const std::map<std::string, std::string>& options));
      MOCK_METHOD3(cloneDataSet,
                   void(const std::string& name, const std::string& cloneName, const std::map<std::string, std::string>& options));
      MOCK_METHOD1(dropDataSet,
                   void(const std::string& name));
      MOCK_METHOD2(renameDataSet,
                   void(const std::string& name, const std::string& newName));
      MOCK_METHOD4(add,
                   void(std::string, DataSet*, std::map<std::string, std::string>, std::size_t));
      MOCK_METHOD0(getEncoding,
                   te::core::EncodingType());
      MOCK_METHOD2(remove,
                   void(std::string, te::da::ObjectIdSet));
      MOCK_METHOD6(update,
                   void(std::string, DataSet*, const std::vector<std::size_t>& properties, te::da::ObjectIdSet, std::map<std::string, std::string>& options, std::size_t));
      MOCK_METHOD4(update,
                   void(const std::string& datasetName, DataSet* dataset, const std::vector< std::set<int> >& properties, const std::vector<size_t>& ids));
      MOCK_METHOD1(create,
                   void(const std::map<std::string, std::string>& dsInfo));
      MOCK_METHOD1(drop,
                   void(const std::map<std::string, std::string>& dsInfo));
      MOCK_METHOD1(exists,
                   bool(const std::map<std::string, std::string>& dsInfo));
      MOCK_METHOD1(getDataSourceNames,
                   std::vector<std::string>(const std::map<std::string, std::string>& dsInfo));
      MOCK_METHOD1(getEncodings,
                   std::vector<te::core::EncodingType>(const std::map<std::string, std::string>& dsInfo));
    };

  }  // namespace da
}  // namespace te

#endif // __TERRAMA2_UNITTEST_CORE_MOCKDATASOURCE_HPP__
