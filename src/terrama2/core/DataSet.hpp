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

  \brief Metadata about a given dataset.

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

// TerraLib
#include <terralib/datatype/TimeDuration.h>

namespace terrama2
{
  namespace core
  {

    class DataProvider;
    typedef std::shared_ptr<DataProvider> DataProviderPtr;

    class Data;
    typedef std::shared_ptr<Data> DataPtr;

    /*!
      \class DataSet

      \brief Contains metadata about a dataset.

      A dataset can be a PCD, occurence or a grid.

      It has the time frequency that this dataset should be collected.
     */
    class DataSet
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
        struct CollectRule {
            uint64_t id_;
            std::string script_;
        };


        DataSet(DataProviderPtr provider, const std::string& name, Kind kind);

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

          \param The name of the dataset.
        */
        void setName(const std::string& name);

        /*!
          \brief It returns the the description of the dataset.

          \return The description of the dataset.
        */
        std::string description() const;

        /*!
          \brief It sets the the description of the dataset.

          \param The description of the dataset.
        */
        void setDescription(const std::string& description);

        /*!
          \brief It returns the the kind of the dataset.

          \return The kind of the dataset.
        */
        Kind kind() const;

        /*!
          \brief It sets the the kind of the dataset.

          \param The kind of the data provider.
        */
        void setKind(const Kind& kind);

        /*!
          \brief It returns the the status of the dataset.

          \return The status of the dataset.
        */
        Status status() const;

        /*!
          \brief It sets the the status of the dataset.

          \param The status of the dataset.
        */
        void setStatus(const Status& status);

        /*!
          \brief It returns the the data provider.

          \return The data provider.
        */
        DataProviderPtr dataProvider() const;

        /*!
          \brief It returns the time frequency that this dataset must try to acquire a new data.

          \return The the time frequency that this dataset must try to acquire a new data.
        */
        te::dt::TimeDuration dataFrequency() const;

        /*!
          \brief It sets the time frequency that this dataset must try to acquire a new data.

          \param The time frequency that this dataset must try to acquire a new data.
        */
        void setDataFrequency(const te::dt::TimeDuration& dataFrequency);

        /*!
          \brief It returns the time scheduled to the next collection.

          \return The time scheduled to the next collection.
        */
        te::dt::TimeDuration schedule() const;

        /*!
          \brief It sets the time scheduled to the next collection.

          \param The time scheduled to the next collection.
        */
        void setSchedule(const te::dt::TimeDuration& schedule);

        /*!
          \brief It returns the time frequency to retry a collection if the data wasn't available in the scheduled time.

          \return The time frequency to retry a collection if the data wasn't available in the scheduled time.
        */
        te::dt::TimeDuration scheduleRetry() const;

        /*!
          \brief It sets the time frequency to retry a collection if the data wasn't available in the scheduled time.

          \param The time frequency to retry a collection if the data wasn't available in the scheduled time.
        */
        void setScheduleRetry(const te::dt::TimeDuration& scheduleRetry);

        /*!
          \brief It returns the time limit to retry a scheduled collection.

          \return The time limit to retry a scheduled collection.
        */
        te::dt::TimeDuration scheduleTimeout() const;

        /*!
          \brief Sets the time limit to retry a scheduled collection.

          \param The time limit to retry a scheduled collection.
        */
        void setScheduleTimeout(const te::dt::TimeDuration& scheduleTimeout);

        /*!
          \brief Returns the map with the dataset metadata.

          \return The map with the dataset metadata.
         */
        std::map<std::string, std::string> metadata() const;

        /*!
           \brief Sets the dataset metadata.

           \param The dataset metadata.
         */
        void setMetadata(const std::map<std::string, std::string>& metadata);

        /*!
           \brief Returns the collect rules.

           \return The collect rules.
         */
        std::vector<CollectRule> collectRules() const;

        /*!
           \brief Sets the collect rules.

           \param The collect rules.
         */
        void setCollectRules(const std::vector<CollectRule>& collectRules);

        /*!
           \brief Returns the list of data

           \return The list of data.
         */
        std::vector<DataPtr> dataList() const;

        /*!
           \brief Sets the list of data.

           \param The list of data.
         */
        void setDataList(const std::vector<DataPtr>& dataList);


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
        std::vector<DataPtr> dataList_;

        friend class DataSetDAO;
    };

    typedef std::shared_ptr<DataSet> DataSetPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASET_HPP__

