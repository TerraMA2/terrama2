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

namespace te
{
  namespace da
  {
    class DataSource;
    class DataSourceTransactor;
  }
}

namespace terrama2
{
  namespace core
  {

    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    class DataSetDAO
    {
      public:

        /*!
          \brief Persists a given dataset in the database, the generated id will be set in the dataset.
          In case the data provider identifier is 0, it will also save the data provider.         *

          \param dataset Dataset to save.
          \param transactor Data source transactor.

          \exception InvalidDataSetIdError
         */
        static void save(DataSetPtr dataset, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Update a given dataset in the database.
          The identifier of the dataset must be valid.

          \param dataset Dataset to update.
          \param transactor Data source transactor.

          \exception InvalidDataSetIdError
         */
        static void update(DataSetPtr dataset, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Removes the dataset with the given id.
          The identifier of the dataset must be valid.
          In case there is an analysis configured to use this dataset, it will not be removed.

          \param id Identifier of the dataset.
          \param transactor Data source transactor.

          \exception InvalidDataSetIdError
         */
        static void remove(int id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Search for a dataset with the given id
          In case none is found it will return an empty smart pointer.

          \param id Identifier of the dataset.
          \param transactor Data source transactor.
          \return A smart pointer to the dataset.
         */
        static DataSetPtr find(int id, te::da::DataSourceTransactor& transactor);

        /*!
          \brief Retrieve all datasets from the database.
          In case none is found it will return an empty vector.

          \param transactor Data source transactor.
          \return Vector with all datasets.
         */
        static std::vector<DataSetPtr> list(te::da::DataSourceTransactor& transactor);

      private:

        //! Not instantiable.
        DataSetDAO();

        //! Not instantiable.
        ~DataSetDAO();
    };

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_DATASETDAO_HPP__
