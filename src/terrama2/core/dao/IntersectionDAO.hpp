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
  \file terrama2/core/dao/IntersectionDAO.hpp

  \brief Persistense layer for intersection information associated to a dataset.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_DAO_FILTERDAO_HPP__
#define __TERRAMA2_CORE_DAO_FILTERDAO_HPP__

// TerraMA2
#include "../Filter.hpp"
#include "../DataSet.hpp"

// STL
#include <memory>

// Forward declaration
namespace te { namespace da { class DataSourceTransactor; } }

namespace terrama2
{
  namespace core
  {
    // forward declaration
    class Intersection;

    namespace dao
    {
      /*!
        \class IntersectionDAO

        \brief Persistense layer for intersection information associated to a dataset.
       */
      class IntersectionDAO
      {
        public:

          /*!
            \brief Insert the intersection information associated to a dataset in the database.

            \param f          Intersection information associated to a dataset.
            \param transactor The data source transactor to be used to perform the insert operation.

            \pre The intersection must be associated to a dataset.
            \pre The dataset associated to the intersection must be already saved in the database.

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void save(const Intersection& intersection, te::da::DataSourceTransactor& transactor);


          /*!
            \brief Update the intersection information associated to a dataset in the database.

            \param f          Intersection information associated to a dataset.
            \param transactor The data source transactor to be used to perform the update operation.

            \pre The intersection must be associated to a dataset.
            \pre The dataset associated to the intersection must be already saved in the database.

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void update(const Intersection& intersection, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Remove the intersection information associated to the given dataset if one exists.

            \param datasetItemId The dataset identifier used to remove the intersection information.
            \param transactor    The data source transactor to be used to perform the delete operation.

            \pre The dataset identifier must be a valid identifier (a value different than 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void remove(uint64_t datasetId, te::da::DataSourceTransactor& transactor);

          /*!
            \brief Load the intersection information for a given dataset.

            \param datasetItemId The dataset identifier used to search for intersection information.
            \param transactor    The data source transactor to be used to perform the load operation.

            \pre The dataset identifier must be a valid identifier (a value different than 0).

            \exception terrama2::Exception If the operation doesn't succeed it will raise an exception.
           */
          static void load(terrama2::core::DataSet& dataset, te::da::DataSourceTransactor& transactor);

        private:

          //! Not instantiable.
          IntersectionDAO();

          //! Not instantiable.
          ~IntersectionDAO();

      };
    } // end namespace dao
  }   // end namespace core
}     // end namespace terrama2

#endif // __TERRAMA2_CORE_FILTERDAO_HPP__
