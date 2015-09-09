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

// STL
#include <memory>
#include <cstdint>

// QT
#include <QObject>

namespace terrama2
{
  namespace core
  {
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
    class DataManager : public QObject
    {
      Q_OBJECT

      private:

        //! Just acessible through instance method.
        DataManager();

        //! Destructor.
        ~DataManager();

        //! No copy allowed.
        DataManager(const DataManager&);

        //! No copy allowed.
        DataManager& operator=(const DataManager&);


      public:

        /*!
          \brief Loads from the database all information about data providers and its datasets.

          If the data manager was already loaded it doesn't have effect.

          Emits dataManagerLoaded() signal when all the data from the database has been loaded.

          \pre The database with TerraMA2 metadata must have been initialized in the application controller.

          \exception

          \note Thread-safe.
         */
        void load();

        /*!
          \brief Unload the metadata of all data providers.

          Emits dataManagerUnloaded() signal when all the data from the database has been loaded.

          \exception

          \note Thread-safe.
         */
        void unload();

        /*!
          \brief Add the data provider to the database and register it in the manager.

          Emits dataProviderAdded() signal if the data provider is saved and registered in the manager.

          \pre The provider must not have an ID.
          \pre A provider with the same name must not be already in the manager.

          \pos The informed data provider will have a valid ID.

          \exception

          \note Thread-safe.
         */
        void add(DataProviderPtr provider);

        /*!
          \note Thread-safe.
         */
        void add(DataSetPtr dataset);

        /*!
          \note Thread-safe.
         */
        void update(DataProviderPtr dataProvider);

        /*!
          \note Thread-safe.
         */
        void update(DataSetPtr dataset);

        /*!
          \note Thread-safe.
         */
        void removeDataProvider(const uint64_t& id);

        /*!
          \note Thread-safe.
         */
        void removeDataSet(const uint64_t& id);

        /*!
          \note Thread-safe.
         */
        DataProviderPtr findDataProvider(const uint64_t& id) const;

        /*!
          \note Thread-safe.
         */
        DataSetPtr findDataSet(const uint64_t& id) const;

        static DataManager& instance();

        std::vector<terrama2::core::DataProviderPtr> listDataProvider() const;

        std::vector<terrama2::core::DataSetPtr> listDataSet() const;

    public slots:

      signals:

        void dataManagerLoaded();

        void dataProviderAdded(DataProviderPtr);
        void dataProviderRemoved(DataProviderPtr);
        void dataProviderUpdated(DataProviderPtr);
        //void dataProviderChanged(DataProviderPtr);

        void dataSetAdded(DataSetPtr);
        void dataSetRemoved(DataSetPtr);
        void dataSetUpdated(DataSetPtr);
        //void dataSetChanged(DataSetPtr);


      private:

        struct Impl;

        Impl* pimpl_;
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAMANAGER_HPP__

