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

// TerraMA2
#include "DataProvider.hpp"
#include "DataSeries.hpp"

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

      The DataManager is a singleton responsible for loading metadata about
      data providers and dataseries. It works like a database cache with TerraMA2
      concepts.

      Take care to keep it synchronized.
     */
    class DataManager : public QObject
    {
      Q_OBJECT

      public:

        DataManager();

        //! Destructor.
        virtual ~DataManager();


        /*!
          \brief Add the data provider to the database and register it in the manager.

          This method will also add all the dataseries contained in the data provider.

          At end it will emit dataProviderAdded(DataProvider) signal.
          At end it will emit dataseriesAdded(DataSeries) for each dataseries added if it's not a shallow save.

          \param provider    The data provider to be added to the database and registered into the manager.
          \param shallowSave If true it will only save the data provider attributes.

          \pre The provider must have a valid ID (its ID must not be zero).
          \pre A provider with the same name must not be already in the manager.
          \pre If not performing a shallow save, the contained dataseries must have a valid ID (all IDs must not be zero).

          \exception terrama2::Exception If it is not possible to add the data provider.

          \note Thread-safe.
         */
        void add(DataProviderPtr provider);

        //TODO: doc here
        void add(DataSeriesPtr dataseries);

        /*!
          \brief Update a given data provider in the database.

          Emits dataProviderUpdated() signal if the data provider is updated successfully.
          Emits dataSeriesAdded() signal if a dataseries was added to the provider and it's not a shallow save.
          Emits dataSeriesRemoved() signal if a dataseries was removed from the provider and it's not a shallow save.
          Emits dataSeriesUpdated() signal if a dataseries was updated and it's not a shallow save.

          \param provider    The data provider to be updated.
          \param shallowSave If true it will update only the data provider attributes.

          \pre The data provider must have a valid ID.
          \pre The data provider must exist in the database.

          \exception terrama2::Exception If it is not possible to update the data provider.

          \note Thread-safe.
         */
        void update(DataProviderPtr provider);

        /*!
          \brief Update a given dataseries in the database.

          Emits dataSeriesUpdated() signal if the dataseries is updated successfully.

          \param dataseries     DataSeries to update.
          \param shallowSave If true it will update only the dataseries attributes.

          \pre The dataseries must have a valid ID.
          \pre The dataseries must exist in the database.

          \exception terrama2::Exception If it is not possible to update the dataseries.

          \note Thread-safe.
         */
        void update(DataSeriesPtr dataseries, const bool shallowSave = false);

        /*!
          \brief Removes a given data provider.

          Emits dataProviderRemoved() signal if the data provider is removed successfully.

          \param id ID of the data provider to remove.
          \param shallowRemove If false will remove every DataSeries dependent from the DataProvider

          \pre The data provider must have a valid ID.

          \pos It will remove all dataseries that belong to this data provider.
          \pos In case there is an analysis that uses one the dataseries it will throw an DataSeriesInUseException().

          \exception terrama2::Exception If it is not possible to remove the data provider.

          \note Thread-safe.
         */

        void removeDataProvider(const DataProviderId id, const bool shallowRemove = false);

        /*!
          \brief Removes the dataseries with the given id.

          \pre The dataseries must have a valid ID.

          Emits dataSeriesRemoved() signal if the dataseries is removed successfully.

          In case there is an analysis configured to use this dataseries, the dataseries will not be removed.

          \param id ID of the dataseries to remove.

          \exception terrama2::Exception If it is not possible to remove the dataseries.

          \note Thread-safe.
         */
        void removeDataSeries(const DataSeriesId id);

        /*!
          \brief Retrieves the data provider with the given name.

          In case there is no data provider in the database with the given name it will return an empty smart pointer.

          \param name The data provider name.

          \return DataProviderPtr A smart pointer to the data provider

          \exception terrama2::Exception If some error occur when trying to find the data provider.

          \note Thread-safe.
         */
        DataProviderPtr findDataProvider(const std::string& name) const;

        /*!
          \brief Retrieves the data provider with the given id.

          In case there is no data provider in the database with the given id it will return an empty smart pointer.

          \exception terrama2::Exception If some error occur when trying to find the data provider.

          \param id The data provider identifier.

          \return DataProvider A smart pointer to the data provider

          \note Thread-safe.
         */
        DataProviderPtr findDataProvider(const DataProviderId id) const;

        /*!
          \brief Search for a dataseries with the given name
          In case none is found it will return an empty smart pointer.

          \param name Name of the dataseries.
          \return A smart pointer to the dataseries.

          \exception terrama2::Exception If some error occur when trying to find the dataseries.

          \note Thread-safe.
         */
        DataSeriesPtr findDataSeries(const std::string& name) const;

        /*!
          \brief Search for a dataseries with the given id
          In case none is found it will return an empty smart pointer.

          \param id Identifier of the dataseries.
          \return A smart pointer to the dataseries.

          \exception terrama2::Exception If some error occur when trying to find the dataseries.

          \note Thread-safe.
         */
        DataSeriesPtr findDataSeries(const DataSeriesId id) const;

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

        std::map<DataProviderId, DataProviderPtr> providers_; //!< A map from data-provider-id to data-provider.
        std::map<DataSeriesId, DataSeriesPtr> dataseries_;       //!< A map from data-set-id to dataseries.
        mutable std::recursive_mutex mtx_;                             //!< A mutex to syncronize all operations.

    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAMANAGER_HPP__
