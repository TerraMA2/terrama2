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
  \file terrama2/collector/ParserFirePoint.cpp

  \brief Parser of occurrences of fires file

  \author Jano Simas
  \author Evandro Delatin
*/


#include "ParserOGR.hpp"
#include "ParserFirePoint.hpp"
#include "DataFilter.hpp"
#include "Exception.hpp"

//QT
#include <QDir>
#include <QDebug>

//STD
#include <memory>

#include <boost/format/exceptions.hpp>

//terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/common/Exception.h>

#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/AttributeConverters.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/geometry.h>

te::dt::AbstractData* XYTo4326PointConverter(te::da::DataSet* dataset, const std::vector<std::size_t>& indexes, int dstType)
{
  assert(dataset);
  assert(indexes.size() == 2);

  te::dt::AbstractData* point = te::da::XYToPointConverter(dataset, indexes, dstType);

  static_cast<te::gm::Point*>(point)->setSRID(4326);

  return point;
}

void terrama2::collector::ParserFirePoint::adapt(te::da::DataSetTypeConverter&converter)
{
  converter.remove("lat");
  converter.remove("lon");

// Generates a point through the x and y coordinates
 te::gm::GeometryProperty* gm = new te::gm::GeometryProperty("geom", 4326, te::gm::PointType);

 std::vector<size_t> latLonAttributes;
 latLonAttributes.push_back(2);
 latLonAttributes.push_back(1);

 converter.add(latLonAttributes ,gm, XYTo4326PointConverter);
}
