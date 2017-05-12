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
  \file terrama2/core/utility/JoinDataSet.hpp

  \brief Join two te::da::DataSet

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_UTILITY_JOIN_DATASET_HPP__
#define __TERRAMA2_CORE_UTILITY_JOIN_DATASET_HPP__

// Terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief The JoinDataSet class creates a virtual te::da::DataSet that joins two DataSet over an arbitrary property.

      Every occurrence of a same value if ds1Attribute will be joined with the same line of the ds2.

      \warning If ds2Attribute isn't unique one value will be used to join, the other occurrences will be inaccessible.

    */
    class JoinDataSet : public te::da::DataSet
    {
    public:
      JoinDataSet(std::unique_ptr<te::da::DataSetType> ds1Type,
                  std::unique_ptr<te::da::DataSet> ds1,
                  const std::string& ds1Attribute,
                  std::unique_ptr<te::da::DataSetType> ds2Type,
                  std::unique_ptr<te::da::DataSet> ds2,
                  const std::string& ds2Attribute);

      virtual ~JoinDataSet() = default;
      JoinDataSet(const JoinDataSet& other) = default;
      JoinDataSet(JoinDataSet&& other) = default;
      JoinDataSet& operator=(const JoinDataSet& other) = default;
      JoinDataSet& operator=(JoinDataSet&& other) = default;

      virtual te::common::TraverseType getTraverseType() const override { return te::common::RANDOM; }
      virtual te::common::AccessPolicy getAccessPolicy() const override { return te::common::RAccess; }
      virtual std::size_t getNumProperties() const override { return properties_.size(); }
      virtual int getPropertyDataType(std::size_t i) const override { return properties_.at(i)->getType(); }
      virtual std::string getPropertyName(std::size_t i) const override { return properties_.at(i)->getName(); }
      virtual std::string getDatasetNameOfProperty(std::size_t i) const override { return properties_.at(i)->getDatasetName(); }
      virtual bool isConnected() const override { return ds1_->isConnected() || ds2_->isConnected(); }

      virtual std::size_t size() const override { return ds1_->size(); }
      virtual bool isEmpty() const override { return ds1_->isEmpty(); }
      virtual bool moveNext() override { return ds1_->moveNext(); }
      virtual bool movePrevious() override { return ds1_->movePrevious(); }
      virtual bool moveBeforeFirst() override { return ds1_->moveBeforeFirst(); }
      virtual bool moveFirst() override { return ds1_->moveFirst(); }
      virtual bool moveLast() override { return ds1_->moveLast(); }
      virtual bool move(std::size_t i) override { return ds1_->move(i); }
      virtual bool isAtBegin() const override { return ds1_->isAtBegin(); }
      virtual bool isBeforeBegin() const override { return ds1_->isBeforeBegin(); }
      virtual bool isAtEnd() const override { return ds1_->isAtEnd(); }
      virtual bool isAfterEnd() const override { return ds1_->isAfterEnd(); }

      virtual std::auto_ptr<te::gm::Envelope> getExtent(std::size_t i) override;
      virtual char getChar(std::size_t i) const override;
      virtual unsigned char getUChar(std::size_t i) const override;
      virtual boost::int16_t getInt16(std::size_t i) const override;
      virtual boost::int32_t getInt32(std::size_t i) const override;
      virtual boost::int64_t getInt64(std::size_t i) const override;
      virtual bool getBool(std::size_t i) const override;
      virtual float getFloat(std::size_t i) const override;
      virtual double getDouble(std::size_t i) const override;
      virtual std::string getNumeric(std::size_t i) const override;
      virtual std::string getString(std::size_t i) const override;
      virtual std::auto_ptr<te::dt::ByteArray> getByteArray(std::size_t i) const override;
      virtual std::auto_ptr<te::gm::Geometry> getGeometry(std::size_t i) const override;
      virtual std::auto_ptr<te::rst::Raster> getRaster(std::size_t i) const override;
      virtual std::auto_ptr<te::dt::DateTime> getDateTime(std::size_t i) const override;
      virtual std::auto_ptr<te::dt::Array> getArray(std::size_t i) const override;
      virtual bool isNull(std::size_t i) const override;

      virtual char getChar(const std::string& name) const override;
      virtual unsigned char getUChar(const std::string& name) const override;
      virtual boost::int16_t getInt16(const std::string& name) const override;
      virtual boost::int32_t getInt32(const std::string& name) const override;
      virtual boost::int64_t getInt64(const std::string& name) const override;
      virtual bool getBool(const std::string& name) const override;
      virtual float getFloat(const std::string& name) const override;
      virtual double getDouble(const std::string& name) const override;
      virtual std::string getNumeric(const std::string& name) const override;
      virtual std::string getString(const std::string& name) const override;
      virtual std::auto_ptr<te::dt::ByteArray> getByteArray(const std::string& name) const override;
      virtual std::auto_ptr<te::gm::Geometry> getGeometry(const std::string& name) const override;
      virtual std::auto_ptr<te::rst::Raster> getRaster(const std::string& name) const override;
      virtual std::auto_ptr<te::dt::DateTime> getDateTime(const std::string& name) const override;
      virtual std::auto_ptr<te::dt::Array> getArray(const std::string& name) const override;
      virtual bool isNull(const std::string& name) const override;

    private:
      void fillProperties(const std::unique_ptr<te::da::DataSetType>& ds1Type, const std::unique_ptr<te::da::DataSetType>& ds2Type);
      void fillKeyMap(const std::unique_ptr<te::da::DataSet>& ds1, const std::string& ds1Attribute, const std::unique_ptr<te::da::DataSet>& ds2, const std::string& ds2Attribute);

      std::size_t getLocalPropertyPos(const std::string& name) const;
      std::size_t getPropertyPos(std::size_t i) const;
      std::unique_ptr<te::da::DataSet>& getDataSet(std::size_t i) const;


      std::unique_ptr<te::da::DataSetType> ds1Type_;
      // the mutable qualifier allow to change lines in const methods to respect the interface
      mutable std::unique_ptr<te::da::DataSet> ds1_;
      std::string ds1Attribute_;
      std::unique_ptr<te::da::DataSetType> ds2Type_;
      // the mutable qualifier allow to change lines in const methods to respect the interface
      mutable std::unique_ptr<te::da::DataSet> ds2_;
      std::string ds2Attribute_;

      std::vector<te::dt::Property*> properties_;
      std::unique_ptr<te::da::DataSetType> dataSetType_;

      std::unordered_map<std::string, int> keyLineMap_;

    };
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_UTILITY_JOIN_DATASET_HPP__
