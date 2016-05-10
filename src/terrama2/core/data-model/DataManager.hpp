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

  \brief Manages metadata about data providers and its dataseries.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_CORE_DATAMANAGER_HPP__

#include "../Shared.hpp"
#include "../Typedef.hpp"

// STL
#include <memory>
#include <mutex>

// QT
#include <QObject>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataManager

      \brief Manages all the metadata about data providers and its related dataseries.

      The DataManager is responsible for loading metadata about
      data providers and dataseries. It works like a database cache with TerraMA2
      concepts.

      Take care to keep it synchronized.
     */
    class DataManager : public QObject
    {
        Q_OBJECT

      public:
        //! Default constructor
        DataManager();

        //! Destructor.
        virtual ~DataManager();
        DataManager(const DataManager& other) = delete;
        DataManager(DataManager&& other) = delete;
        DataManager& operator=(const DataManager& other) = delete;
        DataManager& operator=(DataManager&& other) = delete;

        /*!
          \brief Lock the DataManager and returns a lock handle.

          This method is necessary to ensure data consistency when many querys are made.

          \warning The DataManger is locked by this method use with caution!
        */
        std::unique_lock<std::recursive_mutex> getLock();

        /*!
          \brief Add the DataProvider and DataSeries contained in the QJsonObject

          For each member of the QJsonObject a DataProvider or a DataSeries will be build and added to the DataManager.

          \pre The QJsonObject must have a list of json-converted DataProvider and DataSeries
        */
        virtual void addJSon(const QJsonObject& obj);
        /*!
          \brief Remove the DataProvider and DataSeries contained in the QJsonObject

          For each member of the QJsonObject a DataProvider or a DataSeries will be removed from the DataManager.

          \pre The QJsonObject must have a list of DataProviderId and DataSeriesId
        */
        virtual void removeJSon(const QJsonObject& obj);

        /*!
        \brief Register a DataProvider in the manager.

        At end it will emit dataProviderAdded(DataProviderPtr) signal.

        \param provider    TheDataProvider to be registered into the manager.

        \pre The provider must not have a terrama2::core::InvalidId.
        \pre A provider with the same name must not be already in the manager.

        \exception terrama2::InvalidArgumentException If it is not possible to add the data provider.

        \note Thread-safe.
        */
        virtual void add(DataProviderPtr provider);

        /*!
        \brief Register a DataSeries in the manager.

        At end it will emit dataSeriesAdded(DataSeriesPtr) signal.

        \param dataseries    The DataSeries to be registered into the manager.

        \pre The dataseries must not have a terrama2::core::InvalidId.
        \pre A dataseries with the same name must not be already in the manager.

        \exception terrama2::InvalidArgumentException If it is not possible to add the dataseries.

        \note Thread-safe.
        */
        virtual void add(DataSeriesPtr dataseries);

        /*!
        \brief Update a given data provider.

        Emits dataProviderUpdated() signal if the data provider is updated successfully.

        \param provider    The data provider to be updated.

        \pre The provider must not have a terrama2::core::InvalidId.
        \pre The data provider must exist in the DataManager.

        \exception terrama2::InvalidArgumentException If it is not possible to update the data provider.

        \note Thread-safe.
        */
        virtual void update(DataProviderPtr provider);

        /*!
        \brief Update a given dataseries.

        Emits dataSeriesUpdated() signal if the dataseries is updated successfully.

        \param dataseries     DataSeries to update.
        \param shallowSave If true it will update only the dataseries attributes.

        \pre The dataseries must not have a terrama2::core::InvalidId.
        \pre The dataseries must exist in the DataManager.

        \exception terrama2::InvalidArgumentException If it is not possible to update the dataseries.

        \note Thread-safe.
        */
        virtual void update(DataSeriesPtr dataseries, const bool shallowSave = false);

        /*!
        \brief Removes a given data provider.

        Emits dataProviderRemoved() signal if the data provider is removed successfully.

        \param id ID of the data provider to remove.
        \param shallowRemove If false will remove every DataSeries dependent from the DataProvider

        \post If shallowRemove is false, it will remove all dataseries that access this data provider.

        \exception terrama2::InvalidArgumentException If it is not possible to remove the data provider.

        \note Thread-safe.
        */

        virtual void removeDataProvider(const DataProviderId id, const bool shallowRemove = false);

        /*!
        \brief Removes the dataseries with the given id.

        Emits dataSeriesRemoved() signal if the dataseries is removed successfully.

        \param id ID of the dataseries to remove.

        \exception terrama2::InvalidArgumentException If it is not possible to remove the dataseries.

        \note Thread-safe.
        */
        virtual void removeDataSeries(const DataSeriesId id);

        /*!
        \brief Retrieves the data provider with the given name.

        In case there is no data provider with the given name it will return an empty smart pointer.

        \param name The data provider name.

        \return DataProviderPtr A smart pointer to the data provider

        \exception terrama2::InvalidArgumentException If some error occur when trying to find the data provider.

        \note Thread-safe.
        */
        virtual DataProviderPtr findDataProvider(const std::string& name) const;

        /*!
        \brief Retrieves the data provider with the given id.

        In case there is no data provider with the given id it will return an empty smart pointer.

        \exception terrama2::InvalidArgumentException If some error occur when trying to find the data provider.

        \param id The data provider identifier.

        \return DataProvider A smart pointer to the data provider

        \note Thread-safe.
        */
        virtual DataProviderPtr findDataProvider(const DataProviderId id) const;

        /*!
        \brief Search for a dataseries with the given name

        In case none is found it will return an empty smart pointer.

        \param name Name of the dataseries.
        \return A smart pointer to the dataseries.

        \exception terrama2::InvalidArgumentException If some error occur when trying to find the dataseries.

        \note Thread-safe.
        */
        virtual DataSeriesPtr findDataSeries(const std::string& name) const;

        /*!
        \brief Search for a dataseries with the given id
        In case none is found it will return an empty smart pointer.

        \param id Identifier of the dataseries.
        \return A smart pointer to the dataseries.

        \exception terrama2::InvalidArgumentException If some error occur when trying to find the dataseries.

        \note Thread-safe.
        */
        virtual DataSeriesPtr findDataSeries(const DataSeriesId id) const;

      signals:

        //! Signal to notify that a provider has been added.
        void dataProviderAdded(DataProviderPtr);

        //! Signal to notify that a provider has been removed.
        void dataProviderRemoved(DataProviderId);

        //! Signal to notify that a provider has been updated.
        void dataProviderUpdated(DataProviderPtr);

        //! Signal to notify that a dataseries has been added.
        void dataSeriesAdded(DataSeriesPtr);

        //! Signal to notify that a dataseries has been removed.
        void dataSeriesRemoved(DataSeriesId);

        //! Signal to notify that a dataseries has been updated.
        void dataSeriesUpdated(DataSeriesPtr);

      protected:
        std::map<DataProviderId, DataProviderPtr> providers_; //!< A map from DataProviderId to DataProvider.
        std::map<DataSeriesId, DataSeriesPtr> dataseries_;    //!< A map from DataSeriesId to DataSeries.
        mutable std::recursive_mutex mtx_;                    //!< A mutex to syncronize all operations.
    };

  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATAMANAGER_HPP__
