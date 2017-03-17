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
  \file terrama2/services/alert/core/Alert.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_TE_DATASET_FK_JOIN_HPP__
#define __TERRAMA2_CORE_TE_DATASET_FK_JOIN_HPP__

#include <memory>
#include <unordered_map>
#include <string>

namespace te{
  namespace da{
    class DataSetType;
    class DataSet;
  }
  namespace dt{
    class AbstractData;
    class Property;
  }
}

namespace terrama2
{
  namespace core
  {
    class TeDataSetFKJoin
    {
    public:
      explicit TeDataSetFKJoin( std::shared_ptr<te::da::DataSetType> referrerDataSetType,
                                std::shared_ptr<te::da::DataSet> referrerDataSet,
                                std::string referrerAttribute,
                                std::shared_ptr<te::da::DataSetType> referredDataSetType,
                                std::shared_ptr<te::da::DataSet> referreeDataSet,
                                std::string referredAttribute);
      //! Default destructor
      ~TeDataSetFKJoin() = default;
      TeDataSetFKJoin(const TeDataSetFKJoin& other) = default;
      TeDataSetFKJoin(TeDataSetFKJoin&& other) = default;
      TeDataSetFKJoin& operator=(const TeDataSetFKJoin& other) = default;
      TeDataSetFKJoin& operator=(TeDataSetFKJoin&& other) = default;

      /*!
        \brief Get the value of the property at propertyName of the joined DataSet

        This methos will access the current position of the referee DataSet to get the ForeignKey value to access
        the righ position of the referred DataSet.

        \return Value of the porperty at the referred DataSet
      */
      std::auto_ptr<te::dt::AbstractData> getValue(std::string propertyName) const;
      te::dt::Property* getProperty(std::string propertyName) const;

      inline const std::shared_ptr<te::da::DataSetType>& referrerDataSetType() const { return _referrerDataSetType; }
      inline const std::shared_ptr<te::da::DataSet>& referrerDataSet() const { return _referrerDataSet; }
      inline const std::shared_ptr<te::da::DataSetType>& referredDataSetType() const { return _referredDataSetType; }
      inline const std::shared_ptr<te::da::DataSet>& referredDataSet() const { return _referredDataSet; }

    private:
      void fillPKMap(std::string referredPropertyName, std::shared_ptr<te::da::DataSet> referredDataSet);


      std::unordered_map<std::string, size_t> _referredPKMap; //!< Map of primare key to line
      std::string _referrerPropertyName; //!< Foreign Key property

      std::shared_ptr<te::da::DataSetType> _referrerDataSetType;
      std::shared_ptr<te::da::DataSet> _referrerDataSet; //!< DataSet with a ForeignKey to another DataSet
      std::shared_ptr<te::da::DataSetType> _referredDataSetType;
      std::shared_ptr<te::da::DataSet> _referredDataSet; //!< DataSet referred by fkDataSet
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_TE_DATASET_FK_JOIN_HPP__
