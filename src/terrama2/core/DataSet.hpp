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
  \file terrama2/core/DataSet.hpp

  \brief Models the information of given dataset dataset from a data provider that should be collected by TerraMA2.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATASET_HPP__
#define __TERRAMA2_CORE_DATASET_HPP__

// STL
#include <memory>
#include <string>
#include <vector>

// Boost
#include <boost/noncopyable.hpp>

// TerraLib
#include <terralib/datatype/TimeDuration.h>

namespace terrama2
{
  namespace core
  {
// Forward declaration
    class DataProvider;
    typedef std::shared_ptr<DataProvider> DataProviderPtr;

    class DataSetItem;
    typedef std::shared_ptr<DataSetItem> DataSetItemPtr;

    /*!
      \class DataSet

      \brief Models the information of given dataset dataset from a data provider that should be collected by TerraMA2.

      A dataset cmodels information about:
      - PCDs: fixed location sensors that emmits their measures in the format known as PCD-INPE.
      - Occurrences (or events): something that happens at a given place and time.
      - Coverages: from OGC WCS servers.
      - FeatureTypes: from OGC WFS server.

      A dataset has an associated time interval (or frequency) for being collected.
     */
    class DataSet : public boost::noncopyable
    {
      public:

        //! Dataset type.
        enum Kind
        {
          UNKNOWN_TYPE,
          PCD_TYPE,
          OCCURENCE_TYPE,
          GRID_TYPE
        };

        //! Dataset status.
        enum Status
        {
          ACTIVE,
          INACTIVE
        };

        /*!
          \brief Struct to store the collect rules.
         */
        struct CollectRule
        {
          uint64_t id;
          std::string script;
        };

        /*!
          \brief Constructor

          \param provider The data provider used obtain this dataset.
          \param name The name of the dataset.
          \param kind The kind of the dataset.
        */
        DataSet(DataProviderPtr provider, const std::string& name, Kind kind, const uint64_t id = 0);

        /*!
          \brief Destructor
        */
        ~DataSet();

      public:

        /*!
          \brief It returns the identifier of the dataset.

          \return The identifier of the dataset.
        */
        uint64_t id() const;

        /*!
          \brief It returns the name of the dataset.

          \return The name of the dataset.
        */
        std::string name() const;

        /*!
          \brief It sets the name of the dataset.

          \param name The name of the dataset.
        */
        void setName(const std::string& name);

        /*!
          \brief It returns the the description of the dataset.

          \return The description of the dataset.
        */
        std::string description() const;

        /*!
          \brief It sets the the description of the dataset.

          \param d The description of the dataset.
        */
        void setDescription(const std::string& d);

        /*!
          \brief It returns the the kind of the dataset.

          \return The kind of the dataset.
        */
        Kind kind() const;

        /*!
          \brief It sets the the kind of the dataset.

          \param k The kind of the data provider.
        */
        void setKind(const Kind& k);

        /*!
          \brief It returns the the status of the dataset.

          \return The status of the dataset.
        */
        Status status() const;

        /*!
          \brief It sets the the status of the dataset.

          \param s The status of the dataset.
        */
        void setStatus(const Status s);

        /*!
          \brief It returns the the data provider.

          \return The data provider.
        */
        DataProviderPtr dataProvider() const;

        /*!
          \brief It returns the time frequency that this dataset must try to acquire a new data.

          \return The the time frequency that this dataset must try to acquire a new data.
        */
        const te::dt::TimeDuration& dataFrequency() const;

        /*!
          \brief It sets the time frequency that this dataset must try to acquire a new data.

          \param t The time frequency that this dataset must try to acquire a new data.
        */
        void setDataFrequency(const te::dt::TimeDuration& t);

        /*!
          \brief It returns the time scheduled to the next collection.

          \return The time scheduled to the next collection.
        */
        const te::dt::TimeDuration& schedule() const;

        /*!
          \brief It sets the time scheduled to the next collection.

          \param t The time scheduled to the next collection.
        */
        void setSchedule(const te::dt::TimeDuration& t);

        /*!
          \brief It returns the time frequency to retry a collection if the data wasn't available in the scheduled time.

          \return The time frequency to retry a collection if the data wasn't available in the scheduled time.
        */
        const te::dt::TimeDuration& scheduleRetry() const;

        /*!
          \brief It sets the time frequency to retry a collection if the data wasn't available in the scheduled time.

          \param t The time frequency to retry a collection if the data wasn't available in the scheduled time.
        */
        void setScheduleRetry(const te::dt::TimeDuration& t);

        /*!
          \brief It returns the time limit to retry a scheduled collection.

          \return The time limit to retry a scheduled collection.
        */
        const te::dt::TimeDuration& scheduleTimeout() const;

        /*!
          \brief Sets the time limit to retry a scheduled collection.

          \param t The time limit to retry a scheduled collection.
        */
        void setScheduleTimeout(const te::dt::TimeDuration& t);

        /*!
          \brief Returns the map with the dataset metadata.

          \return The map with the dataset metadata.
         */
        std::map<std::string, std::string> metadata() const;

        /*!
           \brief Sets the dataset metadata.

           \param m The dataset metadata.
         */
        void setMetadata(const std::map<std::string, std::string>& m);

        /*!
           \brief Returns the collect rules.

           \return The collect rules.
         */
        std::vector<CollectRule> collectRules() const;

        /*!
           \brief Sets the collect rules.

           \param rules The collect rules.
         */
        void setCollectRules(const std::vector<CollectRule>& rules);

        /*!
           \brief Returns the list of of dataset items.

           \return The list of dataset items.
         */
        std::vector<DataSetItemPtr> dataSetItemList() const;

        /*!
           \brief Sets the list of dataset items.

           \param items The list of dataset items.
         */
        void setDataSetItemList(const std::vector<DataSetItemPtr>& items);


      protected:

        /*!
          \brief It sets the identifier of the dataset.

          \param The identifier of the dataset.
        */
        void setId(uint64_t id);


      private:

        uint64_t id_;
        std::string name_;
        std::string description_;
        Status status_;
        DataProviderPtr dataProvider_;
        Kind kind_;
        te::dt::TimeDuration dataFrequency_;
        te::dt::TimeDuration schedule_;
        te::dt::TimeDuration scheduleRetry_;
        te::dt::TimeDuration scheduleTimeout_;
        std::vector<CollectRule> collectRules_;
        std::map<std::string, std::string> metadata_;
        std::vector<DataSetItemPtr> dataSetItemList_;

        friend class DataSetDAO;
    };

    typedef std::shared_ptr<DataSet> DataSetPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASET_HPP__

