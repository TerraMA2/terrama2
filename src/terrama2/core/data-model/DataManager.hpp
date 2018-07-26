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
  \file terrama2/core/data-model/DataManager.hpp

  \brief Manages metadata about data providers and its dataseries.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_CORE_DATAMANAGER_HPP__

// TerraMa2
#include "../Config.hpp"
#include "../Shared.hpp"
#include "../Typedef.hpp"

// STL
#include <memory>
#include <mutex>
#include <map>

// QT
#include <QObject>
#include <QJsonObject>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataManager

      \brief Manages metadata of DataProvider and DataSeries.

      For pratical means, the DataManager is the input of all metadata in a service,
      when new metadata arrives, usually as a JSon, the DataManager instantiate
      the appropriate classes and send signals that will be catch by the services.

      The services should have their own derived DataManager that will handle service
      specific structures and abstractions.

      ## Thread safety ##

      The DataManager is thread safe but some times this is not enough.

      In some cases you need to make a serie of calls from DataManager and
      be sure the state is consistent, for that there is getLock().

      The getLock() will lock the DataManager and return a std::unique_lock,
      it will keep the DataManager locked until released. Because of this it
      should be used with extreme caution.

      ## Order of things ##

      Some things should be kept in mind, dependent classes should be added **after**
      the classes they depend on. Ex.: First add DataSeries, than add DataProvider

      Dependent classes should be removed **before** the classes they depend on.
      Ex.: First remove DataProvider, than remove DataSeries

      This is also true for the services DataManager, when implementing addJSon()
      and removeJSon() keep this in mind.
     */
    class TMCOREEXPORT DataManager : public QObject
    {
		Q_OBJECT

      public:
        //! Constructor
        DataManager();

        //! Default destructor.
        virtual ~DataManager() = default;
        DataManager(const DataManager& other) = delete;
        DataManager(DataManager&& other) = delete;
        DataManager& operator=(const DataManager& other) = delete;
        DataManager& operator=(DataManager&& other) = delete;

        /*!
          \brief Lock the DataManager and returns a lock handle.

          This method is necessary to ensure data consistency when many calls are made.

          When using this method you'll receive a std::unique_lock to the DataManger,
          if using in a small context you'll probably have no problem because
          on the destruction of the std::unique_lock the lock is released.

          In the other hand you can release the lock yourself, this is useful when you have a
          long context or an intensive process, all you need to do is call std::unique_lock::unlock()
          after your use of the DataManger.

          \code{.cpp}
            auto lock = dataManager->getLock();

            // series of DataManager calls

            lock.unlock();

            //intensive code
          \endcode

          \warning The DataManger is locked by this method use with caution!
        */
        std::unique_lock<std::recursive_mutex> getLock();

        /*!
          \brief Parsers the QJsonObject for DataProvider and DataSeries to be added.

          For each member of the QJsonObject a DataProvider or a DataSeries will be build and added to the DataManager.

          The valid tags are:
            - "dataproviders"
            - "dataseries"

          For json syntax check the doc of the class.

          \pre The QJsonObject must have a list of json-converted DataProvider and DataSeries
        */
        virtual void addJSon(const QJsonObject& obj);
        /*!
          \brief Parsers the QJsonObject for DataProvider and DataSeries to be removed.

          For each member of the QJsonObject a DataProvider or a DataSeries will be removed from the DataManager.

          The valid tags are:
            - "dataproviders"
            - "dataseries"

          \pre The QJsonObject must have a list of DataProviderId and DataSeriesId
        */
        virtual void removeJSon(const QJsonObject& obj);

        /*!
          \brief Parsers the QJsonObject in order to validate data before being inserted.
        */
        virtual void validateJSon(const QJsonObject& obj);


        /*!
          \brief Register a DataProvider in the manager.

          At end it will emit dataProviderAdded(DataProviderPtr) signal.

          \param provider The DataProvider to be registered into the manager.

          \pre The DataProvider must not have a terrama2::core::InvalidId.
          \pre A DataProvider with the same id must not be already in the manager.

          \exception terrama2::InvalidArgumentException If it is not possible to add the DataProvider.

          \note Thread-safe.
        */
        virtual void add(DataProviderPtr provider);

        /*!
          \brief Register a DataSeries in the manager.

          At end it will emit dataSeriesAdded(DataSeriesPtr) signal.

          \param DataSeries The DataSeries to be registered into the manager.

          \pre The DataSeries must not have a terrama2::core::InvalidId.
          \pre A DataSeries with the same id must not be already in the manager.

          \exception terrama2::InvalidArgumentException If it is not possible to add the DataSeries.

          \note Thread-safe.
        */
        virtual void add(DataSeriesPtr dataseries);

        /*!
          \brief Update a given DataProvider.

          Emits dataProviderUpdated() signal if the data provider is updated successfully.

          \param provider    The DataProvider to be updated.

          \pre The DataProvider must not have a terrama2::core::InvalidId.
          \pre The DataProvider must exist in the DataManager.

          \exception terrama2::InvalidArgumentException If it is not possible to update the DataProvider.

          \note Thread-safe.
        */
        virtual void update(DataProviderPtr provider);

        /*!
          \brief Register a Project in the manager.

          \param project The Project to be registered into the manager.

          \pre The Project must not have a terrama2::core::InvalidId.
          \pre A Project with the same id must not be already in the manager.

          \exception terrama2::InvalidArgumentException If it is not possible to add the Project.

          \note Thread-safe.
        */
        virtual void add(ProjectPtr project);

        /*!
          \brief Update a given Project.

          \param project    The Project to be updated.

          \pre The Project must not have a terrama2::core::InvalidId.
          \pre The Project must exist in the DataManager.

          \exception terrama2::InvalidArgumentException If it is not possible to update the Project.

          \note Thread-safe.
        */
        virtual void update(ProjectPtr project);
        /*!
          \brief Removes a given Project.

          \param id ID of the Project to remove.

          \exception terrama2::InvalidArgumentException If it is not possible to remove the Project.

          \note Thread-safe.
        */
        virtual void removeProject(const ProjectId id);
        /*!
          \brief Retrieves the Project with the given id.

          \param name The Project id.

          \return ProjectPtr A smart pointer to the Project

          \exception terrama2::InvalidArgumentException If some error occur when trying to find the Project.

          \note Thread-safe.
        */
        virtual ProjectPtr findProject(const ProjectId id) const;

        /*!
          \brief Register a Legend in the manager.

          \param legend The Legend to be registered into the manager.

          \pre The Legend must not have a terrama2::core::InvalidId.
          \pre A Legend with the same id must not be already in the manager.

          \exception terrama2::InvalidArgumentException If it is not possible to add the Legend.

          \note Thread-safe.
        */
        virtual void add(LegendPtr legend);

        /*!
          \brief Update a given Legend.

          \param legend The Legend to be updated.

          \pre The Legend must not have a terrama2::core::InvalidId.
          \pre The Legend must exist in the DataManager.

          \exception terrama2::InvalidArgumentException If it is not possible to update the Legend.

          \note Thread-safe.
        */
        virtual void update(LegendPtr legend);

        /*!
          \brief Verify if a given Legend exist

          \param id ID of the legend

          \pre The Legend must not have a terrama2::core::InvalidId.

          \note Thread-safe.
        */
        virtual bool hasLegend(LegendId id) const;
        /*!
          \brief Removes a given Legend.

          \param id ID of the Legend to remove.

          \exception terrama2::InvalidArgumentException If it is not possible to remove the Legend.

          \note Thread-safe.
        */
        virtual void removeLegend(const LegendId id);
        /*!
          \brief Retrieves the Legend with the given id.

          \param name The Legend id.

          \return LegendPtr A smart pointer to the Legend

          \exception terrama2::InvalidArgumentException If some error occur when trying to find the Legend.

          \note Thread-safe.
        */
        virtual LegendPtr findLegend(const LegendId id) const;

        /*!
          \brief Update a given DataSeries.

          Emits dataSeriesUpdated() signal if the DataSeries is updated successfully.

          \param dataseries     DataSeries to update.
          \param shallowSave If true it will update only the DataSeries attributes.

          \pre The DataSeries must not have a terrama2::core::InvalidId.
          \pre The DataSeries must exist in the DataManager.

          \exception terrama2::InvalidArgumentException If it is not possible to update the DataSeries.

          \note Thread-safe.
        */
        virtual void update(DataSeriesPtr dataseries, const bool shallowSave = false);

        /*!
          \brief Removes a given DataProvider.

          Emits dataProviderRemoved() signal if the data provider is removed successfully.

          \param id ID of the DataProvider to remove.
          \param shallowRemove If false will remove every DataSeries dependent from the DataProvider

          \post If shallowRemove is false, it will remove all DataSeries that access this DataProvider.

          \exception terrama2::InvalidArgumentException If it is not possible to remove the DataProvider.

          \note Thread-safe.
        */
        virtual void removeDataProvider(const DataProviderId id, const bool shallowRemove = false);

        /*!
          \brief Removes the DataSeries with the given id.

          Emits dataSeriesRemoved() signal if the DataSeries is removed successfully.

          \param id ID of the DataSeries to remove.

          \exception terrama2::InvalidArgumentException If it is not possible to remove the DataSeries.

          \note Thread-safe.
        */
        virtual void removeDataSeries(const DataSeriesId id);

        /*!
          \brief Retrieves the DataProvider with the given name.

          \param name The DataProvider name.

          \return DataProviderPtr A smart pointer to the DataProvider

          \exception terrama2::InvalidArgumentException If some error occur when trying to find the DataProvider.

          \note Thread-safe.
        */
        virtual DataProviderPtr findDataProvider(const std::string& name) const;

        /*!
          \brief Retrieves the DataProvider with the given id.

          \exception terrama2::InvalidArgumentException If some error occur when trying to find the DataProvider.

          \param id The data provider identifier.

          \return DataProvider A smart pointer to the DataProvider

          \note Thread-safe.
        */
        virtual DataProviderPtr findDataProvider(const DataProviderId id) const;

        virtual bool hasDataProvider(const DataProviderId id) const;

        /*!
          \brief Search for a DataSeries with the given name

          \param name Name of the DataSeries.
          \return A smart pointer to the DataSeries.

          \exception terrama2::InvalidArgumentException If some error occur when trying to find the DataSeries.

          \note Thread-safe.
        */
        virtual DataSeriesPtr findDataSeries(const std::string& name) const;

        /*!
          \brief Search for a DataSeries with the given id

          \param id Identifier of the DataSeries.
          \return A smart pointer to the DataSeries.

          \exception terrama2::InvalidArgumentException If some error occur when trying to find the DataSeries.

          \note Thread-safe.
        */
        virtual DataSeriesPtr findDataSeries(const DataSeriesId id) const;
        virtual bool hasDataSeries(const DataSeriesId id) const;

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
        std::map<ProjectId, ProjectPtr> projects_; //!< A map from ProjectId to Project. 
        std::map<DataSeriesId, DataSeriesPtr> dataseries_;    //!< A map from DataSeriesId to DataSeries.
        std::map<LegendId, LegendPtr> legends_;    //!< A map from LegendId to Legend.
        mutable std::recursive_mutex mtx_;                    //!< A mutex to synchronize all operations.
    };

  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATAMANAGER_HPP__
