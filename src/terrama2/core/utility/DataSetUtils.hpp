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
  \file terrama2/core/data-access/DataAccessorPostGIS.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_UTILS_DATASET_UTILS_HPP__
#define __TERRAMA2_CORE_UTILS_DATASET_UTILS_HPP__

#include <string>
#include <memory>
#include <terralib/datatype/Property.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>

namespace terrama2 {
  namespace core {
    bool isPropertyEqual(te::dt::Property* newProperty, te::dt::Property* oldMember);
    std::unique_ptr<te::dt::Property>    copyProperty(te::dt::Property* property);
    std::shared_ptr<te::da::DataSetType> copyDataSetType(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                         const std::string& newDataSetName);
  } /* core */
} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILS_DATASET_UTILS_HPP__
