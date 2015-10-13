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
  \file terrama2/core/DataManager.hpp

  \brief Manages metadata about data providers and its datasets.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_CORE_DATAMANAGER_HPP__

// TerraLib
#include <terralib/common/Singleton.h>

// STL
#include <memory>

// QT
#include <QObject>

namespace terrama2
{
  namespace core
  {
// Forward declaration
    class DataProvider;
    typedef std::shared_ptr<DataProvider> DataProviderPtr;

    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    /*!
      \class DataManager

      \brief Manages all the metadata about data providers and its related datasets.

      The DataManager is a singleton responsible for loading metadata about
      data providers and datasets.

      It will load the metadata from a database and will keep them
      synchronized.
     */
    class DataManager : public QObject, public te::common::Singleton<DataManager>
    {
      Q_OBJECT

      friend class te::common::Singleton<DataManager>;

      public:

        /*!
          \brief Loads from the database all information about data providers and its datasets.

          If the data manager was already loaded it doesn't have effect.

          Emits dataManagerLoaded() signal when all the data from the database has been loaded.

          \pre The database with TerraMA2 metadata must have been initialized in the application controller.

          \exception terrama2::Exception If it is not possible to load TerraMA2 metadata.

          \note Thread-safe.
         */
        void load();

        /*!
          \brief Unload the metadata of all data providers.

          Emits dataManagerUnloaded() signal when all the data from the database has been loaded.

          \note Thread-safe.
         */
        void unload() noexcept;

        /*!
          \brief Add the data provider to the database and register it in the manager.

          This method will also add all the datasets contained in the data provider.
         
          At end it will emmit the following signals:
          - dataProviderAdded(DataProviderPtr): signal if the data provider is saved and registered in the manager.
          - dataSetAdded(DataSetPtr): one signal for each dataset in the data provider's list added to the database and registered in the manager.
         
          \param provider The data provider to be added to the database and registered into the manager.
          \param shallowSave If true it will update the datasets in this data provider.

          \pre The provider must not have a valid ID (its ID must be zero).
          \pre A provider with the same name must not be already in the manager.
          \pre If there are contained datasets, they must not have a valid ID (all IDs must be zero).

          \pos The informed data provider will have a valid ID (> 0).
          \pos The datasets within this provider will have a valid ID (> 0).

          \exception InvalidDataProviderError, InvalidDataProviderIdError, InvalidDataSetError, InvalidDataSetIdError

          \note Thread-safe.
         */
        void add(DataProviderPtr provider, const bool shallowSave = true);

        /*!
          \brief Add the dataset to the database and register it in the manager.
         
          At end it will emmit the following signals:
          - dataSetAdded(): signal if the dataset is saved and registered in the manager.
          - dataProviderUpdated(): signal to notify that there is a new dataset in the provider.

          \pre The dataset must not have an ID.
          \pre A provider with the same name must not be already in the manager.

          \pos The informed dataset will have a valid ID.

          \param dataset Dataset to add.
          \param shallowSave If true it will update the datasets in this data provider.

          \exception InvalidDataSetError, InvalidDataSetIdError, InvalidDataProviderError

          \note Thread-safe.
         */
        void add(DataSetPtr dataset, const bool shallowSave = true);

        /*!
          \brief Update a given data provider in the database.

          Emits dataProviderUpdated() signal if the data provider is updated successfully.
         
          \param provider The data provider to be updated.
          \param shallowSave If true it will persist the datasets in this data provider.

          \pre The data provider must have a valid ID.
          \pre The data provider must exist in the database.

          \exception InvalidDataProviderError, InvalidDataProviderIdError

          \note Thread-safe.
         */
        void update(DataProviderPtr provider, const bool shallowSave = true);

