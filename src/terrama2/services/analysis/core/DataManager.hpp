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
  \file terrama2/services/analysis/core/DataManager.hpp

  \brief Manages metadata about analysis, data providers and its dataseries.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_ANALYSIS_CORE_DATAMANAGER_HPP__

// TerraMa2
#include "Config.hpp"
#include "Shared.hpp"
#include "Typedef.hpp"
#include "../../../core/data-model/DataManager.hpp"

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
          \class DataManager

          \brief Manages all the metadata about anlysis, data providers and its related dataseries.

          The DataManager is responsible for loading metadata about
          analysis, data providers and dataseries. It works like a database cache with TerraMA2
          concepts.

          Take care to keep it synchronized.
        */
        class TMANALYSISEXPORT DataManager : public terrama2::core::DataManager
        {
		  Q_OBJECT

          public:

            //! Constructor
            DataManager();

            //! Destructor
            virtual ~DataManager ();

            /*!
              \brief Decodes and registers the data model objects received in JSON into the DataManager.

              It decodes the objects from the analysis modules and call the parent function to decode the objects from core module.

              \pre The QJsonObject must have a list of json-converted data model objects.
            */
            virtual void addJSon(const QJsonObject& obj) override;

            /*!
              \brief Parsers the QJsonObject in order to validate data before being inserted.
            */
            virtual void validateJSon(const QJsonObject& obj) override;



            //! Needed for overloading add method.
            using terrama2::core::DataManager::add;

            //! Needed for overloading add method.
            using terrama2::core::DataManager::update;

            /*!
              \brief Register the analysis into the data manager.

              At end it will emit analysisAdded(Analysis) signal.

              \param analysis The analysis to be registered into the data manager.

              \pre The analysis must have a valid ID (its ID must not be zero).
              \pre A analysis with the same name must not be already in the manager.

              \exception terrama2::Exception If it is not possible to add the analysis.

              \note Thread-safe.
            */
            void add(AnalysisPtr analysis);

            /*!
              \brief Update a given analysis in the data manager

              Emits analysisUpdated() signal if the analysis is updated successfully.

              \param analysis The analysis to be updated.

              \pre The analysis must have a valid ID.
              \pre The analysis must exist in the data manager.

              \exception terrama2::Exception If it is not possible to update the analysis.

              \note Thread-safe.
            */
            void update(AnalysisPtr analysis);

            /*!
              \brief Removes the given analysis.

              Emits analysisRemoved() signal if the analysis is removed successfully.

              \param analysisId ID of the analysis to remove.

              \pre The analysis must have a valid ID.

              \exception terrama2::Exception If it is not possible to remove the analysis.

              \note Thread-safe.
            */
            void removeAnalysis(AnalysisId analysisId);

            /*!
              \brief Retrieves the analysis with the given id.

              In case there is no analysis in the database with the given id it will throw an InvalidArgumentException.

              \exception terrama2::Exception If some error occur when trying to find the analysis.

              \param id The analysis identifier.

              \return The analysis object.

              \note Thread-safe.
            */
            AnalysisPtr findAnalysis(const AnalysisId analysisId) const;

            using terrama2::core::DataManager::findDataSeries;

            /*!
            \brief Search for a dataseries with the given name, also look for alias configured in the analysis.

            In case none is found it will return an empty smart pointer.

            \param id The analysis identifier.
            \param name Name of the dataseries.
            \return A smart pointer to the dataseries.

            \exception terrama2::InvalidArgumentException If some error occur when trying to find the dataseries.

            \note Thread-safe.
            */
            terrama2::core::DataSeriesPtr findDataSeries(const AnalysisId analysisId, const std::string& name) const;

            /*!
            \brief Returns true if exists an analysis with the given identifier in the data manager, false otherwise.

            \note Thread-safe.
            */
            bool hasAnalysis(const AnalysisId analysisId) const;


          signals:

            //! Signal to notify that a analysis has been added.
            void analysisAdded(AnalysisPtr);

            //! Signal to notify that a analysis has been removed.
            void analysisRemoved(AnalysisId);

            //! Signal to notify that a analysis has been updated.
            void analysisUpdated(AnalysisPtr);

            //! Signal to notify that a analysis need to be validated.
            void validateAnalysis(AnalysisPtr);


          protected:

            std::map<AnalysisId, AnalysisPtr> analysis_; //!< A map from data-analysis-id to data-analysis.
        };

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_DATAMANAGER_HPP__
