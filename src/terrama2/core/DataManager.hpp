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

// TerraMA2
#include "DataProvider.hpp"
#include "DataSet.hpp"

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
    /*!
      \class DataManager

      \brief Manages all the metadata about data providers and its related datasets.

      The DataManager is a singleton responsible for loading metadata about
      data providers and datasets. It works like a database cache with TerraMA2
      concepts.

      Take care to keep it synchronized.
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

          At end it will emit dataProviderAdded(DataProvider) signal.
          At end it will emit datasetAdded(DataSet) for each dataset added if it's not a shallow save.

          \param provider    The data provider to be added to the database and registered into the manager.
          \param shallowSave If true it will only save the data provider attributes.

          \pre The provider must not have a valid ID (its ID must be zero).
          \pre A provider with the same name must not be already in the manager.
          \pre If not performing a shallow save, the contained datasets must not have a valid ID (all IDs must be zero).

          \pos The informed data provider will have a valid ID (> 0).
          \pos If not performing a shallow save, the datasets within this provider will have a valid ID (> 0).

          \exception terrama2::Exception If it is not possible to add the data provider.

          \note Thread-safe.
         */
        void add(DataProvider& provider, const bool shallowSave = false);

        /*!
          \brief Add the dataset to the database and register it in the manager.

          Emits the dataSetAdded() signal when finished.
          Emits the dataProviderUpdated() signal when finished.

          \param dataset     The dataset to be added.
          \param shallowSave If true it will only save the dataset attributes.

          \pre The dataset must not have an ID.
          \pre A dataset with the same name must not be already in the manager.
          \pre The dataset must be associated to a valid and registered data provider.

          \pos The informed dataset will have a valid ID.

          \exception terrama2::Exception If it is not possible to load add the dataset.

          \note Thread-safe.
         */
        void add(DataSet& dataset, const bool shallowSave = false);

        /*!
          \brief Update a given data provider in the database.

          Emits dataProviderUpdated() signal if the data provider is updated successfully.

          \param provider    The data provider to be updated.
          \param shallowSave If true it will update only the data provider attributes.

          \pre The data provider must have a valid ID.
          \pre The data provider must exist in the database.

          \exception terrama2::Exception If it is not possible to update the data provider.

          \note Thread-safe.
         */
        void update(DataProvider& provider, const bool shallowSave = false);

        /*!
          \brief Update a given dataset in the database.

          Emits dataSetUpdated() signal if the dataset is updated successfully.

          \param dataset     Dataset to update.
          \param shallowSave If true it will update only the dataset attributes.

          \pre The dataset must have a valid ID.
          \pre The dataset must exist in the database.

          \exception terrama2::Exception If it is not possible to update the dataset.

          \note Thread-safe.
         */
        void update(DataSet& dataset, const bool shallowSave = false);

        /*!
          \brief Removes a given data provider.

          Emits dataProviderRemoved() signal if the data provider is removed successfully.

          \param id ID of the data provider to remove.

          \pre The data provider must have a valid ID.

          \pos It will remove all datasets that belong to this data provider.
          \pos In case there is an analysis that uses one the datasets it will throw an DataSetInUseError().

          \exception terrama2::Exception If it is not possible to remove the data provider.

          \note Thread-safe.
         */

        void removeDataProvider(const uint64_t id);

        /*!
          \brief Removes the dataset with the given id.

          \pre The dataset must have a valid ID.

          Emits dataSetRemoved() signal if the dataset is removed successfully.
          At end it will emit the dataProviderUpdated() signal.

          In case there is an analysis configured to use this dataset, the dataset will not be removed.

          \param id ID of the dataset to remove.

          \exception terrama2::Exception If it is not possible to remove the dataset.

          \note Thread-safe.
         */
        void removeDataSet(const uint64_t id);

        /*!
          \brief Retrieves the data provider with the given name.

          In case there is no data provider in the database with the given name it will return an empty smart pointer.

          \param name The data provider name.

          \return DataProvider A smart pointer to the data provider

          \exception terrama2::Exception If some error occur when trying to find the data provider.

          \note Thread-safe.
         */
        DataProvider findDataProvider(const std::string& name) const;

        /*!
          \brief Retrieves the data provider with the given id.

          In case there is no data provider in the database with the given id it will return an empty smart pointer.

          \exception terrama2::Exception If some error occur when trying to find the data provider.

          \param id The data provider identifier.

          \return DataProvider A smart pointer to the data provider

          \note Thread-safe.
         */
        DataProvider findDataProvider(const uint64_t id) const;

        /*!
          \brief Search for a dataset with the given name
          In case none is found it will return an empty smart pointer.

          \param name Name of the dataset.
          \return A smart pointer to the dataset.

          \exception terrama2::Exception If some error occur when trying to find the dataset.

          \note Thread-safe.
         */
        DataSet findDataSet(const std::string& name) const;

        /*!
          \brief Search for a dataset with the given id
          In case none is found it will return an empty smart pointer.

          \param id Identifier of the dataset.
          \return A smart pointer to the dataset.

          \exception terrama2::Exception If some error occur when trying to find the dataset.

          \note Thread-safe.
         */
        DataSet findDataSet(const uint64_t id) const;

        /*!
          \brief Retrieves all data provider.

          In case there is no data provider in the database it will return an empty vector.

          \return std::vector<DataProvider> A list with all data providers.

          \note Thread-safe.
         */
        std::vector<terrama2::core::DataProvider> providers() const;

        /*!
          \brief Retrieve all datasets from the database.
          In case none is found it will return an empty vector.

          \return Vector with all datasets.

          \note Thread-safe.
         */
        std::vector<terrama2::core::DataSet> dataSets() const;

        /*!
         \brief Verify if the given name is being used by another dataset.

         \return True if the given name is available.

         \param name The dataset name.
        */
        bool isDatasetNameValid(const std::string& name) const;

        /*!
         \brief Verify if the given name is being used by another data provider.

         \return True if the given name is available.

         \param name The data provider name.
         */
        bool isDataProviderNameValid(const std::string& name) const;

      signals:

        //! Signal to notify that the data manager has been loaded.
        void dataManagerLoaded();

        //! Signal to notify that the data manager has been unloaded.
        void dataManagerUnloaded();

        //! Signal to notify that a provider has been added.
        void dataProviderAdded(DataProvider);

        //! Signal to notify that a provider has been removed.
        void dataProviderRemoved(DataProvider);

        //! Signal to notify that a provider has been updated.
        void dataProviderUpdated(DataProvider);


        //! Signal to notify that a dataset has been added.
        void dataSetAdded(DataSet);

        //! Signal to notify that a dataset has been removed.
        void dataSetRemoved(uint64_t);

        //! Signal to notify that a dataset has been updated.
        void dataSetUpdated(DataSet);


      protected:

        //! Default constructor: use the getInstance class method to get access to the singleton.
        DataManager();

        //! Destructor.
        ~DataManager();

        struct Impl;

        Impl* pimpl_;  //!< Pimpl idiom.
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAMANAGER_HPP__