        /*!
          \brief Update a given dataset in the database.

          Emits dataSetUpdated() signal if the dataset is updated successfully.

          It will not update the datasets.
         
          \param dataset Dataset to update.
          \param shallowSave If true it will persist the datasets in this data provider.

          \pre The dataset must have an valid ID.
          \pre The dataset must exist in the database.

          \exception InvalidDataSetError, InvalidDataSetIdError, InvalidDataProviderError

          \note Thread-safe.
         */
        void update(DataSetPtr dataset, const bool shallowSave = true);

        /*!
          \brief Removes a given data provider.

          \pre The data provider must have a valid ID.

          Emits dataProviderRemoved() signal if the data provider is removed successfully.

          Emits dataSetRemoved() signal for each dataset that belongs to this data provider.

          It will remove all datasets that belong to this data provider.
          In case there is an analysis that uses one the datasets it will throw an DataSetInUseError().

          \exception InvalidDataProviderIdError, DataSetInUseError

          \param id ID of the data provider to remove.

          \note Thread-safe.
         */

        void removeDataProvider(const uint64_t id);

        /*!
          \brief Removes the dataset with the given id.

          \pre The dataset must have a valid ID.

          Emits dataSetRemoved() signal if the dataset is removed successfully.

          Emits dataProviderUpdated() signal to notify that a dataset was removed from the provider's list.

          In case there is an analysis configured to use this dataset, the dataset will not be removed.

          \param id ID of the dataset to remove.

          \exception InvalidDataSetIdError, DataSetInUseError

          \note Thread-safe.
         */
        void removeDataSet(const uint64_t id);

        /*!
          \brief Retrieves the data provider with the given name.

          \exception InvalidDataProviderIdError

          In case there is no data provider in the database with the given name it will return an empty smart pointer.

          \param name The data provider name.

          \return DataProviderPtr A smart pointer to the data provider

          \note Thread-safe.
         */
        DataProviderPtr findDataProvider(const std::string& name) const;

        /*!
          \brief Retrieves the data provider with the given id.

          \exception InvalidDataProviderIdError

          In case there is no data provider in the database with the given id it will return an empty smart pointer.

          \param id The data provider identifier.

          \return DataProviderPtr A smart pointer to the data provider

          \note Thread-safe.
         */
        DataProviderPtr findDataProvider(const uint64_t id) const;

        /*!
          \brief Search for a dataset with the given name
          In case none is found it will return an empty smart pointer.

          \param name Name of the dataset.
          \return A smart pointer to the dataset.

          \exception InvalidDataSetIdError

          \note Thread-safe.
         */
        DataSetPtr findDataSet(const std::string& name) const;

        /*!
          \brief Search for a dataset with the given id
          In case none is found it will return an empty smart pointer.

          \param id Identifier of the dataset.
          \return A smart pointer to the dataset.

          \exception InvalidDataSetIdError

          \note Thread-safe.
         */
        DataSetPtr findDataSet(const uint64_t id) const;

        /*!
          \brief Retrieves all data provider.

          In case there is no data provider in the database it will return an empty vector.

          \return std::vector<DataProviderPtr> A list with all data providers.

          \note Thread-safe.
         */
        std::vector<terrama2::core::DataProviderPtr> providers() const;

        /*!
          \brief Retrieve all datasets from the database.
          In case none is found it will return an empty vector.

          \return Vector with all datasets.

          \note Thread-safe.
         */
        std::vector<terrama2::core::DataSetPtr> dataSets() const;

      signals:

        void dataManagerLoaded();
        void dataManagerUnloaded();

        void dataProviderAdded(DataProviderPtr);
        void dataProviderRemoved(DataProviderPtr);
        void dataProviderUpdated(DataProviderPtr);

        void dataSetAdded(DataSetPtr);
        void dataSetRemoved(DataSetPtr);
        void dataSetUpdated(DataSetPtr);


      protected:

        //! Default constructor: use the getInstance class method to get access to the singleton.
        DataManager();

        //! Destructor.
        ~DataManager();

        struct Impl;

        Impl* pimpl_;  //!< Using Pimpl idiom.
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAMANAGER_HPP__

