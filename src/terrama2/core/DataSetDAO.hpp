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
  \file terrama2/core/DataSetDAO.hpp

  \brief DataProvider DAO...

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATASETDAO_HPP__
#define __TERRAMA2_CORE_DATASETDAO_HPP__

// STL
#include <vector>
#include <memory>

// terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>



namespace terrama2
{
  namespace core
  {

    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    class DataSetDAO
    {
    public:
      DataSetDAO(std::auto_ptr<te::da::DataSourceTransactor> transactor);
      virtual ~DataSetDAO();

      bool save(DataSetPtr dataset);
      bool update(DataSetPtr dataset);
      bool remove(const int& id);
      DataSetPtr get(const int& id) const;
      std::vector<DataSetPtr> list() const;

    protected:
      std::auto_ptr<te::da::DataSourceTransactor> transactor_;

    };

  } // core
} // terrama2

#endif // __TERRAMA2_CORE_DATASETDAO_HPP__
