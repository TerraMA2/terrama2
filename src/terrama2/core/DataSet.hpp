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

  \brief Models the information of given dataset from a data provider that should be collected by TerraMA2.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATASET_HPP__
#define __TERRAMA2_CORE_DATASET_HPP__

// TerraMA2
#include "DataSetItem.hpp"
#include "Intersection.hpp"

// STL
#include <string>
#include <vector>

// TerraLib
#include <terralib/datatype/TimeDuration.h>

namespace terrama2
{
  namespace core
  {

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
    class DataSet
    {
      public:

        /*! \brief Dataset type.
         Each constant must exist in table terrama2.dataset_type and the value must be the same from column id.
         */
        enum Kind
        {
          UNKNOWN_TYPE = 1,
          PCD_TYPE = 2,
          OCCURENCE_TYPE = 3,
          GRID_TYPE = 4
        };

        //! Dataset status.
        enum Status
        {
          ACTIVE,
          INACTIVE
        };

        //! Struct to store the collect rules.
        struct CollectRule
        {
          uint64_t id;
          std::string script;
          uint64_t datasetId;
        };

        /*!
          \brief Constructor

          \param name       The name of the dataset.
          \param kind       The kind of dataset.
          \param id         The dataset identifier.
          \param providerId The data provider associated to this dataset.
        */
        DataSet(const std::string& name = "", Kind kind = UNKNOWN_TYPE, uint64_t id = 0, uint64_t providerId = 0);

        /*! \brief Destructor. */
        ~DataSet();

      public:

        /*! \brief Returns the identifier of the dataset. */
        uint64_t id() const;

        /*! \brief Sets the identifier of the dataset. */
        void setId(uint64_t id);

        /*! \brief Returns the name of the dataset. */
        const std::string& name() const;

        /*! \brief Sets the name of the dataset. */
        void setName(const std::string& name);

        /*! \brief Returns the description of the dataset. */
        const std::string& description() const;

        /*! \brief Sets the description of the dataset. */
        void setDescription(const std::string& d);

        /*! \brief Returns the kind of the dataset. */
        Kind kind() const;

        /*! \brief Sets the the kind of the dataset. */
        void setKind(const Kind k);

        /*! \brief Returns the the status of the dataset. */
        Status status() const;

        /*! \brief Sets the the status of the dataset. */
        void setStatus(const Status s);

        /*! \brief Returns the asscociated data provider. */
        uint64_t provider() const;

        /*! \brief Sets the asscociated data provider. */
        void setProvider(uint64_t id);

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
        const std::map<std::string, std::string>& metadata() const;

        /*!
           \brief Sets the dataset metadata.

           \param m The dataset metadata.
         */
        void setMetadata(const std::map<std::string, std::string>& m);

        /*!
           \brief Returns the collect rules.

           \return The collect rules.
         */
        std::vector<CollectRule>& collectRules();

        /*!
           \brief Sets the collect rules.

           \param rules The collect rules.
         */
        void setCollectRules(const std::vector<CollectRule>& rules);

        /*!
           \brief Returns the list of dataset items.

           \return The list of dataset items.
         */
        std::vector<DataSetItem>& dataSetItems();

        /*!
           \brief Returns the list of dataset items.

           \return The list of dataset items.
         */
        const std::vector<DataSetItem>& dataSetItems() const;

        /*! \brief Adds a new dataset item to the dataset. */
        void add(DataSetItem& d);

        /*! \brief Removes the give dataset item from the dataset. */
        void removeDataSetItem(uint64_t id);

        /*! \brief It returns the intersection information. */
        void setIntersection(const Intersection& intersection);

        /*! \brief Sets the intersection information. */
        Intersection intersection() const;


      private:


        std::string name_; //!< Dataset name, must be unique.
        Kind kind_; //!< Dataset type.
        uint64_t id_; //!< Identifier of the dataset.
        uint64_t provider_; //!< Identifier of the data provider.
        std::string description_; //!< Brief description of the dataset.
        Status status_; //!< Dataset status for collection.
        te::dt::TimeDuration dataFrequency_; //!< The time frequency that this dataset must try to acquire a new data.
        te::dt::TimeDuration schedule_; //!< The time scheduled to the next collection.
        te::dt::TimeDuration scheduleRetry_; //!< The time frequency to retry a collection if the data wasn't available in the scheduled time.
        te::dt::TimeDuration scheduleTimeout_; //!< The time limit to retry a scheduled collection.
        std::vector<CollectRule> collectRules_; //!< The collect rules.
        std::map<std::string, std::string> metadata_; //!< Dataset Metadata.
        std::vector<DataSetItem> datasetItems_; //!< List of dataset items.
        Intersection intersection_; //!< Intersection information.
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASET_HPP__

