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
  \file terrama2/core/FilterDAO.hpp

  \brief Persistense layer for filter information associated to dataset items.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_FILTERDAO_HPP__
#define __TERRAMA2_CORE_FILTERDAO_HPP__

// TerraMA2
#include "Filter.hpp"

// STL
#include <memory>

// Forward declaration
namespace te { namespace da { class DataSourceTransactor; } }

namespace terrama2
{
  namespace core
  {
    // forward declaration
    class Filter;

    /*!
      \class FilterDAO

      \brief Persistense layer for filter information associated to dataset items.
     */
    class FilterDAO
    {
      public:

        /*!
          \brief Insert the filter information associated to dataset item in the database.

          \param f          Filter information associated to a dataset item.
          \param transactor The data source transactor to be used to perform the insert operation.

          \pre The filter must be associated to a dataset item.
          \pre The dataset item associated to the filter must be already saved in the database.

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void save(const Filter& f, te::da::DataSourceTransactor& transactor);


        /*!
          \brief Update the filter information associated to a dataset item in the database.

          \param f          Filter information associated to a dataset item.
          \param transactor The data source transactor to be used to perform the update operation.

          \pre The filter must be associated to a dataset item.
          \pre The dataset item associated to the filter must be already saved in the database.

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void update(const Filter& f, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Remove the filter information associated to the given dataset item if one exists.

          \param datasetItemId The dataset item identifier used to remove the filter information.
          \param transactor    The data source transactor to be used to perform the delete operation.

          \pre The dataset item identifier must be a valid identifier (a value different than 0).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static void remove(uint64_t datasetItemId, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Load the filter information for a given dataset item if one exists.

          \param datasetItemId The dataset item identifier used to search for filter information.
          \param transactor    The data source transactor to be used to perform the load operation.

          \pre The dataset item identifier must be a valid identifier (a value different than 0).

          \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
         */
        static std::unique_ptr<Filter> load(uint64_t datasetItemId, te::da::DataSourceTransactor& transactor);

      private:

        //! Not instantiable.
        FilterDAO();

        //! Not instantiable.
        ~FilterDAO();

    };

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_FILTERDAO_HPP__
